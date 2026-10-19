/**
 * @file readme_assets.h
 * @brief README documents embedded in the firmware image.
 */

#ifndef README_ASSETS_H
#define README_ASSETS_H

#include <stddef.h>

enum http_readme_language {
    HTTP_README_ENGLISH = 0,
    HTTP_README_CHINESE,
};

/**
 * @brief Get one embedded README document.
 *
 * @param[in] language Requested document language.
 * @param[out] size Embedded document size in bytes.
 * @return Read-only document data, or NULL for an unsupported language.
 */
const unsigned char *http_readme_asset_get(enum http_readme_language language,
                                           size_t *size);

#endif /* README_ASSETS_H */