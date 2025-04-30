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
my $inner_name;
my $inner_addr;
my $inner_length;
my $inner_data;
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
        when ($_ =~ /^.((.{2})*)-\0\+\0-\0\+\0-\0\+\0-\0 \0.* \0C\0O\0R\0E\0D\0U\0M\0P\0 \0v\0\x30\0\.\0\x30\0\.\0\x31\0 \0\+\0-\0\+\0-\0\+\0-\0\+/) {
            open IN, "<:raw:encoding(UTF-16):crlf", $log_file;
            continue;
        }
        when ($_ =~ /(.*)-\+-\+-\+- .* COREDUMP v0\.0\.1 \+-\+-\+-\+/) {
            $line_prefix = length($1) if length($1) != 0;
            $find_coredump = 1;
            $nth_coredump++;
            open OUT, ">$output_dir/coredump${nth_coredump}";
            print OUT "-+-+-+- BFLB COREDUMP v0.0.1 +-+-+-+\r\n\r\n";
            print "[crash_capture] find ${nth_coredump} coredump \n";
        }
        when ($_ =~ /------ DATA BEGIN ([0-9A-Fa-f]+)@([0-9A-Fa-f]+)@([-\w. ]+) ------/) {
            $inner = 1;
            $inner_addr = hex($1);
            $inner_length = hex($2);
            $inner_name = $3;
            $inner_data = "";
            $inner_orig_data = "";
        }
        when ($_ =~ /------ END (.+) ------/) {
            $inner = 0;
            my $orig_crc = $1;
            my $end_crc = unpack "L", decode_base64($1);

            $inner_data = rs_decode($inner_data) if $inner_name =~ s/(.+)\.rs/\1/;
            $inner_data = lzw_decode($inner_data) if $inner_name =~ s/(.+)\.lzw/\1/;
            my $inner_crc = crc32($inner_data);

            if ($end_crc != $inner_crc) {
                printf("crc check addr: %08x, length: %08x, end_crc: %08x, crc: %08x \n", $inner_addr, $inner_length, $end_crc, $inner_crc);
                #$find_coredump = 0;
            } else {
                #print OUT srec_encode($inner_data, $inner_addr);
                print OUT "------ DATA BEGIN " . sprintf("%08X", $inner_addr) . "@" . sprintf("%08X", $inner_length) . "\@save_crash ------\r\n${inner_orig_data}------ END ${orig_crc} ------\r\n\r\n";
            }
        }
        when ($_ =~ /.*/ && $inner == 1) {
            my $line = decode_base64($_);
            $inner_orig_data .= $_;
            $inner_data .= $line;
        }
        when ($_ =~ /^$/) {
            #print "space line\n"
        }
        default {
            #print "other data\n"
        }
    }
}

if($log_file ne "/dev/null" && $find_coredump == 0) {
    die "[crash_capture] no valid coredump find \n";
}

sub srec_encode {
    my ($data, $addr) = @_;
    my @lines = unpack "(a100)*", $data;
    my $ret = "";

    foreach my $line (@lines) {
        # get hex array, see perldoc -f pack
        my $line_hex = unpack "H*", $line;
        my @line_bytes = unpack "W*", $line;
        my $addr_hex = unpack "H8", pack "N", $addr;
        # calc checksum
        my $sum = 0;
        $sum += $_ for (@line_bytes, unpack "W8", pack "N", $addr);
        $sum += scalar(@line_bytes)+4+1;
        $addr += scalar(@line_bytes);

        # byte count, addr, data, checksum
        $ret .= "S3" . (unpack "H2", pack "C", scalar(@line_bytes)+4+1) . $addr_hex . $line_hex . (unpack "H2", pack "C", ~($sum&0xff) & 0xff) . "\n";
    }
    return $ret;
}
