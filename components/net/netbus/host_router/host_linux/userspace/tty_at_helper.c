#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>

#define BUF_SIZE (2048)

/* Configure TTY to raw mode and disable OPOST */
static int set_tty_raw(int fd)
{
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    /* Set raw mode */
    cfmakeraw(&tty);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

/*
 * Ensure buffer ends with '\r\n', avoiding overflow.
 * Returns 0 on success, -1 on error (buffer overflow).
 */
int fix_newline(char *buf, size_t max_size)
{
    size_t len;

    if (!buf || max_size < 3) {
        return -1;
    }

    len = strlen(buf);
    if (len == 0)
        return 0;

    /* Check the last character and handle accordingly */
    if (buf[len - 1] == '\n') {
        /* If ends with '\n', check for preceding '\r' */
        if (len >= 2 && buf[len - 2] == '\r') {
            /* Already ends with '\r\n', do nothing */
            return 0;
        } else {
            /* If only '\n', replace with '\r\n' */
            if (len + 1 < max_size) {
                buf[len - 1] = '\r';  /* Replace '\n' with '\r' */
                buf[len] = '\n';      /* Add '\n' */
                buf[len + 1] = '\0';  /* Null-terminate the string */
                return 0;
            }
        }
    } else if (buf[len - 1] == '\r') {
        /* If ends with '\r', append '\n' */
        if (len + 1 < max_size) {
            buf[len] = '\n';      /* Add '\n' */
            buf[len + 1] = '\0';  /* Null-terminate the string */
            return 0;
        }
    } else {
        /* Otherwise, append '\r\n' */
        if (len + 2 < max_size) {
            buf[len] = '\r';
            buf[len + 1] = '\n';
            buf[len + 2] = '\0'; /* Null-terminate the string */
            return 0;
        }
    }

    /* Buffer overflow or too small to hold '\r\n' */
    return -1;
}

/* Handle I/O between stdin and the TTY device */
static void handle_io(int tty_fd)
{
    fd_set fds;
    char buf[BUF_SIZE];

    while (1) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(tty_fd, &fds);

        int max_fd = (STDIN_FILENO > tty_fd) ? STDIN_FILENO : tty_fd;

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        /* Read from stdin, convert to '\r\n' if necessary, and send to TTY */
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            memset(buf, 0, BUF_SIZE);
            if (fgets(buf, BUF_SIZE, stdin) != NULL) {
                /* Ensure '\r\n' at the end */
                int ret = fix_newline(buf, BUF_SIZE);
                if (ret == 0) {
                    ssize_t written = write(tty_fd, buf, strlen(buf));
                    if (written < 0) {
                        perror("write failed");
                    }
                } else {
                    fprintf(stderr, "Error: buffer overflow while processing newline\n");
                }
            }
        }

        /* Read from TTY and print directly to stdout */
        if (FD_ISSET(tty_fd, &fds)) {
            memset(buf, 0, BUF_SIZE);
            int len = read(tty_fd, buf, BUF_SIZE - 1);
            if (len > 0) {
                buf[len] = '\0';
                printf("%s", buf);
                fflush(stdout);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <tty_device>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open the specified TTY device */
    int tty_fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (tty_fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    int err = set_tty_raw(tty_fd);
    if (err < 0) {
        fprintf(stderr, "Failed to configure tty\n");
        return EXIT_FAILURE;
    }

    printf("Connected to %s. Type messages and press ENTER to send.\n", argv[1]);

    /* Start handling I/O */
    handle_io(tty_fd);
    close(tty_fd);
    return EXIT_SUCCESS;
}
