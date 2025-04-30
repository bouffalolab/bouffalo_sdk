#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define COREDUMP_VERSION "1.0.0"

#define COREDUMP_START_STR "\r\n-+-+-+- BFLB COREDUMP v" COREDUMP_VERSION " +-+-+-+\r\n\r\n"

/*
 * Motorola S-record format encode
 *
 * Similar with followed perl function
 * sub srec_encode {
 *   my ($data, $addr) = @_;
 *   my @lines = unpack "(a100)*", $data;
 *   my $ret = "";
 *
 *   foreach my $line (@lines) {
 *       # get hex array, see perldoc -f pack
 *       my $line_hex = unpack "H*", $line;
 *       my @line_bytes = unpack "W*", $line;
 *       my $addr_hex = unpack "H8", pack "N", $addr;
 *       # calc checksum
 *       my $sum = 0;
 *       $sum += $_ for (@line_bytes, unpack "W8", pack "N", $addr);
 *       $sum += scalar(@line_bytes)+4+1;
 *       $addr += scalar(@line_bytes);
 *
 *       # byte count, addr, data, checksum
 *       $ret .= "S3" . (unpack "H2", pack "C", scalar(@line_bytes)+4+1) . $addr_hex . $line_hex . (unpack "H2", pack "C", ~($sum&0xff) & 0xff) . "\n";
 *   }
 *   return $ret;
 * }
 */
void srec_encode(const uint8_t *data, uint32_t data_length, uint32_t addr) {
    // Assuming maximum line length of 100 characters
    const int MAX_LINE_LENGTH = 100;
    const int MAX_DATA_LENGTH = MAX_LINE_LENGTH / 2; // Each byte represented by 2 characters

    uint8_t line[MAX_DATA_LENGTH];
    uint8_t addr_bytes[4];
    uint8_t sum;
    int i, j;
    uint32_t bytes_remaining = data_length;
    const uint8_t *data_ptr = data;

    while (bytes_remaining > 0) {
        size_t bytes_to_read = (bytes_remaining > MAX_DATA_LENGTH) ? MAX_DATA_LENGTH : bytes_remaining;
        memcpy(line, data_ptr, bytes_to_read);

        // Convert address to bytes
        for (i = 0; i < 4; i++) {
            addr_bytes[i] = (addr >> (8 * (3 - i))) & 0xFF;
        }

        // Calculate checksum
        sum = 0;
        for (i = 0; i < bytes_to_read; i++) {
            sum += line[i];
        }
        for (i = 0; i < 4; i++) {
          sum += addr_bytes[i];
        }
        sum += bytes_to_read + 4 + 1;

        // Encode data into S-record format and print
        printf("S3%02X%02X%02X%02X%02X", (unsigned int)(bytes_to_read + 4 + 1), addr_bytes[0], addr_bytes[1], addr_bytes[2], addr_bytes[3]);
        for (j = 0; j < bytes_to_read; j++) {
          printf("%02X", line[j]);
        }
        printf("%02X\r\n", ~sum & 0xFF);

        // Move to next chunk of data
        data_ptr += bytes_to_read;
        bytes_remaining -= bytes_to_read;
        addr += bytes_to_read;
    }
}

void coredump_print(uintptr_t addr, uintptr_t lma_addr, size_t len, const char *desc)
{
    static int init = 0;
    if (init == 0) {
      puts(COREDUMP_START_STR);
      init = 1;
    }
    srec_encode((const uint8_t *)lma_addr, (uint32_t)len, (uint32_t)addr);
}
