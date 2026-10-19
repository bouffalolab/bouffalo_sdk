#include "bl_lp_internal.h"
#include "bflb_sec_sha.h"
#include "multi_bins.h"

#define BL_LPFW_SHA256_SIZE 32U

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
    const uint8_t *lpfw_start = multi_bins_get_start("LPFW");

    return lpfw_start == NULL ? NULL : (bl_lp_fw_info_t *)(lpfw_start + BL_LPFW_SHA256_SIZE);
}

int bl_lpfw_bin_check(void)
{
    const uint8_t *lpfw_start = multi_bins_get_start("LPFW");
    const uint8_t *lpfw_end = multi_bins_get_end("LPFW");
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if (lpfw_start == NULL || lpfw_end == NULL || lpfw_info == NULL ||
        (uintptr_t)lpfw_end <= (uintptr_t)lpfw_start + BL_LPFW_SHA256_SIZE ||
        (uintptr_t)lpfw_end - (uintptr_t)lpfw_start < BL_LPFW_SHA256_SIZE + sizeof(*lpfw_info)) {
        BL_LP_LOG("lpfw multi_bins descriptor error\r\n");
        return -1;
    }

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

static void lpfw_sec_sha256(uintptr_t addr, uint32_t len, uint8_t *result)
{
    struct bflb_device_s *sha256 = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_group0_request_sha_access(sha256);
    bflb_sha_init(sha256, SHA_MODE_SHA256);
    bflb_sha256_start(sha256, &ctx_sha256);
    bflb_sha256_update(sha256, &ctx_sha256, (const uint8_t *)addr, len);
    bflb_sha256_finish(sha256, &ctx_sha256, result);
}

char *bl_lpfw_bin_get_version_str(void)
{
    bl_lp_fw_info_t *lpfw_info = bl_lpfw_bin_get_info();

    if ((lpfw_info == NULL) || (lpfw_info->magic_code != 0x7766706C)) {
        return NULL;
    }

    return lpfw_info->lpfw_version_str;
}

int bl_lpfw_ram_load(void)
{
    const uint8_t *lpfw_start_addr;
    const uint8_t *lpfw_load_addr;
    const uint8_t *lpfw_end_addr;
    uint32_t lpfw_size;

    if (bl_lpfw_bin_check() < 0) {
        assert(0);
        return -1;
    }

    uint32_t lpfw_addr = bl_lpfw_ram_addr();
    lpfw_start_addr = multi_bins_get_start("LPFW");
    lpfw_end_addr = multi_bins_get_end("LPFW");
    if (lpfw_start_addr == NULL || lpfw_end_addr == NULL) {
        return -1;
    }
    lpfw_load_addr = lpfw_start_addr + BL_LPFW_SHA256_SIZE;
    lpfw_size = (uintptr_t)lpfw_end_addr - (uintptr_t)lpfw_load_addr;

    BL_LP_LOG("lpfw_addr:0x%08x\r\n", lpfw_addr);
    BL_LP_LOG("lpfw_size:%d\r\n", lpfw_size);
    BL_LP_LOG("lpfw_load_addr:0x%08x\r\n", (uint32_t)(uintptr_t)lpfw_load_addr);

    memcpy((void *)lpfw_addr, lpfw_load_addr, lpfw_size);
    bflb_l1c_dcache_clean_range((void *)lpfw_addr, lpfw_size);

    return 0;
}

int bl_lpfw_ram_verify(void)
{
    const uint8_t *lpfw_start;
    const uint8_t *lpfw_end;
    uint32_t lpfw_size;
    uint8_t result[BL_LPFW_SHA256_SIZE];

    if (bl_lpfw_bin_check() < 0) {
        return -1;
    }

    lpfw_start = multi_bins_get_start("LPFW");
    lpfw_end = multi_bins_get_end("LPFW");
    lpfw_size = (uint32_t)((uintptr_t)lpfw_end - (uintptr_t)lpfw_start - BL_LPFW_SHA256_SIZE);
    lpfw_sec_sha256(bl_lpfw_ram_addr(), lpfw_size, result);

    if (memcmp(result, lpfw_start, BL_LPFW_SHA256_SIZE) != 0) {
        BL_LP_LOG("lpfw sha256 check failed\r\n");
        return -1;
    }

    return 0;
}
