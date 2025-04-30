
#include <bl616_sdu.h>
#include <bl616_gpio.h>
#include <bl616_glb_gpio.h>

#include "sdio_port.h"
#include "trcver_sdio.h"

#include <sdiowifi_platform_adapt.h>

#if defined(CFG_BL616)

#ifdef CONFIG_BOUFFALO_SDK
#include "bflb_core.h"
ATTR_NOCACHE_NOINIT_LPFW_SHARE __ALIGNED(32) struct {
    uint32_t buf[SDIO_RX_MAX_PORT_NUM][SDIO_RX_BUF_SIZE / 4];
} sdio_readbuf_wrapper;
uint32_t (*sdio_readbuf)[SDIO_RX_BUF_SIZE / 4] = (void *)&sdio_readbuf_wrapper;

#else
uint32_t (*sdio_readbuf)[SDIO_RX_BUF_SIZE / 4] = (void *)SDIO_RX_BUF_START;
#endif

#else
struct {
    uint32_t buf[SDIO_RX_MAX_PORT_NUM][SDIO_RX_BUF_SIZE / 4];
} __attribute__((aligned(32))) sdio_readbuf_wrapper;
uint32_t (*sdio_readbuf)[SDIO_RX_BUF_SIZE / 4] = (void *)&sdio_readbuf_wrapper;
#endif

void sdio_glb_reset(void)
{
#ifdef CFG_BL616
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDU);
#else
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_SDU);
#endif
}

void sdio_gpio_init(void)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 1;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;

    for (int i = 10; i < 16; i++) {
        gpio_cfg.gpioPin = i;
        gpio_cfg.gpioFun = GPIO_FUN_SDIO;
        GLB_GPIO_Init(&gpio_cfg);
    }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
}

#define WL_REGS8(x)         (*(volatile unsigned char *)(x))
uint8_t sdu_read_s_reg(uint32_t offset)
{
    return WL_REGS8(BL_FUNC_SCRATCH_BASE + offset);
}

void sdu_write_s_reg(uint32_t offset, uint8_t value)
{
    WL_REGS8(BL_FUNC_SCRATCH_BASE + offset) = value;
}

uint32_t sdio_get_rxbuf(uint8_t port_num)
{
    return (uint32_t)SDIO_MAWOC(&sdio_readbuf[port_num]);
}

void sdio_attach_rxbuf(uint8_t fn_num, uint8_t port_num)
{
    SdioFuncReg[fn_num]->WrIdx = port_num;
    SdioFuncReg[fn_num]->SqWriteBase = sdio_get_rxbuf(port_num);
    SdioFuncReg[fn_num]->WrBitMap = (1 << port_num);
}

void sdio_host_check_init(void)
{
    HR_LOGI("sdio_host_check_init\r\n");

    /* clear power value */
    WL_REGS8(BL_FUNC_SCRATCH_BASE)=0;    //clear power value;

    /* reset rd and wr bitmap */
    SdioFuncReg[FUNC_WIFI]->RdBitMap = 0x0;
    SdioFuncReg[FUNC_WIFI]->WrBitMap = 0x0;//reset rd and wr bitmap

    /* toggle SDIO_CCR_CIC_DnLdOvr on WL_SDIO_CCR_CARD_INT_CAUSE */
    SdioFuncReg[FUNC_WIFI]->CardIntStatus = SDIO_CCR_CIC_DnLdOvr;//?
    SdioFuncReg[FUNC_WIFI]->CardIntStatus = 0;///clear int

    SdioFuncReg[FUNC_WIFI]->Config2 |= CONFIG2_MSK ;
    SdioFuncReg[FUNC_WIFI]->Config |= 0x00000010;

    /* unmask the interrupts */
    //SdioFuncReg[FUNC_WIFI]->CardIntMask = SDIO_CCR_CIM_MASK;
    SdioFuncReg[FUNC_WIFI]->CardIntMask = 0x0067;
   /* select interrupt reset mode */
    SdioFuncReg[FUNC_WIFI]->CardIntMode = 0;
}

void gen_card2host_int(void)
{
    //sdio_GEN_CARD2HOST_INT(0,(SDIO_CCR_CS_ReadCISRdy | SDIO_CCR_CS_DnLdRdy  | SDIO_CCR_CS_IORdy));
    //GLB_GPIO_Write(GLB_GPIO_PIN_3, 1);
    sdio_GEN_CARD2HOST_INT(0, SDIO_CCR_CS_UpLdRdy);
    //sdio_GEN_CARD2HOST_INT(1, SDIO_CCR_CS_UpLdRdy);
    //GLB_GPIO_Write(GLB_GPIO_PIN_3, 0);


    return;
}

void hr_coredump(void)
{
#if 0
    char *addr   = NULL;
    addr = (char *)0;
    printf("p 0 = %d\r\n", *addr);
#endif
}

