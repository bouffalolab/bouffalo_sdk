/**
  ******************************************************************************
  * @file    emac_basic.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "bl702_common.h"
#include "bl702_glb.h"
#include "bl702_emac.h"
#include "bflb_platform.h"
#include "emac_bd.h"

/** @addtogroup  BL702_Peripheral_Case
 *  @{
 */

/** @addtogroup  EMAC_BASIC
 *  @{
 */

/** @defgroup  EMAC_BASIC_Private_Macros
 *  @{
 */

#define TEST_ROLE_UIP            (0)
#define TEST_ROLE_TX             (1)
#define TEST_ROLE_RX             (2)
#define TEST_ROLE_TX_RX          (3)

#define EMAC_USE_INSIDE_CLOCK    (0)
#define EMAC_TEST_ROLE           (TEST_ROLE_TX_RX)
#define EMAC_TEST_INTERVAL       (9)
#define EMAC_TEST_TX_INTERVAL_US (3000)

/*@} end of group EMAC_BASIC_Private_Macros */

/** @defgroup  EMAC_BASIC_Private_Types
 *  @{
 */

/*@} end of group EMAC_BASIC_Private_Types */

/** @defgroup  EMAC_BASIC_Private_Variables
 *  @{
 */

/*@} end of group EMAC_BASIC_Private_Variables */


/** @defgroup  EMAC_BASIC_Global_Variables
 *  @{
 */
EMAC_CFG_Type emacCfg={
    .recvSmallFrame=ENABLE,                     /*!< Receive small frmae or not */
    .recvHugeFrame=DISABLE,                     /*!< Receive huge frmae(>64K bytes) or not */
    .padEnable=ENABLE,                          /*!< Enable padding for frame which is less than MINFL or not */
    .crcEnable=ENABLE,                          /*!< Enable hardware CRC or not */
    .noPreamble=DISABLE,                        /*!< Enable preamble or not */
    .recvBroadCast=ENABLE,                      /*!< Receive broadcast frame or not */
    .interFrameGapCheck=ENABLE,                 /*!< Check inter frame gap or not */
    .miiNoPreamble=ENABLE,                      /*!< Enable MII interface preamble or not */
    .miiClkDiv=49,                              /*!< MII interface clock divider from bus clock */
    .maxTxRetry=16,                             /*!< Maximum tx retry count */
    .interFrameGapValue=24,                     /*!< Inter frame gap vaule in clock cycles(default 24)*/
    .minFrameLen=64,                            /*!< Minimum frame length */
    .maxFrameLen=1500,                          /*!< Maximum frame length */
    .collisionValid=16,                         /*!< Collision valid value */
    .macAddr[0]=0x18,                           /*!< MAC Address */
    .macAddr[1]=0xB0,
    .macAddr[2]=0x09,
    .macAddr[3]=0x00,
    .macAddr[4]=0x12,
    .macAddr[5]=0x34,
};
ETHPHY_CFG_Type phyCfg={
    .autoNegotiation=ENABLE,                    /*!< Speed and mode auto negotiation */
    .duplex=EMAC_MODE_FULLDUPLEX,             /*!< Duplex mode */
    .speed=EMAC_SPEED_100M,                   /*!< Speed mode */
    .phyAddress=0x1,                            /*!< PHY address */
};

#if (EMAC_TEST_ROLE & TEST_ROLE_TX)
//00-E0-4C-68-00-B2
//static const uint8_t test_framex[60] = {
//    0x00,0xe0,0x4c,0x68,0x00,0xb2,0x18,0xb0,0x09,0x00,0x12,0x34,0x08,0x00,0x45,0x00,
//    0x00,0x24,0x85,0x7b,0x00,0x00,0x80,0x11,0x00,0x00,0xa9,0xfe,0x19,0x6f,0xa9,0xfe,
//    0xff,0xff,0xd5,0x04,0x05,0xfe,0x00,0x10,0x18,0xea,0x54,0x43,0x46,0x32,0x04,0x00,
//    0x00,0x00
//};
static const uint8_t test_frame[42]={
        0x8c,0xec,0x4b,0x45,0x16,0xbc,0x18,0xB0,0x09,0x00,0x12,0x34,0x08,0x06,
        0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x18,0xB0,0x09,0x00,0x12,0x34,
        0xC0,0xA8,0x0D,0x91,0x8c,0xec,0x4b,0x45,0x16,0xbc,0xa9,0xfe,0x0f,0xb3
};
#define TEST_PATTERN_LEN     (ETH_MAX_PACKET_SIZE - 32)
static uint8_t test_pattern[TEST_PATTERN_LEN] = {0};

static volatile uint32_t tx_pkg_cnt = 0;
static volatile uint32_t tx_err_cnt = 0;
#endif

#if (EMAC_TEST_ROLE & TEST_ROLE_RX)
static volatile uint32_t rx_pkg_cnt = 0;
static volatile uint32_t rx_err_cnt = 0;
static volatile uint32_t rx_bytes = 0;
#endif

#if (EMAC_TEST_ROLE & TEST_ROLE_RX) || (EMAC_TEST_ROLE & TEST_ROLE_TX)
static uint32_t time = 0;
static uint32_t last_time = 0;
#endif

/*@} end of group EMAC_BASIC_Global_Variables */

/** @defgroup  EMAC_BASIC_Private_Fun_Declaration
 *  @{
 */

/*@} end of group EMAC_BASIC_Private_Fun_Declaration */

/** @defgroup  EMAC_BASIC_Private_Functions
 *  @{
 */

static void EMAC_GPIO_Init(void)
{
    uint8_t emacPins[]={GLB_GPIO_PIN_0,GLB_GPIO_PIN_1,GLB_GPIO_PIN_2,
                        GLB_GPIO_PIN_7,GLB_GPIO_PIN_8,
                        GLB_GPIO_PIN_18,GLB_GPIO_PIN_19,GLB_GPIO_PIN_20,GLB_GPIO_PIN_21,GLB_GPIO_PIN_22};

    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_EMAC);

    GLB_GPIO_Func_Init(GPIO_FUN_ETHER_MAC,(GLB_GPIO_Type *)emacPins,sizeof(emacPins));
}

static void EMAC_TX_Done_Callback(void)
{
    uint32_t index = 0;

    index = EMAC_BD_Get_CurActive(EMAC_BD_TYPE_TX);

#if (EMAC_TEST_ROLE & TEST_ROLE_TX)
    tx_pkg_cnt ++;
#endif

    EMAC_BD_TX_dequeue(index);
}

static void EMAC_TX_Error_Callback(void)
{
    uint32_t index = 0;

#if (EMAC_TEST_ROLE & TEST_ROLE_TX)
    tx_err_cnt ++;
#endif

    index = EMAC_BD_Get_CurActive(EMAC_BD_TYPE_TX);

    EMAC_BD_TX_onErr(index);
}

static void EMAC_RX_Done_Callback(void)
{
    uint32_t index = 0;

    index = EMAC_BD_Get_CurActive(EMAC_BD_TYPE_RX);

    EMAC_BD_RX_enqueue(index);
}

static void EMAC_RX_Error_Callback(void)
{
    uint32_t index;

    index = EMAC_BD_Get_CurActive(EMAC_BD_TYPE_RX);

#if (EMAC_TEST_ROLE & TEST_ROLE_RX)
    rx_err_cnt ++;
#endif

    EMAC_BD_RX_onErr(index);
}

static void EMAC_RX_Busy_Callback(void)
{
   MSG("%s:%d\r\n", __func__, __LINE__);
}

/****************************************************************************//**
 * @brief  EMAC basic case funtion
 *
 * @param  EMACId: EMAC ID type
 * @param  EMACCfg: EMAC configuration struct pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type EMAC_Basic_Case()
{
    BL_Err_Type err = SUCCESS;
#if (EMAC_TEST_ROLE & TEST_ROLE_RX)
    uint32_t rx_len = 0;
#endif

#if EMAC_USE_INSIDE_CLOCK
    //enable audio clock */
    PDS_Enable_PLL_Clk(PDS_PLL_CLK_48M);
    PDS_Set_Audio_PLL_Freq(AUDIO_PLL_50000000_HZ);

    GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_INSIDE_50M);
#else
    GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_OUTSIDE_50M);
#endif
    GLB_AHB_Slave1_Clock_Gate(DISABLE,BL_AHB_SLAVE1_EMAC);
    //GLB_Invert_ETH_RX_CLK(0);
    //GLB_Invert_ETH_TX_CLK(0);
    EMAC_Init(&emacCfg);
    err=EMAC_PhyInit(&phyCfg);
    if(TIMEOUT==err){
        MSG("PHY Init timeout\r\n");
        while(1);
    }
    if(ERROR==err){
        MSG("PHY Init error\r\n");
        while(1);
    }
    if(phyCfg.duplex==EMAC_MODE_FULLDUPLEX){
        MSG("EMAC_MODE_FULLDUPLEX\r\n");
    }else{
        MSG("EMAC_MODE_HALFDUPLEX\r\n");
    }
    if(phyCfg.speed==EMAC_SPEED_100M){
        MSG("EMAC_SPEED_100M\r\n");
    }else{
        MSG("EMAC_SPEED_50M\r\n");
    }
    MSG("PHY Init done\r\n");

    EMAC_BD_Init();

    EMAC_Int_Callback_Install(EMAC_INT_TX_DONE_IDX, EMAC_TX_Done_Callback);
    EMAC_Int_Callback_Install(EMAC_INT_TX_ERROR_IDX, EMAC_TX_Error_Callback);

    EMAC_Int_Callback_Install(EMAC_INT_RX_DONE_IDX, EMAC_RX_Done_Callback);
    EMAC_Int_Callback_Install(EMAC_INT_RX_ERROR_IDX, EMAC_RX_Error_Callback);

    EMAC_Int_Callback_Install(EMAC_INT_RX_BUSY_IDX, EMAC_RX_Busy_Callback);

    NVIC_EnableIRQ(EMAC_IRQn);

    EMAC_ClrIntStatus(EMAC_INT_ALL);
    EMAC_IntMask(EMAC_INT_ALL, UNMASK);
#if (EMAC_TEST_ROLE == TEST_ROLE_TX)
    EMAC_IntMask(EMAC_INT_RX_BUSY, MASK);
#endif
    __enable_irq();
    EMAC_Enable();

#if EMAC_TEST_ROLE == TEST_ROLE_UIP
    uip_main();
#endif

#if (EMAC_TEST_ROLE & TEST_ROLE_RX) || (EMAC_TEST_ROLE & TEST_ROLE_TX)
    bflb_platform_init_time();
    bflb_platform_start_time();
#endif

#if (EMAC_TEST_ROLE & TEST_ROLE_TX)
    memset(test_pattern, 0x5a, TEST_PATTERN_LEN);
    memcpy(test_pattern, test_frame, sizeof(test_frame));
#endif

    while(1) {
#if (EMAC_TEST_ROLE & TEST_ROLE_TX)
        EMAC_BD_TX_enqueue(-1, sizeof(test_frame), test_pattern);
        //BL702_Delay_US(EMAC_TEST_TX_INTERVAL_US);
#endif

#if (EMAC_TEST_ROLE & TEST_ROLE_RX)
    EMAC_BD_RX_dequeue(-1, &rx_len, NULL);
    if (rx_len) {
        rx_pkg_cnt ++;
        rx_bytes += rx_len;
    }
#endif

#if (EMAC_TEST_ROLE & TEST_ROLE_RX) || (EMAC_TEST_ROLE & TEST_ROLE_TX)
        time = bflb_platform_get_time_ms() /1000;
        if ((time - last_time) > EMAC_TEST_INTERVAL) {
            last_time = time;
#if (EMAC_TEST_ROLE & TEST_ROLE_TX)
            MSG("tx: %d, err: %d\n\r", tx_pkg_cnt, tx_err_cnt);
#endif
#if (EMAC_TEST_ROLE & TEST_ROLE_RX)
            MSG("rx: %d, err: %d, bytes: %d\n\r", rx_pkg_cnt, rx_err_cnt, rx_bytes);
#endif
        }

    /* more time slice for RX */
#if (EMAC_TEST_ROLE & TEST_ROLE_RX)
    EMAC_BD_RX_dequeue(-1, &rx_len, NULL);
    if (rx_len) {
        rx_pkg_cnt ++;
        rx_bytes += rx_len;
    }
#endif

#endif
    }

    return SUCCESS;
}

/*@} end of group EMAC_BASIC_Private_Functions */

/** @defgroup  EMAC_BASIC_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  EMAC basic case main function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
int main(void)
{
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M,GLB_SYS_CLK_DLL144M);

    bflb_platform_init(0);
    MSG("EMAC basic test\n");

    EMAC_GPIO_Init();

    EMAC_Basic_Case();

    BL_CASE_SUCCESS;
}

/*@} end of group EMAC_BASIC_Public_Functions */

/*@} end of group EMAC_BASIC */

/*@} end of group BL70X_Peripheral_Case */
