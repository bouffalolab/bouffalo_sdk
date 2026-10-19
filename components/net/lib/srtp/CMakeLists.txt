sdk_generate_library()

sdk_library_add_sources(
    libsrtp/srtp/ekt.c
    libsrtp/srtp/srtp.c
    libsrtp/crypto/cipher/cipher.c
    libsrtp/crypto/cipher/null_cipher.c
    libsrtp/crypto/cipher/aes.c
    libsrtp/crypto/cipher/aes_icm.c
    libsrtp/crypto/hash/auth.c
    libsrtp/crypto/hash/null_auth.c
    libsrtp/crypto/hash/hmac.c
    libsrtp/crypto/hash/sha1.c
    libsrtp/crypto/kernel/alloc.c
    libsrtp/crypto/kernel/crypto_kernel.c
    libsrtp/crypto/kernel/err.c
    libsrtp/crypto/kernel/key.c
    libsrtp/crypto/math/datatypes.c
    libsrtp/crypto/math/stat.c
    libsrtp/crypto/replay/rdb.c
    libsrtp/crypto/replay/rdbx.c
    libsrtp/crypto/replay/ut_sim.c
)

sdk_add_include_directories(libsrtp/crypto/include)
sdk_add_include_directories(libsrtp/include)
sdk_add_include_directories(port)
sdk_add_include_directories(include)

sdk_add_compile_definitions(-DHAVE_CONFIG_H)

