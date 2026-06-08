#include "bl_lp_internal.h"
#include "bflb_sec_sha.h"

static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s ctx_sha256;

static uintptr_t bl_lpfw_ram_addr(void)
{
#if defined(BL618DG) && !defined(CPU_MODEL_A0)
    return ((uintptr_t)__lpfw_share_start__ & 0x0FFFFFFF) | 0xA0000000;
#else
    return ((uintptr_t)__lpfw_share_start__ & 0x0FFFFFFF) | 0x60000000;
#endif
}

bl_lp_fw_info_t *bl_lpfw_bin_get_info(void)
{
    return (bl_lp_fw_info_t *)__lpfw_load_addr;
}

int bl_lpfw_bin_check(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if (lpfw_info->magic_code != 0x7766706C) {
        BL_LP_LOG("lpfw magic code error\r\n");
        return -1;
    }

    if ((lpfw_info->lpfw_memory_start & 0x0FFFFFFF) != ((uint32_t)__lpfw_share_start__ & 0x0FFFFFFF)) {
        BL_LP_LOG("lpfw memory start address error: lpfw:0x%08X, app:0x%08X\r\n", lpfw_info->lpfw_memory_start,
                  (uint32_t)__lpfw_share_start__);
        return -2;
    }

    if ((lpfw_info->lpfw_memory_end - lpfw_info->lpfw_memory_start) >
        ((uint32_t)__lpfw_share_end__ - (uint32_t)__lpfw_share_start__)) {
        BL_LP_LOG("lpfw memory size_over\r\n");
        return -3;
    }

    return 0;
}

char *bl_lpfw_bin_get_version_str(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if (lpfw_info->magic_code != 0x7766706C) {
        return NULL;
    }

    return lpfw_info->lpfw_version_str;
}

int bl_lpfw_ram_load(void)
{
    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    uint32_t lpfw_addr = bl_lpfw_ram_addr();
    uint32_t lpfw_size = *(uint32_t *)((uintptr_t)__lpfw_load_addr - 7 * sizeof(uint32_t));

    BL_LP_LOG("lpfw_addr:0x%08x\r\n", lpfw_addr);
    BL_LP_LOG("lpfw_size:%d\r\n", lpfw_size);
    BL_LP_LOG("__lpfw_load_addr:0x%08x\r\n", __lpfw_load_addr);

    memcpy((void *)lpfw_addr, __lpfw_load_addr, lpfw_size);
    bflb_l1c_dcache_clean_range((void *)lpfw_addr, lpfw_size);

    return 0;
}

static void lpfw_sec_sha256(uint32_t addr, uint32_t len, uint8_t *result)
{
    struct bflb_device_s *sha256;

    sha256 = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_group0_request_sha_access(sha256);

    bflb_sha_init(sha256, SHA_MODE_SHA256);
    bflb_sha256_start(sha256, &ctx_sha256);
    bflb_sha256_update(sha256, &ctx_sha256, (const uint8_t *)addr, len);
    bflb_sha256_finish(sha256, &ctx_sha256, result);
}

int bl_lpfw_ram_verify(void)
{
    if (bl_lpfw_bin_check() < 0) {
        assert(0);
    }

    uint32_t lpfw_addr = bl_lpfw_ram_addr();
    uint32_t lpfw_size = *(uint32_t *)((uintptr_t)__lpfw_load_addr - 7 * sizeof(uint32_t));
    uint8_t *lpfw_sha256 = (uint8_t *)((uintptr_t)__lpfw_load_addr - 16 * sizeof(uint32_t));
    uint8_t result[32];

    lpfw_sec_sha256(lpfw_addr, lpfw_size, result);

    if (memcmp(result, lpfw_sha256, 32) != 0) {
        BL_LP_LOG("lpfw sha256 check failed\r\n");
        return -1;
    }

    return 0;
}
