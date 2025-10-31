/*
 * Random number generator
 * Copyright (c) 2010-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef RANDOM_H
#define RANDOM_H

#ifdef CONFIG_NO_RANDOM_POOL
#define random_init(e) do { } while (0)
#define random_deinit() do { } while (0)
#define random_add_randomness(b, l) do { } while (0)
#define random_get_bytes(b, l) os_get_random((b), (l))
#define random_pool_ready() 1
#define random_mark_pool_ready() do { } while (0)
#else /* CONFIG_NO_RANDOM_POOL */
#ifndef CONFIG_RANDOM_OPT
void random_init(const char *entropy_file);
void random_deinit(void);
void random_add_randomness(const void *buf, size_t len);
int random_get_bytes(void *buf, size_t len);
int random_pool_ready(void);
void random_mark_pool_ready(void);
#else
#define random_init(...) do{}while(0)
#define random_deinit(...) do{}while(0)
#define random_add_randomness(buf, len) do{(void)buf; (void)len;}while(0)

static inline int random_get_bytes(void *buf, size_t len)
{
    return os_get_random(buf, len);
}

static inline int random_pool_ready(void)
{
    return 1;
}

#define random_mark_pool_ready(...) do{}while(0)
#endif
#endif /* CONFIG_NO_RANDOM_POOL */

#endif /* RANDOM_H */
