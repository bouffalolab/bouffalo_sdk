#include "sdiowifi_tty.h"

#if SDIOWIFI_TTY
#include <sdiowifi_platform_adapt.h>
#include <vfs.h>
#include <vfs_inode.h>
#include <vfs_register.h>
#include <sdiowifi_securec_wrap.h>

#include "sdiowifi_config.h"
#include "sdio_msg_ext_frame.h"

#include "sdio_port.h"

#define TTY_DBG 0

#define SDIOWIFI_TTY_READ_TO_TICKS sdiowifi_ms2ticks(100)

typedef struct {
    int id;
    bool inited;
    sdiowifi_streambuf_handle_t rcv_sb;

    struct sdiowifi_mgmr *sdm;
} sdiowifi_tty_dev_t;

static sdiowifi_tty_dev_t ttys[SDIOWIFI_TTY_DEV_COUNT];

static int sdiowifi_tty_open(inode_t *inode, file_t *fp)
{
    int ret = -1;
    sdiowifi_tty_dev_t *dev = NULL;

    if (!(fp != NULL && fp->node != NULL)) {
        return ret;
    }
    if (!(fp->node->refs == 1)) {
        return ret;
    }
    dev = (sdiowifi_tty_dev_t *)fp->node->i_arg;

    (void)dev;

    return 0;
}

int sdiowifi_tty_send_bsr(sdiowifi_mgmr_t *sdm, uint16_t tty, bool initial, uint16_t incr)
{
    sdiowifi_tty_dev_t *dev;
    sdiowifi_ext_frame_tty_bsr_t bsr;

    if (!(tty < SDIOWIFI_TTY_DEV_COUNT)) {
        return -1;
    }
    dev = &ttys[tty];

    MEMSET_SAFE(&bsr, sizeof(bsr), 0, sizeof(bsr));
    bsr.hdr.type = SDIOWIFI_EXT_FRAME_TYPE_TTY;
    bsr.tty_id = tty;
    bsr.type = SDIOWIFI_EXT_TTY_BSR;
    bsr.incr = incr;
    if (initial) {
        bsr.flags = SDIOWIFI_EXT_TTY_BSR_INITIAL;
        bsr.incr = sdiowifi_streambuf_spaces_available(dev->rcv_sb);
    }

    return bf1b_msg_send_frame_ext(&sdm->trcver, BF1B_MSG_TYPE_ETH_WIFI_FRAME, BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT, &bsr, sizeof(bsr), NULL, 0);
}

ssize_t sdiowifi_tty_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;
    size_t sz_rcved;
    sdiowifi_tty_dev_t *dev = NULL;
    uint16_t tty_id;

    if (!(fp != NULL && fp->node != NULL)) {
        return ret;
    }

    dev = (sdiowifi_tty_dev_t *)fp->node->i_arg;

    tty_id = dev->id;

    sz_rcved = sdiowifi_streambuf_receive(dev->rcv_sb, buf, nbytes, SDIOWIFI_TTY_READ_TO_TICKS);
    if (sz_rcved > 0) {
        sdiowifi_tty_send_bsr(dev->sdm, tty_id, false, sz_rcved);
    }

    return sz_rcved;
}

ssize_t sdiowifi_tty_write(file_t *fp, const void *buf, size_t nbytes)
{
    ssize_t sz_written = 0;
    int this_tx_len = 0;
    sdiowifi_tty_dev_t *dev = NULL;
    sdiowifi_mgmr_t *sdm;
    sdiowifi_ext_frame_tty_msg_t msg_hdr;

    if (!(fp != NULL && fp->node != NULL)) {
        return -1;
    }

    dev = (sdiowifi_tty_dev_t *)fp->node->i_arg;
    sdm = dev->sdm;

    msg_hdr.hdr.type = SDIOWIFI_EXT_FRAME_TYPE_TTY;
    msg_hdr.type = SDIOWIFI_EXT_TTY_DATA;
    msg_hdr.tty_id = dev->id;

    while (nbytes > 0) {
        this_tx_len = SDIOWIFI_TTY_MIN(2000, nbytes);

        bf1b_msg_send_frame_ext(&sdm->trcver, BF1B_MSG_TYPE_ETH_WIFI_FRAME, BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT,
                &msg_hdr, sizeof(msg_hdr), buf + sz_written, this_tx_len);
        sz_written += this_tx_len;
        nbytes -= this_tx_len;
    }

    return sz_written;
}

static const struct file_ops sdiowifi_tty_ops = {
    .open  = sdiowifi_tty_open,
    .read  = sdiowifi_tty_read,
    .write = sdiowifi_tty_write,
};

int sdiowifi_tty_init(sdiowifi_mgmr_t *sdm, int id, const char *path)
{
    int ret;
    sdiowifi_tty_dev_t *dev;

    if (!(id >= 0 && id < SDIOWIFI_TTY_DEV_COUNT)) {
        return -1;
    }
    HR_LOGI("sdiowifi_tty_init id:%d\r\n", id);

    dev = &ttys[id];

    if (dev->inited) {
        return -1;
    }

    dev->rcv_sb = sdiowifi_streambuf_create(SDIOWIFI_TTY_RCV_BUF_LEN, SDIOWIFI_TTY_TRIGGER_LEVEL);

    dev->sdm = sdm;
    ret = aos_register_driver(path, &sdiowifi_tty_ops, dev);
    if (!ret) {
        dev->inited = true;
    }

    return ret;
}

void sdiowifi_ext_frame_handle_tty_rx(sdiowifi_mgmr_t *sdm, uint16_t tty_id, uint8_t *msg, int msg_len)
{
    sdiowifi_tty_dev_t *dev;
    size_t bytes_sent;

#if TTY_DBG
    sdm_debug("[tty rx] tty_id %u, msg_len %d\r\n", tty_id, msg_len);
#endif

    if (!(tty_id < SDIOWIFI_TTY_DEV_COUNT)) {
        return;
    }
    dev = &ttys[tty_id];

    if (!(dev->inited)) {
        return;
    }

    bytes_sent = sdiowifi_streambuf_send(dev->rcv_sb, msg, msg_len, 0);

    if (bytes_sent != msg_len) {
        sdm_warning("[tty rx] WARN: NO enough space\r\n");
    }
}

#define DEMO_TTY_TEST_READ_BUF 256

// Echo what is received.
static void tty_test_task(void *parm)
{
    char buf[DEMO_TTY_TEST_READ_BUF];
    ssize_t ret;
    int fd;

    fd = aos_open(SDIOWIFI_TTY_PATH, 0);
    if (fd < 0) {
        sdiowifi_task_delete(NULL);
        return;
    }
    while (1) {
        ret = aos_read(fd, buf, DEMO_TTY_TEST_READ_BUF);
        if (ret > 0) {
            HR_LOGI("[tty] read %u Bytes\r\n", ret);
            ret = aos_write(fd, buf, ret);
        }
    }

    // unreachable
    aos_close(fd);
}

void sdiowifi_tty_test(void)
{
    sdiowifi_task_create(tty_test_task, "t_tty", 0, 1024, NULL, 27, NULL);
}

#endif
