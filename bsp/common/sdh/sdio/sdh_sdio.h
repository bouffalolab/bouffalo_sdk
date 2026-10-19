#ifndef __SDH_SDIO_H__
#define __SDH_SDIO_H__

#include "bflb_core.h"
#include "sdh_host.h"

#define SDIO_DEFAULT_BLOCK_SIZE (512)
#define SDIO_TRANSFER_MAX       (2048)
#define SDIO_DATA_TIMEOUT_MS    (1000)
#define SDIO_LP_CLK_HZ          (400 * 1000)
#define SDIO_DEFAULT_CLK_HZ     (50 * 1000 * 1000)
#define SDIO_HS_CLK_HZ          (50 * 1000 * 1000)

#define SDH_SDIO_BUS_WIDTH_1    (1)
#define SDH_SDIO_BUS_WIDTH_4    (4)

#define SDH_SDIO_READ           (0)
#define SDH_SDIO_WRITE          (1)

/* OCR flag, CMD5/R4 */
#define SDIO_OCR_FLAG_READY         (1UL << 31)
#define SDIO_OCR_FLAG_NUM_FUNC_MASK (0x7UL << 28)
#define SDIO_OCR_FLAG_MEM_PRESENT   (1UL << 27)
#define SDIO_OCR_VDD_MASK           (0x00ffffffUL)
#define SDIO_OCR_VDD_32             (1UL << 20)
#define SDIO_OCR_VDD_33             (1UL << 21)
#define SDIO_OCR_VDD_32_33          (SDIO_OCR_VDD_32 | SDIO_OCR_VDD_33)

/* CCCR/FBR register offsets */
#define SDIO_CCCR_CCCR_SDIO_REV     (0x00U)
#define SDIO_CCCR_IO_ENABLE         (0x02U)
#define SDIO_CCCR_IO_READY          (0x03U)
#define SDIO_CCCR_INT_ENABLE        (0x04U)
#define SDIO_CCCR_ABORT             (0x06U)
#define SDIO_CCCR_BUS_IF_CTRL       (0x07U)
#define SDIO_CCCR_CARD_CAP          (0x08U)
#define SDIO_CCCR_CIS_PTR_0         (0x09U)
#define SDIO_CCCR_CIS_PTR_1         (0x0AU)
#define SDIO_CCCR_CIS_PTR_2         (0x0BU)
#define SDIO_CCCR_POWER_CTRL        (0x12U)
#define SDIO_CCCR_SPEED             (0x13U)

#define SDIO_BUS_IF_CTRL_WIDTH_MASK (0x03U)
#define SDIO_BUS_IF_CTRL_WIDTH_1BIT (0x00U)
#define SDIO_BUS_IF_CTRL_WIDTH_4BIT (0x02U)
#define SDIO_BUS_IF_CTRL_CD_DISABLE (1U << 7)

#define SDIO_CARD_CAP_SMB           (1U << 0)
#define SDIO_CARD_CAP_LSC           (1U << 6)
#define SDIO_CARD_CAP_4BLS          (1U << 7)

#define SDIO_HIGHSPEED_SHS          (1U << 0)
#define SDIO_HIGHSPEED_EHS          (1U << 1)

#define SDIO_FBR_BASE(func)         ((uint32_t)(func) * 0x100U)
#define SDIO_FBR_STD_FUNC_IF        (0x00U)
#define SDIO_FBR_EXT_FUNC_IF        (0x01U)
#define SDIO_FBR_POWER              (0x02U)
#define SDIO_FBR_CIS_PTR_0          (0x09U)
#define SDIO_FBR_CIS_PTR_1          (0x0AU)
#define SDIO_FBR_CIS_PTR_2          (0x0BU)
#define SDIO_FBR_BLOCK_SIZE_0       (0x10U)
#define SDIO_FBR_BLOCK_SIZE_1       (0x11U)

#define SDIO_CIS_TPL_NULL           (0x00U)
#define SDIO_CIS_TPL_END            (0xFFU)
#define SDIO_CIS_TPL_MANFID         (0x20U)
#define SDIO_CIS_TPL_FUNCID         (0x21U)
#define SDIO_CIS_TPL_FUNCE          (0x22U)

struct sdh_sdio_s;

typedef void (*sdh_sdio_card_irq_handler_t)(struct sdh_sdio_s *sdio, void *arg);

struct sdh_sdio_func_s {
    struct sdh_sdio_s *card;
    uint8_t num;
    bool enabled;
    uint8_t class_code;
    uint8_t std_if;
    uint32_t cis_ptr;
    uint16_t vendor;
    uint16_t device;
    uint16_t block_size;
    uint16_t max_block_size;
    uint16_t enable_timeout_ms;
	uint16_t max_blksize;	/* maximum block size */
	uint16_t cur_blksize;	/* current block size */
};

struct sdh_sdio_s {
    struct sdh_host_s *host;
    uint32_t bus_clock_hz;
    uint32_t ocr;
    uint32_t rca;

    uint8_t cccr_version;
    uint8_t sdio_version;
    uint8_t num_funcs;
    uint8_t card_cap;
    uint8_t high_speed;
    uint32_t common_cis_ptr;
    uint16_t vendor;
    uint16_t device;
    uint16_t max_block_size;
    uint32_t max_dtr;
    bool card_irq_enabled;
    sdh_sdio_card_irq_handler_t card_irq_handler;
    void *card_irq_arg;
    /* SDIO IRQ thread context, aligns with Linux ksdioirqd model:
     * HW ISR only gives irq_sem, task context claims host then calls handler. */
    void *card_irq_sem;          /* binary semaphore, ISR -> thread */
    void *card_irq_task;         /* TaskHandle_t of ksdioirqd-like thread */
    volatile bool card_irq_stop; /* request thread exit */

    uint32_t card_flag;
#define SDH_SDIO_FLAG_MEM_PRESENT   (1U << 0)
#define SDH_SDIO_FLAG_SUP_4BIT      (1U << 1)
#define SDH_SDIO_FLAG_SUP_HIGHSPEED (1U << 2)
#define SDH_SDIO_FLAG_LOW_SPEED     (1U << 3)
#define SDH_SDIO_FLAG_HIGHSPEED_EN  (1U << 4)
#define SDH_SDIO_FLAG_SUP_MULTIBLK  (1U << 5)

    struct sdh_sdio_func_s functions[8];
};

/* ADMA scatter-gather transfer */

struct sdh_sg_entry {
    void *buffer;
    uint32_t length;
};

struct sdh_scatterlist {
    struct sdh_sg_entry *entries;
    int entry_count;
    int max_entries;
};

#ifndef SDH_ADMA_SG_COUNT_MAX
#define SDH_ADMA_SG_COUNT_MAX  (200)
#endif

#define SDH_SDIO_ADMA_FIX_INC   0  /* fixed address, no increment */
#define SDH_SDIO_ADMA_ADDR_INC  1  /* address increment after each block */

int sdh_sg_start_transfer(struct sdh_scatterlist *sgs, int sg_count,
                                 int total, struct sdh_sdio_func_s *func,
                                 unsigned int fix_inc, bool dir, unsigned int addr);

int sdh_sdio_init(struct sdh_sdio_s *sdio, struct sdh_host_s *sdh_host);
int sdh_sdio_deinit(struct sdh_sdio_s *sdio);
int sdh_sdio_scan_done(struct sdh_sdio_s *sdio);
int sdh_sdio_rescan(struct sdh_sdio_s *sdio);
int sdh_sdio_reset(struct sdh_sdio_s *sdio);

int sdh_sdio_set_clock(struct sdh_sdio_s *sdio, uint32_t freq, uint32_t *actual_freq);
int sdh_sdio_set_bus_width(struct sdh_sdio_s *sdio, uint8_t bus_width);

int sdh_sdio_claim_irq(struct sdh_sdio_func_s *func, sdh_sdio_card_irq_handler_t handler, void *arg);
int sdh_sdio_release_irq(struct sdh_sdio_func_s *func);

int sdh_sdio_readsb(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len);
int sdh_sdio_writesb(struct sdh_sdio_s *sdio, uint8_t func, uint32_t reg, uint8_t *data, uint32_t len);

void sdh_sdio_claim_host(struct sdh_sdio_func_s *func);
void sdh_sdio_release_host(struct sdh_sdio_func_s *func);

int sdh_sdio_func_enable(struct sdh_sdio_func_s *func);
int sdh_sdio_func_disable(struct sdh_sdio_func_s *func);
int sdh_sdio_func_set_block_size(struct sdh_sdio_func_s *func, unsigned int block_size);

unsigned char sdh_sdio_readb(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret);
unsigned short sdh_sdio_readw(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret);
unsigned int sdh_sdio_readl(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret);

void sdh_sdio_writeb(struct sdh_sdio_func_s *func, unsigned char b, unsigned int addr, int *err_ret);
void sdh_sdio_writew(struct sdh_sdio_func_s *func, unsigned short b, unsigned int addr, int *err_ret);
void sdh_sdio_writel(struct sdh_sdio_func_s *func, unsigned int b, unsigned int addr, int *err_ret);

int sdh_sdio_memcpy_fromio(struct sdh_sdio_func_s *func, void *dst, unsigned int addr, int count);
int sdh_sdio_memcpy_toio(struct sdh_sdio_func_s *func, unsigned int addr, const void *src, int count);

int sdh_sdio_func_readsb(struct sdh_sdio_func_s *func, void *dst, unsigned int addr, int count);
int sdh_sdio_func_writesb(struct sdh_sdio_func_s *func, unsigned int addr, const void *src, int count);

unsigned char sdh_sdio_f0_readb(struct sdh_sdio_func_s *func, unsigned int addr, int *err_ret);
void sdh_sdio_f0_writeb(struct sdh_sdio_func_s *func, unsigned char b, unsigned int addr, int *err_ret);

#endif
