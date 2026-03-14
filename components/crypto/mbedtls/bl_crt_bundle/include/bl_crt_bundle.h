#ifndef _BL_CRT_BUNDLE_H_
#define _BL_CRT_BUNDLE_H_

#include "mbedtls/ssl.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Attach certificate bundle to SSL configuration
 *
 * This function attaches the default certificate bundle to the SSL config.
 * The bundle is automatically loaded from multi_bins descriptor.
 *
 * @param conf SSL configuration pointer (can be NULL)
 * @return 0 on success, negative on error
 */
int bl_crt_bundle_attach(void *conf);


/**
 * @brief Detach certificate bundle from SSL configuration
 *
 * @param conf SSL configuration pointer (can be NULL)
 */
void bl_crt_bundle_detach(mbedtls_ssl_config *conf);


/**
 * @brief Set custom certificate bundle
 *
 * @param x509_bundle Bundle data pointer
 * @param bundle_size Bundle size in bytes
 * @return 0 on success, negative on error
 */
int bl_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size);


/**
 * @brief Get certificate bundle location from multi_bins descriptor
 *
 * Queries the multi_bins descriptor table to find the CERTS entry
 * and returns its XIP-mapped address range.
 *
 * @param start_addr [out] Pointer to store bundle start address (XIP)
 * @param end_addr   [out] Pointer to store bundle end address (XIP)
 * @return 0 on success, -1 if CERTS descriptor not found
 *
 * Example:
 * @code
 * const uint8_t *start, *end;
 * if (bl_crt_bundle_get_addr(&start, &end) == 0) {
 *     size_t size = end - start;
 *     printf("Certificate bundle: %p, size: %zu bytes\n", start, size);
 * }
 * @endcode
 */
int bl_crt_bundle_get_addr(const uint8_t **start_addr, const uint8_t **end_addr);

/**
 * @brief Check if certificate bundle is available in firmware
 *
 * @return 1 if available, 0 if not found
 */
int bl_crt_bundle_is_available(void);


#ifdef __cplusplus
}
#endif

#endif //_BL_CRT_BUNDLE_H_
