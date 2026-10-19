/**
 * @file readme_assets.c
 * @brief Embed project README files directly in the firmware image.
 */

#include "readme_assets.h"

#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "incbin.h"

INCBIN(http_readme_en, "../README.md");

#ifdef HTTP_README_CHINESE_ENABLED
INCBIN(http_readme_zh, "../README_zh.md");
#else
static const unsigned char s_empty_readme[] = "";
#endif

const unsigned char *http_readme_asset_get(enum http_readme_language language,
                                           size_t *size)
{
    if (size == NULL) {
        return NULL;
    }

    switch (language) {
        case HTTP_README_ENGLISH:
            *size = ghttp_readme_en_size;
            return ghttp_readme_en_data;
        case HTTP_README_CHINESE:
#ifdef HTTP_README_CHINESE_ENABLED
            *size = ghttp_readme_zh_size;
            return ghttp_readme_zh_data;
#else
            *size = 0;
            return s_empty_readme;
#endif
        default:
            *size = 0;
            return NULL;
    }
}