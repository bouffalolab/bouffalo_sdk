#include <string.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include "log.h"
#include "util.h"


struct code_and_reason {
    enum status_code code;
    const char *reason;
};

static struct code_and_reason status_code_and_reason[] = {
    {CONTINUE,                              "Continue"},
    {OK,                                    "OK"},
    {CREATED,                               "Created"},
    {LOW_ON_STORAGE_SPACE,                  "Low on Storage Space"},
    {MULTIPLE_CHOICES,                      "Multiple Choices"},
    {MOVED_PERMANENTLY,                     "Moved Permanently"},
    {MOVED_TEMPORARILY,                     "Moved Temporarily"},
    {SEE_OTHER,                             "See Other"},
    {NOT_MODIFIED,                          "Not Modified"},
    {USE_PROXY,                             "Use Proxy"},
    {BAD_REQUEST,                           "Bad Request"},
    {UNAUTHORIZED,                          "Unauthorized"},
    {PAYMENT_REQUIRED,                      "Payment Required"},
    {FORBIDDEN,                             "Forbidden"},
    {NOT_FOUND,                             "Not Found"},
    {METHOD_NOT_ALLOWED,                    "Method Not Allowed"},
    {NOT_ACCEPTABLE,                        "Not Acceptable"},
    {PROXY_AUTHENTICATION_REQUIRED,         "Proxy Authentication Required"},
    {REQUEST_TIME_OUT,                      "Request Time-out"},
    {GONE,                                  "Gone"},
    {LENGTH_REQUIRED,                       "Length Required"},
    {PRECONDITION_FAILED,                   "Precondition Failed"},
    {REQUEST_ENTITY_TOO_LARGE,              "Request Entity Too Large"},
    {REQUEST_URI_TOO_LARGE,                 "Request-URI Too Large"},
    {UNSUPPORTED_MEDIA_TYPE,                "Unsupported Media Type"},
    {PARAMETER_NOT_UNDERSTOOD,              "Parameter Not Understood"},
    {CONFERENCE_NOT_FOUND,                  "Conference Not Found"},
    {NOT_ENOUGH_BANDWIDTH,                  "Not Enough Bandwidth"},
    {SESSION_NOT_FOUND,                     "Session Not Found"},
    {METHOD_NOT_VALID_IN_THIS_STATE,        "Method Not Valid in This State"},
    {HEADER_FIELD_NOT_VALID_FOR_RESOURCE,   "Header Field Not Valid for Resource"},
    {INVALID_RANGE,                         "Invalid Range"},
    {PARAMETER_IS_READ_ONLY,                "Parameter Is Read-Only"},
    {AGGREGATE_OPERATION_NOT_ALLOWED,       "Aggregate operation not allowed"},
    {ONLY_AGGREGATE_OPERATION_ALLOWED,      "Only aggregate operation allowed"},
    {UNSUPPORTED_TRANSPORT,                 "Unsupported transport"},
    {DESTINATION_UNREACHABLE,               "Destination unreachable"},
    {INTERNAL_SERVER_ERROR,                 "Internal Server Error"},
    {NOT_IMPLEMENTED,                       "Not Implemented"},
    {BAD_GATEWAY,                           "Bad Gateway"},
    {SERVICE_UNAVAILABLE,                   "Service Unavailable"},
    {GATEWAY_TIME_OUT,                      "Gateway Time-out"},
    {VERSION_NOT_SUPPORTED,                 "Version not supported"},
    {OPTION_NOT_SUPPORTED,                  "Option not supported"},
};

/*
 * Set socket descriptor block mode:
 * block:       1
 * nonblock:    0
 */
int set_block_mode(int fd, int mode)
{
    int flags = 0;

    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        printf(ERR "Get flags of socket error");
        return -1;
    }

    flags = mode ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(fd, F_SETFL, flags) < 0) {
        printf(ERR "Set flags of socket error");
        return -1;
    }
    return 0;
}

/*
 * Create non-blocking socket and bind to the given port.
 */
int create_and_bind(unsigned short port)
{
    int sd = -1;                /* Listen socket. */
    struct sockaddr_in sa;      /* Address of RTSP listen socket. */
    int reuse = 1;
    
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(port);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        printf(EMERG "Create rtsp server listening socket error");
        return -1;
    }

    /* Set socket in nonblock mode. */
    if (set_block_mode(sd, 0) < 0) {
        goto err;
    }
#ifndef BL616
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
                   &reuse, sizeof(reuse)) < 0) {
        printf(ERR "Set listening socket option[SO_REUSEADDR] error");
        goto err;
    }
#endif
    if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        printf(EMERG "Bind listening sockaddr to local port[%hd] error: %s\r\n",
               port, strerror(errno));
        goto err;
    }

    return sd;
err:
    close(sd);
    return -1;
}

const char *get_status_reason(unsigned int code)
{
    int i = 0;
    const char *reason = "Corresponding Reason Undefined";
    int num = sizeof(status_code_and_reason) / sizeof(*status_code_and_reason);

    for (i = 0; i < num; i++) {
        if (status_code_and_reason[i].code == code) {
            reason = status_code_and_reason[i].reason;
        }
    }
    return reason;
}


/**
 * Skip over the first NALU start code found in address [start, end).
 * Return position right after the start code.
 */
char *skip_nalu_start_code(char *start, char *end)
{
    char *ptr = start;

    while (end - ptr > 3) {
        if (*ptr == 0 && *(ptr + 1) == 0 && *(ptr + 2) == 0 && *(ptr + 3) == 1) {
            return (ptr + 4);
        } else if (*ptr == 0 && *(ptr + 1) == 0 && *(ptr + 2) == 1) {
            return (ptr + 3);
        } else {
            ptr++;
        }
    }
    return NULL;
}

/**
 * @nalu: point to the start of NALU, has skipped the NALU start code.
 * Return size of the NALU.
 */
int get_nalu_sz(char *nalu, char *end)
{
#if 1
    const char *p = nalu;
    while (1) {
        if ((p >= end) ||
            (p[0] == 0x0 && p[1] == 0x0 && p[2] == 0x0 && p[3] == 0x0)) {
            p = end;
            break;
        } else if (p[0] == 0x0 && p[1] == 0x0 && p[2] == 0x0 && p[3] == 0x01) {
            if (p + 4 >= end) {
                p = end;
            }
            break;
        } else if (p[0] == 0x0 && p[1] == 0x0 && p[2] == 0x01) {
            if (p + 3 >= end) {
                p = end;
            }
            break;
        }
        p++;
    }
    return  p - nalu;
#else    
    char *ptr = nalu;

    while (ptr < end) {
        if (end - ptr > 3) {
        if ((*ptr == 0 && *(ptr + 1) == 0 && *(ptr + 2) == 0 && *(ptr + 3) == 1) ||
            (*ptr == 0 && *(ptr + 1) == 0 && *(ptr + 2) == 1)) {
            return (ptr - nalu);
            } else if (*ptr == 0 && *(ptr + 1) == 0 && *(ptr + 2) == 0 && *(ptr + 3) == 0) {
                return ptr - nalu;
            }
        } else {
            return (end - nalu);
        }
        ptr++;
    }
    return 0;
#endif
}

/**
 * We will allocate memory here, remember to free the returned memory.
 */
char *encode_base64(const char *src, unsigned int src_sz)
{
    static const char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *res = NULL;
    char *dst = NULL;
    unsigned bits = 0;
    int shift = 0;
    unsigned int left = 0;
    int padded = 0;
    unsigned int res_sz = 0;

    if (src_sz >= UINT_MAX / 4) {
        printf("src_sz >= (UINT_MAX / 4) T_T\r\n");
        return NULL;
    }

    padded = src_sz > (src_sz / 3 * 3);
    res_sz = 4 * (src_sz / 3 + padded);
    res = mallocz(res_sz + 1); /* Extra 1 size allowing for trailing '\0'. */
    if (!res) {
        printf("Allocate memory for encode_base64 failed!\r\n");
        return NULL;
    }
    dst = res;
    left = src_sz;
    while (left) {
        bits = (bits << 8) + *src++;
        left--;
        shift += 8;

        do {
            *dst++ = b64[(bits << 6 >> shift) & 0x3f];
            shift -= 6;
        } while (shift > 6 || (left == 0 && shift > 0));
    }
    while ((dst - res) & 3) {
        *dst++ = '=';
    }
    *dst = '\0';

    return res;
}

/**
 * Return a 64 bits random number.
 */
unsigned long long random64(void)
{
    return ((long long)random() << 32) | random();
}
