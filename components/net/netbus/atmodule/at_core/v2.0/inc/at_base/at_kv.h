#ifndef AT_KV_H
#define AT_KV_H

#include <stddef.h>
#include <stdint.h>

#define AT_KV_NAME_MAX 15

enum {
    AT_KV_U8 = 1,
    AT_KV_I8,
    AT_KV_U16,
    AT_KV_I16,
    AT_KV_U32,
    AT_KV_I32,
    AT_KV_STRING,
    AT_KV_BINARY,
};

/* Namespace and key names are limited to AT_KV_NAME_MAX bytes.
 * Access must be serialized by the caller. */
int at_kv_init(void);
/* Unmount before raw flash writes or erases. */
int at_kv_deinit(void);
/* Returns value length or a negative error; free(*value) on success. */
int at_kv_get(const char *namespace, const char *key, uint8_t *type, void **value);
/* Decimal text; type must be AT_KV_U8..AT_KV_I32. */
int at_kv_parse_number(int type, const char *text, int64_t *value);
/* type == 0 deletes a key; NULL key clears the namespace. */
int at_kv_set(const char *namespace, const char *key, uint8_t type,
              const void *value, size_t length);
typedef int (*at_kv_foreach_cb_t)(const char *name, uint8_t type, void *arg);
/* NULL namespace enumerates namespaces; otherwise enumerates keys and types. */
int at_kv_foreach(const char *namespace, at_kv_foreach_cb_t callback, void *arg);

#endif
