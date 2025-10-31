.. _wifi6_api:

WIFI6
=============
API
------------------
wifi_sta_connect
^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void wifi_connect_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *config);

    Brief:
    Station connects to AP.

    Parameters:
    typedef struct {
      /* SSID of target AP */
      char ssid[MGMR_SSID_LEN];
      /* password of target AP */
      char key[MGMR_KEY_LEN];
      /* BSSID of target AP */
      char bssid_str[MGMR_BSSID_LEN];
      /* AKM of target AP, must be in uppercase,
      * OPEN: For open AP
      * WPA: For AP with WPA/PSK security (pre WPA2)
      * RSN or WPA2: For AP with WPA2/PSK security
      * SAE or WPA3: For AP with WPA3/PSK security
      */
      char akm_str[MGMR_BSSID_LEN];
      /* two frequencies can be specified on which AP will be scanned*/
      uint16_t freq1;
      uint16_t freq2;
      /* configurations for protected management frames */
      uint8_t pmf_cfg;
      /* whether to use the dhcp server provided by AP */
      uint8_t use_dhcp;
      /* listen interval for beacon */
      uint8_t listen_interval;
      /* 0 – quick scan and connect to the first matched AP
      *1 – scan on all channels, which is the default option
      */
      uint8_t scan_mode;
      /* 0 – normal connect, 1 – quick connect */
      uint8_t quick_connect;
      /* timeout of connection process */
      int timeout_ms;
    };

- 返回

  * 0 on success, a negative number otherwise.

wifi_scan
^^^^^^^^^^^^^^^^

- Command Line Entry

.. code-block:: C

    void wifi_scan_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config)

    Brief:
    Start the process of scan.

    Parameters:
    typedef struct {
      /*
      * if specified, only scan the AP whose SSID matches ssid_array
      */
      uint8_t ssid_array[MGMR_SSID_LEN];
      /*
      * if specified, only scan the AP whose BSSID matches bssid
      *c/
      uint8_t bssid[6];
      /* indicate channels on which scan is performed */
      uint8_t channels[MAX_FIXED_CHANNELS_LIMIT];
      /* duration in microseconds for which channel is scanned, default 220000 */
      uint32_t duration;
      } wifi_mgmr_scan_params_t;

- 返回

  * 0 on success, a negative number otherwise.

ping
^^^^^^^^^
- Command Line Entry

.. code-block:: C

    components/net/lwip/lwip_apps/ping/ping.c:
    void ping_cmd(char *buf, int len, int argc, char **argv);

- API

  * Please refer to LWIP raw API.

wifi_sta_info
^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void wifi_sta_info_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_sta_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns);

    Brief:
    Get IP information of the station.

    Parameters:
    addr - the IP address
    mask – network mask
    gw – gateway address
    dns – DNS server address

    返回:
    0 on success, a negative number otherwise.

.. code-block:: C

    int wifi_mgmr_sta_rssi_get(int *rssi);

    Brief:
    Get wifi RSSI.

    Parameters:
    rssi – WiFi RSSI

    返回:
    0 on success, a negative number otherwise.

.. code-block:: C

    int wifi_mgmr_tpc_pwr_get(rf_prw_table_t *power_table);

    Brief:
    get wifi power table

    Parameters:
    typedef struct rf_pwr_table {
        int8_t pwr_11b[4];
        int8_t pwr_11g[8];
        int8_t pwr_11n_ht20[8];
        int8_t pwr_11n_ht40[8];
        int8_t pwr_11ac_vht20[10];
        int8_t pwr_11ac_vht40[10];
        int8_t reserved[10];
        int8_t pwr_11ax_he20[12];
        int8_t pwr_11ax_he40[12];
        int8_t reserved2[12];
        int8_t reserved3[12];
    } rf_pwr_table_t;

    返回:
    0 on success, a negative number otherwise.

wifi_state
^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void cmd_wifi_state_get(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_state_get(void);

    Brief:
    Dump station/ap information.

    返回:
    0 on success, a negative number otherwise.

wifi_sta_rssi
^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void cmd_wifi_state_get(int argc, char **argv);

- API

.. code-block:: C

    Please refer to wifi_sta_info

wifi_sta_disconnect
^^^^^^^^^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void wifi_disconnect_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_sta_disconnect(void);

    Brief:
    Station disconnects from its AP.

- 返回

  * 0 on success, a negative number otherwise.

wifi_ap_start
^^^^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void wifi_mgmr_ap_start_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_ap_start(const wifi_mgmr_ap_params_t *config);

    Brief:
    Start wifi AP.

    Parameters:
    typedef struct {
    	/* the SSID of the AP */
    	char *ssid;
    	/* if both key and akm are NULL, the default key is 12345678 */
    	char *key;
    	/* OPEN/WPA/WPA2 */
    	char *akm;
    	/* default channel is 6 */
    	uint8_t channel;
    	/* channel type */
    	uint8_t type;
    	/* whether start dhcpd */
    	bool use_dhcpd;
        /* dhcpd pool start */
    	int start;
        /* dhcpd pool limit */
    	int limit;
        /* AP IP address */
    	uint32_t ap_ipaddr;
        /* AP IP network mask */
    	uint32_t ap_mask;
        /* STA max inactivity when connected */
    	uint32_t ap_max_inactivity;
    	/* whether use hidden SSID */
    	bool hidden_ssid;
    	/* whether enable isolation */
    	bool isolation;
    };

- 返回

  * 0 on success, a negative number otherwise.

wifi_ap_stop
^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void wifi_mgmr_ap_stop_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_ap_stop(void);

    Brief:
    Stop wifi AP.

- 返回

  * 0 on success, a negative number otherwise.

wifi_ap_conf_max_sta
^^^^^^^^^^^^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void cmd_wifi_ap_conf_max_sta(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_conf_max_sta(uint8_t max_sta_supported);

    Brief:
    Set the maximum number of station.

    Parameters:
    max_sta_supported – the maximum number of station.

- 返回

  * 0 on success, a negative number otherwise.

wifi_ap_mac_get
^^^^^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void cmd_wifi_ap_mac_get(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_ap_mac_get(uint8_t mac[6]);

    Brief:
    Get mac address of AP.

    Parameters:
    mac – MAC address of the AP.

- 返回

  * 0 on success, a negative number otherwise.

wifi_sta_list
^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void wifi_ap_sta_list_get_cmd(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_ap_sta_info_get(struct wifi_sta_basic_info *sta_info, uint8_t idx);

    Brief: get basic information of station.

    Parameters:
    sta_info:
    typedef struct wifi_sta_basic_info {
        uint8_t sta_idx;
        uint8_t is_used;
        uint8_t sta_mac[6];
        uint16_t aid;
    } wifi_sta_basic_info_t;

    idx – the index of station

- 返回

  * 0 on success, a negative number otherwise.

wifi_sniffer_on
^^^^^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void cmd_wifi_sniffer_on(int argc, char **argv);

- API

.. code-block:: C

    Int wifi_mgmr_sniffer_enable(wifi_mgmr_sniffer_item_t sniffer_item);

    Brief:
    Enable wifi sniffer.

    Parameters:
    sniffer_item –
    typedef struct wifi_mgmr_sniffer_item {
        /* interface index */
        char *itf;
        /* channel type */
        uint8_t type;
        /* frequency for Primary 20MHz channel (in MHz) */
        uint16_t prim20_freq;
        /* frequency center of the contiguous channel or center of primary 80+80 (in MHz) */
        uint16_t center1_freq;
        /* frequency center of the non-contiguous secondary 80+80 (in MHz) */
        uint16_t center2_freq;
        /* frame received callback. */
        void *cb;
        /* parameter for the monitor callback. */
        void *cb_arg;
    } wifi_mgmr_sniffer_item_t;

- 返回

  * 0 on success, a negative number otherwise.

wifi_sniffer_off
^^^^^^^^^^^^^^^^^^^^^
- Command Line Entry

.. code-block:: C

    void cmd_wifi_sniffer_off(int argc, char **argv);

- API

.. code-block:: C

    int wifi_mgmr_sniffer_disable(wifi_mgmr_sniffer_item_t sniffer_item);

    Brief:
    Disable wifi sniffer.

    Parameters:
    Please refer to wifi_sniffer_on

- 返回

  * 0 on success, a negative number otherwise.

CLI 命令
-------------

wifi_sta_connect
^^^^^^^^^^^^^^^^^^^^

用于连接 AP,连接成功后，会打印出分配的 ip 地址。

- 第一个参数表示 ssid
- 第二个参数表示 pwd

例如：

.. figure:: img/wifi_cli1.png
    :alt:

.. figure:: img/wifi_cli2.png
    :alt:

wifi_scan
^^^^^^^^^^^^^^^^^^^^

用于扫描 AP ，不需要跟参数。

例如：

.. figure:: img/wifi_cli3.png
    :alt:

ping
^^^^^^^^^^^^^^^^^^^^

用于 ping 网络。

- 第一个参数填写 ip 地址或者域名地址

例如：

.. figure:: img/wifi_cli4.png
    :alt:

wifi_sta_info
^^^^^^^^^^^^^^^^
用于查询STA（站点）的状态信息，例如RSSI（接收信号强度指示器），IP地址和功率表，不需要任何参数。

wifi_state
^^^^^^^^^^^^^^^^
用于查询与AP（接入点）的连接状态信息，不需要任何参数。

wifi_sta_rssi
^^^^^^^^^^^^^^^^^
用于查询STA（站点）的RSSI（接收信号强度指示器），不需要任何参数。

wifi_sta_disconnect
^^^^^^^^^^^^^^^^^^^^
用于与AP断开连接，不需要任何参数。

wifi_ap_start
^^^^^^^^^^^^^^^
用于初始化一个AP（接入点）。

- 第一个参数表示SSID（服务集标识）
- 第二个参数表示密钥
- 第三个参数表示信道

例如: wifi_ap_start -s bouffalolab -k bouffalolab2016 -c 11

wifi_ap_stop
^^^^^^^^^^^^^^^^^
用于关闭AP（接入点），不需要任何参数。

wifi_ap_conf_max_sta
^^^^^^^^^^^^^^^^^^^^^^^
用于设置可以连接的STA（站点）设备的最大数量。

- 第一个参数表示最大连接数

例如: wifi_ap_conf_max_sta 3

wifi_ap_mac_get
^^^^^^^^^^^^^^^^^^^
用于获取AP（接入点）的MAC地址，不需要任何参数。

wifi_sta_list
^^^^^^^^^^^^^^^^^^^
用于查询已连接的STA（站点）设备的信息，不需要任何参数。

wifi_sniffer_on
^^^^^^^^^^^^^^^^^^^
用于启用网络数据包捕获的 sniffer 模式。

- 第一个参数表示接口
- 第二个参数表示频率

例如: wifi_sniffer_on -i wl1 -f 2462

wifi_sniffer_off
^^^^^^^^^^^^^^^^^^^^
用于关闭 sniffer 模式，不需要任何参数。

