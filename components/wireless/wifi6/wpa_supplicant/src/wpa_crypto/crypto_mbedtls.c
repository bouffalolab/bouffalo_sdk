#include "includes.h"
#include "mbedtls/bignum.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "common.h"
#include "crypto.h"
#include "random.h"

#define HSU_HW_ENABLE 0

#if HSU_HW_ENABLE
#include "hsu.h"
#endif

struct crypto_bignum {
	mbedtls_mpi mpi;
};

#if HSU_HW_ENABLE
/**
 * hsu_or_mbedtls_mpi_exp_mod - Modular exponentiation X = A^E (mod N)
 * @X: Result
 * @A: Input Value
 * @E: Exponent
 * @N: Modulus
 * Returns:  0 on success, -1 on failure
 *
 * Try to use RSA accelerator in HSU to do the computation and fallback to
 * mbedtls software implementation is not available.
 */
static int hsu_or_mbedtls_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A,
				      const mbedtls_mpi *E, const mbedtls_mpi *N)
{
	const mbedtls_mpi *array[3] = { A, E, N };
	int len[3];
	int max_len = 32;

	// Because of previous computation a mpi can have a "big" buffer with 0 on the MSB
	// Finding the actual length may allow to select a smaller RSA computation and then
	// significantly reduce computation time.
	// Since minimum RSA size supported by HSU is 32 bytes, don't try to find actual
	// size lower than this. Also since RSA computation selected depends of the maximum
	// length of all parameters, no need to 'reduce' size lower than other parameters.
	for (int i = 0; i < 3 ; i++) {
		const mbedtls_mpi *mpi = array[i];
		int mpi_size = mpi->n * sizeof(mbedtls_mpi_uint);
		mbedtls_mpi_uint *ptr = &mpi->p[mpi->n - 1];
		while ((mpi_size > max_len) && (*ptr-- == 0))
			mpi_size -= sizeof(mbedtls_mpi_uint);

		len[i] = mpi_size;
		if (mpi_size > max_len) {
			max_len = mpi_size;
			if (max_len <= 128) {
				int mod = max_len % 32;
				if (mod)
					max_len += (32 - mod);
			}
			else if (max_len <= 256)
				max_len = 256;
			else if (max_len <= 512)
				max_len = 512;
			else
				goto mbedtls_mpi_exp_mod;
		}
	}

	if (mbedtls_mpi_grow(X, (max_len / sizeof(mbedtls_mpi_uint))))
		goto mbedtls_mpi_exp_mod;

	if (hsu_mod_exp((uint8_t *)A->p, len[0],
			(uint8_t *)E->p, len[1],
			(uint8_t *)N->p, len[2],
			true, (uint8_t *)X->p, &max_len))
		return 0;

mbedtls_mpi_exp_mod:
	return  mbedtls_mpi_exp_mod(X, A, E, N, NULL);
}
#endif

/**
 * crypto_bignum_init - Allocate memory for bignum
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum *crypto_bignum_init(void)
{
	struct crypto_bignum *n;

	n = os_malloc(sizeof(*n));
	if (!n)
		return NULL;

	mbedtls_mpi_init(&n->mpi);

	return n;
}

/**
 * crypto_bignum_init_set - Allocate memory for bignum and set the value
 * @buf: Buffer with unsigned binary value
 * @len: Length of buf in octets
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum *crypto_bignum_init_set(const u8 *buf, size_t len)
{
	struct crypto_bignum *n = crypto_bignum_init();
	if (!n)
		return NULL;

	if (mbedtls_mpi_read_binary(&n->mpi, buf, len)) {
		crypto_bignum_deinit(n, 0);
		return NULL;
	}

	return n;
}

/**
 * crypto_bignum_init_set - Allocate memory for bignum and set the value (uint)
 * @val: Value to set
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum * crypto_bignum_init_uint(unsigned int val)
{
	struct crypto_bignum *n = crypto_bignum_init();
	if (!n)
		return NULL;

	// use mpi_mul_int as mpi_lset only take singed int as parameter
	if (mbedtls_mpi_lset(&n->mpi, 1) ||
	    mbedtls_mpi_mul_int(&n->mpi, &n->mpi, val)) {
		crypto_bignum_deinit(n, 0);
		return NULL;
	}

	return n;
}

/**
 * crypto_bignum_deinit - Free bignum
 * @n: Bignum from crypto_bignum_init() or crypto_bignum_init_set()
 * @clear: Whether to clear the value from memory
 */
void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
	// mbedtls always clear the memory
	mbedtls_mpi_free(&n->mpi);
	os_free(n);
}

/**
 * crypto_bignum_to_bin - Set binary buffer to unsigned bignum
 * @a: Bignum
 * @buf: Buffer for the binary number
 * @len: Length of @buf in octets
 * @padlen: Length in octets to pad the result to or 0 to indicate no padding
 * Returns: Number of octets written on success, -1 on failure
 */
int crypto_bignum_to_bin(const struct crypto_bignum *a,
			 u8 *buf, size_t buflen, size_t padlen)
{
	int res = buflen;
	size_t len = mbedtls_mpi_size(&a->mpi);

	if (len > buflen)
		return -1;

	// mbedtls will always do padding to the size of the buffer.
	if (padlen <= len)
		res = buflen = len;
	else
		res = buflen = padlen;

	if (mbedtls_mpi_write_binary(&a->mpi, buf, buflen))
		return -1;

	return res;
}

/**
 * crypto_bignum_rand - Create a random number in range of modulus
 * @r: Bignum; set to a random value
 * @m: Bignum; modulus
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_rand(struct crypto_bignum *r, const struct crypto_bignum *m)
{
	int size = mbedtls_mpi_size(&m->mpi) + 1;
	void *buf = os_malloc(size);
	int ret = -1;

	if (!buf)
		return -1;

	// As a first step takes the easy option, probably need something more
	// complete using mbedtls_mpi_fill_random
	if (random_get_bytes(buf, size))
		goto end;

	if (mbedtls_mpi_read_binary(&r->mpi, buf, size))
		goto end;

	if (mbedtls_mpi_mod_mpi(&r->mpi, &r->mpi, &m->mpi))
		goto end;

	ret = 0;
end:
	os_free(buf);
	return ret;
}

/**
 * crypto_bignum_add - c = a + b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a + b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_add(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	if (mbedtls_mpi_add_mpi(&c->mpi, &a->mpi, &b->mpi))
		return -1;

	return 0;
}

/**
 * crypto_bignum_mod - c = a % b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a % b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_mod(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	if (mbedtls_mpi_mod_mpi(&c->mpi, &a->mpi, &b->mpi))
		return -1;
	return 0;
}

/**
 * crypto_bignum_exptmod - Modular exponentiation: d = a^b (mod c)
 * @a: Bignum; base
 * @b: Bignum; exponent
 * @c: Bignum; modulus
 * @d: Bignum; used to store the result of a^b (mod c)
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_exptmod(const struct crypto_bignum *a,
			  const struct crypto_bignum *b,
			  const struct crypto_bignum *c,
			  struct crypto_bignum *d)
{
#if HSU_HW_ENABLE
	// Only a odd modulus supported but should be ok as it is always called
	// with prime number as modulus
	if (hsu_or_mbedtls_mpi_exp_mod(&d->mpi, &a->mpi, &b->mpi, &c->mpi))
		return -1;
	return 0;
#else
    return  mbedtls_mpi_exp_mod((mbedtls_mpi *) d, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b, (const mbedtls_mpi *) c, NULL) ? -1 : 0;
#endif
}

/**
 * crypto_bignum_inverse - Inverse a bignum so that a * c = 1 (mod b)
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_inverse(const struct crypto_bignum *a,
			  const struct crypto_bignum *b,
			  struct crypto_bignum *c)
{
	if (mbedtls_mpi_inv_mod(&c->mpi, &a->mpi, &b->mpi))
		return -1;
	return 0;
}

/**
 * crypto_bignum_sub - c = a - b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a - b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_sub(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	if (mbedtls_mpi_sub_mpi(&c->mpi, &a->mpi, &b->mpi))
		return -1;

	return 0;
}

/**
 * crypto_bignum_div - c = a / b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a / b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_div(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	if (mbedtls_mpi_div_mpi(&c->mpi, NULL, &a->mpi, &b->mpi))
		return -1;

	return 0;
}

/**
 * crypto_bignum_addmod - d = a + b (mod c)
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum
 * @d: Bignum; used to store the result of (a + b) % c
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_addmod(const struct crypto_bignum *a,
			 const struct crypto_bignum *b,
			 const struct crypto_bignum *c,
			 struct crypto_bignum *d)
{
	if (mbedtls_mpi_add_mpi(&d->mpi, &a->mpi, &b->mpi))
		return -1;

	if (mbedtls_mpi_mod_mpi(&d->mpi, &d->mpi, &c->mpi))
		return -1;

	return 0;
}

/**
 * crypto_bignum_mulmod - d = a * b (mod c)
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum
 * @d: Bignum; used to store the result of (a * b) % c
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_mulmod(const struct crypto_bignum *a,
			 const struct crypto_bignum *b,
			 const struct crypto_bignum *c,
			 struct crypto_bignum *d)
{
	if (mbedtls_mpi_mul_mpi(&d->mpi, &a->mpi, &b->mpi))
		return -1;

	if (mbedtls_mpi_mod_mpi(&d->mpi, &d->mpi, &c->mpi))
		return -1;

	return 0;
}

/**
 * crypto_bignum_sqrmod - c = a^2 (mod b)
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a^2 % b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_sqrmod(const struct crypto_bignum *a,
			 const struct crypto_bignum *b,
			 struct crypto_bignum *c)
{
	if (mbedtls_mpi_mul_mpi(&c->mpi, &a->mpi, &a->mpi))
		return -1;

	if (mbedtls_mpi_mod_mpi(&c->mpi, &c->mpi, &b->mpi))
		return -1;

	return 0;
}

/**
 * crypto_bignum_rshift - r = a >> n
 * @a: Bignum
 * @n: Number of bits
 * @r: Bignum; used to store the result of a >> n
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_rshift(const struct crypto_bignum *a, int n,
			 struct crypto_bignum *r)
{
	if ((a != r) &&
	    mbedtls_mpi_copy(&r->mpi, &a->mpi))
		return -1 ;

	if (mbedtls_mpi_shift_r(&r->mpi, n))
		return -1;

	return 0;
}

/**
 * crypto_bignum_cmp - Compare two bignums
 * @a: Bignum
 * @b: Bignum
 * Returns: -1 if a < b, 0 if a == b, or 1 if a > b
 */
int crypto_bignum_cmp(const struct crypto_bignum *a,
		      const struct crypto_bignum *b)
{
	return mbedtls_mpi_cmp_mpi(&a->mpi, &b->mpi);
}

/**
 * crypto_bignum_bits - Get size of a bignum in bits
 * @a: Bignum
 * Returns: Number of bits in the bignum
 */
int crypto_bignum_bits(const struct crypto_bignum *a)
{
	return mbedtls_mpi_bitlen(&a->mpi);
}

/**
 * crypto_bignum_is_zero - Is the given bignum zero
 * @a: Bignum
 * Returns: 1 if @a is zero or 0 if not
 */
int crypto_bignum_is_zero(const struct crypto_bignum *a)
{
	if (mbedtls_mpi_cmp_int(&a->mpi, 0))
		return 0;
	return 1;
}

/**
 * crypto_bignum_is_one - Is the given bignum one
 * @a: Bignum
 * Returns: 1 if @a is one or 0 if not
 */
int crypto_bignum_is_one(const struct crypto_bignum *a)
{
	if (mbedtls_mpi_cmp_int(&a->mpi, 1))
		return 0;
	return 1;
}

/**
 * crypto_bignum_is_odd - Is the given bignum odd
 * @a: Bignum
 * Returns: 1 if @a is odd or 0 if not
 */
int crypto_bignum_is_odd(const struct crypto_bignum *a)
{
#ifdef CONFIG_MBEDTLS_V3
    return (mbedtls_mpi_get_bit((const mbedtls_mpi *) a, 0) == 1);
#else
	if (a->mpi.p)
		return (a->mpi.p[0] & 0x1);
	return 0;
#endif
}

/**
 * crypto_bignum_legendre - Compute the Legendre symbol (a/p)
 * @a: Bignum
 * @p: Bignum
 * Returns: Legendre symbol -1,0,1 on success; -2 on calculation failure
 */
int crypto_bignum_legendre(const struct crypto_bignum *a,
			   const struct crypto_bignum *p)
{
#if HSU_HW_ENABLE
	mbedtls_mpi exp, tmp;
	int res = -2;

	mbedtls_mpi_init(&exp);
	mbedtls_mpi_init(&tmp);

	// exp = (p-1) / 2
	if (mbedtls_mpi_sub_int(&exp, &p->mpi, 1) ||
	    mbedtls_mpi_shift_r(&exp, 1))
		goto end;

	if (hsu_or_mbedtls_mpi_exp_mod(&tmp, &a->mpi, &exp, &p->mpi))
		goto end;

	if (mbedtls_mpi_cmp_int(&tmp, 1) == 0)
		res = 1;
	else if (mbedtls_mpi_cmp_int(&tmp, 0) == 0)
		res = 0;
	else
		res = -1;

end:
	mbedtls_mpi_free(&exp);
	mbedtls_mpi_free(&tmp);
	return res;
#else
    mbedtls_mpi exp, tmp;
    int res = -2, ret;

    mbedtls_mpi_init(&exp);
    mbedtls_mpi_init(&tmp);

    /* exp = (p-1) / 2 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&exp, (const mbedtls_mpi *) p, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&exp, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&tmp, (const mbedtls_mpi *) a, &exp, (const mbedtls_mpi *) p, NULL));

    if (mbedtls_mpi_cmp_int(&tmp, 1) == 0) {
        res = 1;
    } else if (mbedtls_mpi_cmp_int(&tmp, 0) == 0
            /* The below check is workaround for the case where HW
             * does not behave properly for X ^ A mod M when X is
             * power of M. Instead of returning value 0, value M is
             * returned.*/
            || mbedtls_mpi_cmp_mpi(&tmp, (const mbedtls_mpi *)p) == 0) {
        res = 0;
    } else {
        res = -1;
    }

cleanup:
    mbedtls_mpi_free(&tmp);
    mbedtls_mpi_free(&exp);
    return res;
#endif
}

#if HSU_HW_ENABLE
/**
 * struct crypto_ec - Elliptic curve context
 *
 * Internal data structure for EC implementation. The contents is specific
 * to the used crypto library.
 */
struct crypto_ec {
	mbedtls_ecp_group group;
	enum hsu_elliptic_curve hsu_curve;  // curve ID in HSU
	int prime_limb; // Size, in mbedtls limb, of the group prime
};

/**
 * crypto_ec_init - Initialize elliptic curve context
 * @group: Identifying number for the ECC group (IANA "Group Description"
 *	attribute registrty for RFC 2409)
 * Returns: Pointer to EC context or %NULL on failure
 */
struct crypto_ec *crypto_ec_init(int group)
{
	struct crypto_ec *ec;
	mbedtls_ecp_group_id grp_id;

	/* Map from IANA registry for IKE D-H groups to Mbed TLS group ID */
	switch (group) {
	case 19:
		grp_id = MBEDTLS_ECP_DP_SECP256R1;
		break;
	case 20:
		grp_id = MBEDTLS_ECP_DP_SECP384R1;
		break;
	case 21:
		grp_id = MBEDTLS_ECP_DP_SECP521R1;
		break;
	case 25:
		grp_id = MBEDTLS_ECP_DP_SECP192R1;
		break;
	case 26:
		// mbedtls support this curve (MBEDTLS_ECP_DP_SECP224R1) but since the prime
		// of this curve is not 3 congruent module 4 the square root algo used in
		// crypto_ec_point_solve_y_coord is not correct.
		return NULL;
	case 28:
		grp_id = MBEDTLS_ECP_DP_BP256R1;
		break;
	case 29:
		grp_id = MBEDTLS_ECP_DP_BP384R1;
		break;
	case 30:
		grp_id = MBEDTLS_ECP_DP_BP512R1;
		break;
	default:
		return NULL;
	}

	ec = os_malloc(sizeof(*ec));
	if (!ec)
		return NULL;

	mbedtls_ecp_group_init(&ec->group);
	if (mbedtls_ecp_group_load(&ec->group, grp_id)) {
		crypto_ec_deinit(ec);
		return NULL;
	}


	switch (group) {
	case 19:
		ec->hsu_curve = HSU_EC_256;
		ec->prime_limb = (32 + sizeof(mbedtls_mpi_uint) - 1) / sizeof(mbedtls_mpi_uint);
		break;
	case 20:
		ec->hsu_curve = HSU_EC_384;
		ec->prime_limb = (48 + sizeof(mbedtls_mpi_uint) - 1) / sizeof(mbedtls_mpi_uint);
		break;
	case 21:
		ec->hsu_curve = HSU_EC_521;
		ec->prime_limb = (66 + sizeof(mbedtls_mpi_uint) - 1) / sizeof(mbedtls_mpi_uint);
		break;
	default:
		ec->hsu_curve = HSU_EC_MAX;
		break;
	}

	return ec;
}

/**
 * crypto_ec_deinit - Deinitialize elliptic curve context
 * @e: EC context from crypto_ec_init()
 */
void crypto_ec_deinit(struct crypto_ec *e)
{
	mbedtls_ecp_group_free(&e->group);
	os_free(e);
}

/**
 * crypto_ec_prime_len - Get length of the prime in octets
 * @e: EC context from crypto_ec_init()
 * Returns: Length of the prime defining the group
 */
size_t crypto_ec_prime_len(struct crypto_ec *e)
{
	return mbedtls_mpi_size(&e->group.P);
}

/**
 * crypto_ec_prime_len_bits - Get length of the prime in bits
 * @e: EC context from crypto_ec_init()
 * Returns: Length of the prime defining the group in bits
 */
size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
	return mbedtls_mpi_bitlen(&e->group.P);
}

/**
 * crypto_ec_order_len - Get length of the order in octets
 * @e: EC context from crypto_ec_init()
 * Returns: Length of the order defining the group
 */
size_t crypto_ec_order_len(struct crypto_ec *e)
{
	return mbedtls_mpi_size(&e->group.N);
}

/**
 * crypto_ec_get_prime - Get prime defining an EC group
 * @e: EC context from crypto_ec_init()
 * Returns: Prime (bignum) defining the group
 */
const struct crypto_bignum * crypto_ec_get_prime(struct crypto_ec *e)
{
	return (const struct crypto_bignum *)&e->group.P;
}

/**
 * crypto_ec_get_order - Get order of an EC group
 * @e: EC context from crypto_ec_init()
 * Returns: Order (bignum) of the group
 */
const struct crypto_bignum * crypto_ec_get_order(struct crypto_ec *e)
{
	return (const struct crypto_bignum *)&e->group.N;
}

/**
 * crypto_ec_get_a - Get 'a' value of an EC curve
 * @e: EC context from crypto_ec_init()
 * Returns: a (bignum) of the group
 */
const struct crypto_bignum * crypto_ec_get_a(struct crypto_ec *e)
{
	return (const struct crypto_bignum *)&e->group.A;
}

/**
 * crypto_ec_get_b - Get 'b' value of an EC curve
 * @e: EC context from crypto_ec_init()
 * Returns: b (bignum) of the group
 */
const struct crypto_bignum * crypto_ec_get_b(struct crypto_ec *e)
{
	return (const struct crypto_bignum *)&e->group.B;
}

/**
 * struct crypto_ec_point - Elliptic curve point
 *
 * Internal data structure for EC implementation to represent a point. The
 * contents is specific to the used crypto library.
 */
struct crypto_ec_point {
	mbedtls_ecp_point point;
};

/**
 * crypto_ec_point_init - Initialize data for an EC point
 * @e: EC context from crypto_ec_init()
 * Returns: Pointer to EC point data or %NULL on failure
 */
struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e)
{
	struct crypto_ec_point *ecp;
	ecp = os_malloc(sizeof(*ecp));
	if (!ecp)
		return NULL;

	mbedtls_ecp_point_init(&ecp->point);
	return ecp;
}

/**
 * crypto_ec_point_deinit - Deinitialize EC point data
 * @p: EC point data from crypto_ec_point_init()
 * @clear: Whether to clear the EC point value from memory
 */
void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
	// always clear memory
	mbedtls_ecp_point_free(&p->point);
	os_free(p);
}

/**
 * crypto_ec_point_x - Copies the x-ordinate point into big number
 * @e: EC context from crypto_ec_init()
 * @p: EC point data
 * @x: Big number to set to the copy of x-ordinate
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_x(struct crypto_ec *e, const struct crypto_ec_point *p,
		      struct crypto_bignum *x)
{
	if (mbedtls_mpi_copy(&x->mpi, &p->point.X))
		return -1;
	return 0;
}

/**
 * crypto_ec_point_to_bin - Write EC point value as binary data
 * @e: EC context from crypto_ec_init()
 * @p: EC point data from crypto_ec_point_init()
 * @x: Buffer for writing the binary data for x coordinate or %NULL if not used
 * @y: Buffer for writing the binary data for y coordinate or %NULL if not used
 * Returns: 0 on success, -1 on failure
 *
 * This function can be used to write an EC point as binary data in a format
 * that has the x and y coordinates in big endian byte order fields padded to
 * the length of the prime defining the group.
 */
int crypto_ec_point_to_bin(struct crypto_ec *e,
			   const struct crypto_ec_point *p, u8 *x, u8 *y)
{
	size_t p_len = crypto_ec_prime_len(e);
	size_t tmp_len = 2 * p_len + 1;
	size_t o_len;
	u8 *tmp;
	int res = -1;

	tmp = os_malloc(tmp_len);
	if (!tmp)
		return -1;

	if (mbedtls_ecp_point_write_binary(&e->group, &p->point,
					   MBEDTLS_ECP_PF_UNCOMPRESSED, &o_len,
					   tmp, tmp_len))
		goto end;

	if (o_len == tmp_len) {
		if (x)
			memcpy(x, &tmp[1], p_len);
		if (y)
			memcpy(y, &tmp[1 + p_len], p_len);

		res = 0;
	}

end:
	os_free(tmp);
	return res;
}

/**
 * crypto_ec_point_from_bin - Create EC point from binary data
 * @e: EC context from crypto_ec_init()
 * @val: Binary data to read the EC point from
 * Returns: Pointer to EC point data or %NULL on failure
 *
 * This function readers x and y coordinates of the EC point from the provided
 * buffer assuming the values are in big endian byte order with fields padded to
 * the length of the prime defining the group.
 */
struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e,
						  const u8 *val)
{
	size_t p_len = crypto_ec_prime_len(e);
	size_t tmp_len = 2 * p_len + 1;
	struct crypto_ec_point *ecp;
	u8 *tmp;

	ecp = crypto_ec_point_init(e);
	if (!ecp)
		return NULL;

	tmp = os_malloc(tmp_len);
	if (!tmp) {
		crypto_ec_point_deinit(ecp, 1);
		return NULL;
	}

	tmp[0] = 0x4; // UNCOMPRESSED
	memcpy(&tmp[1], val, 2 * p_len);

	if (mbedtls_ecp_point_read_binary(&e->group, &ecp->point, tmp, tmp_len)) {
		crypto_ec_point_deinit(ecp, 1);
		ecp = NULL;
	}

	os_free(tmp);
	return ecp;
}

/**
 * crypto_bignum_add - c = a + b
 * @e: EC context from crypto_ec_init()
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a + b
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
			const struct crypto_ec_point *b,
			struct crypto_ec_point *c)
{
	mbedtls_mpi one;
	int ret = -1;
	mbedtls_mpi_init(&one);

	if (mbedtls_mpi_lset(&one, 1))
		goto end;

	if (mbedtls_ecp_muladd(&e->group, &c->point, &one, &a->point,
			       &one, &b->point))
		goto end;

	ret = 0;

end:
	mbedtls_mpi_free(&one);
	return ret;
}

/**
 * crypto_bignum_mul - res = b * p
 * @e: EC context from crypto_ec_init()
 * @p: EC point
 * @b: Bignum
 * @res: EC point; used to store the result of b * p
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
			const struct crypto_bignum *b,
			struct crypto_ec_point *res)
{
	if (e->hsu_curve < HSU_EC_MAX) {
		const uint32_t *P[2] = { p->point.X.p, p->point.Y.p };
		uint32_t *R[2];
		size_t P_len[2];

		if (mbedtls_mpi_grow(&res->point.X, e->prime_limb) ||
		    mbedtls_mpi_grow(&res->point.Y, e->prime_limb) ||
		    mbedtls_mpi_lset(&res->point.Z, 1))
			goto mbedtls_ecp_mul;

		P_len[0] = p->point.X.n * sizeof(mbedtls_mpi_uint) / sizeof(uint32_t);
		P_len[1] = p->point.Y.n * sizeof(mbedtls_mpi_uint) / sizeof(uint32_t);

		R[0] = res->point.X.p;
		R[1] = res->point.Y.p;

		if (hsu_ecp_mul(e->hsu_curve, (uint32_t *)b->mpi.p,
				b->mpi.n * sizeof(mbedtls_mpi_uint) / sizeof(uint32_t),
				P, P_len, R))
			return 0;
	}

mbedtls_ecp_mul:
	if (mbedtls_ecp_mul(&e->group, &res->point, &b->mpi, &p->point,
			    NULL, NULL))
		return -1;

	return 0;
}

/**
 * crypto_ec_point_invert - Compute inverse of an EC point
 * @e: EC context from crypto_ec_init()
 * @p: EC point to invert (and result of the operation)
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
	mbedtls_mpi one;
	mbedtls_mpi minus_one;
	mbedtls_ecp_point zero;
	int ret = -1;

	mbedtls_mpi_init(&one);
	mbedtls_mpi_init(&minus_one);
	mbedtls_ecp_point_init(&zero);

	if (mbedtls_mpi_lset(&one, 1) ||
	    mbedtls_mpi_lset(&minus_one, -1) ||
	    mbedtls_ecp_set_zero(&zero))
		goto end;

	if (mbedtls_ecp_muladd(&e->group, &p->point, &one, &zero,
			       &minus_one, &p->point))
		goto end;

	ret = 0;

end:
	mbedtls_mpi_free(&one);
	mbedtls_mpi_free(&minus_one);
	mbedtls_ecp_point_free(&zero);
	return ret;
}

/**
 * crypto_ec_point_solve_y_coord - Solve y coordinate for an x coordinate
 * @e: EC context from crypto_ec_init()
 * @p: EC point to use for the returning the result
 * @x: x coordinate
 * @y_bit: y-bit (0 or 1) for selecting the y value to use
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
				  struct crypto_ec_point *p,
				  const struct crypto_bignum *x, int y_bit)
{
	struct crypto_bignum *y_sqr;
	mbedtls_mpi exp;
	int ret = -1;

	mbedtls_mpi_init(&exp);

	y_sqr = crypto_ec_point_compute_y_sqr(e, x);
	if (!y_sqr)
		return -1;

	// If p = 3 % 4 (which is the case for all curves except the one for group 26)
	// then y = (y_sqr)^((p + 1) / 4)

	// exp = (p + 1) / 4
	if (mbedtls_mpi_add_int(&exp, &e->group.P, 1) ||
	    mbedtls_mpi_shift_r(&exp, 2))
		goto end;

	if (hsu_or_mbedtls_mpi_exp_mod(&p->point.Y, &y_sqr->mpi, &exp, &e->group.P))
		goto end;

	if (((p->point.Y.p[0] & 0x1) != y_bit) &&
	    mbedtls_mpi_sub_mpi(&p->point.Y, &e->group.P, &p->point.Y))
		goto end;

	if (mbedtls_mpi_copy(&p->point.X, &x->mpi) ||
	    mbedtls_mpi_lset(&p->point.Z, 1))
	    goto end;

	ret = 0;
end:
	mbedtls_mpi_free(&exp);
	crypto_bignum_deinit(y_sqr, 1);
	return ret;
}

/**
 * crypto_ec_point_compute_y_sqr - Compute y^2 = x^3 + ax + b
 * @e: EC context from crypto_ec_init()
 * @x: x coordinate
 * Returns: y^2 on success, %NULL failure
 */
struct crypto_bignum *
crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
			      const struct crypto_bignum *x)
{
	struct crypto_bignum *y_sqr = crypto_bignum_init();

	if (!y_sqr)
		return NULL;

	// x^2 (% p)
	if (mbedtls_mpi_mul_mpi(&y_sqr->mpi, &x->mpi, &x->mpi) ||
	    mbedtls_mpi_mod_mpi(&y_sqr->mpi, &y_sqr->mpi, &e->group.P))
		goto error;

	// (X^2) + a (%p)
	if (e->group.A.p == NULL) {
		// For optimizations mbedtls doesn't store 'a' when it is -3 ...
		if (mbedtls_mpi_sub_int(&y_sqr->mpi, &y_sqr->mpi, 3) ||
		    ((mbedtls_mpi_cmp_int(&y_sqr->mpi, 0) < 0) &&
		     mbedtls_mpi_add_mpi(&y_sqr->mpi, &y_sqr->mpi, &e->group.P)))
			goto error;
	} else if (mbedtls_mpi_add_mpi(&y_sqr->mpi, &y_sqr->mpi, &e->group.A) ||
		   ((mbedtls_mpi_cmp_mpi(&y_sqr->mpi, &e->group.P) >= 0) &&
		    mbedtls_mpi_sub_abs(&y_sqr->mpi, &y_sqr->mpi, &e->group.P)))
		goto error;

	// (x^2 + a) * x (% p)
	if (mbedtls_mpi_mul_mpi(&y_sqr->mpi, &y_sqr->mpi, &x->mpi) ||
	    mbedtls_mpi_mod_mpi(&y_sqr->mpi, &y_sqr->mpi, &e->group.P))
		goto error;

	// ((x^2 + a) * x) + b (%p)
	if (mbedtls_mpi_add_mpi(&y_sqr->mpi, &y_sqr->mpi, &e->group.B) ||
	    ((mbedtls_mpi_cmp_mpi(&y_sqr->mpi, &e->group.P) >= 0) &&
	     mbedtls_mpi_sub_abs(&y_sqr->mpi, &y_sqr->mpi, &e->group.P)))
		goto error;

	return y_sqr;

error:
	crypto_bignum_deinit(y_sqr, 1);
	return NULL;
}

/**
 * crypto_ec_point_is_at_infinity - Check whether EC point is neutral element
 * @e: EC context from crypto_ec_init()
 * @p: EC point
 * Returns: 1 if the specified EC point is the neutral element of the group or
 *	0 if not
 */
int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
				   const struct crypto_ec_point *p)
{
	return mbedtls_ecp_is_zero((mbedtls_ecp_point *)&p->point);
}

/**
 * crypto_ec_point_is_on_curve - Check whether EC point is on curve
 * @e: EC context from crypto_ec_init()
 * @p: EC point
 * Returns: 1 if the specified EC point is on the curve or 0 if not
 */
int crypto_ec_point_is_on_curve(struct crypto_ec *e,
				const struct crypto_ec_point *p)
{
	if (mbedtls_ecp_check_pubkey(&e->group, &p->point))
		return 0;
	return 1;
}

/**
 * crypto_ec_point_cmp - Compare two EC points
 * @e: EC context from crypto_ec_init()
 * @a: EC point
 * @b: EC point
 * Returns: 0 on equal, non-zero otherwise
 */
int crypto_ec_point_cmp(const struct crypto_ec *e,
			const struct crypto_ec_point *a,
			const struct crypto_ec_point *b)
{
	return mbedtls_ecp_point_cmp(&a->point, &b->point);
}

#else
struct crypto_ec {
    mbedtls_ecp_group group;
};

struct crypto_ec *crypto_ec_init(int group)
{
    struct crypto_ec *e;

    mbedtls_ecp_group_id  grp_id;

    /* IANA registry to mbedtls internal mapping*/
    switch (group) {
    case 19:
        /* For now just support NIST-P256.
         * This is of type "short Weierstrass".
         */
        grp_id = MBEDTLS_ECP_DP_SECP256R1;
        break;
    default:
        return NULL;

    }
    e = os_zalloc(sizeof(*e));
    if (e == NULL) {
        return NULL;
    }

    mbedtls_ecp_group_init(&e->group);

    if (mbedtls_ecp_group_load(&e->group, grp_id)) {
        crypto_ec_deinit(e);
        e = NULL;
    }
    if (mbedtls_mpi_sub_int(&e->group.A, &e->group.P, 3)) {
        crypto_ec_deinit(e);
        e = NULL;
    }

    return e;
}


void crypto_ec_deinit(struct crypto_ec *e)
{
    if (e == NULL) {
        return;
    }

    mbedtls_mpi_free(&e->group.A);
    mbedtls_ecp_group_free(&e->group);
    os_free(e);
}


struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e)
{
    mbedtls_ecp_point *pt;
    if (e == NULL) {
        return NULL;
    }

    pt = os_zalloc(sizeof(mbedtls_ecp_point));

    if( pt == NULL) {
        return NULL;
    }

    mbedtls_ecp_point_init(pt);

    return (struct crypto_ec_point *) pt;
}


size_t crypto_ec_prime_len(struct crypto_ec *e)
{
    return mbedtls_mpi_size(&e->group.P);
}


size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
    return mbedtls_mpi_bitlen(&e->group.P);
}


const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) &e->group.P;
}


const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) &e->group.N;
}


void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
    mbedtls_ecp_point_free((mbedtls_ecp_point *) p);
    os_free(p);
}


int crypto_ec_point_to_bin(struct crypto_ec *e,
                           const struct crypto_ec_point *point, u8 *x, u8 *y)
{
    int len = mbedtls_mpi_size(&e->group.P);

    if (x) {
#ifdef CONFIG_MBEDTLS_V3
        if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->MBEDTLS_PRIVATE(X),
                             x, len, len) < 0) {
            return -1;
        }
#else
        if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->X,
                             x, len, len) < 0) {
            return -1;
        }
#endif

    }

    if (y) {
#ifdef CONFIG_MBEDTLS_V3
        if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->MBEDTLS_PRIVATE(Y),
                             y, len, len) < 0) {
            return -1;
        }
#else
        if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->Y,
                             y, len, len) < 0) {
            return -1;
        }
#endif
    }

    return 0;
}


struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e,
        const u8 *val)
{
    mbedtls_ecp_point *pt;
    int len, ret;

    if (e == NULL) {
        return NULL;
    }

    len = mbedtls_mpi_size(&e->group.P);

    pt = os_zalloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point_init(pt);

#ifdef CONFIG_MBEDTLS_V3
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->MBEDTLS_PRIVATE(X), val, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->MBEDTLS_PRIVATE(Y), val + len, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset((&pt->MBEDTLS_PRIVATE(Z)), 1));
#else
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->X, val, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->Y, val + len, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset((&pt->Z), 1));
#endif

    return (struct crypto_ec_point *) pt;

cleanup:
    mbedtls_ecp_point_free(pt);
    os_free(pt);
    return NULL;
}


int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b,
                        struct crypto_ec_point *c)
{
    int ret;
    mbedtls_mpi one;

    mbedtls_mpi_init(&one);

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &one, 1 ));
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(&e->group, (mbedtls_ecp_point *) c, &one, (const mbedtls_ecp_point *)a , &one, (const mbedtls_ecp_point *)b));

cleanup:
    mbedtls_mpi_free(&one);
    return ret ? -1 : 0;
}


int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
                        const struct crypto_bignum *b,
                        struct crypto_ec_point *res)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    MBEDTLS_MPI_CHK(mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0));

    MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&e->group,
                (mbedtls_ecp_point *) res,
                (const mbedtls_mpi *)b,
                (const mbedtls_ecp_point *)p,
                mbedtls_ctr_drbg_random,
                &ctr_drbg));
cleanup:
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    return ret ? -1 : 0;
}


/*  Currently mbedtls does not have any function for inverse
 *  This function calculates inverse of a point.
 *  Set R = -P
 */
static int ecp_opp( const mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_ecp_point *P)
{
    int ret = 0;

    /* Copy */
    if (R != P) {
        MBEDTLS_MPI_CHK(mbedtls_ecp_copy(R, P));
    }

    /* In-place opposite */
#ifdef CONFIG_MBEDTLS_V3
    if (mbedtls_mpi_cmp_int( &R->MBEDTLS_PRIVATE(Y), 0) != 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&R->MBEDTLS_PRIVATE(Y), &grp->P, &R->MBEDTLS_PRIVATE(Y)));
    }
#else
    if (mbedtls_mpi_cmp_int( &R->Y, 0) != 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&R->Y, &grp->P, &R->Y));
    }
#endif

cleanup:
    return ( ret );
}

int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
    return ecp_opp(&e->group, (mbedtls_ecp_point *) p, (mbedtls_ecp_point *) p) ? -1 : 0;
}

int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
                                  struct crypto_ec_point *p,
                                  const struct crypto_bignum *x, int y_bit)
{
    mbedtls_mpi temp;
    mbedtls_mpi *y_sqr, *y;
    mbedtls_mpi_init(&temp);
    int ret = 0;

#ifdef CONFIG_MBEDTLS_V3
    y = &((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y);
#else
    y = &((mbedtls_ecp_point *)p)->Y;
#endif

    /* Faster way to find sqrt
     * Works only with curves having prime p
     * such that p ≡ 3 (mod 4)
     *  y_ = (y2 ^ ((p+1)/4)) mod p
     *
     *  if LSB of both x and y are same: y = y_
     *   else y = p - y_
     * y_bit is LSB of x
     */
    y_bit = (y_bit != 0);

    y_sqr = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, x);

    if (y_sqr) {

        MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&temp, &e->group.P, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_div_int(&temp, NULL, &temp, 4));
        MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(y, y_sqr, &temp, &e->group.P, NULL));

        if (y_bit != mbedtls_mpi_get_bit(y, 0))
            MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(y, &e->group.P, y));

#ifdef CONFIG_MBEDTLS_V3
        MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&((mbedtls_ecp_point* )p)->MBEDTLS_PRIVATE(X), (const mbedtls_mpi*) x));
        MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Z), 1));
#else
        MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&((mbedtls_ecp_point* )p)->X, (const mbedtls_mpi*) x));
        MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&((mbedtls_ecp_point *)p)->Z, 1));
#endif
    } else {
        ret = 1;
    }
cleanup:
    mbedtls_mpi_free(&temp);
    mbedtls_mpi_free(y_sqr);
    os_free(y_sqr);
    return ret ? -1 : 0;
}

struct crypto_bignum *
crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
                              const struct crypto_bignum *x)
{
    mbedtls_mpi temp, temp2, num;
    int ret = 0;

    mbedtls_mpi *y_sqr = os_zalloc(sizeof(mbedtls_mpi));
    if (y_sqr == NULL) {
        return NULL;
    }

    mbedtls_mpi_init(&temp);
    mbedtls_mpi_init(&temp2);
    mbedtls_mpi_init(&num);
    mbedtls_mpi_init(y_sqr);

    /* y^2 = x^3 + ax + b  mod  P*/
    /* mbedtls does not have mod-add or mod-mul apis.
     *
     */

    /* Calculate x^3  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &num, 3));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&temp, (const mbedtls_mpi *) x, &num, &e->group.P, NULL));

    /* Calculate ax  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &num, -3));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&temp2, (const mbedtls_mpi *) x, &num));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &e->group.P));

    /* Calculate ax + b  mod P. Note that b is already < P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &e->group.B));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &e->group.P));

    /* Calculate x^3 + ax + b  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &temp));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(y_sqr, &temp2, &e->group.P));


cleanup:
    mbedtls_mpi_free(&temp);
    mbedtls_mpi_free(&temp2);
    mbedtls_mpi_free(&num);
    if (ret) {
        mbedtls_mpi_free(y_sqr);
        os_free(y_sqr);
        return NULL;
    } else {
        return (struct crypto_bignum *) y_sqr;
    }
}



int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
                                   const struct crypto_ec_point *p)
{
    return mbedtls_ecp_is_zero((mbedtls_ecp_point *) p);
}

int crypto_ec_point_is_on_curve(struct crypto_ec *e,
                                const struct crypto_ec_point *p)
{
    mbedtls_mpi y_sqr_lhs, *y_sqr_rhs = NULL, two;
    int ret = 0, on_curve = 0;

    mbedtls_mpi_init(&y_sqr_lhs);
    mbedtls_mpi_init(&two);

    /* Calculate y^2  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &two, 2));
#ifdef CONFIG_MBEDTLS_V3
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_sqr_lhs, &((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y) , &two, &e->group.P, NULL));

    y_sqr_rhs = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, (const struct crypto_bignum *) & ((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X));
#else
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_sqr_lhs, &((const mbedtls_ecp_point *)p)->Y , &two, &e->group.P, NULL));

    y_sqr_rhs = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, (const struct crypto_bignum *) & ((const mbedtls_ecp_point *)p)->X);
#endif

    if (y_sqr_rhs && (mbedtls_mpi_cmp_mpi(y_sqr_rhs, &y_sqr_lhs) == 0)) {
        on_curve = 1;
    }

cleanup:
    mbedtls_mpi_free(&y_sqr_lhs);
    mbedtls_mpi_free(&two);
    mbedtls_mpi_free(y_sqr_rhs);
    os_free(y_sqr_rhs);
    return (ret == 0) && (on_curve == 1);
}

int crypto_ec_point_cmp(const struct crypto_ec *e,
                        const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b)
{
    return mbedtls_ecp_point_cmp((const mbedtls_ecp_point *) a,
                                 (const mbedtls_ecp_point *) b);
}

/**
 * crypto_ec_order_len - Get length of the order in octets
 * @e: EC context from crypto_ec_init()
 * Returns: Length of the order defining the group
 */
size_t crypto_ec_order_len(struct crypto_ec *e)
{
	return mbedtls_mpi_size(&e->group.N);
}

/**
 * crypto_ec_get_a - Get 'a' value of an EC curve
 * @e: EC context from crypto_ec_init()
 * Returns: a (bignum) of the group
 */
const struct crypto_bignum * crypto_ec_get_a(struct crypto_ec *e)
{
	return (const struct crypto_bignum *)&e->group.A;
}

/**
 * crypto_ec_get_b - Get 'b' value of an EC curve
 * @e: EC context from crypto_ec_init()
 * Returns: b (bignum) of the group
 */
const struct crypto_bignum * crypto_ec_get_b(struct crypto_ec *e)
{
	return (const struct crypto_bignum *)&e->group.B;
}

int mbedtls_hardclock_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
uint32_t rtos_now(bool isr);
    unsigned long timer = rtos_now(false);
    ((void) data);
    *olen = 0;

    if( len < sizeof(unsigned long) )
        return( 0 );

    memcpy( output, &timer, sizeof(unsigned long) );
    *olen = sizeof(unsigned long);

    return( 0 );
}

#endif
