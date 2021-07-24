#include <stdbool.h>
#include <string.h>
#include <net/buf.h>
#include "oad_service.h"
#include "oad.h"
#include "oad_main.h"
#ifdef CONFIG_BT_SETTINGS
#include "settings.h"
#include "ef_def.h"
#endif
#include "conn_internal.h"
#include "hal_boot2.h"
#include "bl_flash.h"
#include "bl_sys.h"
#include "log.h"

#define OTA_WRITE_FLASH_SIZE (256 * 16)
#define WBUF_SIZE(CON)       (OTA_WRITE_FLASH_SIZE + bt_gatt_get_mtu(CON))
#define UPGRD_TIMEOUT        K_SECONDS(2)

static app_check_oad_cb app_check_cb = NULL;
struct oad_env_tag oad_env;

struct wflash_data_t {
    u8_t *wdata_buf;
    u16_t index;
} __packed;

static struct wflash_data_t wData;

static bool check_data_valid(struct oad_file_info *file_info)
{
    if (file_info->manu_code != oad_env.file_info.manu_code || file_info->file_ver != oad_env.upgrd_file_ver) {
        return false;
    }

    return true;
}

static void oad_notify_image_info(struct bt_conn *conn)
{
    u8_t *buf = (u8_t *)k_malloc(sizeof(u8_t) * 256);
    u8_t index = 0;
    char *build_date = __DATE__;
    char *build_time = __TIME__;
    char *build_ver = BL_SDK_VER;

    if (buf) {
        memset(buf, 0, 256);
    } else {
        BT_WARN("Buffer allocation failed\r\n");
        return;
    }

    buf[index++] = OAD_CMD_IMAG_INFO;

    if (strlen(build_date) + index <= 256) {
        buf[index] = strlen(build_date);
        memcpy(&buf[++index], build_date, strlen(build_date));
        index += strlen(build_date);
    } else {
        BT_WARN("No enough space\r\n");
    }

    if (strlen(build_time) + index <= 256) {
        buf[index] = strlen(build_time);
        memcpy(&buf[++index], build_time, strlen(build_time));
        index += strlen(build_time);
    } else {
        BT_WARN("No enough space\r\n");
    }

    if (strlen(build_ver) + index <= 256) {
        buf[index] = strlen(build_ver);
        memcpy(&buf[++index], build_ver, strlen(build_ver));
        index += strlen(build_ver);
    } else {
        BT_WARN("No enough space\r\n");
    }

    BT_WARN("Info:%s,%s,%s\r\n", build_date, build_time, build_ver);
    BT_WARN("Send:%s\r\n", bt_hex(buf, index));
    bt_oad_notify(conn, buf, index);
    k_free(buf);
}

static void oad_notify_block_req(struct bt_conn *conn)
{
    struct net_buf_simple *buf = NET_BUF_SIMPLE(sizeof(struct oad_block_req_t) + OAD_OPCODE_SIZE);
    struct oad_block_req_t *block_req;

    net_buf_simple_init(buf, 0);
    *(buf->data) = OAD_CMD_IMAG_BLOCK_REQ;
    block_req = (struct oad_block_req_t *)(buf->data + 1);
    buf->len = sizeof(struct oad_block_req_t) + OAD_OPCODE_SIZE;

    block_req->file_info.file_ver = oad_env.upgrd_file_ver;
    block_req->file_info.manu_code = oad_env.file_info.manu_code;
    block_req->file_offset = oad_env.upgrd_offset;

    bt_oad_notify(conn, buf->data, buf->len);
}

static void oad_notify_upgrd_end(struct bt_conn *conn, u8_t status)
{
    struct net_buf_simple *buf = NET_BUF_SIMPLE(sizeof(struct oad_upgrd_end_t) + OAD_OPCODE_SIZE);
    struct oad_upgrd_end_t *upgrd_end;

    if (status == OAD_SUCC) {
        BT_WARN("Submit upgrade work\r\n");
        k_delayed_work_submit(&oad_env.upgrd_work, UPGRD_TIMEOUT);
    }

    net_buf_simple_init(buf, 0);
    *(buf->data) = OAD_CMD_IMAG_UPGRD_END;
    upgrd_end = (struct oad_upgrd_end_t *)(buf->data + 1);
    buf->len = sizeof(struct oad_upgrd_end_t) + OAD_OPCODE_SIZE;
    upgrd_end->file_info.file_ver = oad_env.upgrd_file_ver;
    upgrd_end->file_info.manu_code = oad_env.file_info.manu_code;
    upgrd_end->status = status;

    bt_oad_notify(conn, buf->data, buf->len);
}

static void oad_notity_image_identity(struct bt_conn *conn)
{
    struct net_buf_simple *buf = NET_BUF_SIMPLE(sizeof(struct oad_image_identity_t));
    struct oad_image_identity_t *identity;

    net_buf_simple_init(buf, 0);
    *(buf->data) = OAD_CMD_IMAG_IDENTITY;
    identity = (void *)(buf->data + 1);
    buf->len = sizeof(struct oad_image_identity_t) + OAD_OPCODE_SIZE;

    identity->file_info.file_ver = oad_env.file_info.file_ver;
    identity->file_info.manu_code = oad_env.file_info.manu_code;
    identity->file_size = oad_env.cur_file_size;
    identity->crc32 = 0;

    bt_oad_notify(conn, buf->data, buf->len);
}

void oad_upgrade(struct k_work *work)
{
    int ret = 0;
    HALPartition_Entry_Config ptEntry;

    BT_WARN("oad_upgrade\r\n");
    oad_env.file_info.file_ver = oad_env.upgrd_file_ver;
#if defined(CONFIG_BT_SETTINGS)

    struct oad_ef_info ef_info;
    memset(&ef_info, 0, sizeof(struct oad_ef_info));
    bt_settings_set_bin(NV_IMG_info, (uint8_t *)&ef_info, sizeof(struct oad_ef_info));

#endif

    ret = hal_boot2_get_active_entries_byname((uint8_t *)"FW", &ptEntry);

    if (ret) {
        BT_WARN("Failed to get active entries by name\r\n");
        return;
    }

    ptEntry.len = oad_env.upgrd_file_size;
    hal_boot2_update_ptable(&ptEntry);
    bl_sys_reset_system();
}

static u8_t oad_write_flash(u8_t *data, u16_t len)
{
    uint32_t size = 0;
    uint32_t wflash_address = 0;

    if (!oad_env.new_img_addr) {
        if (hal_boot2_partition_addr_inactive("FW", (uint32_t *)&oad_env.new_img_addr, &size)) {
            BT_WARN("New img address is null\r\n");
            return OAD_ABORT;
        }

        BT_WARN("Upgrade file size is %d\r\n", oad_env.upgrd_file_size);

        if (oad_env.upgrd_file_size <= size) {
            bl_flash_erase(oad_env.new_img_addr, oad_env.upgrd_file_size);
        } else {
            return -1;
        }
    }

    BT_WARN("upgrd_offset is 0x%x len (%d)\r\n", oad_env.upgrd_offset, len);

    if (oad_env.w_img_end_addr <= oad_env.new_img_addr && !oad_env.w_img_end_addr) {
        wflash_address = oad_env.new_img_addr;
    } else if (oad_env.w_img_end_addr) {
        wflash_address = oad_env.w_img_end_addr;
    } else {
        BT_WARN("Write flash address invalid\r\n");
    }

    BT_WARN("Start address : 0x%x\r\n", wflash_address);
    bl_flash_write(wflash_address, data, len);
    oad_env.w_img_end_addr = wflash_address + len;
    BT_WARN("End address : 0x%x\r\n", wflash_address + len);

    return 0;
}

static u8_t oad_image_data_handler(struct bt_conn *conn, const u8_t *data, u16_t len)
{
    u16_t left_size = 0;
    u16_t wlen = 0;
#if defined(CONFIG_BT_SETTINGS)
    static u32_t write_count = 0;
#endif

    if (!wData.wdata_buf) {
        wData.wdata_buf = (u8_t *)k_malloc(WBUF_SIZE(conn));

        if (!wData.wdata_buf) {
            BT_WARN("Buf is NULL\r\n");
            return OAD_ABORT;
        };

        memset(wData.wdata_buf, 0, WBUF_SIZE(conn));

        wData.index = 0;
    }

    if (wData.wdata_buf) {
        left_size = WBUF_SIZE(conn) - wData.index;
        BT_WARN("left_size (0x%x) wData.index (0x%x) len (%d)\r\n", left_size, wData.index, len);

        if (left_size >= len) {
            memcpy((wData.wdata_buf + wData.index), data, len);
            wData.index += len;

            if (wData.index >= OTA_WRITE_FLASH_SIZE && wData.index <= WBUF_SIZE(conn)) {
                wlen = wData.index - OTA_WRITE_FLASH_SIZE;
                oad_write_flash(wData.wdata_buf, OTA_WRITE_FLASH_SIZE);
#if defined(CONFIG_BT_SETTINGS)
                write_count += 1;
                struct oad_ef_info ef_info;
                memcpy(&ef_info.file_info, &oad_env.file_info, sizeof(struct oad_file_info));
                ef_info.file_offset = write_count * OTA_WRITE_FLASH_SIZE;
                ef_info.last_wflash_addr = oad_env.w_img_end_addr;
                ef_info.upgrd_crc32 = oad_env.upgrd_crc32;

                bt_settings_set_bin(NV_IMG_info, (uint8_t *)&ef_info, sizeof(struct oad_ef_info));
                BT_WARN("ef_info: file ver(%d) manu code(0x%x) file offset(0x%x) last_adder (0x%x)\r\n", ef_info.file_info.file_ver, ef_info.file_info.manu_code,
                        ef_info.file_offset, ef_info.last_wflash_addr);
#endif
                wData.index = 0;
                memcpy((wData.wdata_buf + wData.index), (wData.wdata_buf + OTA_WRITE_FLASH_SIZE), wlen);
                wData.index += wlen;
            }

        } else {
            BT_WARN("No space for store data\r\n");
        }
    }

    oad_env.upgrd_offset += len;

    if (oad_env.upgrd_offset > oad_env.upgrd_file_size) {
        return OAD_INVALID_IMAG;
    } else if (oad_env.upgrd_offset == oad_env.upgrd_file_size) {
        if (wData.index) {
            oad_write_flash(wData.wdata_buf, wData.index);
        }

        if (wData.wdata_buf) {
            k_free(wData.wdata_buf);
            wData.wdata_buf = NULL;
        }

        return OAD_UPGRD_CMPLT;
    } else {
        return OAD_REQ_MORE_DATA;
    }
}

static void oad_image_info_handler(struct bt_conn *conn, const u8_t *data, u16_t len)
{
    oad_notify_image_info(conn);
}

static u8_t oad_image_block_resp_handler(struct bt_conn *conn, const u8_t *data, u16_t len)
{
    struct oad_block_rsp_t *block_rsp;
    const u8_t *rsp_data;
    u8_t status = OAD_SUCC;

    switch (*data) {
        case OAD_SUCC: {
            block_rsp = (struct oad_block_rsp_t *)data;

            if (!check_data_valid(&block_rsp->file_info)) {
                status = OAD_INVALID_IMAG;
                break;
            }

            if (block_rsp->file_offset != oad_env.upgrd_offset) {
                status = OAD_MALORMED_CMD;
                break;
            }

            rsp_data = data + OAD_BLK_RSP_DATA_OFFSET;
            status = oad_image_data_handler(conn, rsp_data, block_rsp->data_size);

            if (status == OAD_UPGRD_CMPLT) {
                oad_notify_upgrd_end(conn, OAD_SUCC);
            } else if (status == OAD_REQ_MORE_DATA) {
                oad_notify_block_req(conn);
            } else {
                oad_notify_upgrd_end(conn, status);
            }
        } break;

        case OAD_ABORT: {
            bl_flash_erase(oad_env.new_img_addr, oad_env.upgrd_file_size);
        } break;

        default:
            status = OAD_MALORMED_CMD;
    }

    return status;
}

static void oad_image_identity_handler(struct bt_conn *conn, const u8_t *data, u16_t len)
{
    struct oad_image_identity_t *identity = (struct oad_image_identity_t *)(data);

    BT_WARN("File size=[0x%x] [0x%x] [0x%x] [0x%x]\r\n", identity->file_size, identity->file_info.file_ver,
            identity->file_info.manu_code, identity->crc32);
#if defined(CONFIG_BT_SETTINGS)
    size_t llen = 0;
    struct oad_ef_info ef_info;

    memset(&ef_info, 0, sizeof(struct oad_ef_info));
    bt_settings_get_bin(NV_IMG_info, (uint8_t *)&ef_info, sizeof(struct oad_ef_info), &llen);
    BT_WARN("ef_info: file ver(%d) manu code(0x%x) file offset(0x%x) last_adder (0x%x)\r\n", ef_info.file_info.file_ver, ef_info.file_info.manu_code,
            ef_info.file_offset, ef_info.last_wflash_addr);
#endif

    if (identity->file_info.manu_code == oad_env.file_info.manu_code &&
        (app_check_cb)(oad_env.file_info.file_ver, identity->file_info.file_ver)) {
#if defined(CONFIG_BT_SETTINGS)

        if (identity->crc32 && ef_info.upgrd_crc32 == identity->crc32) {
            if (ef_info.file_offset && ef_info.file_offset <= identity->file_size) {
                oad_env.upgrd_offset = ef_info.file_offset;
            }

            oad_env.new_img_addr = ef_info.last_wflash_addr;

        } else
#endif
        {
            oad_env.upgrd_offset = 0x00;
        }

        oad_env.upgrd_file_ver = identity->file_info.file_ver;
        oad_env.upgrd_file_size = identity->file_size;
        oad_env.upgrd_crc32 = identity->crc32;
        BT_WARN("Send the image block request\r\n");
        oad_notify_block_req(conn);
    } else {
        oad_notity_image_identity(conn);
    }
}

static void oad_recv_callback(struct bt_conn *conn, const u8_t *data, u16_t len)
{
    if (len) {
        if (*data == OAD_CMD_IMAG_IDENTITY && ((len - 1) == sizeof(struct oad_image_identity_t))) {
            oad_image_identity_handler(conn, data + 1, len - 1);
        }

        if (*data == OAD_CMD_IMAG_BLOCK_RESP) {
            oad_image_block_resp_handler(conn, data + 1, len - 1);
        }

        if (*data == OAD_CMD_IMAG_INFO) {
            oad_image_info_handler(conn, data + 1, len - 1);
        }
    }
}

static void oad_disc_callback(struct bt_conn *conn, u8_t reason)
{
    if (wData.wdata_buf) {
        k_free(wData.wdata_buf);
        wData.wdata_buf = NULL;
        wData.index = 0;
    }
}

void oad_service_enable(app_check_oad_cb cb)
{
    //todo: get current file info for oad_env.fileinfo

    app_check_cb = cb;
    oad_env.file_info.file_ver = LOCAL_FILE_VER;
    oad_env.file_info.manu_code = LOCAL_MANU_CODE;
    oad_env.new_img_addr = 0;
    bt_oad_service_enable();
    bt_oad_register_recv_cb(oad_recv_callback);
    bt_oad_register_disc_cb(oad_disc_callback);

    k_delayed_work_init(&oad_env.upgrd_work, oad_upgrade);
}
