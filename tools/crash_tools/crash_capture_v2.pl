#!/usr/bin/perl

#use Inline C;
use File::Basename qw(dirname);
use Cwd qw(abs_path);
use strict;
use feature "switch";
use open qw(:std :encoding(UTF-8));
no warnings 'experimental';

BEGIN {
    push @INC, dirname(abs_path($0));
}
use MIME::Base64 qw(decode_base64 encode_base64);
use Digest::CRC qw(crc32);

my $inner;
my $inner_orig_data;
my $line_prefix = 0;
my $find_coredump = 0;
my $output_dir = dirname(abs_path($0))."/output";
my $log_file = $ARGV[0]; shift @ARGV;
my $toolchain = $ENV{TOOLCHAIN} eq "" && "riscv64-unknown-elf-" || $ENV{TOOLCHAIN};
my $nth_coredump = 0;
unlink $output_dir;
mkdir $output_dir;

open IN, "<$log_file";
while(<IN>) {
    $_ = substr($_, $line_prefix);
    given ($_) {
        # utf16-le \0 + 2n + x0 x0 x0 x0 x0 .... , utf16-be 2n + 0x 0x 0x 0x 0x 0x ....
        # so this match utf16-le and utf16-be
        when ($_ =~ /^.((.{2})*)-\0\+\0-\0\+\0-\0\+\0-\0 \0.* \0C\0O\0R\0E\0D\0U\0M\0P\0 \0v\0\x31\0\.\0\x30\0\.\0\x30\0 \0\+\0-\0\+\0-\0\+\0-\0\+/) {
            open IN, "<:raw:encoding(UTF-16):crlf", $log_file;
            continue;
        }
        when ($_ =~ /(.*)-\+-\+-\+- .* COREDUMP v1\.0\.0 \+-\+-\+-\+/) {
            $line_prefix = length($1) if length($1) != 0;
            $inner = 1;
            $nth_coredump++;
            open OUT, ">$output_dir/coredump${nth_coredump}";
            print "[crash_capture] find ${nth_coredump} coredump \n";
        }
        when ($_ =~ /(.*)-\+-\+-\+- .* COREDUMP END \+-\+-\+-\+/) {
            $inner = 0;

            print OUT "${inner_orig_data}"
        }
        when ($_ !~ /^[\r\n]*$/ && $inner == 1) {
            if(srec_decode_oneline($_)) {
                $inner_orig_data .= $_;
            } else {
                print "${_}checksum fail \r\n";
            }
        }
        default {
            #print "other data\n"
        }
    }
}

if($nth_coredump == 0) {
    die "[crash_capture] no valid coredump find \n";
}

sub srec_decode_oneline {
    my ($line) = @_;

    my ($type, $count, $rest) = unpack "A2A2A*", $line;
    my %address_hex_lengths = (
        'S1' => 4,
        'S2' => 6,
        'S3' => 8,
    );
    my $address_hex_length = %address_hex_lengths{$type};
    my $data_hex_length = (hex($count) - 1 - $address_hex_length / 2) * 2;
    my ($address_hex, $data_hex, $checksum_hex) = unpack "A${address_hex_length}A${data_hex_length}A2", $rest;

    my $calculated_checksum = 0;
    my @line_bytes = unpack "C*", pack "H*", $data_hex;
    $calculated_checksum += $_ for (@line_bytes, unpack "C8", pack "H8", $address_hex);
    $calculated_checksum += scalar(@line_bytes) + 4 + 1;
    $calculated_checksum = (~$calculated_checksum) & 0xff;

    if (hex($checksum_hex) != $calculated_checksum) {
        return 0;
    }
    return 1;
}
