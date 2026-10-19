/*
 * tty echo demo
 *
 * This demo reads from file TEST_FILE, writes it content
 * to /dev/blttysd0, and read back to verify if the device
 * correctly echoes.
 *
 * Only for demo!
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <termios.h>

#define DEVICE "/dev/blttysd0"
#define TEST_FILE "test.bin"
#define CHUNK_SIZE 1024
#define TIMEOUT_SEC 0
#define TIMEOUT_USEC 100000

int main()
{
    struct termios term;
    char read_buffer[CHUNK_SIZE];
    char write_buffer[CHUNK_SIZE];
    char verify_buffer[CHUNK_SIZE];
    int fd;
    FILE *file;

    // Open the file
    file = fopen(TEST_FILE, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }

    // Open the device
    fd = open(DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open device");
        fclose(file);
        return -1;
    }

    // Set terminal to RAW mode
    if (tcgetattr(fd, &term) == -1) {
        perror("Failed to get terminal attributes");
        fclose(file);
        close(fd);
        return -1;
    }
    cfmakeraw(&term);
    if (tcsetattr(fd, TCSANOW, &term) == -1) {
        perror("Failed to set terminal attributes");
        fclose(file);
        close(fd);
        return -1;
    }

    // Read and write the file in chunks
    size_t bytes_read;
    size_t bytes_written;
    size_t total_bytes_read = 0;
    do {
        bytes_read = fread(write_buffer, sizeof(char), CHUNK_SIZE, file);
        if (bytes_read == 0 && ferror(file)) {
            perror("Error reading file");
            fclose(file);
            close(fd);
            return -1;
        } else if (bytes_read == 0 && feof(file)){
            break;
        }

        bytes_written = write(fd, write_buffer, bytes_read);
        if (bytes_written == -1) {
            perror("Error writing to device");
            fclose(file);
            close(fd);
            return -1;
        }

        if (bytes_written != bytes_read) {
            fprintf(stderr, "Incomplete write to device\n");
            fclose(file);
            close(fd);
            return -1;
        }

        // Read the content back in chunks and verify
        ssize_t bytes_read_chunk;
        size_t bytes_verified = 0;
        size_t expected_bytes = bytes_read;
        fd_set read_fds;
        struct timeval timeout;
        int select_result;

        do {
            FD_ZERO(&read_fds);
            FD_SET(fd, &read_fds);

            timeout.tv_sec = TIMEOUT_SEC;
            timeout.tv_usec = TIMEOUT_USEC;

            select_result = select(fd + 1, &read_fds, NULL, NULL, &timeout);
            if (select_result == -1) {
                perror("Error in select");
                fclose(file);
                close(fd);
                return -1;
            } else if (select_result == 0) {
                fprintf(stderr, "Timeout occurred\n");
                continue;
            }

            bytes_read_chunk = read(fd, read_buffer, CHUNK_SIZE);
            if (bytes_read_chunk == -1) {
                perror("Error reading from device");
                fclose(file);
                close(fd);
                return -1;
            } else if (bytes_read_chunk == 0) {
                fprintf(stderr, "Unexpected end of file\n");
                fclose(file);
                close(fd);
                return -1;
            }

            if (bytes_verified + bytes_read_chunk > expected_bytes) {
                fprintf(stderr, "Read more bytes than expected\n");
                fclose(file);
                close(fd);
                return -1;
            }

            memcpy(verify_buffer + bytes_verified, read_buffer, bytes_read_chunk);
            bytes_verified += bytes_read_chunk;
        } while (bytes_verified < expected_bytes);

        if (memcmp(write_buffer, verify_buffer, expected_bytes) != 0) {
            printf("Contents do not match\n");
            fclose(file);
            close(fd);
            return -1;
        }

        total_bytes_read += bytes_read;
    } while (bytes_read > 0);

    printf("Contents match\n");

    fclose(file);
    close(fd);

    return 0;
}
