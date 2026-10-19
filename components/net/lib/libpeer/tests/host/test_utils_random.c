/* Exercise production owner RNG with real mbedTLS, including reseeding and
 * failure cleanup. Counting wrappers do not replace successful crypto. */
#include <assert.h>
#include <stdlib.h>
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
static unsigned seed_calls, entropy_calls;
static int fail_seed, fail_entropy, fail_malloc;
static int counted_entropy(void *context, unsigned char *output, size_t size)
{
    ++entropy_calls;
    if (fail_entropy) return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    return mbedtls_entropy_func(context, output, size);
}
static int counted_seed(mbedtls_ctr_drbg_context *context,
                        int (*entropy)(void *, unsigned char *, size_t),
                        void *entropy_context, const unsigned char *custom, size_t size)
{
    ++seed_calls;
    if (fail_seed) return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    return mbedtls_ctr_drbg_seed(context, entropy, entropy_context, custom, size);
}
static void *checked_malloc(size_t size) { return fail_malloc ? NULL : malloc(size); }
#define mbedtls_entropy_func counted_entropy
#define mbedtls_ctr_drbg_seed counted_seed
#define malloc checked_malloc
#include "utils.c"
#undef malloc
#undef mbedtls_ctr_drbg_seed
#undef mbedtls_entropy_func
int main(void)
{
    unsigned char previous[14], current[14];
    char text[33];
    UtilsRandom *a = utils_random_create(), *b = utils_random_create();
    assert(a && b && seed_calls == 2);
    unsigned initial_entropy = entropy_calls;
    assert(utils_random_get_bytes(a, previous, sizeof(previous)) == 0);
    for (unsigned i = 0; i < 1000; ++i) {
        assert(utils_random_get_bytes(a, current, sizeof(current)) == 0);
        assert(memcmp(previous, current, sizeof(current)));
        memcpy(previous, current, sizeof(current));
    }
    assert(seed_calls == 2 && entropy_calls == initial_entropy);
    assert(utils_random_get_string(b, text, 32) == 0 && strlen(text) == 32);
    for (unsigned i = 0; i < 32; ++i)
        assert(strchr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", text[i]));
    mbedtls_ctr_drbg_set_reseed_interval(&a->drbg, 1);
    for (unsigned i = 0; i < 3; ++i)
        assert(utils_random_get_bytes(a, current, sizeof(current)) == 0);
    assert(entropy_calls > initial_entropy);
    fail_entropy = 1;
    mbedtls_ctr_drbg_set_prediction_resistance(&a->drbg, MBEDTLS_CTR_DRBG_PR_ON);
    strcpy(text, "secret");
    assert(utils_random_get_string(a, text, 32) != 0 && text[0] == '\0');
    utils_random_destroy(a); utils_random_destroy(b);
    fail_entropy = 0; fail_seed = 1;
    assert(!utils_random_create());
    fail_seed = 0; fail_malloc = 1;
    assert(!utils_random_create());
    utils_random_destroy(NULL);
    puts("Owner RNG seeded once across 1000 probes, automatic reseeding and fault cleanup passed");
    return 0;
}
