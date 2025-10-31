/**
  ******************************************************************************
  * @file    at_ble_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "at_main.h"
#include "at_core.h"

//////////////////////////////////
#include <FreeRTOS.h>
#include <timers.h>
#include <semphr.h>
#include "utils_hex.h"

#include "at_main.h"
#include "at_core.h"
#include "at_ble_config.h"
#include "at_ble_main.h"

#define AT_BLE_CMD_PRINTF printf
#define AT_BLE_SCAN_DEBUG_OUTPUT 0

static int get_mac_from_string(char *string, uint8_t mac[6])
{
    int i, j = 0;
    char mac_string[13];

    if (strlen(string) != 17) {
        return -1;
    }

    memset(mac_string, 0, sizeof(mac_string));
    for (i = 0; i < strlen(string); i++) {
        if (i % 3 == 2) {
            if (string[i] != ':') {
                return -1;
            }
        } else {
            mac_string[j++] = string[i];
        }
    }

    if (utils_hex2bin((const char *)mac_string, strlen(mac_string), mac, 6) > 0) {
        return 0;
    }

    return -1;
}

static void hex_to_string(uint8_t *hex, int hex_len, char *string, int string_len)
{
    int i;

    memset(string, 0, string_len);
    for (i = 0; i < hex_len; i++) {
        snprintf(string + strlen(string), string_len - strlen(string), "%02x", hex[i]);
    }
}

static int at_query_cmd_ble_init(int argc, const char **argv)
{
    at_response_string("+BLEINIT:%d\r\n", at_ble_config->work_role);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_init(int argc, const char **argv)
{
    int role = 0;

    AT_CMD_PARSE_NUMBER(0, &role);

    if (role != BLE_DISABLE && role != BLE_CLIENT && role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (at_ble_init(role) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    at_ble_config->work_role = role;

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_addr(int argc, const char **argv)
{
    uint8_t addr[6];
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    at_ble_get_public_addr(addr);
    at_response_string("+BLEADDR:\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
            addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_addr(int argc, const char **argv)
{
    char addr_string[18];
    uint8_t addr[6];

    AT_CMD_PARSE_STRING(0, addr_string, sizeof(addr_string));
    if (get_mac_from_string(addr_string, addr) != 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    at_ble_set_public_addr(addr);
    
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_name(int argc, const char **argv)
{
    at_response_string("+BLENAME:%s\r\n", at_ble_config->ble_name);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_name(int argc, const char **argv)
{
    char ble_name[29+1];

    AT_CMD_PARSE_STRING(0, ble_name, sizeof(ble_name));
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    strlcpy(at_ble_config->ble_name, ble_name, sizeof(at_ble_config->ble_name));
    if(bt_set_name(at_ble_config->ble_name))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    if (at->store)
        at_ble_config_save(AT_CONFIG_KEY_BLE_NAME);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_scan_param(int argc, const char **argv)
{
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    at_response_string("+BLESCANPARAM:%d,%d,%d,%d,%d\r\n",
            at_ble_config->scan_param.scan_type,
            at_ble_config->scan_param.own_addr_type,
            at_ble_config->scan_param.filter_policy,
            at_ble_config->scan_param.scan_interval,
            at_ble_config->scan_param.scan_window);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_scan_param(int argc, const char **argv)
{
    int scan_type = 0;
    int own_addr_type = 0;
    int filter_policy = 0;
    int scan_interval = 0;
    int scan_window = 0;

    AT_CMD_PARSE_NUMBER(0, &scan_type);
    AT_CMD_PARSE_NUMBER(1, &own_addr_type);
    AT_CMD_PARSE_NUMBER(2, &filter_policy);
    AT_CMD_PARSE_NUMBER(3, &scan_interval);
    AT_CMD_PARSE_NUMBER(4, &scan_window);
    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if (scan_type != 0 && scan_type != 1)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (own_addr_type != 0 && own_addr_type != 1 && own_addr_type != 2 && own_addr_type != 3)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (filter_policy != 0 && filter_policy != 1 && filter_policy != 2 && filter_policy != 3)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (scan_interval < 0x4 || scan_interval > 0x4000)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (scan_window < 0x4 || scan_window > 0x4000)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    at_ble_config->scan_param.scan_type = scan_type;
    at_ble_config->scan_param.own_addr_type = own_addr_type;
    at_ble_config->scan_param.filter_policy = filter_policy;
    at_ble_config->scan_param.scan_interval = scan_interval;
    at_ble_config->scan_param.scan_window = scan_window;

    return AT_RESULT_CODE_OK;
}

static int at_ble_scan_callback(uint8_t addr_type, uint8_t *addr, int8_t rssi, uint8_t evtype, uint8_t *data, int len)
{
    char ble_scan_result[200] = {0};
    char scan_data[65] = {0};
    char *adv_data = "";
    char *scan_rsp_data = "";

    hex_to_string(data, len, scan_data, sizeof(scan_data));
    if (evtype == 0x00 || evtype == 0x01 || evtype == 0x02 || evtype == 0x03) {
        adv_data = scan_data; // adv
    } else if (evtype == 0x04)  {
        scan_rsp_data = scan_data; //scan rsp
    }

    snprintf(ble_scan_result, sizeof(ble_scan_result), "+BLE:SCAN:\"%02x:%02x:%02x:%02x:%02x:%02x\",%d,%s,%s,%d\r\n",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
            rssi,
            adv_data,
            scan_rsp_data,
            addr_type);
    AT_CMD_RESPONSE(ble_scan_result);
    #if AT_BLE_SCAN_DEBUG_OUTPUT
    AT_BLE_CMD_PRINTF(ble_scan_result);
    #endif
    return 0;
}

static int at_setup_cmd_ble_scan(int argc, const char **argv)
{
    int enable = 0;

    AT_CMD_PARSE_NUMBER(0, &enable);

    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if (enable != 0 && enable != 1)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (enable == 0) {
        at_ble_scan_stop();
    }
    else if (enable == 1) {
        at_ble_scan_start(at_ble_scan_callback);
    }
    
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_scan_rsp_data(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    char scan_rsp_data[62 + 1];

    AT_CMD_PARSE_STRING(0, scan_rsp_data, sizeof(scan_rsp_data));

    at_ble_config->scan_rsp_data.len = utils_hex2bin(scan_rsp_data, strlen(scan_rsp_data), at_ble_config->scan_rsp_data.data, sizeof(at_ble_config->scan_rsp_data.data));
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_adv_param(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    at_response_string("+BLEADVPARAM:%d,%d,%d,%d\r\n",
            at_ble_config->adv_param.adv_int_min, at_ble_config->adv_param.adv_int_max,
            at_ble_config->adv_param.adv_type,
            at_ble_config->adv_param.channel_map);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_adv_param(int argc, const char **argv)
{
    int adv_int_min = 0;
    int adv_int_max = 0;
    int adv_type = 0;
    int channel_map = 0;
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &adv_int_min);
    AT_CMD_PARSE_NUMBER(1, &adv_int_max);
    AT_CMD_PARSE_NUMBER(2, &adv_type);
    AT_CMD_PARSE_NUMBER(3, &channel_map);

    if (adv_int_min < 0x0020 || adv_int_min > 0x4000)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (adv_int_max < 0x0020 || adv_int_max > 0x4000)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (adv_int_min > adv_int_max)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (adv_type < 0 || adv_type > 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (channel_map != 1 && channel_map != 2 && channel_map != 4 && channel_map != 7)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    at_ble_config->adv_param.adv_int_min = adv_int_min;
    at_ble_config->adv_param.adv_int_max = adv_int_max;
    at_ble_config->adv_param.adv_type = adv_type;
    at_ble_config->adv_param.channel_map = channel_map;

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_adv_data(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    char adv_data[62 + 1];

    AT_CMD_PARSE_STRING(0, adv_data, sizeof(adv_data));

    at_ble_config->adv_data.len = utils_hex2bin(adv_data, strlen(adv_data), at_ble_config->adv_data.data, sizeof(at_ble_config->adv_data.data));
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_ble_adv_start(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if (at_ble_adv_start() != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_ble_adv_stop(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if (at_ble_adv_stop() != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_conn(int argc, const char **argv)
{

    int i;
    uint8_t addr[6];
    int conn_num = 0;

    for (i = 0; i < BLE_CONN_MAX_NUM; i++) {
        if (at_ble_is_valid_conn_idx(i) && at_ble_is_connected(i) && at_ble_conn_get_addr(i, addr)) {
            at_response_string("+BLECONN:%d,\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n", i, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
            conn_num++;
        }
    }

    if (conn_num == 0) {
        at_response_string("+BLECONN:\r\n");
    }
    
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_conn(int argc, const char **argv)
{
    int conn_index = 0;
    char addr_string[18];
    uint8_t remote_address[6];
    int addr_type_valid = 0, addr_type = 0;
    int timeout_valid = 0, timeout = 5;

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_STRING(1, addr_string, sizeof(addr_string));
    AT_CMD_PARSE_OPT_NUMBER(2, &addr_type, addr_type_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &timeout, timeout_valid);

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (get_mac_from_string(addr_string, remote_address) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (addr_type_valid) {
        if (addr_type != 0 && addr_type != 1)
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (timeout_valid) {
        if (timeout < 3 || timeout > 30)
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if (!at_ble_conn(conn_index, remote_address, addr_type, timeout))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_TIMEOUT);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_conn_param(int argc, const char **argv)
{
    int min_interval = 0;
    int max_interval = 0;
    int cur_interval = 0;
    int latency = 0;
    int timeout = 0;
    int i = 0;
    int connected_num = 0;

    for (i = 0; i < BLE_CONN_MAX_NUM; i++) {
        if (at_ble_is_connected(i) && at_ble_conn_get_param(i, &min_interval, &max_interval, &cur_interval, &latency, &timeout)) {
            at_response_string("+BLECONNPARAM:%d,%d,%d,%d,%d,%d\r\n",
                    i, min_interval, max_interval, cur_interval, latency, timeout);
            connected_num++;
        }
    }

    if (connected_num <= 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_conn_param(int argc, const char **argv)
{
    int conn_index = 0;
    int min_interval = 0;
    int max_interval = 0;
    int latency = 0;
    int timeout = 0;

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &min_interval);
    AT_CMD_PARSE_NUMBER(2, &max_interval);
    AT_CMD_PARSE_NUMBER(3, &latency);
    AT_CMD_PARSE_NUMBER(4, &timeout);

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (min_interval < 0x6 || min_interval > 0xC80)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (max_interval < 0x6 || max_interval > 0xC80)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (min_interval > max_interval)
       return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (latency < 0x0 || latency > 0x1F3)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (timeout < 0xA || timeout > 0xC80)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
        

    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_conn_update_param(conn_index, min_interval, max_interval, latency, timeout))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_disconn(int argc, const char **argv)
{
    int conn_index = 0;

    AT_CMD_PARSE_NUMBER(0, &conn_index);

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_disconn(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_datalen(int argc, const char **argv)
{
    int conn_index = 0;
    int tx_octets =  0;
    int tx_time   =  0;
    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &tx_octets);
    AT_CMD_PARSE_NUMBER(2, &tx_time);
    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (tx_octets < 0x1B || tx_octets > 0xFB)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (tx_time <= 0x0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (at_ble_conn_update_datalen(conn_index, tx_octets,tx_time))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_exchange_mtu(int argc, const char **argv)
{
    int conn_index = 0;

    AT_CMD_PARSE_NUMBER(0, &conn_index);

    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_conn_update_mtu(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_gatts_service(int argc, const char **argv)
{
    int i;
    uint8_t uuid[16];
    uint8_t type;
    char uuid_string[33];
    uint8_t uuid_type;
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    for (i = 0; i < BLE_SRV_MAX_NUM; i++) {
        if (at_ble_gatts_service_get(i, uuid, &type,&uuid_type)) {
            if(uuid_type==0)
                hex_to_string(uuid, 2, uuid_string, sizeof(uuid_string));
            else
                hex_to_string(uuid, sizeof(uuid), uuid_string, sizeof(uuid_string));
            at_response_string("+BLEGATTSSRV:%d,\"%s\",%d,%d\r\n", i, uuid_string, type,uuid_type);
        }
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_service_create(int argc, const char **argv)
{
    int srv_idx = 0;
    char srv_uuid_string[33]={0};
    int srv_type = 0;
    uint8_t srv_uuid[16]={0};
    int srv_uuid_len = 0;
    int uuid_type = 0;
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &srv_idx);
    AT_CMD_PARSE_STRING(1, srv_uuid_string, sizeof(srv_uuid_string));
    AT_CMD_PARSE_NUMBER(2, &srv_type);
    AT_CMD_PARSE_NUMBER(3, &uuid_type);

    if (srv_idx < 0 || srv_idx >= BLE_SRV_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    srv_uuid_len = utils_hex2bin(srv_uuid_string, strlen(srv_uuid_string), srv_uuid, sizeof(srv_uuid));
    if (srv_uuid_len != 16&&srv_uuid_len != 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (srv_type != BLE_GATT_SRV_PRIMARY && srv_type != BLE_GATT_SRV_SECONDARY)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (uuid_type != 0 && uuid_type != 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(srv_uuid_len==2&&uuid_type != 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(srv_uuid_len==16&&uuid_type!= 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_gatts_service_set(srv_idx, srv_uuid, srv_type,uuid_type)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_service_delete(int argc, const char **argv)
{
    int srv_idx = 0;

    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &srv_idx);

    if (srv_idx < 0 || srv_idx >= BLE_SRV_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (!at_ble_gatts_service_del(srv_idx)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_service_register(int argc, const char **argv)
{
    int enable = 0;

    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &enable);
    if(enable!= 0 && enable!= 1)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_gatts_service_register(enable))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_gatts_char(int argc, const char **argv)
{
    int i, j;
    uint8_t uuid[16];
    uint32_t prop;
    uint32_t perm;
    uint8_t uuidtype;
    char uuid_string[33];

    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    for (i = 0; i < BLE_SRV_MAX_NUM; i++) {
        for (j = 0; j < BLE_CHAR_MAX_NUM; j++) {
            if (at_ble_gatts_service_char_get(i, j, uuid, &prop,&perm,&uuidtype)) {
                if(uuidtype==0)
                    hex_to_string(uuid, 2, uuid_string, sizeof(uuid_string));
                else
                    hex_to_string(uuid, sizeof(uuid), uuid_string, sizeof(uuid_string));
                at_response_string("+BLEGATTSCHAR:%d,%d,\"%s\",%d,%d,%d\r\n", i, j, uuid_string, prop,perm,uuidtype);
            }
        }
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_char_create(int argc, const char **argv)
{
    int srv_idx = 0;
    int char_idx = 0;
    char char_uuid_string[33];
    int char_prop = 0;
    int char_perm = 0;
    int all_prop = BLE_GATT_CHAR_PROP_READ|BLE_GATT_CHAR_PROP_WRITE_WITHOUT_RESP|BLE_GATT_CHAR_PROP_NOTIFY|BLE_GATT_CHAR_PROP_WRITE|BLE_GATT_CHAR_PROP_INDICATE;
    int all_perm= BLE_GATT_PERM_READ|BLE_GATT_PERM_WRITE;
    uint8_t char_uuid[16];
    int char_uuid_len = 0;
    int uuid_type = 0;
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &srv_idx);
    AT_CMD_PARSE_NUMBER(1, &char_idx );
    AT_CMD_PARSE_STRING(2, char_uuid_string, sizeof(char_uuid_string));
    AT_CMD_PARSE_NUMBER(3, &char_prop);
    AT_CMD_PARSE_NUMBER(4, &char_perm);
    AT_CMD_PARSE_NUMBER(5, &uuid_type);
    if (srv_idx < 0 || srv_idx >= BLE_SRV_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_idx < 0 || char_idx >= BLE_CHAR_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    char_uuid_len = utils_hex2bin(char_uuid_string, strlen(char_uuid_string), char_uuid, sizeof(char_uuid));
    if (char_uuid_len != 16&&char_uuid_len != 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (uuid_type != 0 && uuid_type != 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(char_uuid_len==2&&uuid_type != 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(char_uuid_len==16&&uuid_type!= 2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_prop & (~all_prop))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_perm&(~all_perm))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_gatts_service_char_set(srv_idx, char_idx, char_uuid, char_prop,char_perm,uuid_type)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_notify(int argc, const char **argv)
{
    int srv_idx = 0;
    int char_idx = 0;
    int length = 0;
    int recv_num = 0;
    int send_num = 0;

    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &srv_idx);
    AT_CMD_PARSE_NUMBER(1, &char_idx );
    AT_CMD_PARSE_NUMBER(2, &length);

    if (srv_idx < 0 || srv_idx >= BLE_SRV_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_idx < 0 || char_idx >= BLE_CHAR_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (length <= 0 || length > 244) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    char *buffer = (char *)pvPortMalloc(length);
    if (!buffer) {
        AT_BLE_CMD_PRINTF("malloc %d bytes failed\r\n", length);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    at_response_result(AT_RESULT_CODE_OK);

    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < length) {
       recv_num += AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    send_num = at_ble_gatts_service_notify(srv_idx, char_idx, buffer, recv_num);
    vPortFree(buffer);


    if (send_num != recv_num) {
        at_response_string("%s", AT_CMD_MSG_SEND_FAIL);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }
    at_response_string("%s", AT_CMD_MSG_SEND_OK);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_indicate(int argc, const char **argv)
{
    int srv_idx = 0;
    int char_idx = 0;
    int length = 0;
    int recv_num = 0;
    int send_num = 0;

    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &srv_idx);
    AT_CMD_PARSE_NUMBER(1, &char_idx );
    AT_CMD_PARSE_NUMBER(2, &length);

    if (srv_idx < 0 || srv_idx >= BLE_SRV_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_idx < 0 || char_idx >= BLE_CHAR_MAX_NUM)
       return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (length <= 0 || length > 244) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    char *buffer = (char *)pvPortMalloc(length);
    if (!buffer) {
        AT_BLE_CMD_PRINTF("malloc %d bytes failed\r\n", length);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < length) {
       recv_num += AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    send_num = at_ble_gatts_service_indicate(srv_idx, char_idx, buffer, recv_num);
    vPortFree(buffer);

    if (send_num != recv_num) {
        at_response_string("%s", AT_CMD_MSG_SEND_FAIL);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }
    at_response_string("%s", AT_CMD_MSG_SEND_OK);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gatts_read(int argc, const char **argv)
{
    int srv_idx = 0;
    int char_idx = 0;
    int length = 0;
    int recv_num = 0;
    int send_num = 0;

    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &srv_idx);
    AT_CMD_PARSE_NUMBER(1, &char_idx );
    AT_CMD_PARSE_NUMBER(2, &length);

    if (srv_idx < 0 || srv_idx >= BLE_SRV_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_idx < 0 || char_idx >= BLE_CHAR_MAX_NUM)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (length <= 0 || length > 244) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    char *buffer = (char *)pvPortMalloc(length);
    if (!buffer) {
        AT_BLE_CMD_PRINTF("malloc %d bytes failed\r\n", length);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < length) {
        recv_num += AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    send_num = at_ble_gatts_service_read(srv_idx, char_idx, buffer, recv_num);
    vPortFree(buffer);

    if (send_num != recv_num) {
        at_response_string("%s", AT_CMD_MSG_SEND_FAIL);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }

    at_response_string("%s", AT_CMD_MSG_SEND_OK);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gattc_service_discover(int argc, const char **argv)
{
    int conn_index = 0;

    AT_CMD_PARSE_NUMBER(0, &conn_index);

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    
    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if (!at_ble_gattc_service_discover(conn_index, 5000)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gattc_char_discover(int argc, const char **argv)
{
    int conn_index = 0;
    int srv_idx = 0;

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &srv_idx);

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (srv_idx < 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if (!at_ble_gattc_service_char_discover(conn_index, srv_idx, 5000)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gattc_write(int argc, const char **argv)
{
    int conn_index = 0;
    int srv_idx = 0;
    int char_idx = 0;
    int length = 0;
    int recv_num = 0;
    int send_num = 0;

    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &srv_idx);
    AT_CMD_PARSE_NUMBER(2, &char_idx );
    AT_CMD_PARSE_NUMBER(3, &length);

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (srv_idx < 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_idx < 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (length <= 0 || length > 244) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    char *buffer = (char *)pvPortMalloc(length);
    if (!buffer) {
        AT_BLE_CMD_PRINTF("malloc %d bytes failed\r\n", length);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < length) {
        recv_num +=AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    send_num = at_ble_gattc_service_write(conn_index, srv_idx, char_idx, buffer, recv_num);
    vPortFree(buffer);

    if (send_num != recv_num) {
        at_response_string("%s", AT_CMD_MSG_SEND_FAIL);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }
    at_response_string("%s", AT_CMD_MSG_SEND_OK);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gattc_read(int argc, const char **argv)
{
    int conn_index = 0;
    int srv_idx = 0;
    int char_idx = 0;

    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &srv_idx);
    AT_CMD_PARSE_NUMBER(2, &char_idx );

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (srv_idx < 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (char_idx < 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);

    if (!at_ble_gattc_service_read(conn_index, srv_idx, char_idx, 5000)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_tx_power(int argc, const char **argv)
{
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    at_response_string("+BLETXPWR:%d\r\n", at_ble_get_tx_power());
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_tx_power(int argc, const char **argv)
{
    int power = 0;

    AT_CMD_PARSE_NUMBER(0, &power);

    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(power<0||power>20)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (at_ble_set_tx_power(power) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_sec_param(int argc, const char **argv)
{
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    at_response_string("+BLESECPARAM:%d\r\n",at_ble_config->ble_sec_param);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_param(int argc, const char **argv)
{
    int sec_param = 0;
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    AT_CMD_PARSE_NUMBER(0, &sec_param);
    if(sec_param < 0 || sec_param > 4)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_sec_paramter_setup(sec_param))
       return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    at_ble_config->ble_sec_param=sec_param;
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_cannel(int argc, const char **argv)
{
    int index = 0;
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);;
    AT_CMD_PARSE_NUMBER(0, &index);
    if (!at_ble_is_valid_conn_idx(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_sec_auth_cancel(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_passkey_confirm(int argc, const char **argv)
{
    int index = 0;
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    AT_CMD_PARSE_NUMBER(0, &index);
    if (!at_ble_is_valid_conn_idx(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_sec_auth_passkey_confirm(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_pairing_confirm(int argc, const char **argv)
{
    int index = 0;
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    AT_CMD_PARSE_NUMBER(0, &index);
    if (!at_ble_is_valid_conn_idx(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_sec_auth_pairing_confirm(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_passkey(int argc, const char **argv)
{
    int index = 0;
    int key   = 0;
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    AT_CMD_PARSE_NUMBER(0, &index);
    AT_CMD_PARSE_NUMBER(1, &key);
    if (!at_ble_is_valid_conn_idx(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (key > 999999|| key < 0) 
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_sec_auth_passkey(index,key))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_ble_ltk_list(int argc, const char **argv)
{
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    at_ble_get_ltk_list();
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_unpair(int argc, const char **argv)
{
    char addr_string[18];
    uint8_t remote_address[6];
    int addrtype = 0;
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    AT_CMD_PARSE_STRING(0, addr_string, sizeof(addr_string));
    AT_CMD_PARSE_NUMBER(1, &addrtype);
    if (get_mac_from_string(addr_string, remote_address) != 0)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (addrtype != 0 && addrtype != 1)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_get_unpair(remote_address,addrtype))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_sec_start(int argc, const char **argv)
{
    int index = 0;
    int lvl   = 0;
    AT_CMD_PARSE_NUMBER(0, &index);
    AT_CMD_PARSE_NUMBER(1, &lvl);
    if (at_ble_config->work_role == BLE_DISABLE)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(lvl<0||lvl>4)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_valid_conn_idx(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_sec_start_security(index,lvl))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gattc_subscribe(int argc, const char **argv)
{
    int index = 0;
    int c_handle = 0;
    int v_handle = 0;
    int val = 0;
    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &index);
    AT_CMD_PARSE_NUMBER(1, &c_handle);
    AT_CMD_PARSE_NUMBER(2, &v_handle);
    AT_CMD_PARSE_NUMBER(3, &val);
    if (!at_ble_is_valid_conn_idx(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(val!=1&&val!=2)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_subscribe(index,c_handle,v_handle,val)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_ble_gattc_unsubscribe(int argc, const char **argv)
{
    int conn_index = 0;
    int value_handle = 0;
    if (at_ble_config->work_role != BLE_CLIENT)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &value_handle );

    if (!at_ble_is_valid_conn_idx(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_is_connected(conn_index))
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (!at_ble_unsubscribe(conn_index,value_handle)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

#if defined(CONFIG_BT_BAS_SERVER)
static int at_exe_cmd_ble_bas_register(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(at_ble_register_bas()==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

static int at_exe_cmd_ble_bas_unregister(int argc, const char **argv)
{
   if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(at_ble_unregister_bas()==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

static int at_query_cmd_ble_bas_getlevel(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if(at_ble_get_battery_level()!=-1)
    {
        at_response_string("+BLE:BASLEVEL:%d\r\n",at_ble_get_battery_level());
        return AT_RESULT_CODE_OK;
    }
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

static int at_setup_cmd_ble_bas_setlevel(int argc, const char **argv)
{
    
    int conn_index = 0;
    int value_handle = 0;
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    AT_CMD_PARSE_NUMBER(0, &conn_index);
    AT_CMD_PARSE_NUMBER(1, &value_handle );
    if(value_handle<0||value_handle>100)
        return AT_RESULT_CODE_FAIL;
    if(at_ble_set_battery_level(conn_index, value_handle)!=-1)
    {
        return AT_RESULT_CODE_OK;
    }
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

#endif

static int at_exe_cmd_ble_ias_register(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(at_ble_register_ias()==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

static int at_exe_cmd_ble_ias_unregister(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(at_ble_unregister_ias()==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

#if defined (CONFIG_BT_DIS_SERVER)
static int at_setup_cmd_ble_dis_register(int argc, const char **argv)
{
    int vid_src;
    int vid;
    int pid;
    int pnp_ver;
    AT_CMD_PARSE_NUMBER(0, &vid_src);
    AT_CMD_PARSE_NUMBER(1, &vid);
    AT_CMD_PARSE_NUMBER(2, &pid);
    AT_CMD_PARSE_NUMBER(3, &pnp_ver);
    if (vid_src < 0 || vid_src > 255)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (vid < 0 || vid > 65535)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (pid < 0 || pid > 65535)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (pnp_ver < 0 || pnp_ver > 65535)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(at_ble_register_dis(vid_src, vid, pid, pnp_ver)==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);

}

static int at_exe_cmd_ble_dis_unregister(int argc, const char **argv)
{
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);

    if(at_ble_unregister_dis()==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}

static int at_setup_cmd_ble_dis_set(int argc, const char **argv)
{
    char disname_string[21+1] = {0};
    char disname_value[21+1]  = {0};
    int  disname_value_len  =  0 ;
    AT_CMD_PARSE_STRING(0, disname_string, sizeof(disname_string));
    AT_CMD_PARSE_STRING(1, disname_value, sizeof(disname_value));
    AT_CMD_PARSE_NUMBER(2, &disname_value_len);
    if (at_ble_config->work_role != BLE_SERVER)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_INIT);
    if(disname_value_len<=0 || disname_value_len >21)
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    if(at_ble_dis_set(disname_string,disname_value,disname_value_len)==0)
        return AT_RESULT_CODE_OK;
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
}
#endif

static const at_cmd_struct at_ble_cmd[] = {
    {"+BLEINIT", NULL, at_query_cmd_ble_init, at_setup_cmd_ble_init, NULL, 1, 1},
    {"+BLEADDR", NULL, at_query_cmd_ble_addr, at_setup_cmd_ble_addr, NULL, 1, 1},
    {"+BLENAME", NULL, at_query_cmd_ble_name, at_setup_cmd_ble_name, NULL, 1, 1},
    {"+BLESCANPARAM", NULL, at_query_cmd_ble_scan_param, at_setup_cmd_ble_scan_param, NULL, 5, 5},
    {"+BLESCAN", NULL, NULL, at_setup_cmd_ble_scan, NULL, 1, 1},
    {"+BLESCANRSPDATA", NULL, NULL, at_setup_cmd_ble_scan_rsp_data, NULL, 1, 1},
    {"+BLEADVPARAM", NULL, at_query_cmd_ble_adv_param, at_setup_cmd_ble_adv_param, NULL, 4, 4},
    {"+BLEADVDATA", NULL, NULL, at_setup_cmd_ble_adv_data, NULL, 1, 1},
    {"+BLEADVSTART", NULL, NULL, NULL, at_exe_cmd_ble_adv_start, 0, 0},
    {"+BLEADVSTOP", NULL, NULL, NULL, at_exe_cmd_ble_adv_stop, 0, 0},
    {"+BLECONN", NULL, at_query_cmd_ble_conn, at_setup_cmd_ble_conn, NULL, 2, 4},
    {"+BLECONNPARAM", NULL, at_query_cmd_ble_conn_param, at_setup_cmd_ble_conn_param, NULL, 5, 5},
    {"+BLEDISCONN", NULL, NULL, at_setup_cmd_ble_disconn, NULL, 1, 1},
    {"+BLEDATALEN", NULL, NULL, at_setup_cmd_ble_datalen, NULL, 3, 3},
    {"+BLEEXCHANGEMTU", NULL, NULL, at_setup_cmd_ble_exchange_mtu, NULL, 1, 1},
    {"+BLEGATTSSRV", NULL, at_query_cmd_ble_gatts_service, NULL, NULL, 0, 0},
    {"+BLEGATTSSRVCRE", NULL, NULL, at_setup_cmd_ble_gatts_service_create, NULL, 4, 4},
    {"+BLEGATTSSRVDEL", NULL, NULL, at_setup_cmd_ble_gatts_service_delete, NULL, 1, 1},
    {"+BLEGATTSREGISTER", NULL, NULL, at_setup_cmd_ble_gatts_service_register, NULL, 1, 1},
    {"+BLEGATTSCHAR", NULL, at_query_cmd_ble_gatts_char, NULL, NULL, 0, 0},
    {"+BLEGATTSCHARCRE", NULL, NULL, at_setup_cmd_ble_gatts_char_create, NULL, 6, 6},
    {"+BLEGATTSNTFY", NULL, NULL, at_setup_cmd_ble_gatts_notify, NULL, 3, 3},
    {"+BLEGATTSIND", NULL, NULL, at_setup_cmd_ble_gatts_indicate, NULL, 3, 3},
    {"+BLEGATTSRD", NULL, NULL, at_setup_cmd_ble_gatts_read, NULL, 3, 3},
    {"+BLEGATTCSRVDIS", NULL, NULL, at_setup_cmd_ble_gattc_service_discover, NULL, 1, 1},
    {"+BLEGATTCCHARDIS", NULL, NULL, at_setup_cmd_ble_gattc_char_discover, NULL, 2, 2},
    {"+BLEGATTCWR", NULL, NULL, at_setup_cmd_ble_gattc_write, NULL, 4, 4},
    {"+BLEGATTCRD", NULL, NULL, at_setup_cmd_ble_gattc_read, NULL, 3, 3},
    {"+BLEGATTCSUBSCRIBE", NULL, NULL, at_setup_cmd_ble_gattc_subscribe, NULL, 4, 4},
    {"+BLEGATTCUNSUBSCRIBE", NULL, NULL, at_setup_cmd_ble_gattc_unsubscribe, NULL, 2, 2},
    {"+BLETXPWR", NULL, at_query_cmd_ble_tx_power, at_setup_cmd_ble_tx_power, NULL, 1, 1},
    {"+BLESECPARAM", NULL, at_query_cmd_ble_sec_param, at_setup_cmd_ble_sec_param, NULL, 1, 1},
    {"+BLESECCANNEL", NULL, NULL, at_setup_cmd_ble_sec_cannel, NULL, 1, 1},
    {"+BLESECPASSKEYCONFIRM", NULL, NULL, at_setup_cmd_ble_sec_passkey_confirm, NULL, 1, 1},
    {"+BLESECPAIRINGCONFIRM", NULL, NULL, at_setup_cmd_ble_sec_pairing_confirm, NULL, 1, 1},
    {"+BLESECPASSKEY", NULL, NULL, at_setup_cmd_ble_sec_passkey, NULL, 2, 2},
    {"+BLESECGETLTKLIST", NULL, at_query_cmd_ble_ltk_list, NULL, NULL, 0, 0},
    {"+BLESECUNPAIR", NULL, NULL, at_setup_cmd_ble_sec_unpair, NULL, 2, 2},
    {"+BLESECSTART", NULL, NULL, at_setup_cmd_ble_sec_start, NULL, 2, 2},
#if defined(CONFIG_BT_BAS_SERVER)
    {"+BLEBASINIT", NULL, NULL, NULL, at_exe_cmd_ble_bas_register, 0, 0},
    {"+BLEBASDEINIT", NULL, NULL, NULL, at_exe_cmd_ble_bas_unregister, 0, 0},
    {"+BLEBASLVLGET", NULL, at_query_cmd_ble_bas_getlevel, NULL,NULL, 0, 0},
    {"+BLEBASLVLSET", NULL, NULL, at_setup_cmd_ble_bas_setlevel, NULL, 2, 2},
#endif
#if defined (CONFIG_BT_IAS_SERVER)
    {"+BLEIASINIT", NULL, NULL, NULL, at_exe_cmd_ble_ias_register, 0, 0},
    {"+BLEIASDEINIT", NULL, NULL, NULL, at_exe_cmd_ble_ias_unregister, 0, 0},
#endif
#if defined (CONFIG_BT_DIS_SERVER)
    {"+BLEDISINIT", NULL, NULL, at_setup_cmd_ble_dis_register, NULL, 4, 4},
    {"+BLEDISDEINIT", NULL, NULL, NULL, at_exe_cmd_ble_dis_unregister, 0, 0},
    {"+BLEDISSET", NULL, NULL, at_setup_cmd_ble_dis_set,NULL, 3, 3},
#endif
};

bool at_ble_cmd_regist(void)
{
    at_ble_config_init();

    at_register_function(at_ble_config_default, at_ble_stop);
    
    if (at_cmd_register(at_ble_cmd, sizeof(at_ble_cmd) / sizeof(at_ble_cmd[0])) == 0)
        return true;
    else
        return false;
}

