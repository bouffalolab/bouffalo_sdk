#ifndef GCM_SOFTWARE_BACKEND_H
#define GCM_SOFTWARE_BACKEND_H

#include <stddef.h>

/* Opaque boundary between the public GCM_ALT and the private upstream build. */
size_t gcm_software_backend_context_size(void);
void gcm_software_backend_init(void *ctx);
void gcm_software_backend_free(void *ctx);
int gcm_software_backend_setkey(void *ctx, int cipher,
                                const unsigned char *key, unsigned int bits);
int gcm_software_backend_starts(void *ctx, int mode,
                                const unsigned char *iv, size_t iv_len);
int gcm_software_backend_update_ad(void *ctx, const unsigned char *input, size_t len);
int gcm_software_backend_update(void *ctx, const unsigned char *input, size_t len,
                                unsigned char *output, size_t output_size, size_t *output_len);
int gcm_software_backend_finish(void *ctx, unsigned char *output, size_t output_size,
                                size_t *output_len, unsigned char *tag, size_t tag_len);
int gcm_software_backend_crypt_and_tag(void *ctx, int mode, size_t len,
                                       const unsigned char *iv, size_t iv_len,
                                       const unsigned char *add, size_t add_len,
                                       const unsigned char *input, unsigned char *output,
                                       size_t tag_len, unsigned char *tag);
int gcm_software_backend_auth_decrypt(void *ctx, size_t len,
                                      const unsigned char *iv, size_t iv_len,
                                      const unsigned char *add, size_t add_len,
                                      const unsigned char *tag, size_t tag_len,
                                      const unsigned char *input, unsigned char *output);
#endif
