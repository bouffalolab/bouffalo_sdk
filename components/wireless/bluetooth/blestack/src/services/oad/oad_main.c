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
#if !defined(CONFIG_BL_SDK)
#include "hal_boot2.h"
#include "bl_flash.h"
#include "bl_sys.h"
#include "hosal_ota.h"
#include "softcrc.h"
#include "hal_sys.h"
#else
#include "partition.h"
#include "bflb_flash.h"
#include "utils_sha256.h"
#endif
#include "bt_log.h"

#if defined(CONFIG_BL_SDK)
pt_table_stuff_config pt_table_stuff[2];
pt_table_id_type active_id;
pt_table_entry_config pt_fw_entry;
#define OTA_PARTITION_NAME_TYPE_FW    "FW"
static int fw_check_hash256(void);
#endif //CONFIG_BL_SDK

#define OTA_WRITE_FLASH_SIZE    (256*16)
#define WBUF_SIZE(CON)          (OTA_WRITE_FLASH_SIZE + bt_gatt_get_mtu(CON))
#define UPGRD_TIMEOUT           K_SECONDS(2)

static app_check_oad_cb app_check_cb = NULL;
struct oad_env_tag oad_env;

struct wflash_data_t{
    u8_t *wdata_buf;
    u16_t index;
}__packed;

static struct wflash_data_t wData;

static bool check_data_valid(struct oad_file_info *file_info)
{
    if(file_info->manu_code != oad_env.file_info.manu_code || file_info->file_ver != oad_env.upgrd_file_ver)
        return false;

    return true;
}

static void oad_notify_image_info(struct bt_conn *conn)
{
    u8_t *buf = (u8_t*)k_malloc(sizeof(u8_t)*256);
    u8_t index = 0;
    char *build_date = __DATE__;
    char *build_time = __TIME__;
    char *build_ver =  BTBLE_SDK_VER;

    if(buf){
        memset(buf,0,256);
    }else{
        BT_WARN("Buffer allocation failed\r\n");
        return;
    }
    buf[index++] = OAD_CMD_IMAG_INFO;
    if(strlen(build_date)+index <= 256){
        buf[index] =  strlen(build_date);
        memcpy(&buf[++index],build_date,strlen(build_date));
        index += strlen(build_date);
    }else{
        BT_WARN("No enough space\r\n");
    }
    if(strlen(build_time)+index <= 256){
        buf[index] =  strlen(build_time);
        memcpy(&buf[++index],build_time,strlen(build_time));
        index += strlen(build_time);
    }else{
        BT_WARN("No enough space\r\n");
    }

    if(strlen(build_ver)+index <= 256){
        buf[index] =  strlen(build_ver);
        memcpy(&buf[++index],build_ver,strlen(build_ver));
        index += strlen(build_ver);
    }else{
        BT_WARN("No enough space\r\n");
    }
    BT_WARN("Info:%s,%s,%s\r\n",build_date,build_time,build_ver);
    BT_WARN("Send:%s\r\n",bt_hex(buf,index));
    bt_oad_notify(conn,  buf, index);
    k_free(buf);
}

static void oad_notify_block_req(struct bt_conn *conn)
{
    struct net_buf_simple *buf = NET_BUF_SIMPLE(sizeof(struct oad_block_req_t) + OAD_OPCODE_SIZE);
    struct oad_block_req_t *block_req;
    
    net_buf_simple_init(buf, 0);
    *(buf->data) = OAD_CMD_IMAG_BLOCK_REQ;
    block_req = (struct oad_block_req_t *)(buf->data+1);
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

    if(status == OAD_SUCC)
    {
        BT_WARN("Submit upgrade work\r\n");
        #if !defined(CONFIG_BL_SDK)
        if(!hosal_ota_finish(1,0))
        #else
        if(!fw_check_hash256())
        #endif
            k_delayed_work_submit(&oad_env.upgrd_work, UPGRD_TIMEOUT);
        else{
            BT_WARN("Hash256 check failed");
            status=OAD_CHECK_HASH256_FAIL;
        }
    }
    
    net_buf_simple_init(buf, 0);
    *(buf->data) = OAD_CMD_IMAG_UPGRD_END;
    upgrd_end = (struct oad_upgrd_end_t *)(buf->data+1);
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
    identity = (void *)(buf->data+1);
    buf->len = sizeof(struct oad_image_identity_t)+ OAD_OPCODE_SIZE;

    identity->file_info.file_ver = oad_env.file_info.file_ver;
    identity->file_info.manu_code = oad_env.file_info.manu_code;
    identity->file_size = oad_env.cur_file_size;
    identity->crc32 = 0;

    bt_oad_notify(conn, buf->data, buf->len);
}

void ota_finish(struct k_work *work)
{

    BT_WARN("oad_upgrade\r\n");
    oad_env.file_info.file_ver = oad_env.upgrd_file_ver;

#if defined(CONFIG_BT_SETTINGS)
    struct oad_ef_info ef_info;
    memset(&ef_info,0,sizeof(struct oad_ef_info));
    bt_settings_set_bin(NV_IMG_info, (uint8_t*)&ef_info, sizeof(struct oad_ef_info));
#endif

    #if defined(CONFIG_BL_SDK)
    GLB_SW_POR_Reset();
    #else
    hal_reboot();
    #endif
}

static u8_t oad_write_flash(uint32_t filesize, uint32_t offset,u8_t *data, u16_t len)
{
    #if defined(CONFIG_BL_SDK)
    uint32_t size = 0;
    uint32_t wflash_address = 0;

    if (!oad_env.new_img_addr){

        if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW, &pt_fw_entry)) {
            BT_WARN("PtTable get active entry fail!\r\n");
            return OAD_ABORT;
        }

        oad_env.new_img_addr = pt_fw_entry.start_address[!pt_fw_entry.active_index];
        size = pt_fw_entry.max_len[!pt_fw_entry.active_index];
        
        BT_WARN("Upgrade file size is %d\r\n", oad_env.upgrd_file_size);
        if(oad_env.upgrd_file_size <= size){
            BT_WARN("flash erase\r\n");
            bflb_flash_erase(oad_env.new_img_addr, oad_env.upgrd_file_size);
        }else{
            return -1;
        }
    }

    BT_WARN("upgrd_offset is 0x%x len (%d)\r\n",oad_env.upgrd_offset,len);

    if(oad_env.w_img_end_addr <= oad_env.new_img_addr && !oad_env.w_img_end_addr ){
        wflash_address = oad_env.new_img_addr;
    }else if(oad_env.w_img_end_addr){
        wflash_address = oad_env.w_img_end_addr;
    }else{
        BT_WARN("Write flash address invalid\r\n");
    }
    
    BT_WARN("Start address : 0x%x\r\n",wflash_address);
    bflb_flash_write(wflash_address, data, len);
    oad_env.w_img_end_addr = wflash_address + len;
    BT_WARN("End address : 0x%x\r\n",wflash_address + len);
    #else
    hosal_ota_update(filesize,offset,data,len);
    #endif

    return 0;
}

static u8_t oad_image_data_handler(struct bt_conn *conn,const u8_t *data, u16_t len)
{
	u16_t left_size = 0;
	u16_t oDataLen = 0;
	static u32_t write_count = 0;

	if(!wData.wdata_buf){
		wData.wdata_buf = (u8_t*)k_malloc(WBUF_SIZE(conn));
		if(!wData.wdata_buf){
		   BT_WARN("Buf is NULL\r\n");
		   return OAD_ABORT;
		};
		memset(wData.wdata_buf,0,WBUF_SIZE(conn));
		wData.index = 0; 
	}
	
	if(wData.wdata_buf){
		left_size = /*WBUF_SIZE(conn)*/OTA_WRITE_FLASH_SIZE - wData.index;
		BT_WARN("left_size (0x%x) wData.index (0x%x) len (%d)\r\n",left_size,wData.index,len);
		if(left_size >= len ){
			memcpy((wData.wdata_buf+wData.index),data,len);
			wData.index += len;

		}else{
			oDataLen = len - left_size;
			memcpy((wData.wdata_buf+wData.index),data,left_size);
			wData.index += left_size;
			if(wData.index == OTA_WRITE_FLASH_SIZE){
                
				if(oad_write_flash(oad_env.upgrd_file_size,oad_env.hosal_offset,wData.wdata_buf,OTA_WRITE_FLASH_SIZE)){
					BT_ERR("Failed to write flash\r\n");
					return OAD_ABORT;
				}
			}else{
				BT_ERR("Unexpect result\r\n");
				return OAD_ABORT;
			}

			write_count += 1;
			oad_env.hosal_offset = write_count * OTA_WRITE_FLASH_SIZE;
#if defined(CONFIG_BT_SETTINGS)
			struct oad_ef_info ef_info;
			memcpy(&ef_info.file_info,&oad_env.file_info,sizeof(struct oad_file_info));
			ef_info.file_offset = write_count * OTA_WRITE_FLASH_SIZE;
			ef_info.last_wflash_addr = oad_env.w_img_end_addr;
			ef_info.upgrd_crc32 = oad_env.upgrd_crc32;

			bt_settings_set_bin(NV_IMG_info, (uint8_t*)&ef_info, sizeof(struct oad_ef_info));
			BT_WARN("ef_info: file ver(%lu) manu code(0x%x) file offset(0x%lx) last_adder (0x%lx)\r\n",ef_info.file_info.file_ver,ef_info.file_info.manu_code,
																					   ef_info.file_offset,ef_info.last_wflash_addr);
#endif
			wData.index = 0;
			memcpy((wData.wdata_buf+wData.index),(data+left_size),oDataLen);
			wData.index += oDataLen;
		}
	}

	oad_env.upgrd_offset += len;
	if(oad_env.upgrd_offset > oad_env.upgrd_file_size){
		return OAD_INVALID_IMAG;
	}else if(oad_env.upgrd_offset == oad_env.upgrd_file_size){
		if(wData.index){
			oad_write_flash(oad_env.upgrd_file_size,oad_env.hosal_offset,wData.wdata_buf, wData.index);
		}

		if(wData.wdata_buf){
			k_free(wData.wdata_buf);
			wData.wdata_buf = NULL;
		}

		return OAD_UPGRD_CMPLT;
	}else{
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

    switch(*data){
        case OAD_SUCC:
        {
            block_rsp = (struct oad_block_rsp_t *)data;
            if(!check_data_valid(&block_rsp->file_info)){
                status = OAD_INVALID_IMAG;
                break;
            }

            if(block_rsp->file_offset != oad_env.upgrd_offset){
                status = OAD_MALORMED_CMD;
                break;
            }
            
            rsp_data = data + OAD_BLK_RSP_DATA_OFFSET;
            status = oad_image_data_handler(conn,rsp_data, block_rsp->data_size);
            if(status == OAD_UPGRD_CMPLT){
                oad_notify_upgrd_end(conn, OAD_SUCC);
            }else if(status == OAD_REQ_MORE_DATA){
                oad_notify_block_req(conn);
            }else{
                oad_notify_upgrd_end(conn, status);
            }
        }
        break;
        case OAD_ABORT:
        {
			#if !defined(CONFIG_BL_SDK)
			bl_flash_erase(oad_env.new_img_addr, oad_env.upgrd_file_size);
			#else
            bflb_flash_erase(oad_env.new_img_addr, oad_env.upgrd_file_size);
			#endif
        }
        break;
        
        default:
            status = OAD_MALORMED_CMD;

    }
    return status;
}

static void oad_image_identity_handler(struct bt_conn *conn, const u8_t *data, u16_t len)
{
    struct bt_le_conn_param conn_param;
    struct oad_image_identity_t *identity = (struct oad_image_identity_t *)(data);
	int err = 0;

    BT_WARN("File size=[0x%lx] [0x%lx] [0x%x] [0x%lx]\r\n",identity->file_size,identity->file_info.file_ver,
                                                identity->file_info.manu_code,identity->crc32);
#if defined(CONFIG_BT_SETTINGS)
    size_t  llen = 0;
    struct oad_ef_info ef_info;

    memset(&ef_info,0,sizeof(struct oad_ef_info));
    bt_settings_get_bin(NV_IMG_info, (uint8_t*)&ef_info,sizeof(struct oad_ef_info),&llen);
    BT_WARN("ef_info: file ver(%lu) manu code(0x%x) file offset(0x%lx) last_adder (0x%lx)\r\n",ef_info.file_info.file_ver,ef_info.file_info.manu_code,
                                                                                           ef_info.file_offset,ef_info.last_wflash_addr);
#else
    oad_env.new_img_addr = 0;
	oad_env.w_img_end_addr = 0;    
#endif

    if(identity->file_info.manu_code == oad_env.file_info.manu_code &&
	    (app_check_cb)(oad_env.file_info.file_ver, identity->file_info.file_ver)){

#if defined(CONFIG_BT_SETTINGS)
        if(identity->crc32 && ef_info.upgrd_crc32 == identity->crc32){
            if(ef_info.file_offset && ef_info.file_offset <= identity->file_size){
                oad_env.upgrd_offset = ef_info.file_offset;
            }

            oad_env.new_img_addr = ef_info.last_wflash_addr;

        }else
#endif
        {
            oad_env.upgrd_offset = 0x00;
        }

        #if defined (BFLB_BLE_ENABLE_OR_DISABLE_SLAVE_PREF_CONN_PARAM_UDPATE)
        bt_conn_enable_peripheral_pref_param_update(conn, false);
        #endif
        conn_param.interval_max = 6;
        conn_param.interval_min = 6;
		conn_param.latency = 0;
		conn_param.timeout = 500;//5s
		err = bt_conn_le_param_update(conn, &conn_param);
		if(err)
			BT_WARN("fail to start conn update\r\n");
		else
			BT_WARN("start conn update\r\n");
		
        oad_env.upgrd_file_ver = identity->file_info.file_ver;
        oad_env.upgrd_file_size = identity->file_size;
        oad_env.upgrd_crc32 = identity->crc32;
        BT_WARN("Send the image block request\r\n");
        #if !defined(CONFIG_BL_SDK)
        hosal_ota_start(oad_env.upgrd_file_size);
        #endif
        oad_notify_block_req(conn);
    }else{
        
        oad_notity_image_identity(conn);
    }
}
  
static void oad_recv_callback(struct bt_conn *conn, const u8_t *data, u16_t len)
{  
    if (len){
        if (*data == OAD_CMD_IMAG_IDENTITY && ((len - 1) == sizeof(struct oad_image_identity_t))){
            oad_image_identity_handler(conn, data+1, len-1);
        }if (*data == OAD_CMD_IMAG_BLOCK_RESP){
            oad_image_block_resp_handler(conn, data+1, len-1); 
        }if(*data == OAD_CMD_IMAG_INFO){
            oad_image_info_handler(conn, data+1, len-1);
        }
    }
}

static void oad_disc_callback(struct bt_conn *conn,u8_t reason)
{
    if(wData.wdata_buf){
        k_free(wData.wdata_buf);
        wData.wdata_buf = NULL;
        wData.index = 0;
    }
}
#if defined(CONFIG_BL_SDK)

static int fw_check_hash256(void)
{
    uint32_t bin_size;
    uint32_t hash_addr;

    if (oad_env.upgrd_file_size <= 32) {
        return -1;
    }
    if(oad_env.w_img_end_addr <=32){
        return -1;
    }

    bin_size = oad_env.upgrd_file_size - 32;
    hash_addr = oad_env.w_img_end_addr - 32;

    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW, &pt_fw_entry)) 
    {
        BT_WARN("Failed to get active entries by name\r\n");
        return -1;
    }

#define CHECK_IMG_BUF_SIZE 512
    uint32_t read_size;
    sha256_context ctx_sha256;
    uint8_t sha256_result[32];
    uint8_t sha256_img[32];
    int i, offset = 0;
    uint8_t r_buf[CHECK_IMG_BUF_SIZE];

    BT_WARN("[OTA]prepare OTA partition info\r\n");
    utils_sha256_init(&ctx_sha256);
    utils_sha256_starts(&ctx_sha256);
    memset(sha256_result, 0, sizeof(sha256_result));

    offset = 0;
    while (offset < bin_size) {
        (bin_size - offset >= CHECK_IMG_BUF_SIZE) ? (read_size = CHECK_IMG_BUF_SIZE):(read_size = bin_size - offset);
        if (bflb_flash_read(oad_env.new_img_addr+offset, r_buf, read_size)) {
            BT_WARN("flash read failed \r\n");
            return -1;
        }
        utils_sha256_update(&ctx_sha256, (const uint8_t *)r_buf, read_size);
        offset += read_size;
    }

    utils_sha256_finish(&ctx_sha256, sha256_result);

    bflb_flash_read(hash_addr,sha256_img,32);
    for (i = 0; i < 32; i++) {
        BT_WARN("%02X", sha256_img[i]);
    }
    BT_WARN("\r\nHeader SET SHA256 Checksum:");
    for (i = 0; i < 32; i++) {
        BT_WARN("%02X", sha256_result[i]);
    }

    if (memcmp(sha256_result, (const void *)sha256_img, 32) != 0) {
        BT_WARN("sha256 check error\r\n");
        return -1;
    }

    pt_fw_entry.len = bin_size;
    BT_WARN("[OTA] Update PARTITION, partition len is %lu\r\n", pt_fw_entry.len);
    pt_fw_entry.active_index = !(pt_fw_entry.active_index & 0x01);
    pt_fw_entry.age++;
    int status = pt_table_update_entry(!active_id, &pt_table_stuff[active_id], &pt_fw_entry);
    if (status != 0) 
    {
        BT_WARN("pt table update fail! %d\r\n", status);
        return -1;
    }
    return 0;
}

void my_dump_partition(pt_table_stuff_config *pt_stuff)
{
    BT_WARN("======= PtTable_Config @%p=======\r\n", pt_stuff);
    BT_WARN("magicCode 0x%08X;", (unsigned int)(pt_stuff->pt_table.magicCode));
    BT_WARN(" version 0x%04X;", pt_stuff->pt_table.version);
    BT_WARN(" entryCnt %u;", pt_stuff->pt_table.entryCnt);
    BT_WARN(" age %lu;", pt_stuff->pt_table.age);
    BT_WARN(" crc32 0x%08X\r\n", (unsigned int)pt_stuff->pt_table.crc32);
    BT_WARN(" idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age \r\n");
    for (uint32_t i = 0; i < pt_stuff->pt_table.entryCnt; i++) {
        BT_WARN("[%02d] ", i);
        BT_WARN("  %02u", (pt_stuff->pt_entries[i].type));
        BT_WARN("     %u", (pt_stuff->pt_entries[i].device));
        BT_WARN("        %u", (pt_stuff->pt_entries[i].active_index));
        BT_WARN("       %8s", (pt_stuff->pt_entries[i].name));
        BT_WARN("   0x%08lx", (pt_stuff->pt_entries[i].start_address[0]));
        BT_WARN("   0x%08lx", (pt_stuff->pt_entries[i].start_address[1]));
        BT_WARN("   0x%08lx", (pt_stuff->pt_entries[i].max_len[0]));
        BT_WARN("   0x%08lx", (pt_stuff->pt_entries[i].max_len[1]));
        BT_WARN("   %lu\r\n", (pt_stuff->pt_entries[i].age));
    }
}
#endif
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

#if defined(CONFIG_BL_SDK)
    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        BT_WARN("No valid PT\r\n");
        return;
    }
    BT_WARN("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);
    //my_dump_partition(&pt_table_stuff[active_id]);
#endif
    k_delayed_work_init(&oad_env.upgrd_work, ota_finish);
}
