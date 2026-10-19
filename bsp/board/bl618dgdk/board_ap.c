#if !__has_include("board_overlay.h")

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "bflb_common.h"
#include "bflb_l1c.h"
#include "bflb_sf_ctrl.h"
#include "bl618dg_glb.h"
#include "bl618dg_tzc_sec.h"
#include "multi_bins.h"

#define NP_FW_HEADER_SIZE 0x1000U

#ifdef CONFIG_PSRAM_XIP
static uintptr_t start_load_np(void)
{
    multi_bin_desc_t desc;

    if (multi_bins_get_desc("NP", &desc) < 0 ||
        desc.end_addr <= desc.start_addr ||
        desc.vma_end <= desc.vma_start ||
        desc.vma_end - desc.vma_start != desc.end_addr - desc.start_addr ||
        desc.vma_start != CONFIG_PSRAM_ADDRESS + CONFIG_PSRAM_FOR_AP_SIZE ||
        desc.vma_end - desc.vma_start <= NP_FW_HEADER_SIZE) {
        return 0;
    }
    return desc.vma_start + NP_FW_HEADER_SIZE;
}
#endif

void boot_up_np(void)
{
    Tzc_Sec_Set_Master_Group(TZC_SEC_MASTER_NP, 1);
#ifdef CONFIG_PSRAM_XIP
    uintptr_t np_entry = start_load_np();
    multi_bin_desc_t np = { 0 };

    multi_bins_get_desc("NP", &np);
    printf("[AP] NP image: offset=%08lx-%08lx vma=%08lx-%08lx\r\n",
           (unsigned long)np.start_addr, (unsigned long)np.end_addr,
           (unsigned long)np.vma_start, (unsigned long)np.vma_end);

    if (np_entry == 0) {
        puts("invalid NP PSRAM image !!!\r\n");
        while (1) {}
    }
    bflb_l1c_dcache_clean_all();
#else
    multi_bin_desc_t self;
    multi_bin_desc_t np;
    extern uint32_t __start;
    uint32_t ap_offset = bflb_sf_ctrl_get_flash_image_offset(0, 0);
    uint32_t boot_addr = ap_offset;

    if (multi_bins_get_desc("SELF", &self) < 0 ||
        self.end_addr <= self.start_addr ||
        multi_bins_get_desc("NP", &np) < 0 ||
        np.end_addr <= np.start_addr ||
        np.start_addr < self.start_addr) {
        puts("invalid NP image !!!\r\n");
        while (1) {}
    }

    /* The NP descriptor starts at its header; XIP maps the payload after it. */
    boot_addr += np.start_addr - self.start_addr + NP_FW_HEADER_SIZE;
    bflb_sf_ctrl_set_flash_image_offset(boot_addr, 1, 0);
#endif
#ifdef CONFIG_PSRAM_XIP
    GLB_Set_CPU_Reset_Address(GLB_CORE_ID_NP, (uint32_t)np_entry);
#else
    GLB_Set_CPU_Reset_Address(GLB_CORE_ID_NP, (uint32_t)&__start);
#endif
    BL_Err_Type ret = GLB_Release_CPU(GLB_CORE_ID_NP);
    printf("[AP] NP release: %s\r\n", ret == SUCCESS ? "OK" : "FAILED");
}

void boot_up_lp(uint32_t address)
{
    multi_bin_desc_t lp = { 0 };
    const uint8_t *mini_code_src = multi_bins_get_start("LP");

    multi_bins_get_desc("LP", &lp);
    uint32_t mini_code_start = lp.start_addr;
    uint32_t mini_code_end = lp.end_addr;
    uint32_t mini_code_len = mini_code_end > mini_code_start ? mini_code_end - mini_code_start : 0;

    printf("mini code start: 0x%08x, end: 0x%08x, len: %d\r\n", mini_code_start, mini_code_end, mini_code_len);
    Tzc_Sec_Set_Master_Group(TZC_SEC_MASTER_LP, 0);

    if (mini_code_src != NULL && mini_code_len != 0) {
        GLB_Release_Mini_Sys();
#if defined(CPU_MODEL_A0)
        GLB_Set_MINI_FCLK(ENABLE, GLB_MINI_FCLK_XCLK, 0);
#else
        GLB_Set_MINI_FCLK(ENABLE, GLB_MINI_FCLK_RC32M, 0);
#endif
        //GLB_Select_LPCPU_Jtag();
        arch_delay_us(10);

        memcpy((void *)address, mini_code_src, mini_code_len);
        bflb_l1c_dcache_clean_all();

        __asm__ volatile("" : "+r"(address));
        printf("Copy gmini_sysData to 0x%08x,[1][31:0]=%08x,len=%d\r\n", address,
               *(volatile uint32_t *)((uintptr_t)address + 4), mini_code_len);

        GLB_Set_CPU_Reset_Address(GLB_CORE_ID_LP, (uint32_t)address);

        GLB_Release_CPU(GLB_CORE_ID_LP);
    }
}

#endif
