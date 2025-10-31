#include <sdiowifi_securec_wrap.h>
#include <stdint.h>
#ifdef CFG_BL616
#include <bl616_sdu.h>
#include <bl616_gpio.h>
#include <bl616_glb_gpio.h>
#else
#include <bl602_glb.h>
#include <bl602.h>
#include <bl602_sdu.h>
#include <platform_device.h>
#endif
#include <bflb_irq.h>
#include <sdiowifi_utils.h>
#include "trcver_sdio.h"
#include "net_wifi_transceiver.h"
#include "sdiowifi_mgmr.h"
#include "sdiowifi_mgmr_type.h"
//#include <bl_timer.h>

#include <sdiowifi_platform_adapt.h>

#include <sdio_port.h>

#define SDU_INT_HOST        0
#define SDU_SEND_TIME_OUT   500

static bool sdu_software_reset = false;
static bl_sdio_read_cb_t sd_read_callback = NULL;
static sdiowifi_task_t sdu_irq_handle = NULL;
static int send_timeout = 0;
static int twin_send_timeout = 0;

#define SDIO_TX_WDT_TIMEOUT     (450)

#define SDU_WORKER_EV_NONE      (0)
#define SDU_WORKER_EV_RESET     (1 << 0)
#define SDU_WORKER_EV_TX_WDT    (1 << 1)
static long sdu_worker_ev;

/* Check whether rx buf has been attached to one port by corresponding bit */
static volatile uint16_t sdu_rx_buf_attached[NUM_FUNC];

void sdu_interrupt_entry(int irq, void *arg);
void sdu_soft_reset_interrupt_entry(int irq, void *arg);
void sdu_interrupt_task(void *pvParameters);

static void sdu_attach_rx_bufs(uint8_t fn_num, uint8_t port_num)
{
    sdio_attach_rxbuf(fn_num, port_num);
    sdu_rx_buf_attached[fn_num] |= (1 << port_num);

#if SDU_INT_HOST
    /* Generate host sdio interrupt */
    sdio_GEN_CARD2HOST_INT(fn_num,(SDIO_CCR_CS_ReadCISRdy | SDIO_CCR_CS_DnLdRdy  | SDIO_CCR_CS_IORdy));
#endif
}

typedef void (*sdio_tx_cfm_cb_t)(int idx, void *arg);

struct sdio_tx_desc {
    bool used;
    bool is_cmd; // true: cmd, false: frame
    sdio_tx_cfm_cb_t cb;
    void *cb_arg;
    int cb_arg_idx;
    struct sdio_tx_desc *twin;
    uint8_t ref;
    sdiowifi_tick_t tick_attached;
};

static struct sdio_tx_desc sdio_tx_descs[16] = { 0 };
static size_t sdio_tx_attached = 0;
static sdiowifi_timer_handle_t sdio_tx_wdt_timer;

#pragma pack(push, 1)
struct sdio_top_msg {
    uint8_t type_lsb;
    uint8_t type_msb;
    uint8_t len_lsb;            // length of pld, excluding padding
    uint8_t len_msb;
    uint8_t pld_off;
    uint8_t is_amsdu;
    uint8_t has_twin;
    uint8_t subtype_lsb;
    uint8_t subtype_msb;
    uint8_t first_part_len_lsb; // length of pld, excluding padding
    uint8_t first_part_len_msb;
    uint8_t _pad0[1];
    uint8_t pld_w_pad[];
};
#pragma pack(pop)

static uint8_t curr_upld_port = 0;
static uint8_t curr_dnld_port = 0;
static volatile uint16_t rd_bitmap_old = 0;

void sdu_software_reset_set(int enable)
{
    sdu_software_reset = !!enable;
}

void notify_sdu_worker(int event, int isr)
{
    if (!sdu_irq_handle) {
        HR_LOGD("%s(%d): sdu worker task is not found\r\n", __func__, __LINE__);
        return;
    }
    if (event) {
        sdu_worker_ev |= event;
    }
    sdiowifi_task_notify(sdu_irq_handle, isr);
}

static void sdio_tx_timer_cb(sdiowifi_timer_handle_t timer, void *arg)
{
    notify_sdu_worker(SDU_WORKER_EV_TX_WDT, 0);
}

static void start_tx_wdt(void)
{
    if (!(sdiowifi_timer_is_valid(sdio_tx_wdt_timer))) {
        sdiowifi_timer_start(sdio_tx_wdt_timer);
    }
}

void bl_sdio_tx_timer_stop(void)
{
    if (sdiowifi_timer_is_valid(sdio_tx_wdt_timer)) {
        sdiowifi_timer_stop(sdio_tx_wdt_timer);
    }
}

void trcver_sdio_stats_display(void)
{
    printf("[%ld] hostrouter rd:0x%08lX, wr:0x%08lX\r\n",
            sdiowifi_tick_now(0),
            SdioFuncReg[0]->RdBitMap, SdioFuncReg[0]->WrBitMap);
}

int sdu_send_data(void *data, uint32_t len, bool is_cmd, sdio_tx_cfm_cb_t cb, void *cb_arg)
{
    const uint8_t fn = 0;
    uint64_t count_time;
    struct sdio_tx_desc *desc;

    (void)len;

    count_time = CPU_Get_MTimer_US();
    while ((rd_bitmap_old & (1<< curr_upld_port))){
        if((CPU_Get_MTimer_US() - count_time) / 1000 >
           (send_timeout ? 10 : SDU_SEND_TIME_OUT)){
            send_timeout = 1;
            HR_LOGW("sdu send timeout\r\n");
            return BF1B_MSG_ERR_TIMEOUT;
        } else if (send_timeout) {
            /* after once timeout maybe next use delay for other thread scheduler and reduce timeout val*/
            sdiowifi_delay_ms(1);
        }
    }
    send_timeout = 0;

    desc = &sdio_tx_descs[curr_upld_port];
    if (desc->used) {
        HR_LOGD("sdio tx desc used!\r\n");
        hr_coredump();
        return BF1B_MSG_ERR_DESC_USED;
    }

    sdiowifi_enter_critical();
    desc->used = true;
    desc->is_cmd = is_cmd;
    desc->cb = cb;
    desc->cb_arg = cb_arg;
    desc->cb_arg_idx = curr_upld_port;
    desc->ref = 1;
    desc->twin = NULL;
    SdioFuncReg[fn]->RdIdx = curr_upld_port;
    SdioFuncReg[fn]->RdLen[curr_upld_port] = SDIO_TX_BUF_SIZE;
    SdioFuncReg[fn]->SqReadBase = (uint32_t)data;
    SdioFuncReg[fn]->RdBitMap = 1 << curr_upld_port;
    rd_bitmap_old = rd_bitmap_old | (1 << curr_upld_port);
    /* HR_LOGD("sdu_send_data %lu port %u, old %x\r\n", len, curr_upld_port, rd_bitmap_old); */
    sdio_GEN_CARD2HOST_INT(0, SDIO_CCR_CS_UpLdRdy);
    curr_upld_port = (curr_upld_port + 1) % 16;
    desc->tick_attached = sdiowifi_tick_now(0);
    ++sdio_tx_attached;
    sdiowifi_exit_critical(0);

    start_tx_wdt();

    return 0;
}

static void sdu_receive_data(uint8_t int_status)
{
    /* The port number for host to download packets*/
    uint8_t fn;
    /* uint8_t value; */
    uint8_t CRCError;
    uint8_t *recv_buf=NULL;

    for (fn = 0; fn < NUM_FUNC; fn++)
    {
        switch(fn)
        {
            case FUNC_WIFI:
            {
                /* value = SdioFuncReg[fn]->CardIntStatus; */
                /* Read the CRC error for the CMD 53 write*/
                CRCError = SdioFuncReg[fn]->HostTransferStatus;

                if (int_status & SDIO_CCR_CIC_DnLdOvr)
                {
                    /* Clear Interrupt Bit */
                    //SdioFuncReg[fn]->CardIntStatus = ~value|SDIO_CCR_CIC_PwrUp;
                    /* SdioFuncReg[fn]->CardIntStatus = value & 0xfe; */
                    if(CRCError & SDIO_CCR_HOST_INT_DnLdCRC_err)
                    {
                        break;
                    }
                    while (1) {
                        if (!(SdioFuncReg[fn]->WrBitMap & (1 << curr_dnld_port)) &&
                            (sdu_rx_buf_attached[fn] & (1 << curr_dnld_port))) {
                            recv_buf = (uint8_t *)sdio_get_rxbuf(curr_dnld_port);
                            /*clear current download port attach flag*/
                            sdu_rx_buf_attached[fn] &= ~(1 << curr_dnld_port);///clear attached flag

                            uint16_t len;
                            len = recv_buf[0] | (recv_buf[1] << 8);
                            if (sd_read_callback != NULL) {
                                sd_read_callback(NULL, recv_buf + 2, len);
                            }

                            sdu_attach_rx_bufs(fn, curr_dnld_port);
                            /*move on to next port*/
                            curr_dnld_port++;
                            if (curr_dnld_port == SDIO_RX_MAX_PORT_NUM) {
                                curr_dnld_port = 0;
                            }
                        } else {
                            break;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

int bl_sdu_init(void)
{
    HR_LOGI("bl_sdu_init\r\n");
    sdio_gpio_init();

    GLB_Config_SDIO_Host_Reset_System(0);
    GLB_Config_SDIO_Host_Reset_SDU(1);
    GLB_Config_SDIO_Host_Interrupt_CPU(1);

    bflb_irq_attach(SDU_SOFT_RST_IRQn, sdu_soft_reset_interrupt_entry, NULL);
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR |= 0x2;   /* rising edge trigger */
    bflb_irq_enable(SDU_SOFT_RST_IRQn);

    sdiowifi_task_create(sdu_interrupt_task, "sdu_worker", 0, 512, NULL, 31, &sdu_irq_handle);
    sdio_tx_wdt_timer = sdiowifi_timer_create(sdio_tx_timer_cb, NULL, SDIO_TX_WDT_TIMEOUT, 1, 0);

    sdio_glb_reset();

    bflb_irq_attach(SDIO_IRQn, sdu_interrupt_entry, NULL);
    bflb_irq_enable(SDIO_IRQn);

    return 0;
}

int32_t bl_sdio_handshake(void)
{
    sdio_host_check_init();

    for (int i = 0; i < SDIO_RX_MAX_PORT_NUM; ++i) {
        sdu_attach_rx_bufs(FUNC_WIFI, i);
    }
    return 0;
}

int bl_sdio_read_cb_register(void *env, bl_sdio_read_cb_t cb, void *cb_arg)
{
    sd_read_callback = cb;

    return 0;
}

#define LSB(x) ((x) & 0xff)
#define SLSB(x) (((x) >> 8) & 0xff)
#define OFFSET_OF(mem, st) ((uintptr_t)&(((st *)0)->mem))

struct sd_cmd_alloc_header {
    uint8_t guard_[16];
    void *orig;
    uint8_t data[];
};

static void sd_cmd_cfm_cb(int idx, void *arg)
{
#ifdef CFG_BL616
    if (arg) {
        struct sd_cmd_alloc_header *hdr;
        hdr = (void *)((uint8_t *)arg - sizeof(*hdr));
        vPortFree(hdr->orig);
    }
#else
    vPortFree(arg);
#endif
}

static void *sd_cmd_buf_alloc()
{
    void *ret;
#ifdef CFG_BL616
    size_t alloc_sz = sizeof(struct sd_cmd_alloc_header) + SDIO_TX_BUF_SIZE + 32;
    ret = pvPortMalloc(alloc_sz);
    if (ret) {
        void *cpy = ret;
        struct sd_cmd_alloc_header *hdr;
        L1C_DCache_Clean_Invalid_By_Addr((uintptr_t)ret, alloc_sz);
        hdr = (void *)SDIO_MAWOC(ret);
        hdr->orig = cpy;
        ret = hdr->data;
    }
#else
    ret = pvPortMalloc(SDIO_TX_BUF_SIZE);
#endif
    return ret;
}

int bl_sdio_write_cmd(void *env, uint16_t type, uint16_t subtype, const void *headroom, uint16_t headroom_len, const void *tailroom, uint16_t tailroom_len)
{
    const uint8_t *p1 = (const uint8_t *)headroom;
    size_t p1_len = (size_t)headroom_len;
    const uint8_t *p2 = (const uint8_t *)tailroom;
    size_t p2_len = (size_t)tailroom_len;

    int ret;
    struct sdio_top_msg *msg;
    void *p_buf;
    size_t buf_size = SDIO_TX_BUF_SIZE;
    size_t len = p1_len + p2_len;

    if (sizeof(struct sdio_top_msg) + len > SDIO_TX_BUF_SIZE) {
        return -1;
    }

    p_buf = sd_cmd_buf_alloc();
    if (!p_buf) {
        return -1;
    }

    msg = (struct sdio_top_msg *)p_buf;
    msg->type_lsb = LSB(type);
    msg->type_msb = SLSB(type);
    msg->len_lsb = LSB(len);
    msg->len_msb = SLSB(len);
    msg->pld_off = OFFSET_OF(pld_w_pad, struct sdio_top_msg);
    msg->subtype_lsb = LSB(subtype);
    msg->subtype_msb = SLSB(subtype);
    msg->is_amsdu = false;
    msg->has_twin = 0;

    p_buf += msg->pld_off;
    buf_size -= msg->pld_off;

    MEMCPY_SAFE((uint8_t *)p_buf, buf_size, p1, p1_len);
    if (p2) {
        p_buf += p1_len;
        buf_size -= p1_len;
        MEMCPY_SAFE((uint8_t *)p_buf, buf_size, p2, p2_len);
    }

    ret = sdu_send_data(msg, len, false, sd_cmd_cfm_cb, msg);
    if (ret) {
        sd_cmd_cfm_cb(-1, msg);
    }
    return ret;
}

int bl_sdio_write_pbuf(void *env, uint16_t type, uint16_t subtype, struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg)
{
    struct sdio_top_msg *msg;
    void *data_ptr = p->payload;
    uint16_t data_len = p->tot_len;
    struct pbuf *next;

    msg = (struct sdio_top_msg *)SDM_ALIGN_LO(data_ptr - sizeof(*msg), 4);

    msg->type_lsb = LSB(type);
    msg->type_msb = SLSB(type);
    msg->len_lsb = LSB(data_len);
    msg->len_msb = SLSB(data_len);
    msg->pld_off = data_ptr - (void *)msg;
    msg->is_amsdu = is_amsdu;
    msg->has_twin = 0;
    msg->subtype_lsb = LSB(subtype);
    msg->subtype_msb = SLSB(subtype);
    msg->first_part_len_lsb = LSB(p->len);
    msg->first_part_len_msb = SLSB(p->len);

    next = p->next;
    if (next) {
        HR_LOGE("BL616 this should not happen, this is an A-AMSDU with two pbds\r\n");
        return BF1B_MSG_ERR_TIMEOUT;
    } else {
        return sdu_send_data(msg, data_len, false, cb, cb_arg);
    }
}

#define BIT_ISSET(val, bitn) ((val) & (1 << (bitn)))

// Function to handle SDIO interrupts
static void __attribute__((section(".tcm_code"))) sdu_interrupt_handle(void)
{
    uint8_t value;
    uint16_t rd_bitmap;
    uint16_t rd_bitmap_diff;
    struct sdio_tx_desc *desc;

    // Read the value of SDIO interrupt status register
    value = SdioFuncReg[0]->CardIntStatus;
    // Clear the interrupt status and set pwrup flag
    SdioFuncReg[0]->CardIntStatus = ~value | SDIO_CCR_CIC_PwrUp;

    // If it's a download data over interrupt, process the download data
    if (value & SDIO_CCR_CIC_DnLdOvr) {
        sdu_receive_data(value);
    }

    // If it's an upload data over interrupt, process the upload data
    if (value & SDIO_CCR_CIC_UpLdOvr) {
        // Read the read data bitmap and calculate the difference
        rd_bitmap = SdioFuncReg[0]->RdBitMap;
        rd_bitmap_diff = rd_bitmap ^ rd_bitmap_old;

        // If there's a difference in the read data bitmap, process the corresponding upload data
        if (rd_bitmap_diff) {
            for (int i = 0; i < 16; ++i) {
                // Check if the current bit is set
                if (!(BIT_ISSET(rd_bitmap_diff, i)) || !(BIT_ISSET(rd_bitmap_old, i))) {
                    continue;
                }

                // Check if the current descriptor is available and not processed
                desc = &sdio_tx_descs[i];
                if (!(desc->used) || !(desc->cb) || !(desc->twin == NULL)) {
                    continue;
                }

                // If the reference count is zero, process the descriptor
                desc->ref--;
                if (desc->ref != 0) {
                    continue;
                }

                // Call the callback function to process the descriptor and mark it as unused
                desc->cb(i, desc->cb_arg);
                sdiowifi_enter_critical();
                desc->used = false;
                --sdio_tx_attached;
                rd_bitmap_old &= ~(1<<i);
                sdiowifi_exit_critical(0);
            }
        } else {
            // If there's no difference in the read data bitmap, log the event
            HR_LOGD("sdu_interrupt_handle rd_bitmap_diff:%d\r\n", rd_bitmap_diff);
        }
        // Update the old read data bitmap
    }
}

static void sdio_tx_timedout()
{
    // TODO take your action here
    /* sdiowifi_mgmr_reinit(false); */
}

static void check_tx_timeout(void)
{
    int i;
    sdiowifi_tick_t now = sdiowifi_tick_now(0);
    size_t timedout = 0;

    if (sdio_tx_attached == 0) {
        return;
    }
    for (i = 0; i < sizeof(sdio_tx_descs) / sizeof(sdio_tx_descs[0]); ++i) {
        struct sdio_tx_desc *d = &sdio_tx_descs[i];
        if (!d->used) {
            continue;
        }
        if (!(now - d->tick_attached > sdiowifi_ms2ticks(SDIO_TX_WDT_TIMEOUT))) {
            continue;
        }

        if (d->cb) {
            d->cb(i, d->cb_arg);
        }
        sdiowifi_enter_critical();
        d->used = false;
        --sdio_tx_attached;
        sdiowifi_exit_critical(0);
        ++timedout;
        // XXX twin not considered
    }

    if (timedout) {
        sdio_tx_timedout();
    }
}

static void bl_sdu_reinit(void)
{
    bflb_irq_disable(SDIO_IRQn);

    // sdio bit_map flag reset
    curr_upld_port = 0;
    curr_dnld_port = 0;
    rd_bitmap_old = 0;

    // tx buffer reset
    for (int i = 0; i < sizeof(sdio_tx_descs) / sizeof(sdio_tx_descs[0]); ++i) {
        struct sdio_tx_desc *desc = &sdio_tx_descs[i];
        if (desc->used) {
            desc->cb(i, desc->cb_arg);
        }
    }
    MEMSET_SAFE(sdio_tx_descs,  sizeof(sdio_tx_descs),  0,  sizeof(sdio_tx_descs));
    sdiowifi_enter_critical();
    sdio_tx_attached = 0;
    sdiowifi_exit_critical(0);

    if (sdu_software_reset) {
        HR_LOGD("SDU Software Reset\r\n");
        sdio_glb_reset();
        sdu_software_reset = false;
    }

    GLB_Config_SDIO_Host_Reset_System(0);
    GLB_Config_SDIO_Host_Reset_SDU(1);
    GLB_Config_SDIO_Host_Interrupt_CPU(1);

    bflb_irq_attach(SDU_SOFT_RST_IRQn, sdu_soft_reset_interrupt_entry, NULL);
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR |= 0x2;   /* rising edge trigger */
    bflb_irq_enable(SDU_SOFT_RST_IRQn);

    sdio_gpio_init();

    bl_sdio_handshake();

    bflb_irq_enable(SDIO_IRQn);
}

static int sdio_software_reset(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;

    HR_LOGI("sdio_software_reset\r\n");

    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return -1;
    }
    bflb_net_wifi_trcver_set_present(&sdm->trcver, false);
    bl_sdu_reinit();
    bf1b_net_wifi_trcver_reinit(&sdm->trcver);
    sdiowifi_tx_buf_update_credit(&sdm->tx_desc);
    return 0;
}

void __attribute__((section(".tcm_code"))) sdu_interrupt_entry(int irq, void *arg)
{
    bflb_irq_disable(SDIO_IRQn);
    notify_sdu_worker(SDU_WORKER_EV_NONE, 1);
}

void sdu_soft_reset_interrupt_entry(int irq, void *arg)
{
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].IP = 0;
    bflb_irq_disable(SDIO_IRQn);

    sdu_software_reset_set(false);
    HR_LOGW("recv reset signal from isr, -> set SDU_WORKER_EV_RESET\r\n");
    notify_sdu_worker(SDU_WORKER_EV_RESET, 1);
}

void __attribute__((section(".tcm_code"))) sdu_interrupt_task(void *pvParameters)
{
    sdu_irq_handle = sdiowifi_get_task_handle();

    while(1) {
        sdiowifi_task_wait_notification(SDIOWIFI_WAIT_FOREVER);

        sdu_interrupt_handle();

        if (sdu_worker_ev & SDU_WORKER_EV_RESET) {
            sdu_worker_ev &= ~SDU_WORKER_EV_RESET;
            HR_LOGI("SDU_WORKER_EV_RESET\r\n");
            sdio_software_reset();
        }

        if (sdu_worker_ev & SDU_WORKER_EV_TX_WDT) {
            sdu_worker_ev &= ~SDU_WORKER_EV_TX_WDT;
            HR_LOGI("SDU_WORKER_EV_TX_WDT\r\n");
            check_tx_timeout();
        }

        if (sdio_tx_attached == 0) {
#ifdef SDIOWIFI_HEARTBEAT
#else
            HR_LOGD("sdio_tx_attached is zero.\r\n");
#endif
            bl_sdio_tx_timer_stop();
        }

        bflb_irq_enable(SDIO_IRQn);
    }
}
