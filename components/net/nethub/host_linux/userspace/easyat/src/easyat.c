#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>

#define SERIAL_DEVICE "/dev/ttyHD0"
#define BAUD_RATE B115200
#define BUFFER_SIZE 2048

// Serial port file descriptor
static int serial_fd = -1;

// Initialize serial port
int serial_init() {
    struct termios options;

    // Open serial port device
    serial_fd = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd < 0) {
        printf("Failed to open serial port %s: %s\n", SERIAL_DEVICE, strerror(errno));
        return -1;
    }
    // Virtual serial port does not support baud rate configuration for now
    // Restore to blocking mode
    fcntl(serial_fd, F_SETFL, 0);

    // Get current configuration
    if (tcgetattr(serial_fd, &options) != 0) {
        printf("Failed to get serial port configuration: %s\n", strerror(errno));
        close(serial_fd);
        return -1;
    }

    // Set baud rate
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);

    // 8N1 configuration (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;   // No parity
    options.c_cflag &= ~CSTOPB;   // 1 stop bit
    options.c_cflag &= ~CSIZE;    // Clear data bit setting
    options.c_cflag |= CS8;       // 8 data bits

    // Enable receiver, ignore modem control lines
    options.c_cflag |= (CLOCAL | CREAD);

    // Raw input mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Raw output mode
    options.c_oflag &= ~OPOST;

    // Disable software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Set timeout
    options.c_cc[VTIME] = 0;     // Non-blocking read
    options.c_cc[VMIN] = 0;

    // Apply configuration
    if (tcsetattr(serial_fd, TCSANOW, &options) != 0) {
        printf("Failed to set serial port configuration: %s\n", strerror(errno));
        close(serial_fd);
        return -1;
    }

    return 0;
}

// Send command to serial port
int send_cmd(const char *cmd) {
    int len = strlen(cmd);
    printf("-> %s", cmd);  // Display sent command
    fflush(stdout);

    int sent = write(serial_fd, cmd, len);
    if (sent != len) {
        printf("Failed to send command: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

// Wait for response and check if it contains target string
int wait_response(const char *target, int timeout_sec) {
    char buffer[BUFFER_SIZE];
    int buffer_pos = 0;
    time_t start_time = time(NULL);

    while (time(NULL) - start_time < timeout_sec) {
        fd_set read_fds;
        struct timeval tv;
        int result;

        FD_ZERO(&read_fds);
        FD_SET(serial_fd, &read_fds);

        tv.tv_sec = 1;  // Wait for 1 second each time
        tv.tv_usec = 0;

        result = select(serial_fd + 1, &read_fds, NULL, NULL, &tv);

        if (result < 0) {
            printf("select failed: %s\n", strerror(errno));
            return -1;
        } else if (result == 0) {
            // Timeout, continue loop
            continue;
        }

        if (FD_ISSET(serial_fd, &read_fds)) {
            int bytes_read = read(serial_fd, buffer + buffer_pos, BUFFER_SIZE - buffer_pos - 1);
            if (bytes_read > 0) {
                buffer_pos += bytes_read;
                buffer[buffer_pos] = '\0';

                // Display received data
                printf("%s", buffer + buffer_pos - bytes_read);
                fflush(stdout);

                // Check if target string received
                if (strstr(buffer, target) != NULL) {
                    return 1;  // Successfully found target string
                }
#if 0
                // Check if ERROR received
                if (strstr(buffer, "ERROR") != NULL) {
                    return 0;  // Received error response
                }
#endif

                // Prevent buffer overflow
                if (buffer_pos > BUFFER_SIZE / 2) {
                    memmove(buffer, buffer + buffer_pos / 2, buffer_pos / 2);
                    buffer_pos /= 2;
                }
            } else if (bytes_read < 0) {
                printf("Failed to read from serial port: %s\n", strerror(errno));
                return -1;
            }
        }
    }

    return 0;  // Timeout, target string not found
}

// Close serial port
void serial_close() {
    if (serial_fd >= 0) {
        close(serial_fd);
        serial_fd = -1;
    }
}

// WiFi connection command
int cmd_connect(const char *ssid, const char *password) {
    char cmd[256];
    int result;

    if (!ssid) {
        printf("SSID cannot be empty\n");
        return 1;
    }
    // set mode
    if (send_cmd("AT+CWMODE=1\r\n") != 0) {
        return 1;
    }
    result = wait_response("OK", 2);
    if (result != 1) {
        printf("WiFi set mode error\n");
        return 1;
    }

    // Build connection command: AT+CWJAP="ssid","password"
    if (password) {
        snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    } else {
        snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"\"\r\n", ssid);
    }

    if (send_cmd(cmd) != 0) {
        return 1;
    }

    // Wait 20 seconds, check GOTIP response
    result = wait_response("GOTIP", 20);
    if (result == 1) {
        return 0;
    } else if (result == 0) {
        printf("WiFi connection failed or timeout\n");
        return 1;
    } else {
        return 1;
    }
}

// Get connection status command
int cmd_get_link_status() {
    printf("Checking connection status...\n");

    if (send_cmd("AT+CIPSTA?\r\n") != 0) {
        return 1;
    }

    // Wait 2 seconds, check status response
    int result = wait_response("OK", 2);
    if (result == 1) {
        return 0;
    } else {
        return 1;
    }
}

// WiFi scan command
int cmd_wifi_scan() {
    printf("Scanning WiFi networks...\n");

    if (send_cmd("AT+CWLAP\r\n") != 0) {
        return 1;
    }

    // Wait 10 seconds, check scan results
    int result = wait_response("+CW:SCAN_DONE", 10);
    if (result == 1) {
        printf("WiFi scan completed\n");
        return 0;
    } else {
        printf("WiFi scan failed or no networks found\n");
        return 1;
    }
}

// Disconnect from AP command
int cmd_disconnect_ap() {
    printf("Disconnecting from AP...\n");

    if (send_cmd("AT+CWQAP\r\n") != 0) {
        return 1;
    }

    // Wait 5 seconds, check OK response
    int result = wait_response("OK", 5);
    if (result == 1) {
        printf("Successfully disconnected from AP\n");
        return 0;
    } else {
        printf("Failed to disconnect from AP or timeout\n");
        return 1;
    }
}

// Show usage help
void show_usage(const char *prog_name) {
    printf("Usage:\n");
    printf("  %s connect <ssid> [password]  - Connect to WiFi\n", prog_name);
    printf("  %s disconnect_ap              - Disconnect from AP\n", prog_name);
    printf("  %s get_link_status            - Get connection status\n", prog_name);
    printf("  %s wifi_scan                  - Scan WiFi networks\n", prog_name);
    printf("\n");
    printf("Examples:\n");
    printf("  %s connect MyWiFi password123\n", prog_name);
    printf("  %s connect OpenWiFi\n", prog_name);
    printf("  %s disconnect_ap\n", prog_name);
}

int main(int argc, char *argv[]) {
    int result = 0;

    // Check parameter count
    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }

    // Initialize serial port
    if (serial_init() != 0) {
        return 1;
    }

    // Parse command
    if (strcmp(argv[1], "connect") == 0) {
        if (argc < 3 || argc > 4) {
            printf("connect command parameter error\n");
            show_usage(argv[0]);
            result = 1;
        } else {
            const char *password = (argc == 4) ? argv[3] : NULL;
            result = cmd_connect(argv[2], password);
        }
    } else if (strcmp(argv[1], "get_link_status") == 0) {
        if (argc != 2) {
            printf("get_link_status command takes no parameters\n");
            show_usage(argv[0]);
            result = 1;
        } else {
            result = cmd_get_link_status();
        }
    } else if (strcmp(argv[1], "disconnect_ap") == 0) {
        if (argc != 2) {
            printf("disconnect_ap command takes no parameters\n");
            show_usage(argv[0]);
            result = 1;
        } else {
            result = cmd_disconnect_ap();
        }
    } else if (strcmp(argv[1], "wifi_scan") == 0) {
        if (argc != 2) {
            printf("wifi_scan command takes no parameters\n");
            show_usage(argv[0]);
            result = 1;
        } else {
            result = cmd_wifi_scan();
        }
    } else {
        printf("Unknown command: %s\n", argv[1]);
        show_usage(argv[0]);
        result = 1;
    }

    // Close serial port
    serial_close();

    return result;
}
