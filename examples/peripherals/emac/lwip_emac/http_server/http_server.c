/**
 * @file http_server.c
 * @brief Application callbacks for the built-in lwIP HTTP server.
 */

#include <limits.h>
#include <string.h>

#include "lwip/apps/httpd.h"
#include "lwip/apps/fs.h"
#include "lwip/err.h"
#include "lwip/tcpip.h"

#include "http_server.h"
#include "readme_assets.h"

#define DBG_TAG "HTTP_SERVER"
#include "log.h"

#define HTTP_README_EN_URI "/docs/readme-en.md"
#define HTTP_README_ZH_URI "/docs/readme-zh.md"

/**
 * @brief Expose a read-only INCBIN asset through the lwIP custom file API.
 */
static int http_open_readme(struct fs_file *file, enum http_readme_language language)
{
    size_t size;
    const unsigned char *data = http_readme_asset_get(language, &size);

    if ((data == NULL) || (size > INT_MAX)) {
        return 0;
    }

    file->data = (const char *)data;
    file->len = (int)size;
    file->index = file->len;
    file->pextension = NULL;
    file->flags = 0;

    return 1;
}

int fs_open_custom(struct fs_file *file, const char *name)
{
    if ((file == NULL) || (name == NULL)) {
        return 0;
    }

    if (strcmp(name, HTTP_README_EN_URI) == 0) {
        return http_open_readme(file, HTTP_README_ENGLISH);
    }

    if (strcmp(name, HTTP_README_ZH_URI) == 0) {
        return http_open_readme(file, HTTP_README_CHINESE);
    }

    return 0;
}

void fs_close_custom(struct fs_file *file)
{
    (void)file;
}

/**
 * @brief Initialize lwIP httpd from the TCP/IP thread.
 */
static void http_server_init_callback(void *argument)
{
    (void)argument;

    httpd_init();
    LOG_I("Built-in lwIP HTTP server listening on port %u\r\n", HTTPD_SERVER_PORT);
}

int http_server_init(void)
{
    err_t err;

    err = tcpip_callback(http_server_init_callback, NULL);
    if (err != ERR_OK) {
        LOG_E("Queue lwIP HTTP server initialization failed: %d\r\n", err);
        return -1;
    }

    return 0;
}
