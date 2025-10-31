/**
  ******************************************************************************
  * @file    at_base_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>

#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_base_config.h"
#include "bflb_adc.h"
#include "bflb_boot2.h"
#include "partition.h"
#include "bflb_flash.h"
#include "bflb_gpio.h"
#include "at_ota.h"
#include "mem.h"
#include "at_through.h"
#include "bflb_adc.h"
#include "at_fs.h"
#include <sys/fcntl.h>
#include <at_main.h>

#define AT_FS_TYPE_LFS        0 // default littlefs

#define AT_FS_DELETE          0 
#define AT_FS_CREATE          1 
#define AT_FS_WRITE           2 
#define AT_FS_READ            3 
#define AT_FS_QUERY_SIZE      4 
#define AT_FS_QUERY_LIST      5 

#define OTP_PN_ADDR_START               0x100
#define OTP_QC_PN_LEN                   (24)

#define AT_GPIO_PULL_NONE     0
#define AT_GPIO_PULL_UP       1
#define AT_GPIO_PULL_DOWN     2

#define AT_CMD_GET_VERSION(v) (int)((v>>24)&0xFF),(int)((v>>16)&0xFF),(int)((v>>8)&0xFF),(int)(v&0xFF)

static int at_exe_cmd_rst(int argc, const char **argv)
{
    int i;

    at_response_string(AT_CMD_MSG_OK);

    /* stop all service */
    for (i = 0; i < AT_CMD_MAX_FUNC; i++) {
        if (at->function_ops[i].stop_func)
            at->function_ops[i].stop_func();
    }

    vTaskDelay(pdMS_TO_TICKS(100));
    bl_sys_reset_por();
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_gmr(int argc, const char **argv)
{
    char *outbuf = NULL;
    char **ctx = NULL;
    char *version = NULL;
    uint32_t core_version;
    char core_compile_time[32];

    size_t outbuf_len = 1024;
    outbuf = (char *)pvPortMalloc(outbuf_len);
    if (!outbuf)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);

    extern const char* bl_sys_version(const char ***ctx);

    core_version = at_cmd_get_version();
    at_cmd_get_compile_time(core_compile_time, sizeof(core_compile_time));

    snprintf(outbuf, outbuf_len, "AT version:%d.%d.%d.%d(%s)\r\n", AT_CMD_GET_VERSION(core_version), core_compile_time);

    while ((version = bl_sys_version(&ctx))) {
        snprintf(outbuf+strlen(outbuf), outbuf_len-strlen(outbuf), "%s\r\n", version);
    }
    snprintf(outbuf+strlen(outbuf), outbuf_len-strlen(outbuf), "compile time:%s %s\r\n",  __DATE__, __TIME__);
    AT_CMD_RESPONSE(outbuf);
    vPortFree(outbuf);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cmd(int argc, const char **argv)
{
    int i, n = 0;
    int t, q, s, e;
    char *name;
    char outbuf[64];

    memset(outbuf, 0, sizeof(outbuf));
    snprintf(outbuf, sizeof(outbuf), "+CMD:0,\"AT\",0,0,0,1\r\n");
    AT_CMD_RESPONSE(outbuf);
    for (i = 0, n = 0; i < AT_CMD_MAX_NUM && n < at->num_commands; i++) {
        name = at->commands[i]->at_name;
        if (name) {
            t = at->commands[i]->at_test_cmd ? 1 : 0;
            q = at->commands[i]->at_query_cmd ? 1 : 0;
            s = at->commands[i]->at_setup_cmd ? 1 : 0;
            e = at->commands[i]->at_exe_cmd ? 1 : 0;
            memset(outbuf, 0, sizeof(outbuf));
            snprintf(outbuf, sizeof(outbuf), "+CMD:%d,\"AT%s\",%d,%d,%d,%d\r\n", ++n, name, t, q, s, e);
            AT_CMD_RESPONSE(outbuf);
        }
    }

    return AT_RESULT_CODE_OK;
}

#if 0
static int at_setup_cmd_gslp(int argc, const char **argv)
{
    int sleep_time = 0;
    uint8_t weakup_pin = 0xFF;

    AT_CMD_PARSE_NUMBER(0, &sleep_time);

    at_response_string(AT_CMD_MSG_OK);
    vTaskDelay(pdMS_TO_TICKS(100));

    if (sleep_time <= 0) {
        hal_reboot();
    }
    else {
        hal_hbn_init(&weakup_pin, 1);
        hal_hbn_enter((uint32_t)sleep_time);
    }
    return AT_RESULT_CODE_OK;
}
#endif

static int at_exe_cmd_close_echo(int argc, const char **argv)
{
    at->echo = 0;
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_open_echo(int argc, const char **argv)
{
    at->echo = 1;
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_restore(int argc, const char **argv)
{
    int i;

    /* stop all service */
    for (i = 0; i < AT_CMD_MAX_FUNC; i++) {

        if (at->function_ops[i].stop_func)
            at->function_ops[i].stop_func();

        if (at->function_ops[i].restore_func)
            at->function_ops[i].restore_func();
    }

    at_response_string(AT_CMD_MSG_OK);
    vTaskDelay(pdMS_TO_TICKS(100));
    bl_sys_reset_por();

    return AT_RESULT_CODE_OK;
}

static int at_query_fakeout(int argc, const char **argv)
{
    at_response_string("+FAKEOUTPUT:%d\r\n", at->fakeoutput);
    at_response_string(AT_CMD_MSG_OK);
    return AT_RESULT_CODE_OK;
}

static int at_exe_fakeout(int argc, const char **argv)
{
    int enable = 0;

    AT_CMD_PARSE_NUMBER(0, &enable);

    if (enable == 0 || enable == 1) {
        at->fakeoutput = enable;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_sysram(int argc, const char **argv)
{
    int lwip_heap = 0;

#if CONFIG_ATMODULE_NETWORK
    lwip_heap = at_lwip_heap_free_size();
#endif
    //at_response_string("+SYSRAM:%d,%d", info.free_size, info.total_size);
    at_response_string("+SYSRAM:%d,%d", kfree_size(), lwip_heap);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_sysmsg(int argc, const char **argv)
{
    at_response_string("+SYSMSG:%d", at_base_config->sysmsg_cfg.byte);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_sysmsg(int argc, const char **argv)
{
    int sysmsg = 0;

    AT_CMD_PARSE_NUMBER(0, &sysmsg);

    if (sysmsg >= 0 && sysmsg <= 7) {
        at_base_config->sysmsg_cfg.byte = (uint8_t)sysmsg;
        if (at->store) {
            at_base_config_save(AT_CONFIG_KEY_SYS_MSG);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

#if 0
static int at_query_cmd_sysflash(int argc, const char **argv)
{
    return 0;
}

static int at_setup_cmd_sysflash(int argc, const char **argv)
{
    return 0;
}

static int at_query_cmd_rfpower(int argc, const char **argv)
{
    return 0;
}

static int at_setup_cmd_rfpower(int argc, const char **argv)
{
    return 0;
}

static int at_exe_cmd_sysrollback(int argc, const char **argv)
{
    return 0;
}

static int at_query_cmd_systimestamp(int argc, const char **argv)
{
    uint32_t systimestamp = (at_base_config->systime_stamp + rtc_get_timestamp_ms())/1000;
    at_response_string("+SYSTIMESTAMP:%d", systimestamp);
    return 0;
}

static int at_setup_cmd_systimestamp(int argc, const char **argv)
{
    int systimestamp;
    uint64_t time_stamp;

    AT_CMD_PARSE_NUMBER(0, &systimestamp);

    time_stamp = systimestamp;
    at_base_config->systime_stamp = time_stamp*1000 - rtc_get_timestamp_ms();
    return AT_RESULT_CODE_OK;
}
#endif

static int at_query_cmd_syslog(int argc, const char **argv)
{
    at_response_string("+SYSLOG:%d\r\n", at->syslog);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_syslog(int argc, const char **argv)
{
    int enable = 0;

    AT_CMD_PARSE_NUMBER(0, &enable);

    if (enable == 0 || enable == 1) {
        at->syslog = enable;
        at_base_config->sysmsg_cfg.syslog = enable;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (at->store) {
        at_base_config_save(AT_CONFIG_KEY_SYS_MSG);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_sysstore(int argc, const char **argv)
{
    at_response_string("+SYSSTORE:%d\r\n", at->store);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_sysstore(int argc, const char **argv)
{
    int store = 0;

    AT_CMD_PARSE_NUMBER(0, &store);

    if (store == 0 || store == 1) {
        at->store = (uint8_t)store;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

#define AVERAGE_COUNT 5
static int at_query_temp(int argc, const char **argv)
{
    struct bflb_device_s *adc;
    float average_filter = 0.0;
   
    struct bflb_adc_channel_s chan;

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_2P0V;

    chan.pos_chan = ADC_CHANNEL_TSEN_P;
    chan.neg_chan = ADC_CHANNEL_GND;

    adc = bflb_device_get_by_name("adc");
    bflb_adc_init(adc, &cfg);
    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);
    
    bflb_adc_channel_config(adc, &chan, 1);

    for (int i = 0; i < AVERAGE_COUNT; i++) {
        average_filter += bflb_adc_tsen_get_temp(adc);
    }
    average_filter = average_filter / AVERAGE_COUNT;
    printf("temp = %f\r\n", average_filter);

    at_response_string("+TEMP:%f\r\n", average_filter);

    return AT_RESULT_CODE_OK;
}

#if CONFIG_ATMODULE_EFUSE
static int at_setup_efuse_write(int argc, const char **argv)
{
    int nbytes = 0, word = 0;
    char addr[12] = {0};
    char *endptr;
    uint32_t address = 0;
    int recv_num = 0;
    struct bflb_device_s *efuse_dev;

    AT_CMD_PARSE_NUMBER(0, &nbytes);
    AT_CMD_PARSE_STRING(1, addr, sizeof(addr));
    address = strtoul(addr, &endptr, 16);
    
    if (nbytes <= 0 || nbytes > 8192) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    word = ((nbytes + 3) & ~3) >> 2;
    char *buffer = (char *)pvPortMalloc(word * 4);
    if (!buffer) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(buffer, 0, word * 4);
    
    at_response_result(AT_RESULT_CODE_OK);

    while(recv_num < nbytes) {
        recv_num += AT_CMD_DATA_RECV(buffer + recv_num, nbytes - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    efuse_dev = bflb_device_get_by_name("ef_ctrl");

    printf("efuse write 0x%x %d \r\n", address, word);
    bflb_ef_ctrl_write_direct(efuse_dev, address, (uint32_t *)buffer, word, 0);
    vPortFree(buffer);

    return AT_RESULT_CODE_SEND_OK;
}

static int at_setup_efuse_read(int argc, const char **argv)
{
    int nbytes = 0, word = 0;
    char addr[12] = {0};
    char *endptr;
    uint32_t address = 0;
    int send_num = 0;
    int reload = 0, reload_valid = 0;
    struct bflb_device_s *efuse_dev;

    AT_CMD_PARSE_NUMBER(0, &nbytes);
    AT_CMD_PARSE_STRING(1, addr, sizeof(addr));
    AT_CMD_PARSE_OPT_NUMBER(2, &reload, reload_valid);
    address = strtoul(addr, &endptr, 16);
    
    if (nbytes <= 0 || nbytes > 8192) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    
    word = ((nbytes + 3) & ~3) >> 2;
    char *buffer = (char *)pvPortMalloc(word * 4 + 2);
    if (!buffer) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(buffer, 0, word * 4 + 2);

    efuse_dev = bflb_device_get_by_name("ef_ctrl");

    printf("efuse read 0x%x %d \r\n", address, word);
    bflb_ef_ctrl_read_direct(efuse_dev, address, (uint32_t *)buffer, word, reload_valid ? reload : 0);
    
    at_write("+EFUSE-R:%d,", nbytes);
    buffer[nbytes] = '\r';
    buffer[nbytes + 1] = '\n';
    nbytes += 2;
    send_num = AT_CMD_DATA_SEND(buffer, nbytes);

    vPortFree(buffer);

    if (send_num == nbytes) {
        return AT_RESULT_CODE_OK;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }
}

static int at_setup_efuse_write_cfm(int argc, const char **argv)
{
    struct bflb_device_s *efuse_dev;

    efuse_dev = bflb_device_get_by_name("ef_ctrl");

    bflb_ef_ctrl_write_direct(efuse_dev, 0, NULL, 0, 1);

    return AT_RESULT_CODE_OK;
}
#endif

#if CONFIG_ATMODULE_FLASH
static int at_setup_flash_write(int argc, const char **argv)
{
    int nbytes = 0;
    char addr[12] = {0};
    char *endptr;
    uint32_t address = 0;
    int recv_num = 0;
    int ret = 0;

    AT_CMD_PARSE_NUMBER(0, &nbytes);
    AT_CMD_PARSE_STRING(1, addr, sizeof(addr));
    address = strtoul(addr, &endptr, 16);

    if (nbytes <= 0 || nbytes > 8192) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    char *buffer = (char *)pvPortMalloc(nbytes);
    if (!buffer) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(buffer, 0, nbytes);

    at_response_result(AT_RESULT_CODE_OK);

    while(recv_num < nbytes) {
        recv_num += AT_CMD_DATA_RECV(buffer + recv_num, nbytes - recv_num);
        printf("xxxx recv_num:%d nbytes:%d\r\n", recv_num, nbytes);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    printf("flash write 0x%x %d \r\n", address, nbytes);
    ret = bflb_flash_write(address, buffer, nbytes);
    vPortFree(buffer);

    if (ret) {
        return AT_RESULT_CODE_SEND_FAIL;
    }
    return AT_RESULT_CODE_SEND_OK;
}

static int at_setup_flash_read(int argc, const char **argv)
{
    int nbytes = 0;
    char addr[12] = {0};
    char *endptr;
    uint32_t address = 0;
    int send_num = 0;
    int ret = 0;

    AT_CMD_PARSE_NUMBER(0, &nbytes);
    AT_CMD_PARSE_STRING(1, addr, sizeof(addr));
    address = strtoul(addr, &endptr, 16);

    if (nbytes <= 0 || nbytes > 8192) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    char *buffer = (char *)pvPortMalloc(nbytes + 2);
    if (!buffer) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(buffer, 0, nbytes);

    printf("flash read 0x%x %d \r\n", address, nbytes);
    ret = bflb_flash_read(address, buffer, nbytes);

    if (ret) {
        vPortFree(buffer);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    at_write("+FLASH-R:%d,", nbytes);
    buffer[nbytes] = '\r';
    buffer[nbytes + 1] = '\n';
    nbytes += 2;
    send_num = AT_CMD_DATA_SEND(buffer, nbytes);

    vPortFree(buffer);

    if (send_num == nbytes) {
        return AT_RESULT_CODE_OK;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }
}

static int at_setup_flash_erase(int argc, const char **argv)
{
    int nbytes = 0;
    char addr[12] = {0};
    char *endptr;
    uint32_t address = 0;
    int ret = 0;

    AT_CMD_PARSE_NUMBER(0, &nbytes);
    AT_CMD_PARSE_STRING(1, addr, sizeof(addr));
    address = strtoul(addr, &endptr, 16);

    printf("flash erase 0x%x %d \r\n", address, nbytes);
    ret = bflb_flash_erase(address, nbytes);

    if (ret) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_IO_ERROR);
    }
    return AT_RESULT_CODE_OK;
}
#endif

#if CONFIG_ATMODULE_GPIO
static int at_setup_gpio_output(int argc, const char **argv)
{
    int pin, pull_state, cfgset;
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    AT_CMD_PARSE_NUMBER(0, &pin);
    AT_CMD_PARSE_NUMBER(1, &pull_state);

    if (pull_state == AT_GPIO_PULL_NONE) {
        cfgset = GPIO_OUTPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0;
    } else if (pull_state == AT_GPIO_PULL_UP) {
        cfgset = GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0;
    } else if (pull_state == AT_GPIO_PULL_DOWN) {
        cfgset = GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    bflb_gpio_init(gpio, pin, cfgset);

    return AT_RESULT_CODE_OK;
}

static int at_setup_gpio_set(int argc, const char **argv)
{
    int pin, state;
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    AT_CMD_PARSE_NUMBER(0, &pin);
    AT_CMD_PARSE_NUMBER(1, &state);

    if (state < 0 || state > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (state) {
        bflb_gpio_set(gpio, pin);    
    } else {
        bflb_gpio_reset(gpio, pin);    
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_gpio_input(int argc, const char **argv)
{
    int pin, pull_state, cfgset;
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    AT_CMD_PARSE_NUMBER(0, &pin);
    AT_CMD_PARSE_NUMBER(1, &pull_state);

    if (pull_state == AT_GPIO_PULL_NONE) {
        cfgset = GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0;
    } else if (pull_state == AT_GPIO_PULL_UP) {
        cfgset = GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0;
    } else if (pull_state == AT_GPIO_PULL_DOWN) {
        cfgset = GPIO_INPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    bflb_gpio_init(gpio, pin, cfgset);

    return AT_RESULT_CODE_OK;
}

static int at_query_gpio_input(int argc, const char **argv)
{
    int pin;
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    AT_CMD_PARSE_NUMBER(0, &pin);
    at_response_string("+IOIN=%d:%d\r\n", pin, bflb_gpio_read(gpio, pin));

    return AT_RESULT_CODE_OK;
}

static int at_setup_gpio_analog_input(int argc, const char **argv)
{
    int pin;
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    AT_CMD_PARSE_NUMBER(0, &pin);

    bflb_gpio_init(gpio, pin, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);

    return AT_RESULT_CODE_OK;
}
#endif

static int at_query_part(int argc, const char **argv)
{
    int i;
    pt_table_stuff_config *part = bflb_boot2_get_pt_config();

    for (i = 0; i < part->pt_table.entryCnt; i++) {
        at_response_string("+PART=%d,%d,\"%8s\",0x%08lx,0x%08lx,%lu,%lu\r\n",
                           part->pt_entries[i].active_index,
                           part->pt_entries[i].age, 
                           part->pt_entries[i].name,
                           part->pt_entries[i].start_address[0],
                           part->pt_entries[i].start_address[1],
                           part->pt_entries[i].max_len[0],
                           part->pt_entries[i].max_len[1]);
    }
    return AT_RESULT_CODE_OK;
}

#if CONFIG_ATMODULE_OTA 

#define OTA_BUFFER_LEN (4096)
static at_ota_handle_t g_ota_handle = NULL;
static int g_ota_recv_total = 0;
static int g_ota_recv_cnt   = 0;
static int g_ota_start = 0;

struct ota_buf {
    uint32_t len;
    uint8_t buf[0];
};

static int ota_start_process(int id, void *arg)
{
    int ota = (int)arg;

    if (ota == 0 && g_ota_handle) {
        at_ota_abort(g_ota_handle);
        g_ota_handle = NULL;
        g_ota_recv_total = 0;
    }
    return 0;
}

static int ota_trans_process(int id, void *arg)
{
    struct ota_buf *buffer = (uint8_t *)arg;
    int head_offset = 0;

    if (!g_ota_handle) {
        g_ota_handle = at_ota_start((at_ota_header_t *)buffer->buf);
        head_offset = sizeof(at_ota_header_t);
    }

    if (!g_ota_handle) {
        goto _fail;
    }

    if (at_ota_update(g_ota_handle, g_ota_recv_total, buffer->buf + head_offset, buffer->len - head_offset) != 0) {
        at_ota_abort(g_ota_handle);
        goto _fail;
    }

    if (0 == g_ota_recv_total) {
        g_ota_recv_cnt = 1;
    } else {
        g_ota_recv_cnt++;
    }
    g_ota_recv_total += (buffer->len - head_offset);

    printf("OTA-%d, l:%d, boff:%08X-%d, t:%d/%d, H:%02X:%02X:%02X:%02X\r\n",
            g_ota_recv_cnt, buffer->len,
            g_ota_recv_total+512, g_ota_recv_total+512,
            g_ota_recv_total, g_ota_handle->file_size,
            buffer[0], buffer[1], buffer[2], buffer[3]);
    vPortFree(buffer);
    return 0;

_fail:
    vPortFree(buffer);
    g_ota_handle = NULL;
    g_ota_recv_total = 0;
    return 0;
}

static int ota_finish_process(int id, void *arg)
{
    printf("ota_recv_total:%d \r\n", g_ota_recv_total);
    g_ota_recv_total = 0;

    if (at_ota_finish(g_ota_handle, 1, 0) != 0) {
        at_ota_abort(g_ota_handle);
        g_ota_handle = NULL;
        return 0;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    at_wifi_sta_disconnect();
    bl_sys_reset_por();
    return 0;
}

static int at_query_ota_start(int argc, const char **argv)
{
    at_response_string("+OTASTART:%d\r\n", g_ota_start);
    return AT_RESULT_CODE_OK;
}

static int at_setup_ota_start(int argc, const char **argv)
{
    int ota;
   
    AT_CMD_PARSE_NUMBER(0, &ota);

    if (ota != 0 && ota != 1 ) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    struct at_workq wq = {
        .pfunc = ota_start_process,
        .arg = (void *)ota,
    };
    at_workq_send(AT_EVENT_OTA, &wq, portMAX_DELAY);

    g_ota_start = ota;

    return AT_RESULT_CODE_OK;
}

static int at_setup_ota_send(int argc, const char **argv)
{
    int ret = 0;
    int len, recv_size = 0;
    struct ota_buf *buffer;

    AT_CMD_PARSE_NUMBER(0, &len);

    if (g_ota_start == 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    }
    if (len <=0 || len > OTA_BUFFER_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (g_ota_handle == NULL && len < sizeof(at_ota_header_t)) {
        printf("OTA head size is not enough\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    buffer = pvPortMalloc(sizeof(struct ota_buf) + len);
    if (!buffer) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(buffer, 0, sizeof(struct ota_buf) + len);

    at_response_string("%s%s", AT_CMD_MSG_OK, AT_CMD_MSG_WAIT_DATA);

    while(recv_size < len) {
        recv_size += AT_CMD_DATA_RECV(buffer->buf + recv_size, len - recv_size);
    }
    at_response_string("Recv %d bytes\r\n", recv_size);

    buffer->len = recv_size;
    struct at_workq wq = {
        .pfunc = ota_trans_process,
        .arg = buffer,
    };
    at_workq_send(AT_EVENT_OTA, &wq, portMAX_DELAY);

    if (len == recv_size) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }

    return ret;
}

static int at_setup_ota_finish_reset(int argc, const char **argv)
{
    if (!g_ota_handle) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    }

    struct at_workq wq = {
        .pfunc = ota_finish_process,
        .arg = NULL,
    };
    at_workq_send(0, &wq, portMAX_DELAY);

    return AT_RESULT_CODE_OK;
}
#endif 

#if CONFIG_ATMODULE_FS
static int at_setup_fs(int argc, const char **argv)
{
    int ret = -1;
    int type, opt, offset, len, recv_size = 0;
    uint8_t offset_valid = 0;
    uint8_t len_valid = 0;
    char filename[128] = {0};
    char *buffer, *buf;
    struct at_direct dir_res;
    at_dir_t dir;

    AT_CMD_PARSE_NUMBER(0, &type);
    AT_CMD_PARSE_NUMBER(1, &opt);
    AT_CMD_PARSE_STRING(2, filename, sizeof(filename));
    AT_CMD_PARSE_OPT_NUMBER(3, &offset, offset_valid);
    AT_CMD_PARSE_OPT_NUMBER(4, &len, len_valid);

    if (type != AT_FS_TYPE_LFS) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    switch (opt) {
        case AT_FS_DELETE:
            ret = at_fs_remove(filename);
        break;
        case AT_FS_CREATE:
            ret = at_fs_open(filename, O_CREAT | O_RDWR);
            if (ret < 0) {
                break;
            }
            at_fs_close(ret);
            ret = AT_RESULT_CODE_OK;
        break;
        case AT_FS_WRITE:

            if (!offset_valid || !len_valid) {
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
            }

            buffer = calloc(len, 1);
            if (!buffer) {
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
            }
            at_response_result(AT_RESULT_CODE_OK);
            AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);

            while(recv_size < len) {
                recv_size += AT_CMD_DATA_RECV(buffer + recv_size, len - recv_size);
            }

            at_response_string("Recv %d bytes\r\n", recv_size);

            if (at_write_file(filename, offset, buffer, len) != len) {
                free(buffer);
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_IO_ERROR);
            }
            free(buffer);

            if (len == recv_size) {
                ret = AT_RESULT_CODE_SEND_OK;
            } else {
                ret = AT_RESULT_CODE_SEND_FAIL;
            }
        break;
        case AT_FS_READ:
            if (!offset_valid || !len_valid) {
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
            }

            buffer = calloc(len + 1, 1);
            if (!buffer) {
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
            }

            buf = malloc(len + 32);
            if (!buf) {
                free(buffer);
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
            }

            ret = at_read_file(filename, offset, buffer, len); 
            if (ret < 0) {
                free(buf);
                free(buffer);
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_IO_ERROR);
            }
            ret = snprintf(buf, len + 32, "+FS:READ,%d,%s\r\n", ret, buffer);
            AT_CMD_DATA_SEND(buf, ret);
            free(buffer);
            free(buf);
            ret = AT_RESULT_CODE_OK;
        break;
        case AT_FS_QUERY_SIZE:
            ret = at_filesize_get(filename);
            if (ret < 0) {
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_IO_ERROR);
            }
            at_response_string("+FS:SIZE,%d\r\n", ret);
            ret = AT_RESULT_CODE_OK;
        break;
        case AT_FS_QUERY_LIST:
            dir = at_fs_opendir(filename);
            if (!dir) {
                return AT_RESULT_WITH_SUB_CODE(AT_SUB_IO_ERROR);
            }
            at_response_string("+FS:LIST\r\n");
            while (at_fs_readdir(dir, &dir_res) == 0) {
                at_response_string("%s\r\n", dir_res.d_name);
            }
            at_fs_closedir(dir);
            ret = AT_RESULT_CODE_OK;
        break;
        default:
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    
    return ret;
}
#endif

static int otp_get_mac_addr(uint8_t mac_0[6], uint8_t mac_1[6], uint8_t mac_2[6])
{
    int ret_0,ret_1,ret_2;
         
    ret_0 = bflb_efuse_read_mac_address_opt(0, mac_0, 1);
    ret_1 = bflb_efuse_read_mac_address_opt(1, mac_1, 1); 
    ret_2 = bflb_efuse_read_mac_address_opt(2, mac_2, 1);
    
    if ((ret_0 == 0) && (ret_1 == 0) && (ret_2 == 0)) {
        return -1;
    } else {        
        return 0;
    }
}

static int at_query_gmac(int argc, const char **argv)
{
    uint8_t mac0[6], mac1[6], mac2[6];

    if (otp_get_mac_addr(mac0, mac1, mac2) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_COMMON_ERROR);
    }
    at_response_string("+GMACSLOT0:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                      mac0[0], mac0[1], mac0[2], mac0[3], mac0[4], mac0[5]);
    at_response_string("+GMACSLOT1:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                      mac1[0], mac1[1], mac1[2], mac1[3], mac1[4], mac1[5]);
    at_response_string("+GMACSLOT2:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                      mac2[0], mac2[1], mac2[2], mac2[3], mac2[4], mac2[5]);
    
    return AT_RESULT_CODE_OK;
}

static int otp_get_part_number(char *pnstr_buf, int buflen)
{
    int n, off = 0;
    int is_null = 1;
    uint32_t pn_value[OTP_QC_PN_LEN / 4];
    int i,j;
    char pn_char;

    for (int i = 0; i < (OTP_QC_PN_LEN / 4); i++){
        bflb_ef_ctrl_read_direct(NULL, OTP_PN_ADDR_START + 4*i, &pn_value[i], 1, 1);
    }

    for (i = 0; i < (OTP_QC_PN_LEN / 4); i++){
        if (pn_value[i] != 0) {
            is_null = 0;
        }
    }

    if (is_null) {
        n = snprintf(pnstr_buf + off, buflen - off, "(null)");
        if (n > 0) {
            off += n;
        }
        goto __end;
    }

    for (i = 0; i < (OTP_QC_PN_LEN / 4); i++){
        for (j = 0; j < 4; j++) {
            pn_char = (pn_value[i] >> (j * 8)) & 0xFF;
            if (pn_char == 0x3){ /*end of text*/
                //mfg_atcmd_print("\n");
                //return QC_AT_CMD_RESPONSE(result, field_value);
                goto __end;
            }
            //mfg_atcmd_print("%c", pn_char);
            n = snprintf(pnstr_buf + off, buflen - off, "%c", pn_char);
            if (n > 0) {
                off += n;
            }
        }
    }

__end:
    return off;
}

static void part_number_dump(void)
{
    char buffer[64] = {0};

    otp_get_part_number(buffer, sizeof(buffer));

    printf("Part number:%s\r\n", buffer);
}

static int at_query_pn(int argc, const char **argv)
{
    int n, off = 0;
    char buffer[64];

    n = snprintf(buffer + off, sizeof(buffer), "+PN:");
    if (n > 0) {
        off += n;
    }

    n = otp_get_part_number(buffer + off, sizeof(buffer) - off);
    if (n > 0) {
        off += n;
    }

    n = snprintf(buffer + off, sizeof(buffer) - off, "\r\n");
    if (n > 0) {
        off += n;
    }
    AT_CMD_DATA_SEND(buffer, off);

    return AT_RESULT_CODE_OK;
}

static int at_setup_mfg(int argc, const char **argv)
{
    extern void mfg_config(void); 

    mfg_config();

    at_response_result(AT_RESULT_CODE_OK);

    vTaskDelay(pdMS_TO_TICKS(100));

    bl_sys_reset_por();

    return AT_RESULT_CODE_OK;
}

static void adc_vbat_init(void)
{
    struct bflb_device_s *adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);
    bflb_adc_vbat_enable(adc);
}

static int adc_vbat_get(void)
{
    struct bflb_adc_channel_s chan;
    struct bflb_adc_result_s result;
    struct bflb_device_s *adc = bflb_device_get_by_name("adc");

    chan.pos_chan = ADC_CHANNEL_VABT_HALF;
    chan.neg_chan = ADC_CHANNEL_GND;
    bflb_adc_channel_config(adc, &chan, 1);

    bflb_adc_start_conversion(adc);
    while (bflb_adc_get_count(adc) == 0) {
    }
    uint32_t raw_data = bflb_adc_read_raw(adc);

    bflb_adc_parse_result(adc, &raw_data, &result, 1);
    bflb_adc_stop_conversion(adc);

    return (uint32_t)(result.millivolt * 2);
}

static int at_query_vbat(int argc, const char **argv)
{
    int vbat_mv;
    //uint64_t time_us = bflb_mtimer_get_time_us();
    adc_vbat_init();
    vbat_mv = adc_vbat_get();   
    //time_us = bflb_mtimer_get_time_us() - time_us;

    at_response_string("+VBAT:%d\r\n",vbat_mv);

    //printf("vBat = %d mV conver_time:%lld us\r\n", (uint32_t)(vbat_mv), time_us);

    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_base_cmd[] = {
    {"+RST", NULL, NULL, NULL, at_exe_cmd_rst, 0, 0},
    {"+GMR", NULL, NULL, NULL, at_exe_cmd_gmr, 0, 0},
    {"+CMD", NULL, at_query_cmd_cmd, NULL, NULL, 0, 0},
    //{"+GSLP", NULL, NULL, at_setup_cmd_gslp, NULL, 1, 1},
    {"E0", NULL, NULL, NULL, at_exe_cmd_close_echo, 0, 0},
    {"E1", NULL, NULL, NULL, at_exe_cmd_open_echo, 0, 0},
    {"+RESTORE", NULL, NULL, NULL, at_exe_cmd_restore, 0, 0},
    {"+FAKEOUTPUT", NULL, at_query_fakeout, at_exe_fakeout, NULL, 1, 1},
#if 0
    {"+UART_CUR", NULL, at_query_cmd_uart_cur, at_setup_cmd_uart_cur, NULL, 5, 5},
    {"+UART_DEF", NULL, at_query_cmd_uart_def, at_setup_cmd_uart_def, NULL, 5, 5},
    {"+SLEEP", NULL, at_query_cmd_sleep, at_setup_cmd_sleep, NULL, 1, 1},
#endif
    {"+SYSRAM", NULL, at_query_cmd_sysram, NULL, NULL, 0, 0},
    {"+SYSMSG", NULL, at_query_cmd_sysmsg, at_setup_cmd_sysmsg, NULL, 1, 1},
#if 0
    {"+SYSFLASH", NULL, at_query_cmd_sysflash, at_setup_cmd_sysflash, NULL, 5, 5},
    {"+RFPOWER", NULL, at_query_cmd_rfpower, at_setup_cmd_rfpower, NULL, 1, 4},
    {"+SYSROLLBACK", NULL, NULL, NULL, at_exe_cmd_sysrollback, 0, 0},
    {"+SYSTIMESTAMP", NULL, at_query_cmd_systimestamp, at_setup_cmd_systimestamp, NULL, 1, 1},
#endif
    {"+SYSLOG", NULL, at_query_cmd_syslog, at_setup_cmd_syslog, NULL, 1, 1},
    //{"+SLEEPWKCFG", NULL, NULL, at_setup_cmd_sleepwkcfg, NULL, 2, 3},
    {"+SYSSTORE", NULL, at_query_cmd_sysstore, at_setup_cmd_sysstore, NULL, 1, 1},
#if 0
    {"+SYSREG", NULL, NULL, at_setup_cmd_sysreg, NULL, 2, 3},
    {"+SYSTEMP", NULL, at_query_cmd_systemp, NULL, NULL, 0, 0},
    {"+FLASH", NULL, at_query_cmd_flash, NULL, NULL, 0, 0},
#endif
    {"+TEMP", NULL, at_query_temp, NULL, NULL, 0, 0},
#if CONFIG_ATMODULE_EFUSE
    {"+EFUSE-W", NULL, NULL, at_setup_efuse_write, NULL, 2, 3},
    {"+EFUSE-R", NULL, NULL, at_setup_efuse_read, NULL, 2, 3},
    {"+EFUSE-CFM", NULL, NULL, NULL, at_setup_efuse_write_cfm, 0, 0},
#endif
#if CONFIG_ATMODULE_FLASH
    {"+FLASH-W", NULL, NULL, at_setup_flash_write, NULL, 2, 2},
    {"+FLASH-R", NULL, NULL, at_setup_flash_read, NULL, 2, 2},
    {"+FLASH-E", NULL, NULL, at_setup_flash_erase, NULL, 2, 2},
#endif
#if CONFIG_ATMODULE_GPIO
    {"+IOPUPD", NULL, NULL, at_setup_gpio_output, NULL, 2, 2},
    {"+IOOUT", NULL, NULL, at_setup_gpio_set, NULL, 2, 2},
    {"+IOIN", NULL, at_query_gpio_input, at_setup_gpio_input, NULL, 1, 1},
    {"+IORST", NULL, NULL, at_setup_gpio_analog_input, NULL, 1, 1},
#endif
    {"+PART", NULL, at_query_part, NULL, NULL, 0, 0},
#if CONFIG_ATMODULE_OTA
    {"+OTASTART", NULL, at_query_ota_start, at_setup_ota_start, NULL, 1, 1},
    {"+OTASEND", NULL, NULL, at_setup_ota_send, NULL, 1, 1},
    {"+OTAFIN", NULL, NULL, NULL, at_setup_ota_finish_reset, 0, 0},
#endif
#if CONFIG_ATMODULE_FS
    {"+FS", NULL, NULL, at_setup_fs, NULL, 2, 5},
#endif
    {"+GMAC", NULL, at_query_gmac, NULL, NULL, 0, 0},
    {"+PN", NULL, at_query_pn, NULL, NULL, 0, 0},
    {"+MFG", NULL, NULL, NULL, at_setup_mfg, 0, 0},
    {"+VBAT", NULL, at_query_vbat, NULL, NULL, 0, 0},
};

bool at_base_cmd_regist(void)
{
    at_base_config_init();

    at_port_para_set(at_base_config->uart_cfg.baudrate,
            at_base_config->uart_cfg.databits,
            at_base_config->uart_cfg.stopbits,
            at_base_config->uart_cfg.parity,
            at_base_config->uart_cfg.flow_control);

    part_number_dump();
    at_register_function(at_base_config_default, NULL);

    if (at_cmd_register(at_base_cmd, sizeof(at_base_cmd) / sizeof(at_base_cmd[0])) == 0)
        return true;
    else
        return false;
}

