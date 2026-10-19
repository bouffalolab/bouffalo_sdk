==============
AT Command Set
==============

Introduction
============

This document introduces the AT command set and how to use it. The command set includes: basic AT commands, Wi-Fi\ :sup:`®` function AT commands, TCP/IP toolbox AT commands, and Bluetooth\ :sup:`®` LE-related AT commands.

AT command types
================

AT commands are divided into three types

- **Query command**

  - Format: ``AT+<x>?``
  - Description: Retrieves the current value of a parameter.

- **Set command**

  - Format: ``AT+<x>=<···>``
  - Description: Sets user-defined parameters to specified values.

- **Execute command**

  - Format: ``AT+<x>``
  - Description: Executes the function associated with a variable parameter, enforcing immutability within the module.

.. note::

    - Not every AT command has all three types mentioned above.
    - Currently, only strings and integers are supported as input parameters in AT commands.
    - Angle brackets ``< >`` indicate required parameters.
    - Square brackets ``[ ]`` indicate optional parameters that can be omitted. The default value of the parameter will be used instead when you omit it.
    - If the parameter you want to omit is followed by another parameter, you must use a comma to indicate it. For example: ``AT+CWJAP="ssid","password",,1``.
    - String values need to be enclosed in double quotation marks, such as: ``AT+CWSAP="AP-NAME","12345678"``.
    - Each AT command shouldn't exceed 256 bytes in length.
    - AT commands end with a new line (CR-LF), so the serial tool should be set to New Line Mode.
    - The parameter storage in AT commands is implemented using the EasyFlash library.


AT message types
================
Two types of AT messages are returned from the AT command port: responses and reports.

AT response messages
--------------------
Each AT command input returns response messages to inform the sender of the result of the AT command. The final message in a response will always be either 'OK' or 'ERROR'.


- **OK**: AT command processed successfully and returned OK
- **ERROR**: AT command error or an error occurred during execution
- **SEND OK**: Data has been sent to the protocol stack
- **SEND FAIL**: An error occurred while sending the data to the protocol stack
- **SEND CANCELLED**: Cancel sending data in Wi-Fi normal transmission mode
- **+<Command Name>:``...``**: Provides detailed results of the AT command processing to the sender.


AT message reports
------------------
AT will report important state changes or messages in the system.


- **ready**: The AT firmware is ready
- **busy p...**: Busy processing. The system is handling the previous command and can't accept new input
- **ERR CODE:<0x%08x>**: Error code for different commands
- **+CW:CONNECTING**: Wi-Fi station interface is connecting to an AP
- **+CW:CONNECTED**: Wi-Fi station interface has connected to an AP
- **+CW:ERROR,<reason_code>**: Wi-Fi station interface connection to AP failed (<reason_code>: optional failure reason)
- **+CW:GOTIP**: Wi-Fi station interface has obtained the IPv4 address
- **+CW:DISCONNECTED**: Wi-Fi station interface has disconnected from an AP
- **+CW:STA_CONNECTED <sta_mac>**: A station has connected to the Wi-Fi softAP interface of AT (<sta_mac>: MAC address)
- **+CW:STA_DISCONNECTED <sta_mac>**: A station has disconnected from the Wi-Fi softAP interface of AT (<sta_mac>: MAC address)
- **+CW:DIST_STA_IP <sta_mac>,<sta_ip>**: The Wi-Fi softAP interface of AT assigns an IP address to the station (<sta_mac>: MAC address, <sta_ip>: IP address)
- **+CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<freq_offset>,<freqcal_val>,<pairwise_cipher>,<group_cipher>,<bgn>,<wps>**: Wi-Fi scan beacon information of nearby access points
- **+CW:SCAN_DONE**: Wi-Fi scan completed
- **+CWSTATE:<state>,"<ssid>"**: Wi-Fi state change (<state>: 0=idle, 1=connecting, 2=connected, 3=disconnected, 4=error; <ssid>: SSID string)
- **+CWJAP:"<ssid>","<bssid>",<channel>,<rssi>,<authmode>**: Wi-Fi join AP result (<ssid>: SSID, <bssid>: MAC, <channel>, <rssi>, <authmode>)
- **+CWRECONNCFG:<interval>,<repeat>**: Wi-Fi reconnect configuration (<interval>: seconds, <repeat>: count)
- **+CWLAPOPT:<show_hidden>,<max>,<min>,<sort>,<filter>**: Wi-Fi scan options
- **+CWSAP:"<ssid>","********",<channel>,<authmode>,<maxconn>,<hidden>**: SoftAP configuration (password is masked for security)
- **+CWLIF:<ip>,<mac>**: List of stations connected to the SoftAP
- **+CWDHCP:<state>**: DHCP state
- **+CWDHCPS:<lease_time>,<start_ip>,<stop_ip>**: DHCP server configuration
- **+CWAUTOCONN:<state>**: Autoconnect state
- **+CIPSTAMAC:"<mac>"**: Station MAC address
- **+CIPAPMAC:"<mac>"**: SoftAP MAC address
- **+CIPSTA:<item>:"<value>"**: Station IP/gateway/netmask/IPv6
- **+CIPAP:<item>:"<value>"**: SoftAP IP/gateway/netmask/IPv6
- **+CWAPPROTO:<proto>**: SoftAP protocol
- **+CWSTAPROTO:<proto>**: Station protocol
- **+CWHOSTNAME:<hostname>**: Host name
- **+CWCOUNTRY:<policy>,"<country>"**: Country configuration
- **+CWEVT:<enable>**: Wi-Fi event enable
- **+CWANTENABLE:<dyn>,<stat>,<pin>**: Antenna configuration
- **+CWANT:<ant>**: Current antenna
- **+CWANTNUM:<num>**: Antenna count
- **+CWNETMODE:<mode>**: Network mode
- **+CIP[:<conn_id>],CONNECTED**: A network connection of which ID is <conn_id> is established (ID=0 by default)
- **+CIP[:<conn_id>],DISCONNECTED**: A network connection of which ID is <conn_id> ends (ID=0 by default)
- **+LINK_CONN:<status_type>,<link_id>,<ip_type>,<terminal_type>,<remote_ip>,<remote_port>,<local_port>**: Detailed connection info for TCP/UDP/SSL
- **+CIPSTATUS:<link_id>,"<type>","<remote_ip>",<remote_port>,<local_port>,<terminal_type>**: Connection status
- **+CIPSTART:<link_id>,"<type>","<remote_ip>",<remote_port>,<local_port>,<terminal_type>**: Start connection
- **+CIPERROR:<link_id>,<error_code>**: Network connection error
- **+CIPRECV:<link_id>,<length>**: Data received in passive mode
- **+CIPSEND:<link_id>,<length>**: Data sent in passive mode
- **+CIPSENDL:<sent_length>,<recv_length>**: Data send progress in passthrough mode
- **+CIPSENDLCFG:<report_size>,<transmit_size>**: Passthrough send config
- **+CIPDINFO:<true|false>**: Data info mode
- **+CIPEVT:<enable>**: Network event enable
- **+CIPMUX:<mode>**: Multiplex mode
- **+CIPRECVMODE:<mode>**: Passive receive mode
- **+CIPRECVBUF:<size>**: Passive receive buffer size
- **+CIPRECVLEN:<link_id>,<size>**: Passive receive data length
- **+CIPSERVER:<enable>,<port>,"<type>",<ca_enable>,<keepalive>**: Server config
- **+CIPSERVERMAXCONN:<maxconn>**: Maximum server connections
- **+CIPSSLCSNI[:<link_id>],<sni>**: SSL SNI configuration
- **+CIPSSLCPSK[:<link_id>],"<psk>","<hint>"**: SSL PSK configuration
- **+CIPSTO:<timeout>**: Server timeout
- **+CIPSNTPCFG:<enable>,<timezone>,"<server1>","<server2>","<server3>"**: SNTP configuration
- **+CIPSNTPTIME:<time>**: SNTP time
- **+CIPSNTPINTV:<interval>**: SNTP interval
- **+CIPRECONNINTV:<interval>**: Reconnect interval
- **+MDNS:<data_len>,...**: mDNS query result
- **+PING:<time>ms|TIMEOUT**: Ping result
- **+GET_CLOCK:<source>**: Clock source
- **+TWT:ACTIVE:<s>,<t>,<e>,<n>,<m>,<type>**: TWT active information
- **+TWT:INACTIVE**: TWT inactive
- **+SYSRAM:<free>,<lwip>**: System RAM information
- **+SYSMSG:<byte>**: System message configuration
- **+SYSTIMESTAMP:<timestamp>**: System timestamp
- **+SYSLOG:<enable>**: System log enable
- **+SYSSTORE:<enable>**: System store enable
- **+TEMP:<temp>**: Temperature
- **+IOIN=<pin>:<value>**: IO input value
- **+PART:<idx>,<type>,"<name>",<addr>,<size>,<used>,<total>**: Partition information
- **+OTASTART:<flag>**: OTA start
- **+FS:READ,<size>,<data>**: File read
- **+FS:SIZE,<size>**: File size
- **+FS:LIST**: File list
- **+GMACSLOT[0-2]:<mac>**: MAC slot information
- **+VBAT:<vbat_mv>**: Battery voltage
- **+IPD [<link_id>,]<length>[,"<remote_ip>",<remote_port>][,<data>]**: Data received from the network. Format depends on AT+CIPMUX, AT+CIPRECVTYPE, AT+CIPDINFO (see note below)
- **+MQTT:CONNECTED,<linkid>**: MQTT has connected to Brokers
- **+MQTT:DISCONNECTED,<linkid>**: MQTT has been disconnected from Brokers
- **+MQTT:SUBRECV,<linkid>,<topic_length>,<data_length>,"<topic>",<data>**: MQTT message received on subscribed topic
- **+MQTTUSERCFG:<linkid>,<scheme>,"<client_id>","<username>","<password>","<lwt_topic>","<lwt_msg>","<lwt_qos>"**: MQTT user config
- **+MQTTCLIENTID:<linkid>,"<client_id>"**: MQTT client ID
- **+MQTTUSERNAME:<linkid>,"<username>"**: MQTT username
- **+MQTTPASSWORD:<linkid>,"<password>"**: MQTT password
- **+MQTTCONNCFG:<linkid>,<keepalive>,<clean_session>,"<will_topic>","<will_msg>",<will_qos>,<will_retain>**: MQTT connection config
- **+MQTTSUB:<linkid>,<msg_id>,<topic>,<qos>**: MQTT subscribe
- **+MQTTSUB:ALREADY_SUBSCRIBE**: Already subscribed
- **+MQTTUNSUB:NO_UNSUBSCRIBE**: No unsubscribe
- **+MQTTSNI:<linkid>,<sni>**: MQTT SNI
- **+HTTPSTATUS:<linkid>,<result>**: HTTP status
- **+HTTPSSLCFG:<linkid>,<sslmode>,"<ca>","<cert>","<key>"**: HTTP SSL config
- **+HTTPGETSIZE:<linkid>,<content_len>**: HTTP GET size
- **+HTTPURLCFG:<linkid>,<url_size>,<url>**: HTTP URL config
- **+HTTPRECVMODE:<mode>**: HTTP receive mode
- **+HTTPRECVBUF:<size>**: HTTP receive buffer size
- **+HTTPRECVLEN:<linkid>,<size>**: HTTP receive length
- **Data in passthrough mode**: In passthrough mode, data has been received from the network or Bluetooth
- **SEND CANCELLED**: Cancel sending data in Wi-Fi normal transmission mode
- **+BLE:CONNECTED:<idx>,"<addr>",<conn_role>**: A Bluetooth LE connection established (<idx>: connection index, <conn_role>: connection role (0: master, 1: slave), <addr>: MAC address)
- **+BLE:DISCONNECTED:<idx>,"<addr>",<conn_role>**: A Bluetooth LE connection ends
- **+BLE:CONNPARAM:<idx>,<interval>,<latency>,<timeout>,<min_ce>,<max_ce>**: Bluetooth LE connection parameters update
- **+BLE:SCAN:<addr>,<rssi>,<adv_data>,<scan_rsp_data>,<addr_type>**: Bluetooth LE scan information of nearby devices
- **+BLE:DATALENUPDATED:<idx>,<tx_octets>,<tx_time>,<rx_octets>,<rx_time>**: Bluetooth LE data length updated
- **+BLE:PASSKEYDISPLAY:<passkey>**: Bluetooth pairing passkey display
- **+BLE:PASSKEYCONFIRM:<addr>PASSKEY:<passkey>**: Bluetooth pairing passkey confirm (<addr>: MAC address, <passkey>: 6-digit passkey)
- **+BLE:PASSKEYENTRY:<addr>**: Bluetooth pairing passkey entry (<addr>: MAC address)
- **+BLE:PAIRCANNELED:<addr>**: Bluetooth pairing canceled (<addr>: MAC address)
- **+BLE:PAIRINGCONFIRM:<addr>**: Bluetooth pairing confirm (<addr>: MAC address)
- **+BLE:PAIRINGCOMPLETED:<bonded> BTADDR: <addr> LTK: <ltk>**: Bluetooth pairing completed (<bonded>: Bonded/Paired, <addr>: MAC address, <ltk>: LTK)
- **+BLE:PAIRINGFAILED:<addr>**: Bluetooth pairing failed (<addr>: MAC address)
- **+BLESECGETLTKLIST:BONDADDR<addr>LTK:<ltk>**: Bluetooth LTK list (<addr>: MAC address, <ltk>: LTK)
- **+BLE:GATTWRITE:<idx>,<srv_idx>,<char_idx>,<len>,<data>**: Bluetooth GATT write (<idx>: connection index, <srv_idx>: service index, <char_idx>: characteristic index, <len>: length, <data>: data )
- **+BLE:READPERMISSION:<idx>,<srv_idx>,<char_idx>**: Bluetooth GATT server read permission event (notifies that a remote client issued a read request on characteristic <char_idx> in service <srv_idx> for connection <idx>)
- **+BLE:GATTREAD:<idx>,<srv_idx>,<char_idx>,<len>**: Bluetooth GATT read (<idx>: connection index, <srv_idx>: service index, <char_idx>: characteristic index, <len>: length)
- **+BLE:INDICATION:<status>,<srv_idx>,<char_idx>**: Bluetooth indication (<status>: 0=disable, 1=enable, 2=ack, <srv_idx>, <char_idx>)
- **+BLE:NOTIFICATION:<status>,<srv_idx>,<char_idx>**: Bluetooth notification (<status>: 0=disable, 1=enable, <srv_idx>, <char_idx>)
- **+BLE:NOTIDATA:<idx>,<srv_idx>,<char_idx>,<len>,<data>**: Bluetooth notification data (<idx>: connection index, <srv_idx>: service index, <char_idx>: characteristic index, <len>: data_length , <data>: data)
- **+BLE:SRV:<idx>,<srv_idx>,<uuid>,<type>,<start_handle>,<end_handle>**: Bluetooth service info
- **+BLE:SRVCHAR:<idx>,<srv_idx>,<char_idx>,<uuid>,<props>,<handle>,<value_handle>**: Bluetooth characteristic info
- **+BLE:MTUSIZE:<idx>,<mtu>**: Bluetooth MTU size
- **+BLE:IAS:<hex>**: Bluetooth Immediate Alert Service
- **+BLEINIT:<role>**: Bluetooth init role
- **+BLEADDR:"<addr>"**: Bluetooth address
- **+BLESTATICADDR:"<addr>"**: Bluetooth static random address
- **+BLENAME:<name>**: Bluetooth name
- **+BLESCANPARAM:<interval>,<window>,<type>,<own_addr_type>,<filter_policy>**: Bluetooth scan params
- **+BLEADVPARAM:<interval_min>,<interval_max>,<type>,<own_addr_type>**: Bluetooth adv params
- **+BLECONN:<idx>,"<addr>"**: Bluetooth connection
- **+BLECONNPARAM:<idx>,<interval>,<latency>,<timeout>,<min_ce>,<max_ce>**: Bluetooth connection params
- **+BLEGATTSSRV:<idx>,"<uuid>",<type>,<uuid_type>**: Bluetooth GATT server service
- **+BLEGATTSCHAR:<idx>,<srv_idx>,"<uuid>",<prop>,<perm>,<uuidtype>**: Bluetooth GATT server char
- **+BLETXPWR:<pwr>**: Bluetooth TX power
- **+BLESECPARAM:<param>,<level>,<bond>**: Bluetooth security param
- **+BLEBASLEVEL:<level>**: Bluetooth battery level
- **+QUITT**: AT quits from the Wi-Fi passthrough mode

.. note::

   - Format: ``id``, ``<length>``, ``<data>``

   - If ``AT+CIPMUX=0``, ``AT+CIPRECVTYPE=0``, and ``AT+CIPDINFO=1``, the output format is:
     ``+IPD:<length>,"<remote_ip>",<remote_port>,<data>``

   - If ``AT+CIPMUX=1``, ``AT+CIPRECVTYPE=<link_id>,0``, and ``AT+CIPDINFO=1``, the output format is:
     ``+IPD:<link_id>,<length>,"<remote_ip>",<remote_port>,<data>``

   - Field descriptions:

     - ``link_id``: Connection ID
     - ``length``: Length of the data
     - ``remote_ip``: Remote IP address
     - ``remote_port``: Remote port number
     - ``data``: Actual data content

   - For SSL connections in passive receive mode (``AT+CIPRECVTYPE=1``), the ``length`` returned by the AT command may not match the actual readable SSL data length.
     This is because the AT command prioritizes the readable data length at the SSL layer.
     If the SSL layer has no readable data (for example, length is 0), the AT command will return the readable data length from the socket layer instead.



===================
Basic AT commands
===================

.. table::
    :widths: 30,70
    :width: 100%
    :align: center

    +----------------------+------------------------------------------------------------------------------+
    |  Command             | Description                                                                  |
    +======================+==============================================================================+
    | `AT`_                | Test AT startup                                                              |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+GMR`_            | View version information                                                     |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+CMD?`_           | List all AT commands and types supported in current firmware                 |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+RST`_            | Restart the module                                                           |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+RESTORE`_        | Reset                                                                        |
    +----------------------+------------------------------------------------------------------------------+
    | `ATE`_               | Configure AT command echoing                                                 |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+SYSRAM?`_        | Query current remaining heap size and lwip remaining heap size               |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+SYSMSG?`_        | Query/set system prompt information                                          |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+SYSLOG?`_        | Enable or disable the AT error code prompt                                   |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+SYSSTORE?`_      | Query/set parameter storage mode                                             |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+TEMP?`_          | Query the temperature of the chip                                            |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+EFUSE-W`_        | eFuse write operation                                                        |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+EFUSE-R`_        | eFuse read operation                                                         |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+EFUSE-CFM`_      | eFuse write confirmation will write directly to eFuse                        |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+FLASH-W`_        | Flash write operation                                                        |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+FLASH-R`_        | Flash read operation                                                         |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+FLASH-E`_        | Flash erase operation                                                        |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+IOPUPD`_         | Initialize GPIO pins as pull-up/pull-down mode                               |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+IOOUT`_          | Set GPIO output high/low level                                               |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+IOIN`_           | Query/set GPIO in the input mode                                             |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+IORST`_          | Reset a GPIO's analog input mode                                             |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+PART`_           | Get information about the partitions                                         |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+OTASTART`_       | Start OTA transmission                                                       |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+OTASEND`_        | Send OTA data                                                                |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+OTAFIN`_         | OTA transmission completed                                                   |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+FS`_             | Filesystem operations                                                        |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+GMAC`_           | Query the MAC addresses of 3 slots in eFuse                                  |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+PN`_             | Query the part number                                                        |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+VBAT`_           | Query the voltage value at the supply pin (unit: mV)                         |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+MINIDUMP`_       | Dump minidump binary by hex                                                  |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+EFUSE-W-HEX`_    | eFuse write operation with hexadecimal data                                  |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+EFUSE-R-HEX`_    | eFuse read operation with hexadecimal data                                   |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+FLASH-W-HEX`_    | Flash write operation with hexadecimal data                                  |
    +----------------------+------------------------------------------------------------------------------+
    | `AT+FLASH-R-HEX`_    | Flash read operation with hexadecimal data                                   |
    +----------------------+------------------------------------------------------------------------------+


Basic AT command description
============================


AT
----------
Function: Test AT startup.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT

**Response**

.. container:: screenoutput

 .. code::

    OK

AT+GMR
----------
Function: View version.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+GMR

**Response**

.. container:: screenoutput

 .. code::

    <AT version info>
    <macsw version info>
    <phyrf version info>
    <bthost version info>
    <btblecontroller version info>
    <SDK version info>
    <compile time>

    OK

**Parameters**

-  **<AT version info>**: AT version information
-  **<macsw version info>**: macsw component version information
-  **<phyrf version info>**: phyrf component version information
-  **<bthost version info>**: bthost component version information
-  **<btblecontroller version info>**: btblecontroller component version information
-  **<SDK version info>**: SDK version information
-  **<compile time>**: Compile time

**Example**

::

    AT+GMR
    AT version:1.0.0.1(Apr 30 2025 11:47:03)
    component_version_macsw_1.6.38
    lib_version_phyrf_00f7ad715
    component_version_btble_1.8.25
    lib_version_btblecontroller_1.6.89
    component_version_sdk_2.0.75
    compile time:Apr 30 2025 11:46:58

    OK

AT+RST
----------
Function: Restart the module.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+RST

**Response**

.. container:: screenoutput

 .. code::

    OK

AT+RESTORE
----------
Function: Restore factory settings.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+RESTORE

**Response**

.. container:: screenoutput

 .. code::

    OK

AT+CMD?
----------
Function: List all AT commands and types supported in current firmware.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CMD?

**Response**

.. container:: screenoutput

 .. code::

    +CMD:<index>,<AT command name>,<support test command>,<support Query command>,<support Set command>,<support Execute command>

    OK

**Parameters**

-  **<index>**: AT command sequence number.
-  **<AT command name>**: AT command name.
-  **<support test command>**: 0 means not supported, 1 means supported.
-  **<support Query command>**: 0 means not supported, 1 means supported.
-  **<support Set command>**: 0 means not supported, 1 means supported.
-  **<support Execute command>**: 0 means not supported, 1 means supported.

**Example**

::

    AT+CMD?
    +CMD:0,\"AT\",0,0,0,1
    +CMD:1,\"AT+RST\",0,0,0,1
    +CMD:2,\"AT+GMR\",0,0,0,1
    +CMH:3,\"AT+CMD\",0,1,0,0
    +CMD:4,\"ATE0\",0,0,0,1
    +CMD:5,\"ATE1\",0,0,0,1
    +CMD:6,\"AT+RESTORE\",0,0,0,1
    +CMD:7,\"AT+SLEEP\",0,1,1,0
    OK


ATE
----------
Function: Configure AT command echoing.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    ATE0

or

::

    ATE1

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **ATE0**: Switch echo off.
-  **ATE1**: Switch echo on.

AT+SYSRAM?
----------
Function: Query current remaining heap size and lwip remaining heap size.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+SYSRAM?

**Response**

.. container:: screenoutput

 .. code::

    +SYSRAM:<remaining RAM size>,<lwip heap size>
    OK

**Parameters**

-  **<remaining RAM size>**: current system remaining heap size. Unit: byte.
-  **<lwip heap size>**: current lwip remaining heap size. Unit: byte.

**Example**

::

    AT+SYSRAM?
    +SYSRAM:148408,84044
    OK

AT+SYSMSG?
----------
Function: Query/set system prompt information.

Query command
^^^^^^^^^^^^^

**Function**

Query the current system prompt information state.

**Command**

::

    AT+SYSMSG?

**Response**

.. container:: screenoutput

 .. code::

    +SYSMSG:<state>
    OK

Set command
^^^^^^^^^^^

**Function**

Configure system prompt information.

**Command**

::

    AT+SYSMSG=<state>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<state>**:

   - Bit0: Prompt when quitting Wi-Fi passthrough mode.

     - 0: Print no prompt when quitting Wi-Fi passthrough mode.
     - 1: Print ``+QUITT`` when quitting Wi-Fi passthrough mode.

   - Bit1: Connection prompt information type.

     - 0: Use simple prompt information, such as ``+CIP:XX,CONNECTED``.
     - 1: Use detailed prompt information, such as ``+LINK_CONN:status_type,link_id,ip_type,terminal_type,remote_ip,remote_port,local_port``.

   - Bit2: Connection status prompt for Wi-Fi passthrough mode, Bluetooth LE SPP and Bluetooth SPP.

     - 0: Print no prompt information.
     - 1: Print one of the following prompt information when Wi-Fi, socket, Bluetooth LE or Bluetooth status is changed:

      ::

           - "+CIP:CONNECTED\r\n" or the message prefixed with "+LINK_CONN:"
           - "+CIP:DISCONNECTED\r\n"
           - "+CW:CONNECTING\r\n"
           - "+CW:CONNECTED\r\n"
           - "+CW:GOTIP\r\n"
           - "+CW:ERROR\r\n"
           - "+CW:DISCONNECTED\r\n"
           - "+CW:SCAN_DONE\r\n"
           - the message prefixed with "+CW:STA_CONNECTED"
           - the message prefixed with "+CW:STA_DISCONNECTED"
           - the message prefixed with "+CW:DIST_STA_IP"
           - the message prefixed with "+CW:SCAN_DONE"
           - the message prefixed with "+BLECONN"
           - the message prefixed with "+BLEDISCONN"

.. note::

    -  The configuration changes will be saved in the easyflash area if ``AT+SYSSTORE=1``.
    -  If you set Bit0 to 1, it will prompt "+QUITT" when you quit Wi-Fi passthrough mode.
    -  If you set Bit1 to 1, it will impact the of command AT+CIPSTART and AT+CIPSERVER. It will supply "+LINK_CONN:status_type,link_id,ip_type,terminal_type,remote_ip,remote_port,local_port" instead of "+CIP:XX,CONNECTED".

**Example**

::

    // print no prompt info when quitting Wi-Fi passthrough mode
    // print detailed connection prompt info
    // print no prompt info when the connection status is changed
    AT+SYSMSG=2

AT+SYSLOG?
----------
Function: Enable or disable the AT error code prompt.

Query command
^^^^^^^^^^^^^

**Function**

Query whether the AT error code prompt is enabled or not.

**Command**

::

    AT+SYSLOG?

**Response**

.. container:: screenoutput

 .. code::

    +SYSLOG:<status>

    OK

Set command
^^^^^^^^^^^

**Function**

Enable or disable the AT error code prompt.

**Command**

::

    AT+SYSLOG=<status>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<status>**: enable or disable

   -  0: disable
   -  1: enable

**Example**

::

   // enable AT error code prompt
   AT+SYSLOG=1

   OK
   AT+FAKE
   ERR CODE:0x01090000

   ERROR


::

   // disable AT error code prompt
   AT+SYSLOG=0

   OK
   AT+FAKE
   // No `ERR CODE:0x01090000`

   ERROR

- **Error Code Format**: A 32-bit hexadecimal value structured as:

  - **Category**: bits 32–24 (stationary value `0x01`)
  - **Subcategory**: bits 23–16 (error type)
  - **Extension**: bits 15–0

- **Error types**:

  - ``AT_SUB_OK``: ``0x00`` — OK
  - ``AT_SUB_COMMON_ERROR``: ``0x01`` — Reserved
  - ``AT_SUB_NO_TERMINATOR``: ``0x02`` — Terminator character not found (`\r\n` expected)
  - ``AT_SUB_NO_AT``: ``0x03`` — Starting `AT` not found (or entered as `at`, `At`, or `aT`)
  - ``AT_SUB_PARA_LENGTH_MISMATCH``: ``0x04`` — Parameter length mismatch
  - ``AT_SUB_PARA_TYPE_MISMATCH``: ``0x05`` — Parameter type mismatch
  - ``AT_SUB_PARA_NUM_MISMATCH``: ``0x06`` — Parameter number mismatch
  - ``AT_SUB_PARA_VALUE_INVALID``: ``0x07`` — Parameter value is out of range
  - ``AT_SUB_PARA_PARSE_FAIL``: ``0x08`` — Failed to parse parameter
  - ``AT_SUB_UNSUPPORT_CMD``: ``0x09`` — Command not supported
  - ``AT_SUB_CMD_EXEC_FAIL``: ``0x0A`` — Command execution failed
  - ``AT_SUB_CMD_PROCESSING``: ``0x0B`` — Previous command is still processing
  - ``AT_SUB_CMD_OP_ERROR``: ``0x0C`` — Command operation type error
  - ``AT_SUB_NO_MEMORY``: ``0x0D`` — Memory allocation error
  - ``AT_SUB_NO_RESOURCE``: ``0x0E`` — Resource allocation error
  - ``AT_SUB_TIMEOUT``: ``0x0F`` — Operation timed out
  - ``AT_SUB_NOT_ALLOWED``: ``0x10`` — Operation not allowed
  - ``AT_SUB_NOT_INIT``: ``0x11`` — Operation not initialized
  - ``AT_SUB_IO_ERROR``: ``0x12`` — Device I/O error
  - ``AT_SUB_OP_ADDR_ERROR``: ``0x13`` — Operation address error
  - ``AT_SUB_HANDLE_INVALID``: ``0x14`` — Invalid handle for operation


AT+SYSSTORE?
------------
Function: Query/set parameter storage mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the AT parameter storage mode.

**Command**

::

    AT+SYSSTORE?

**Response**

.. container:: screenoutput

 .. code::

    +SYSSTORE:<store_mode>

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+SYSSTORE=<store_mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<store_mode>**:

   -  0: command configuration isn't stored into flash.
   -  1: command configuration is stored into flash. (Default)

.. note::

    - This command affects Set commands only. Query commands are always fetched from RAM.

**Example**

::

   AT+SYSSTORE=0
   AT+CWMODE=1  // Not stored into flash
   AT+CWJAP="test","1234567890" // Not stored into flash

   AT+SYSSTORE=1
   AT+CWMODE=3  // Stored into flash
   AT+CWJAP="test","1234567890" // Stored into flash


AT+TEMP?
----------
Function: Query the temperature of the chip.

Query command
^^^^^^^^^^^^^

**Function**

Query the AT parameter storage mode.

**Command**

::

    AT+TEMP?

**Response**

.. container:: screenoutput

 .. code::

    +TEMP:<temperature>

    OK

**Parameters**

-  **<temperature>**: The temperature of the chip (Unit:°C).


AT+EFUSE-W
----------
Function: eFuse write operation. The data will be written into the eFuse after the write confirmation operation is performed.

Set command
^^^^^^^^^^^

**Command**

::

    AT+EFUSE-W=<nbytes>,<addr>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving serial data. You should enter the data and when the data length reaches the ``<nbytes>`` value, the transmission of data starts.

If the connection can't be established or is disrupted during data transmission, the system returns:

.. container:: screenoutput

 .. code::

    ERROR

If the data has been successfully sent to the protocol stack (It doesn't mean that the data has been sent to the opposite end), the system returns:

.. container:: screenoutput

 .. code::

    SEND OK

**Parameters**

-  **<nbytes>**: The number of bytes to be written needs to be aligned to 4 bytes. Maximum: 8192 bytes.

-  **<addr>**: The eFuse address, specified as a string.


AT+EFUSE-R
----------
Function: eFuse read operation.

Set command
^^^^^^^^^^^

**Function**

Read the content of the eFuse at the specified address.

**Command**

::

    AT+EFUSE-R=<nbytes>,<addr>,<reload>

**Response**

.. container:: screenoutput

 .. code::

    +EFUSE-R:<nbytes>,[data]

    OK

.. note::

    -  Among these parameters, 'nbytes' and 'addr' are mandatory inputs, while others are optional.
    -  The number of bytes read needs to be aligned to 4 bytes, up to a maximum length of 8192 bytes.

**Example**

::

    AT+EFUSE-R=4,"0x1c"

**Response**

.. container:: screenoutput

 .. code::

    +EFUSE-R:<nbytes>,[data]

    OK

**Parameters**

-  **<nbytes>**: The number of bytes to be read.
-  **<addr>**: This is the address of eFuse, it needs to be filled in as a string.
-  **<reload>**: The read operation reload from the eFuse address.

   -  0: Indicates no reloading, the read data may be from the eFuse backup area.
   -  1: Indicates reloading.

**Example**

::

    // read eFuse cmd
    AT+EFUSE-R=4,"0x1c"

    // Response
    +EFUSE-R:1234
    OK


    // write eFuse cmd
    AT+EFUSE-W=4,"0x1c"

    // Response
    OK

    // write data
    aaaa

    // Response
    SEND OK

    // read eFuse cmd
    AT+EFUSE-R=4,"0x1c"

    // Response
    +EFUSE-R:4,aaaa
    OK

    // read eFuse cmd
    AT+EFUSE-R=4,"0x1c",1

    // Response
    +EFUSE-R:0000
    OK

AT+EFUSE-CFM
------------
Function: eFuse write confirmation operation.

Set command
^^^^^^^^^^^

**Function**

eFuse write confirmation will write directly to eFuse.

**Command**

::

    AT+EFUSE-CFM

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    // write eFuse cmd
    AT+EFUSE-W=4,"0x1c"

    // Response
    OK

    // write data
    aaaa

    // Response
    SEND OK

    // write eFuse confirmation cmd
    AT+EFUSE-CFM

    // Response
    OK

    // read eFuse cmd
    AT+EFUSE-R=4,"0x1c"

    // Response
    +EFUSE-R:aaaa
    OK


AT+FLASH-W
----------
Function: Flash write operation.

Set command
^^^^^^^^^^^

**Command**

::

    AT+FLASH-W=<nbytes>,<addr>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving serial data. You should enter the data, and when the data length reaches the ``<nbytes>`` value, the transmission of data starts.

If the connection can't be established or is disrupted during data transmission, the system returns:

.. container:: screenoutput

 .. code::

    ERROR

If the data has been successfully sent to the protocol stack (It doesn't mean that the data has been sent to the opposite end), the system returns:

.. container:: screenoutput

 .. code::

    SEND OK

**Parameters**

-  **<nbytes>**: The number of bytes to be written. Maximum: 8192 bytes.

-  **<addr>**: This is the address of Flash, it needs to be filled in as a string.


AT+FLASH-R
----------
Function: Flash read operation.

Set command
^^^^^^^^^^^

**Function**

Read the content of the Flash at the specified address.

**Command**

::

    AT+FLASH-R=<nbytes>,<addr>

**Response**

.. container:: screenoutput

 .. code::

    +FLASH-R:[data]
    OK

**Example**

::

    // flash read cmd
    AT+FLASH-R=4,"0x210000"

    // Response
    +FLASH-R:ffff
    OK

**Parameters**

-  **<nbytes>**: The number of bytes to be read.
-  **<addr>**: This is the address of Flash, it needs to be filled in as a string.

AT+FLASH-E
----------
Function: Flash erase operation.

Set command
^^^^^^^^^^^

**Function**

Erase the content of the Flash at the specified address.

**Command**

::

    AT+FLASH-E=<nbytes>,<addr>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    // flash erase cmd
    AT+FLASH-E=4096,"0x210000"

    // Response
    OK

**Parameters**

-  **<nbytes>**: The number of bytes to be erased.
-  **<addr>**: This is the address of Flash, it needs to be filled in as a string.


AT+IOPUPD
----------
Function: Initialize GPIO pins as pull-up/pull-down mode.

Set command
^^^^^^^^^^^

**Command**

::

    AT+IOPUPD=<pin>,<state>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    // Initialize GPIO cmd
    AT+IOPUPD=15,1

    //Response
    OK

**Parameters**

-  **<pin>**: The GPIO pin number.
-  **<state>**: Requires initialization as pull-up, pull-down or float.

   -  0: float.
   -  1: Pull-up.
   -  2: Pull-down.

AT+IOOUT
----------
Function: Set the GPIO output to high/low level.

Set command
^^^^^^^^^^^

**Command**

::

    AT+IOOUT=<pin>,<state>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    // Set GPIO cmd
    AT+IOOUT=15,1

    //Response
    OK

**Parameters**

-  **<pin>**: The GPIO pin number.
-  **<state>**: The GPIO output to high/low level.

   -  0: low level.
   -  1: high level.

AT+IOIN
----------
Function: Query/set the GPIO input mode.

Set command
^^^^^^^^^^^

**Command**

::

    AT+IOIN=<pin>,<pull_state>

**Response**

.. container:: screenoutput

 .. code::

    OK

Query command
^^^^^^^^^^^^^^

**Command**

::

    AT+IOIN=<pin>?

**Response**

.. container:: screenoutput

 .. code::

    +IOIN=<pin>:<state>
    OK

**Example**

::

    // Set GPIO input cmd
    AT+IOIN=15

    //Response
    OK

    // Query GPIO input state
    AT+IOIN=15?

    //Response
    +IOIN=15:1
    OK

**Parameters**

-  **<pin>**: The GPIO pin number.

-  **<state>**: Requires initialization as pull-up, pull-down or float.

   -  0: float.
   -  1: Pull-up.
   -  2: Pull-down.

AT+IORST
----------
Function: Restore GPIO to floating input state.

Set command
^^^^^^^^^^^

**Command**

::

    AT+IORST=<pin>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    // Reset GPIO cmd
    AT+IORST=15

    //Response
    OK

**Parameters**

-  **<pin>**: The GPIO pin number.


AT+PART
----------
Function: Get information about the partitions.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+PART?

**Response**

.. container:: screenoutput

 .. code::

    +PART:<active_idx>,<age>,<"name">,<addr0>,<addr1>,<len0>,<len1>

    +PART:<active_idx>,<age>,<"name">,<addr0>,<addr1>,<len0>,<len1>

    ...

    OK

**Example**

::

    // Reset GPIO cmd
    AT+PART?

    //Response

    +PART=0,0,"   Boot2",0x00000000,0x00000000,57344,0

    +PART=0,0,"      FW",0x00010000,0x00210000,2097152,1474560

    +PART=0,0,"     mfg",0x00210000,0x00000000,1474560,0

    +PART=0,0,"   media",0x00378000,0x00000000,462848,0

    +PART=0,0,"     PSM",0x003e9000,0x00000000,32768,0

    OK

**Parameters**

-  **<active_idx>**:

    - 0: The current active partition index is 0.
    - 1: The current active partition index is 1.

-  **<age>**: The number of updates for this partition. For example, when the "FW" partition is updated through OTA, the age will increase with each OTA update.
-  **<name>**: The partition name.
-  **<addr0>**: The address of partition index 0.
-  **<addr1>**: The address of partition index 1.
-  **<len0>**: The length of partition index 0.
-  **<len1>**: The length of partition index 1.


AT+OTASTART
-----------
Function: Start OTA transmission.

Set command
^^^^^^^^^^^

**Command**

::

    AT+OTASTART=<enable>

**Parameters**

-  **<enable>**:

    - 0: Terminate OTA transmission.
    - 1: Start OTA transmission.

**Response**

.. container:: screenoutput

 .. code::

    ERROR

or

.. container:: screenoutput

 .. code::

    OK


AT+OTASEND
----------
Function: Send OTA data. This command will write the data to the OTA partition in real time and perform validation.


Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+OTASEND=<length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter the data now. When the requirement of message length determined by the parameter <length> is met, the transmission starts.

If the transmission is successful, AT returns:


.. container:: screenoutput

 .. code::

    SEND OK

Otherwise, it returns:

.. container:: screenoutput

 .. code::

    SEND FAIL


**Parameters**

- **<length>**: OTA data length. The length limit range is 0~4096, and the first packet of data for OTA should be greater than 512 bytes.


AT+OTAFIN
----------
Function: Verify and reboot from another partition.

Set command
^^^^^^^^^^^

**Command**

::

    AT+OTAFIN

**Response**

If the OTA has already written to the corresponding partition and verified the integrity successfully, and is about to reboot, return:

.. container:: screenoutput

 .. code::

    OK

If the reboot fails, return:

.. container:: screenoutput

 .. code::

    ERROR


AT+FS
----------
Function: Filesystem operations.

Set command
^^^^^^^^^^^

**Command**

::

    AT+FS=<type>,<operation>,<filename>[,<offset>][,<length>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<type>**: only LittleFS is currently supported.

    - 0: LittleFS

- **<operation>**:

    - 0: delete file.
    - 1: create file.
    - 2: write file.
    - 3: read file.
    - 4: query the size of the file.
    - 5: list files in a specific directory. Only root directory is currently supported.

- **<filename>**: the filename that needs to be operated on.

- **<offset>**: apply to writing and reading operations only.

- **<length>**: data length, applying to writing and reading operations only.

**Example**

::

    // Query the list of files in the current directory '.'
    AT+FS=0,5,"."

    // Response
    +FS:LIST
    .
    ..
    ca_1.crt
    ca_1.key
    ca_2.crt
    ca_2.key
    client_1.crt
    client_1.key
    client_2.crt
    client_2.key
    server_1.crt
    server_1.key
    server_2.crt
    server_2.key

    OK


AT+GMAC
----------
Function: Query the MAC addresses of 3 slots in eFuse.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+GMAC?

**Response**

.. container:: screenoutput

 .. code::

    +GMACSLOT0:<mac_addr>

    +GMACSLOT1:<mac_addr>

    +GMACSLOT2:<mac_addr>

    OK

**Parameters**

- **<mac_addr>**: MAC address in eFuse.

**Example**

::

    // Query the MAC addresses of 3 slots in eFuse.
    AT+GMAC?

    // Response
    +GMACSLOT0:9c:b7:93:0c:c1:26
    +GMACSLOT1:00:00:00:00:00:00
    +GMACSLOT2:00:b7:00:00:00:00

    OK


AT+PN
----------
Function: Query the part number.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+PN?

**Response**

.. container:: screenoutput

 .. code::

    +PN:<pn_num>

    OK

**Parameters**

- **<pn_num>**: The part number in eFuse.

**Example**

::

    AT+PN?

    // Response
    +PN:CP90-79196-2

    OK



AT+VBAT
----------
Function: Query the voltage value at the supply pin.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+VBAT?

**Response**

.. container:: screenoutput

 .. code::

    +VBAT:<vbat_val>

    OK

**Parameters**

- **<vbat_val>**: The voltage value at the supply pin (unit: mV).

**Example**

::

    AT+VBAT?

    // Response
    +VBAT:3326

    OK


AT+MINIDUMP
-----------
Function: Dump minidump partition by hex, run ``xxd -r -p hex.output crash.bin``, refer ``examples/crash/README_v1.md`` to use ``crash.bin``

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+MINIDUMP

**Response**

.. container:: screenoutput

 .. code::

    hex dump ......

    OK

**Parameters**

- None

**Example**

::

    AT+MINIDUMP

    // Response
    hex dump ......

    OK


AT+EFUSE-W-HEX
--------------
Function: Write data to eFuse in hexadecimal format. The input data is a hex string, which is converted to binary and written to the specified address. Data is word-aligned (4 bytes), and confirmation (AT+EFUSE-CFM) is required after writing.

Set command
^^^^^^^^^^^^^

**Command**

::

    AT+EFUSE-W-HEX=<nbytes>,"<address>"

    // Followed by sending the hex string data (length = nbytes * 2)

**Response**

.. container:: screenoutput

 .. code::

    OK

    Recv <X> hex chars

    SEND OK

**Parameters**

- **<nbytes>**:  Number of bytes to write (1 to 8192).
- **<address>**: Starting address in hexadecimal format (for example, "0x1c").

**Example**

::

    AT+EFUSE-W-HEX=10,"0x2c"

    // Send hex string: 01020304050607080910

    // Response
    OK

    Recv 20 hex chars

    SEND OK


AT+EFUSE-R-HEX
--------------
Function: Read data from eFuse in hexadecimal format. The output is the hex string representation of the read binary data.

Set command
^^^^^^^^^^^^^

**Command**

::

    AT+EFUSE-R-HEX=<nbytes>,"<address>"

**Response**

.. container:: screenoutput

 .. code::

    +EFUSE-R-HEX:<nbytes>,<hex data>

    OK

**Parameters**

- **<nbytes>**:  Number of bytes to read (1 to 8192).
- **<address>**: Starting address in hexadecimal format (for example, "0x1c").

**Example**

::

    AT+EFUSE-R-HEX=10,"0x2c"

    // Response
    +EFUSE-R-HEX:10,01020304050607080910

    OK


AT+FLASH-W-HEX
--------------
Function: Write data to flash in hexadecimal format. The input data is a hex string, which is converted to binary and written to the specified address.

Set command
^^^^^^^^^^^^^

**Command**

::

    AT+FLASH-W-HEX=<nbytes>,"<address>"

    // Followed by sending the hex string data (length = nbytes * 2)

**Response**

.. container:: screenoutput

 .. code::

    OK

    Recv <X> hex chars

    SEND OK

**Parameters**

- **<nbytes>**:  Number of bytes to write (1 to 8192).
- **<address>**: Starting address in hexadecimal format (for example, "0x210000").

**Example**

::

    AT+FLASH-W-HEX=10,"0x210000"

    // Send hex string: 01020304050607080910

    // Response
    OK

    Recv 20 hex chars

    SEND OK


AT+FLASH-R-HEX
--------------
Function: Read data from flash in hexadecimal format. The output is the hex string representation of the read binary data.

Set command
^^^^^^^^^^^^^

**Command**

::

    AT+FLASH-R-HEX=<nbytes>,"<address>"

**Response**

.. container:: screenoutput

 .. code::

    +FLASH-R-HEX:<nbytes>,<hex data>

    OK

**Parameters**

- **<nbytes>**:  Number of bytes to read (1 to 8192).
- **<address>**: Starting address in hexadecimal format (for example, "0x210000").

**Example**

::

    AT+FLASH-R-HEX=10,"0x210000"

    // Response
    +FLASH-R-HEX:10,01020304050607080910

    OK


==========================
Wi-Fi AT commands
==========================
.. table::
    :widths: 30,70
    :width: 100%
    :align: center

    +----------------------+-------------------------------------------------------------------+
    |  Instruction         | Description                                                       |
    +======================+===================================================================+
    | `AT+WIFISP`_         | Query/set enable/disable WLAN radio                               |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWMODE`_         | Set Wi-Fi mode                                                    |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWNETMODE`_      | Query device to work in NCP/RCP mode.                             |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWSTATE`_        | Query the Wi-Fi state and Wi-Fi information                       |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWJAP`_          | Connect to AP                                                     |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWRECONNCFG`_    | Query/set the Wi-Fi reconnecting configuration                    |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWLAPOPT`_       | Set AT+CWLAP command scan result attributes                       |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWLAP`_          | Scan nearby AP                                                    |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWQAP`_          | Disconnect AP                                                     |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWCREDADD`_      | Add WiFi credentials to secure storage with AES encryption        |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWCREDDEL`_      | Delete WiFi credentials from secure storage                       |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWCRED`_         | Query stored WiFi credentials                                     |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWJAPS`_         | Connect to AP using securely stored credentials                   |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWSAP`_          | Query/Set the configuration of a SoftAP                           |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWLIF`_          | Obtain IP address of the station that connects to a SoftAP        |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWQIF`_          | Disconnect stations from a SoftAP                                 |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWDHCP`_         | Enable/disable DHCP                                               |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWDHCPS`_        | Query/set the IP addresses allocated by a SoftAP DHCP server      |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWAUTOCONN`_     | Set up automatic reconnection                                     |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CIPSTAMAC`_      | Query/set the MAC address of a station                            |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CIPAPMAC`_       | Query/set the MAC address of a SoftAP                             |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CIPSTA`_         | Query/set the IP address of a station                             |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CIPAP`_          | Query/set the IP address of a SoftAP                              |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWHOSTNAME`_     | Query/set the host name of a station                              |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWCOUNTRY`_      | Query/set the Wi-Fi country code                                  |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWMONITOR`_      | Set Wi-Fi to enter monitor mode                                   |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWSTAPROTO`_     | Query/set the 802.11 b/g/n/ax protocol standard of station mode   |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWAPPROTO`_      | Query/set the 802.11 b/g/n/ax protocol standard of SoftAP mode    |
    +----------------------+-------------------------------------------------------------------+
    | `AT+WPS`_            | Enable the WPS function                                           |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWANTENABLE`_    | Enable/disable antenna diversity function                         |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWANTNUM`_       | Get the maximum number of antennas for the current device         |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWANT`_          | Set/query the currently used antenna                              |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWCOEXDUTY`_     | Set WiFi/BLE coexistence duty cycle (WiFi active time in ms)      |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CWCOEXEN`_       | Enable/Disable WiFi/BLE coexistence mode                          |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CSIDMASTART`_    | Start CSI DMA capture on one buffer                               |
    +----------------------+-------------------------------------------------------------------+
    | `AT+CSIDMASTOP`_     | Stop CSI DMA capture and release resources                        |
    +----------------------+-------------------------------------------------------------------+

Wi-Fi AT command description
============================

AT+WIFISP
------------
Function: Query/set enable/disable WLAN radio.

Query command
^^^^^^^^^^^^^

**Function**

Query the WLAN radio enablement status.

**Command**

::

    AT+WIFISP?

**Response**

.. container:: screenoutput

 .. code::

    +WIFISP:<enable>

    OK

Set command
^^^^^^^^^^^

**Function**

Enable/disable WLAN radio.

**Command**

::

    AT+WIFISP=<enable>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<enable>**:

   -  0: Disable WLAN radio.
   -  1: Enable WLAN radio.


AT+CWMODE
------------
Function: Query/set Wi-Fi mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the Wi-Fi mode of devices.

**Command**

::

    AT+CWMODE?

**Response**

.. container:: screenoutput

 .. code::

    +CWMODE:<mode>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the Wi-Fi mode of devices.

**Command**

::

    AT+CWMODE=<mode>[,<auto_connect>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**:

   -  0: Null mode. Wi-Fi RF will be disabled.
   -  1: Station mode.
   -  2: SoftAP mode.
   -  3: SoftAP + Station mode.

-  **<auto_connect>**: Enable or disable automatic connection to an AP when you change the mode of the device from the SoftAP mode or null mode to the station mode. Default: 1. If you omit the parameter, the default value will be used, i.e. automatically connecting to an AP.

   -  0: The device won't automatically connect to an AP.
   -  1: The device will automatically connect to an AP if the configuration to connect to the AP has already been saved in flash before.

.. note::

   -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
   -  SoftAP + Station mode requires AP and STA to be on the same channel

**Example**

::

    AT+CWMODE=1


AT+CWNETMODE
------------
Function: Query device working in NCP/RCP mode.

Query command
^^^^^^^^^^^^^

**Function**

Query whether the device is operating in NCP/RCP mode.

**Command**

::

    AT+CWNETMODE?

**Response**

.. container:: screenoutput

 .. code::

    +CWNETMODE:<netmode>

    OK

**Parameters**

-  **<netmode>**:

   -  0: RCP mode.
   -  1: NCP mode.

.. note::

   -  When the device is in RCP mode, all network, HTTP, and MQTT-related AT commands become invalid.

AT+CWSTATE
----------
Function: Query the Wi-Fi state and Wi-Fi information.

Query command
^^^^^^^^^^^^^

**Function**

Query the Wi-Fi state and Wi-Fi information of devices.

**Command**

::

    AT+CWSTATE?

**Response**

.. container:: screenoutput

 .. code::

    +CWSTATE:<state>,<"ssid">,<mode>,<auth>

    OK

**Parameters**

-  **<state>**: current Wi-Fi state.

   -  0: station has not started any Wi-Fi connection.
   -  1: station has connected to an AP, but doesn't get an IPv4 address yet.
   -  2: station has connected to an AP, and got an IPv4 address.
   -  3: station is in Wi-Fi connecting or reconnecting state.
   -  4: station is in Wi-Fi disconnected state.

-  **<\"ssid\">**: the SSID of the target AP.

-  **<mode>**: Wi-Fi protocol mode (bitmask value).

   -  A bitmask indicating supported 802.11 protocols:
      -  Bit 0 (0x01): 802.11b
      -  Bit 1 (0x02): 802.11g
      -  Bit 2 (0x04): 802.11n (2.4GHz)
      -  Bit 3 (0x08): 802.11ax (Wi-Fi 6, 2.4GHz)
   -  Example: 7 (0x07) indicates support for 802.11b/g/n.
   -  Default: 0 (no protocols or unknown).

-  **<auth>**: Authentication/Encryption type (ECN value).

   -  0: OPEN (no encryption)
   -  1: WEP
   -  2: WPA-PSK
   -  3: WPA2-PSK
   -  4: WPA/WPA2-PSK
   -  5: WPA Enterprise
   -  6: WPA3-SAE
   -  7: WPA2-PSK/WPA3-SAE
   -  8: Unknown
   -  Default: 0 (OPEN) when not connected.

.. note::

   - When station isn't connected to an AP, it's recommended to use this command to query Wi-Fi information; after station is connected to an AP, it's recommended to use AT+CWJAP to query Wi-Fi information.
   - The <mode> and <auth> parameters are derived from the connection or configuration; they may default to 0 in disconnected states or if unavailable.

AT+CWJAP
----------------
Function: Connect to an AP.

Query command
^^^^^^^^^^^^^

**Function**

Query the AP to which the station is already connected.

**Command**

::

    AT+CWJAP?

**Response**

.. container:: screenoutput

 .. code::

    +CWJAP:<ssid>,<bssid>,<channel>,<rssi>,<wep_en>

    OK

Set command
^^^^^^^^^^^

**Function**

Connect a station to a targeted AP.

**Command**

::

    AT+CWJAP=[<ssid>],[<pwd>][,<bssid>][,<wep_en>][,<reserve>][,<reserve>][,<reserve>][,<pmf>]

.. note::

    -  Among these parameters, 'ssid' and 'pwd' are mandatory inputs, while others are optional.
    -  For non-open WiFi (with password), the credentials will be stored in a 5-slot encrypted cache.
       If the cache is full, connection will fail. You can use ``AT+CWCRED?`` to query stored credentials.

**Response**

.. container:: screenoutput

 .. code::

    +CW:CONNECTED
    +CW:GOTIP

    OK

or

.. container:: screenoutput

 .. code::

    +CW:CONNECTING

    OK

    +CW:ERROR,<reason_code>

or

.. container:: screenoutput

 .. code::

    +CWJAP:<error code>
    ERROR

Execute command
^^^^^^^^^^^^^^^

**Function**

Connect a station to a targeted AP with last Wi-Fi configuration.

**Command**

::

    AT+CWJAP

**Response**

.. container:: screenoutput

 .. code::

    +CW:CONNECTED
    +CW:GOTIP

    OK

or

.. container:: screenoutput

 .. code::

    +CWJAP:<error code>
    ERROR

**Parameters**

-  **<ssid>**: the SSID of the target AP.

   -  Escape character syntax is needed if SSID or password contains special characters, such ``,``, ``"``, or ``\\``.

-  **<pwd>**: password, Maximum: 64-byte ASCII.
-  **<bssid>**: the MAC address of the target AP. It can't be omitted when multiple APs have the same SSID.
-  **<channel>**: channel.
-  **<rssi>**: signal strength.
-  **<wep_en>**: Is the connected AP using WEP encryption?

   -  0: Not WEP encryption.
   -  1: It's WEP encryption.
-  **<pmf>**: Protected Management Frames. Default: 1.

   -  0 means disable PMF.
   -  bit 0: PMF capable, advertises support for protected management frame. Device will prefer to connect in PMF mode if other device also advertises PMF capability.
   -  bit 1: PMF required, advertises that protected management frame is required. Device will not associate to non-PMF capable devices.

-  **<error code>**: (for reference only)

   -  1: connection timeout.
   -  2: wrong password.
   -  3: can't find the target AP.
   -  4: connection failed.
   -  others: unknown error occurred.

-  **<reason_code>**:

   - 1: Failure to allocate memory for transmitting an authentication frame.
   - 2: Authentication process failed.
   - 3: The authentication algorithm wasn't accepted or failed.
   - 4: Failure to allocate memory for transmitting an association frame.
   - 5: Association process with the access point failed.
   - 6: Deauthentication by the access point occurred when not connected.
   - 7: Deauthentication by the access point occurred while connected.
   - 8: A timeout occurred during the 4-way handshake, likely due to PSK (Pre-Shared Key) issues.
   - 9: Failed to transmit a deauthentication frame during the 4-way handshake.
   - 10: Failed to allocate memory for a deauthentication frame during the 4-way handshake.
   - 11: Timeout waiting for authentication or association response.
   - 12: No BSSID or channel found during the scan process.
   - 13: Failed to create a channel context when attempting to join a network.
   - 14: Failure to join a network.
   - 15: Failed to add a station (STA) to the network.
   - 16: Beacon loss occurred, potentially leading to disconnection.
   - 17: The network security settings don't match.
   - 18: An error occurred due to an incorrect WEP key length.
   - 19: User-initiated disconnection with deauthentication.
   - 20: User-initiated disconnection without sending a deauthentication frame.
   - 21: Disconnection occurred because the firmware failed to send a power-save null frame.
   - 22: Traffic loss was detected.
   - 23: Switching to a new channel failed.
   - 24: Confirmation of authentication or association response failed.
   - 25: Reassociation process is starting.

.. note::

    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  This command requires Station mode to be enabled.
    -  The parameter ``<reconn_interval>`` of this command is the same as ``<interval_second>`` of the command AT + CWRECONNCFG . Therefore, if you omit ``<reconn_interval>`` when running this command, the interval between Wi-Fi reconnections will use the default value 1.
    -  If the ``<ssid>`` and ``<password>`` parameter are omitted, AT will use the last configuration.
    -  Execute command has the same maximum timeout to Setup command. The default value is 15 seconds, but you can change it by setting the parameter ``<jap_timeout>``.
    -  The password is encrypted using AES-128-CBC with eFuse hardware key before storage.
    -  The encrypted configuration will be saved to auto-connection storage if AT+SYSSTORE=1.
    -  The password is only decrypted in memory when needed for actual connection and immediately cleared after use.

**Example**

::

    // If the target AP's SSID is "abc" and the password is "0123456789", the command should be:
    AT+CWJAP="abc","0123456789"

    // If the target AP's SSID is "ab\,c" and the password is "0123456789"\", the command should be:
    AT+CWJAP="ab\\\,c","0123456789\"\\"

    // If multiple APs all have the SSID of "abc", the target AP can be found by BSSID:
    AT+CWJAP="abc","0123456789","ca:d7:19:d8:a6:44"

    // If it is required that connect to a AP by protected management frame, the command should be:
    AT+CWJAP="abc","0123456789",,,,,,,3

AT+CWRECONNCFG
---------------
Function:  Query/set the Wi-Fi reconnecting configuration.

Query command
^^^^^^^^^^^^^

**Function**

Query the configuration of Wi-Fi reconnect.

**Command**

::

    AT+CWRECONNCFG?

**Response**

.. container:: screenoutput

 .. code::

    +CWRECONNCFG:<interval_second>,<repeat_count>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the configuration of Wi-Fi reconnect.

**Command**

::

    AT+CWRECONNCFG=<interval_second>,<repeat_count>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<interval_second>**: the interval between Wi-Fi reconnections. Unit: second. Default: 0. Maximum: 7200.

   -  0: The station won't reconnect to the AP when disconnected.
   -  [1,7200]: The station will reconnect to the AP at the specified interval when disconnected.

-  **<repeat_count>**: the number of attempts the device makes to reconnect to the AP. This parameter only works when the parameter ``<interval_second>`` isn't 0. Default: 0. Maximum: 1000.

   -  0: The station will always try to reconnect to AP.
   -  [1,1000]: The station will attempt to reconnect to AP for the specified times.

**Example**

::

    // The station tries to reconnect to AP at the interval of one second for 100 times.
    AT+CWRECONNCFG=1,100

    // The station won't reconnect to AP when disconnected.
    AT+CWRECONNCFG=0,0

.. note::

    -  The parameter ``<interval_second>`` of this command is the same as the parameter ``[<reconn_interval>]`` of the command AT+CWJAP.
    -  This command works for passive disconnection from APs, Wi-Fi mode switch, and Wi-Fi auto connect after power on.

AT+CWLAPOPT
-----------
Function:  Set the configuration for the command AT+CWLAP.

Set command
^^^^^^^^^^^

**Command**

::

    AT+CWLAPOPT=<sort_enable>,<print mask>[,<rssi filter>][,<authmode mask>][,<max_count>]

**Response**

.. container:: screenoutput

 .. code::

    OK

or

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

-  **<sort_enable>**: Enable/disable sorting scan results by RSSI (default sorting).
-  **<print mask>**: determine whether the following parameters are shown in the result of AT+CWLAP. Default: 0x7FF. If you set them to 1, it means showing the corresponding parameters; if you set them as 0, it means NOT showing the corresponding parameters.

   -  bit 0: determine whether <ecn> will be shown.
   -  bit 1: determine whether <ssid> will be shown.
   -  bit 2: determine whether <rssi> will be shown.
   -  bit 3: determine whether <mac> will be shown.
   -  bit 4: determine whether <channel> will be shown.
   -  bit 5: determine whether <freq_offset> will be shown.
   -  bit 6: determine whether <freqcal_val> will be shown.
   -  bit 7: determine whether <pairwise_cipher> will be shown.
   -  bit 8: determine whether <group_cipher> will be shown.
   -  bit 9: determine whether <bgn> will be shown.
   -  bit 10: determine whether <wps> will be shown.

-  **[<rssi filter>]**: determine whether the result of the command AT+CWLAP will be filtered according to ``rssi filter``. In other words, the result of the command will **NOT** show the APs whose signal strength is below ``rssi filter``. Unit: dBm. Default: –100. Range: [–100,40].
-  **[<authmode mask>]**: determine whether APs with the following authmodes are shown in the result of AT+CWLAP. Default: 0xFFFF. If you set ``bit x`` to 1, the APs with the corresponding authmode will be shown. If you set ``bit x`` to 0, the APs with the corresponding authmode won't be shown;

   -  bit 0: determine whether APs with ``OPEN`` authmode will be shown.
   -  bit 1: determine whether APs with ``WEP`` authmode will be shown.
   -  bit 2: determine whether APs with ``WPA_PSK`` authmode will be shown.
   -  bit 3: determine whether APs with ``WPA2_PSK`` authmode will be shown.
   -  bit 4: determine whether APs with ``WPA_WPA2_PSK`` authmode will be shown.
   -  bit 5: determine whether APs with ``WPA2_ENTERPRISE`` authmode will be shown.
   -  bit 6: determine whether APs with ``WPA3_PSK`` authmode will be shown.
   -  bit 7: determine whether AP with ``WPA2_WPA3_PSK`` authmode will be shown.

-  **[<max_count>]**: The maximum number of Wi-Fi networks scanned is 50 by default, meaning the CWLAP command response can include up to 50 scanned results. Range: (0,50].

**Example**

::

    // The first parameter is 1, meaning that the result of the command AT+CWLAP will be ordered according to RSSI;
    // The second parameter is 31, namely 0x1F, meaning that the corresponding bits of <print mask> are set to 1. All parameters will be shown in the results of AT+CWLAP.
    AT+CWLAPOPT=1,31
    AT+CWLAP

    // Just show the AP which authmode is OPEN
    AT+CWLAPOPT=1,31,-100,1
    AT+CWLAP

AT+CWLAP
----------
Function: List available APs.

Set command
^^^^^^^^^^^

**Function**

Query the APs with specified parameters, such as the SSID, MAC address, or channel.

**Command**

::

    AT+CWLAP=[<scan_type>,<ssid>,<mac>,<channel>,<scan_time_min>,<scan_time_max>]

Execute command
^^^^^^^^^^^^^^^

**Function**

List all available APs.

**Command**

::

    AT+CWLAP

**Response**

.. container:: screenoutput

 .. code::

    +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<freq_offset>,<freqcal_val>,<pairwise_cipher>,<group_cipher>,<bgn>,<wps>

    OK

**Parameters**

-  **<ecn>**: encryption method.

   -  0: OPEN
   -  1: WEP
   -  2: WPA_PSK
   -  3: WPA2_PSK
   -  4: WPA_WPA2_PSK
   -  5: WPAx_ENTERPRISE
   -  6: WPA3_PSK
   -  7: WPA2_WPA3_PSK

-  **<scan_type>**: Wi-Fi scan type:

   -  0: active scan
   -  1: passive scan

-  **<ssid>**: string parameter showing SSID of the AP.
-  **<rssi>**: signal strength.
-  **<mac>**: string parameter showing MAC address of the AP.
-  **<channel>**: channel.
-  **<scan_time_min>**: the minimum active scan time per channel. Unit: millisecond. Range [0,1500]. If the scan type is passive, this parameter is invalid.
-  **<scan_time_max>**: the maximum active scan time per channel. Unit: millisecond. Range [0,1500]. If this parameter is 0, the firmware will use the default time: 120 ms for active scan; 360 ms for passive scan.
-  **<freq_offset>**: frequency offset (reserved item).
-  **<freqcal_val>**: frequency calibration value (reserved item).
-  **<pairwise_cipher>**: pairwise cipher type.

   -  0: None
   -  1: WEP40
   -  2: WEP104
   -  3: TKIP
   -  4: CCMP
   -  5: TKIP and CCMP
   -  6: AES-CMAC-128
   -  7: Unknown

-  **<group_cipher>**: group cipher type, same enumerated value to ``<pairwise_cipher>``.
-  **<bgn>**: 802.11 b/g/n. If the corresponding bit is 1, the corresponding mode is enabled; if the corresponding bit is 0, the corresponding mode is disabled.

   -  bit 0: bit to identify if 802.11b mode is enabled or not
   -  bit 1: bit to identify if 802.11g mode is enabled or not
   -  bit 2: bit to identify if 802.11n mode is enabled or not
   -  bit 3: bit to identify if 802.11ax mode is enabled or not

-  **<wps>**: wps flag.

   - 0: WPS disabled
   - 1: WPS enabled

**Example**

::

    AT+CWLAP=0,"Wi-Fi","ca:d7:19:d8:a6:44",6,0,400,1000

    // Search for APs with a specific SSID:
    AT+CWLAP=0,"Wi-Fi"

AT+CWQAP
------------
Function: Disconnect from an AP.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+CWQAP=[<restore>]

**Response**

.. container:: screenoutput

 .. code::

    OK


**Command**

::

    AT+CWQAP

**Response**

.. container:: screenoutput

 .. code::

    OK


**Parameters**

-  **Optional <restore>**: flag to erase the saved SSID and password information.

   -  0: don't erase the saved SSID and password information (default).
   -  1: Erase the saved SSID and password information.

AT+CWCREDADD
----------------
Function: Add WiFi credentials to secure storage with AES encryption.

Set command
^^^^^^^^^^^

**Function**

Add encrypted WiFi credentials (SSID and password) to secure storage.

**Command**

::

    AT+CWCREDADD=<ssid>,<pwd>

**Response**

.. container:: screenoutput

 .. code:: text

    OK

or

::

    ERROR

Parameters
^^^^^^^^^^

-  **<ssid>**: the SSID of the target AP, MAX: 32-byte ASCII.

   -  Escape character syntax is needed if SSID contains special characters, such ``,``, ``"``, or ``\\``.

-  **<pwd>**: password, range: 8~63 byte ASCII.

   -  The password will be encrypted using AES-128-CBC with hardware key from eFuse before storage.

.. note::

    -  The credentials will be stored in flash if AT+SYSSTORE=1, otherwise only kept in RAM (temporary).
    -  Maximum 5 secure credentials can be stored.
    -  If the SSID already exists, the password will be updated.
    -  Passwords are encrypted using AES-128-CBC mode with a unique hardware key stored in eFuse.

Example
^^^^^^^^

::

    // Add credentials for AP "MyWiFi" with password "12345678"
    AT+CWCREDADD="MyWiFi","12345678"

    // Add credentials with special characters in SSID
    AT+CWCREDADD="My\,WiFi","password123"

AT+CWCREDDEL
----------------
Function: Delete WiFi credentials from secure storage.

Set command
^^^^^^^^^^^

**Function**

Delete specified WiFi credentials from secure storage.

**Command**

::

    AT+CWCREDDEL=<ssid>

**Response**

.. container:: screenoutput

 .. code:: text

    OK

or

::

    ERROR

Parameters
^^^^^^^^^^

-  **<ssid>**: the SSID of the credentials to delete.

.. note::

    -  The credential will be removed from memory cache.
    -  If the credential exists in flash, it will also be removed from flash.
    -  Returns error if the specified SSID is not found.

Example
^^^^^^^^

::

    AT+CWCREDDEL="MyWiFi"

AT+CWCRED
------------
Function: Query stored WiFi credentials.

Query command
^^^^^^^^^^^^^

**Function**

List all WiFi credentials stored in secure storage.

**Command**

::

    AT+CWCRED?

**Response**

.. container:: screenoutput

 .. code:: text

    +CWCRED:"<ssid1>"
    +CWCRED:"<ssid2>"
    ...

    OK

or

::

    OK

Parameters
^^^^^^^^^^

-  **<ssid>**: the SSID of stored credentials.

.. note::

    -  This command lists all credentials currently stored in memory.
    -  Credentials added with AT+SYSSTORE=1 are persistent (saved to flash) and survive reboots.
    -  Credentials added with AT+SYSSTORE=0 are temporary (RAM only) and will be lost after reboot.
    -  The output does not distinguish between persistent and temporary credentials.

Example
^^^^^^^^

::

    AT+CWCRED?
    +CWCRED:"MyHomeWiFi"
    +CWCRED:"OfficeWiFi"
    +CWCRED:"GuestWiFi"

    OK

AT+CWJAPS
------------
Function: Connect to an AP using securely stored credentials.

Set command
^^^^^^^^^^^

**Function**

Connect to a specified AP using encrypted credentials from secure storage.

**Command**

::

    AT+CWJAPS=<ssid>

**Response**

.. container:: screenoutput

 .. code:: text

    +CW:CONNECTED
    +CW:GOTIP

    OK

or

::

    +CW:CONNECTING

    OK

    +CW:ERROR,<reason_code>

or

::

    ERROR

Parameters
^^^^^^^^^^

-  **<ssid>**: the SSID of the target AP. Credentials must be pre-stored using AT+CWCREDADD.

-  **<reason_code>**: Same as AT+CWJAP reason codes.

.. note::

    -  This command requires Station mode to be enabled (AT+CWMODE=1 or 3).
    -  The SSID must exist in secure storage, added via AT+CWCREDADD.
    -  The password is automatically decrypted from secure storage before connection.
    -  The decrypted password is immediately cleared from memory after use.
    -  This command does not affect AT+SYSSTORE - it only uses existing stored credentials.

Example
^^^^^^^^

::

    // First, add credentials to secure storage
    AT+CWCREDADD="MyWiFi","12345678"

    // Then connect using secure credentials
    AT+CWJAPS="MyWiFi"

    // The device will connect to "MyWiFi" using the encrypted stored password

AT+CWSAP
------------
Function: Query/set the configuration of a SoftAP.

Query command
^^^^^^^^^^^^^

**Function**

Query the configuration parameters of a SoftAP.

**Command**

::

    AT+CWSAP?

**Response**

.. container:: screenoutput

 .. code::

    +CWSAP:<ssid>,"********",<channel>,<ecn>,<max conn>,<ssid hidden>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the configuration of a SoftAP.

**Command**

::

    AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn>[,<max conn>][,<ssid hidden>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<ssid>**: string parameter showing SSID of the AP.
-  **<pwd>**: string parameter showing the password. Length: 8 ~ 64 bytes ASCII.
-  **<channel>**: channel ID.
-  **<ecn>**: encryption method; WEP isn't supported.

   -  0: OPEN
   -  2: WPA_PSK
   -  3: WPA2_PSK
   -  4: WPA3_PSK

-  **[<max conn>]**: maximum number of stations that SoftAP can connect. Range: [1,4].
-  **[<ssid hidden>]**:

   -  0: broadcasting SSID (default).
   -  1: not broadcasting SSID.

.. note::

    -  This command works only when AT+CWMODE=2 or AT+CWMODE=3.
    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  The password is encrypted using AES-CBC before being stored in Flash for enhanced security.
    -  The query command returns "********" for the password field instead of the actual password.

AT+CWLIF
--------
Function: Obtain IP address of the station that connects to a SoftAP.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+CWLIF

**Response**

.. container:: screenoutput

 .. code::

    +CWLIF:<ip addr>,<mac>

    OK

**Parameters**

-  **<ip addr>**: IP address of the station that connects to the SoftAP.
-  **<mac>**: MAC address of the station that connects to the SoftAP.

.. note::

   -  This command can't get a static IP. It works only when DHCP of both the SoftAP and the connected station are enabled.

AT+CWQIF
---------
Function: Disconnect stations from a SoftAP.

Execute command
^^^^^^^^^^^^^^^

**Function**

Disconnect all stations that are connected to the SoftAP.

**Command**

::

    AT+CWQIF

**Response**

.. container:: screenoutput

 .. code::

    OK

Set command
^^^^^^^^^^^

**Function**

Disconnect a specific station from the SoftAP.

**Command**

::

    AT+CWQIF=<mac>

**Example**

::

    AT+CWQIF="18:b9:05:ed:59:cc"

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mac>**: MAC address of the station to disconnect.

AT+CWDHCP
----------
Function: Enable/disable DHCP.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CWDHCP?

**Response**

.. container:: screenoutput

 .. code::

  <state>

Set command
^^^^^^^^^^^

**Function**

Enable/disable DHCP.

**Command**

::

    AT+CWDHCP=<operate>,<mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<operate>**:

   -  0: disable
   -  1: enable

-  **<mode>**:

   -  Bit0: Station DHCP
   -  Bit1: SoftAP DHCP

-  **<state>**: the status of DHCP

   - Bit0:

     - 0: Station DHCP is disabled.
     - 1: Station DHCP is enabled.

   - Bit1:

     - 0: SoftAP DHCP is disabled.
     - 1: SoftAP DHCP is enabled.

.. note::

    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  This Set command correlates with the commands that set static IP, such as AT+CIPSTA and AT+CIPAP:

    -  If DHCP is enabled, static IP address will be disabled;
    -  If static IP address is enabled, DHCP will be disabled;
    -  The last configuration overwrites the previous configuration.

**Example**

::

    // Enable Station DHCP. If the last DHCP mode is 2, the current DHCP mode will be 3.
    AT+CWDHCP=1,1

    // Disable SoftAP DHCP. If the last DHCP mode is 3, the current DHCP mode will be 1.
    AT+CWDHCP=0,2

AT+CWDHCPS
-----------------------------------------------------------------------------------------------
Function:Query/set the IP addresses allocated by a SoftAP DHCP server.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CWDHCPS?

**Response**

.. container:: screenoutput

 .. code::

    +CWDHCPS=<lease time>,<start IP>,<end IP>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the IP address range of the SoftAP DHCP server.

**Command**

::

    AT+CWDHCPS=<enable>,<lease time>,<start IP>,<end IP>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<enable>**:

   -  1: Enable DHCP server settings. The parameters below have to be set.
   -  0: Disable DHCP server settings and use the default IP address range.

-  **<lease time>**: lease time. Unit: minute. Range [1,2880].
-  **<start IP>**: start IP address of the IP address range that can be obtained from SoftAP DHCP server.
-  **<end IP>**: end IP address of the IP address range that can be obtained from SoftAP DHCP server.

.. note::

    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  This AT command works only when both SoftAP and DHCP server are enabled for devices.
    -  The IP address should be in the same network segment as the IP address of SoftAP.

**Example**

::

    AT+CWDHCPS=1,3,"192.168.4.10","192.168.4.15"

    AT+CWDHCPS=0 // Disable the settings and use the default IP address range.

AT+CWAUTOCONN
--------------------------------------------------------------------------------
Function: Automatically connect to an AP when powered on

Set command
^^^^^^^^^^^

**Command**

::

    AT+CWAUTOCONN=<enable>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<enable>**:

   -  1: Enable automatic connection to an AP when powered on. (Default)
   -  0: Disable automatic connection to an AP when powered on.

.. note::

   -  The configuration changes will be saved in the easyflash area.

**Example**

::

    AT+CWAUTOCONN=1

AT+CIPSTAMAC
----------------------------------------------------------------------------
Function: Query/set the MAC address of a station

Query command
^^^^^^^^^^^^^

**Function**

Query the MAC address of the station.

**Command**

::

    AT+CIPSTAMAC?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSTAMAC:<mac>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the MAC address of a station.

**Command**

::

    AT+CIPSTAMAC=<mac>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mac>**: string parameter showing MAC address of a station.

.. note::

    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  The MAC address of SoftAP is the same as Station, and when Station MAC is set, SoftAP's MAC will also be automatically updated.
    -  Bit 0 of the MAC address can't be 1. For example, a MAC address can be "1a:…" but not "15:…".
    -  FF:FF:FF:FF:FF:FF and 00:00:00:00:00:00 are invalid MAC address and can't be set.

**Example**

::

    AT+CIPSTAMAC="1a:fe:35:98:d3:7b"

AT+CIPAPMAC
--------------------------------------------------------------------------
Function: Query/set the MAC address of a SoftAP

Query command
^^^^^^^^^^^^^
**Function**

Query the MAC address of the SoftAP.

**Command**

::

    AT+CIPAPMAC?

**Response**

.. container:: screenoutput

 .. code::

    +CIPAPMAC:<mac>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the MAC address of the SoftAP.

**Command**

::

    AT+CIPAPMAC=<mac>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mac>**: string parameter showing MAC address of the SoftAP.

.. note::

    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  The MAC address of SoftAP is the same as Station, and when SoftAP MAC is set, Station's MAC will also be automatically updated.
    -  Bit 0 of the MAC address can't be 1. For example, a MAC address can be "18:…" but not "15:…".
    -  FF:FF:FF:FF:FF:FF and 00:00:00:00:00:00 are invalid MAC and can't be set.

**Example**

::

    AT+CIPAPMAC="18:fe:35:98:d3:7b"

AT+CIPSTA
------------------------------------------------------------------------
Function: Query/set the IP address of a station

Query command
^^^^^^^^^^^^^

**Function**

Query the IP address of the station.

**Command**

::

    AT+CIPSTA?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSTA:ip:<"ip">
    +CIPSTA:gateway:<"gateway">
    +CIPSTA:netmask:<"netmask">
    +CIPSTA:ip6ll:<"ipv6 addr">
    +CIPSTA:ip6gl:<"ipv6 addr">
    OK

Set command
^^^^^^^^^^^

**Function**

Set the IPv4 address of the station (static IPv6 not supported).

**Command**

::

    AT+CIPSTA=<"ip">[,<"gateway">,<"netmask">,<"dns">]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<\"ip\">**: string parameter showing the IPv4 address of the station.
-  **<\"gateway\">**: gateway.
-  **<\"netmask\">**: netmask.
-  **<”ipv6 addr”>**: string parameter showing the IPv6 address of the station.

.. note::

    -  For the Query command, only when the station is connected to an AP or the static IP address is configured can its IP address be queried.
    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  The Set command correlates with the commands that set DHCP, such as AT+CWDHCP.

    -  If static IP address is enabled, DHCP will be disabled;
    -  If DHCP is enabled, static IP address will be disabled;
    -  The last configuration overwrites the previous configuration.

**Example**

::

    AT+CIPSTA="192.168.6.100","192.168.6.1","255.255.255.0"

AT+CIPAP
----------------------------------------------------------------------
Function: Query/set the IP address of a SoftAP

Query command
^^^^^^^^^^^^^

**Function**

Query the IP address of the SoftAP.

**Command**

::

    AT+CIPAP?

**Response**

.. container:: screenoutput

 .. code::

    +CIPAP:ip:<"ip">
    +CIPAP:gateway:<"gateway">
    +CIPAP:netmask:<"netmask">

    OK

Set command
^^^^^^^^^^^

**Function**

Set the IPv4 address of the SoftAP.

**Command**

::

    AT+CIPAP=<"ip">[,<"gateway">,<"netmask">]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<"ip\">**: string parameter showing the IPv4 address of the SoftAP.
-  **<\"gateway\">**: gateway.
-  **<\"netmask\">**: netmask.

.. note::

    -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1.
    -  The Set command correlates with the commands that set DHCP, such as AT+CWDHCP.

    -  If static IP address is enabled, DHCP will be disabled;
    -  If DHCP is enabled, static IP address will be disabled;
    -  The last configuration overwrites the previous configuration.

**Example**

::

    AT+CIPAP="192.168.5.1","192.168.5.1","255.255.255.0"

AT+CWHOSTNAME
--------------------------------------------------------------------------
Function: Query/set the host name of a station

Query command
^^^^^^^^^^^^^

**Function**

Query the host name of station.

**Command**

::

    AT+CWHOSTNAME?

**Response**

.. container:: screenoutput

 .. code::

    +CWHOSTNAME:<hostname>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the host name of station.

**Command**

::

    AT+CWHOSTNAME=<hostname>

**Response**

.. container:: screenoutput

 .. code::

    OK

If the station mode isn't enabled, the command will return:

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

-  **<hostname>**: the host name of the station. Maximum length: 32 bytes.

.. note::

   -  The configuration changes aren't saved in the flash.

**Example**

::

    AT+CWMODE=3
    AT+CWHOSTNAME="my_test"

AT+CWCOUNTRY
--------------------------------------------------------------------
Function: Query/set the Wi-Fi country code

Query command
^^^^^^^^^^^^^

**Function**

Query Wi-Fi country code information.

**Command**

::

    AT+CWCOUNTRY?

**Response**

.. container:: screenoutput

 .. code::

    +CWCOUNTRY:<country_policy>,<country_code>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the Wi-Fi country code information.

**Command**

::

    AT+ CWCOUNTRY=<country_policy>,<country_code>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<country_policy>**:

   -  0: will change the county code to be the same as the AP that the device is connected to.
   -  1: the country code won't change, always be the one set by command.

-  **<country_code>**: country code. Maximum length: 3 characters.

.. note::

   -  The configuration changes aren't saved in the flash.

**Example**

::

    AT+CWMODE=3
    AT+CWCOUNTRY=1,"CN"


AT+CWMONITOR
--------------------------------------------------------------------
Function: Set Wi-Fi to enter monitor mode

Set command
^^^^^^^^^^^

**Function**

Set Wi-Fi to enter monitor mode.

**Command**

::

    AT+ CWMONITOR=<enable>,<channel>,[<min_pkg_len>],[<max_pkg_len>],[<output>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    +CWMONITOR,<channel>,<pkg_len>:<data>
    ...

**Parameters**

-  **<enable>**:

   -  0: Stop the monitor mode.
   -  1: Set Wi-Fi to enter monitor mode.

-  **<channel>**: Channels that need to be monitored.
-  **<min_pkg_len>**: Filter and receive packets with the minimum length.
-  **<max_pkg_len>**: Filter and receive packets with the maximum length.
-  **<output>**:

   -  0: Suppress ``+CWMONITOR`` hex data output (monitor is still active).
   -  1: Enable ``+CWMONITOR`` hex data output (default).

-  **<pkg_len>**: Received packet length.
-  **<data>**: Received raw data packet, displayed in hexadecimal format.


.. note::

   -  The monitored channel needs to be within the channel range set by the `AT+CWCOUNTRY`_.

**Example**

::

    AT+CWMONITOR=1,6

    OK

    +CWMONITOR,6,177:xxxxxxxxxxxxxx
    ...


AT+CWSTAPROTO
-----------------------------------------------------------------------
Function: Query/set the 802.11 b/g/n/ax protocol standard of station mode

Query command
^^^^^^^^^^^^^

**Function**

Query the 802.11 b/g/n/ax protocol standard of station mode.

**Command**

::

    AT+CWSTAPROTO?

**Response**

.. container:: screenoutput

 .. code::

    +CWSTAPROTO:<protocol>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the 802.11 b/g/n/ax protocol standard of station mode.

**Command**

::

    AT+CWSTAPROTO=<protocol>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<protocol>**:

   - bit0: 802.11b protocol standard.

   - bit1: 802.11g protocol standard.

   - bit2: 802.11n protocol standard.

   - bit3: 802.11ax protocol standard.


.. note::

   -  It will only take effect on the next connection.

   -  By default, PHY mode is 802.11 b/g/n/ax mode.

**Example**

::

    AT+CWSTAPROTO=1

    OK

    AT+CWMODE=1

    OK

    AT+CWJAP="SSID","12345678"

    ...



AT+CWAPPROTO
-----------------------------------------------------------------------
Query/set the 802.11 b/g/n/ax protocol standard of softAP mode


Query command
^^^^^^^^^^^^^

**Function**

Query the 802.11 b/g/n/ax protocol standard of softAP mode.

**Command**

::

    AT+CWAPPROTO?

**Response**

.. container:: screenoutput

 .. code::

    +CWAPPROTO:<protocol>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the 802.11 b/g/n/ax protocol standard of softAP mode.

**Command**

::

    AT+CWAPPROTO=<protocol>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<protocol>**:

   - bit0: 802.11b protocol standard.

   - bit1: 802.11g protocol standard.

   - bit2: 802.11n protocol standard.

   - bit3: 802.11ax protocol standard.


.. note::

   -  It will only take effect on the next connection.

   -  By default, PHY mode is 802.11 b/g/n/ax mode.

**Example**

::

    AT+CWAPPROTO=1

    OK

    AT+CWMODE=2

    OK

    AT+CWSAP="AP_SSID","12345678",1,3,10,0

    ...


AT+WPS
-------------------------
Enable the WPS function

Set command
^^^^^^^^^^^

**Function**

Enable the WPS function.

**Command**

::

    AT+WPS=<enable>[,<auth floor>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<enable>**:

   - 1: Enable WPS (Wi-Fi Protected Setup) that uses PBC (Push Button Configuration) mode.

   - 0: Disable WPS that uses PBC mode.

-  **<auth floor>**:

   - 0: Auth open (Default).

   - 1: Auth WEP.

   - 2: Auth WPA PSK.

   - 3: Auth WPA2 PSK.

   - 4: Auth WPA/WPA2 PSK.

.. note::

      -  WPS can only be used when the station is enabled.
      -  WPS doesn't support WEP (Wired-Equivalent Privacy) encryption.

**Example**

::

    AT+CWMODE=1

    OK

    AT+WPS=1

    OK

    ...


AT+CWANTENABLE
----------------------------------------------------
Enable/disable antenna diversity function.

Query command
^^^^^^^^^^^^^

**Function**

Query whether the antenna diversity function is enabled.

**Command**

::

    AT+CWANTENABLE?

**Response**

.. container:: screenoutput

 .. code::

    +CWANTENABLE:<dynamic_enable>,<static_enable>,<pin>

    OK


Set command
^^^^^^^^^^^

**Function**

Enable antenna diversity function.

**Command**

::

    AT+CWANTENABLE=<dynamic_enable>,<static_enable>[,<pin>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<dynamic_enable>**:

   - 1: Enable dynamic antenna diversity function.

   - 0: Disable dynamic antenna diversity function.

-  **<static_enable>**:

   - 1: Enable static antenna diversity function.

   - 0: Disable static antenna diversity function.

-  **<pin>**: Pin number.

**Example**

::

    AT+CWANTENABLE=1,1,0

    OK

    AT+RST

    OK

    AT+CWMODE=1

    OK

    AT+CWJAP="SSID","password"

    OK

    ...

.. note::

   -  This parameter takes effect after a restart.


AT+CWANTNUM
----------------------------------------------------------
Get the maximum number of antennas for the current device.

Query command
^^^^^^^^^^^^^

**Function**

Get the maximum number of antennas for the current device.

**Command**

::

    AT+CWANTNUM?

**Response**

.. container:: screenoutput

 .. code::

    +CWANTNUM:<numbers>

    OK



AT+CWANT
-------------------------------------
Set/query the currently used antenna.

Query command
^^^^^^^^^^^^^

**Function**

Query the currently used antenna.

**Command**

::

    AT+CWANT?

**Response**

.. container:: screenoutput

 .. code::

    +CWANT:<ant_id>

    OK


Set command
^^^^^^^^^^^

**Function**

Enable automatic antenna diversity function.

**Command**

::

    AT+CWANT=<ant_id>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<ant_id>**:

   - Antenna Number ID.

**Example**

::

    AT+CWANT=1

    OK

    AT+CWANT=0

    OK


AT+CWCOEXDUTY
-------------------------------------
Set WiFi/BLE coexistence duty cycle.

Set command
^^^^^^^^^^^

**Function**

Set the WiFi active time (in milliseconds) within one TBTT period for WiFi/BLE coexistence.

**Command**

::

    AT+CWCOEXDUTY=<active_ms>

**Response**

.. container:: screenoutput

 .. code:: text

    OK

Parameters
^^^^^^^^^^

-  **<active_ms>**:

   - WiFi active time in milliseconds within one TBTT period (102.4ms).
   - Range: 10~90
   - 10: WiFi gets 10ms, BLE gets ~92ms (WiFi ~10% duty cycle)
   - 50: WiFi gets 50ms, BLE gets ~52ms (WiFi ~49% duty cycle)
   - 90: WiFi gets 90ms, BLE gets ~12ms (WiFi ~88% duty cycle)

Note
^^^^

- The TBTT period is fixed at 102.4ms (102400us).
- This command configures the time-slicing between WiFi and BLE when coexistence is enabled.
- The actual duty cycle is: WiFi duty = active_ms / 102.4
- Must enable coexistence mode first using AT+CWCOEXEN=1
- Valid range is 10~90ms to ensure both WiFi and BLE get reasonable time slices

Example
^^^^^^^^

::

    // Set WiFi to use 50ms, BLE uses ~52ms (roughly 50% duty cycle)
    AT+CWCOEXDUTY=50

    OK

    // Set WiFi to use 80ms, BLE uses ~22ms (roughly 78% duty cycle)
    AT+CWCOEXDUTY=80

    OK

    // Set WiFi to use 30ms, BLE uses ~72ms (roughly 29% duty cycle)
    AT+CWCOEXDUTY=30

    OK


AT+CWCOEXEN
-------------------------------------
Enable/Disable WiFi/BLE coexistence mode.

Set command
^^^^^^^^^^^

**Function**

Enable or disable WiFi/BLE coexistence mode by entering or exiting power save mode.

**Command**

::

    AT+CWCOEXEN=<enable>

**Response**

.. container:: screenoutput

 .. code:: text

    OK

Parameters
^^^^^^^^^^

-  **<enable>**:

   - 0: Disable WiFi/BLE coexistence mode (exit power save mode)
   - 1: Enable WiFi/BLE coexistence mode (enter power save mode)

Note
^^^^

- Enabling coexistence calls wifi_mgmr_sta_ps_enter() to enter power save mode.
- Disabling coexistence calls wifi_mgmr_sta_ps_exit() to exit power save mode.
- When coexistence is enabled, WiFi and BLE share RF access based on the duty cycle set by AT+CWCOEXDUTY.
- The duty cycle must be configured using AT+CWCOEXDUTY before or after enabling coexistence.
- Coexistence mode requires WiFi to be in station mode and connected to an AP.
- To avoid interference with WiFi beacon intervals (typically 100 ms multiples), Bluetooth connection intervals should be configured to non-100 ms multiples.

Example
^^^^^^^^

::

    // Enable WiFi/BLE coexistence (enter power save mode)
    AT+CWCOEXEN=1

    OK

    // Disable WiFi/BLE coexistence (exit power save mode)
    AT+CWCOEXEN=0

    OK


AT+CSIDMASTART
-------------------------------------
Function: Start CSI (Channel State Information) DMA capture on one buffer

Set command
^^^^^^^^^^^

**Function**

Arm a CSI DMA buffer to capture per-packet channel estimation data from a
target peer. The first start also creates the CSI worker task that prints
the decoded ``CSI: ...`` reports.

.. note::

   -  CSI data is only produced while the device is in monitor (sniffer) mode
      and a frame whose source MAC matches ``<mac>`` is received on the
      monitored channel. After ``AT+CSIDMASTART``, enter monitor mode with
      `AT+CWMONITOR`_ on the channel of the target peer; otherwise no
      ``+CSIDMA:`` report is ever emitted (the command returning ``OK`` only
      means the buffer was configured, not that data has been captured).
   -  When using CSI capture, pass ``<output>=0`` to ``AT+CWMONITOR`` to
      suppress the raw hex ``+CWMONITOR`` data output — only ``+CSIDMA:``
      reports are needed. Example: ``AT+CWMONITOR=1,11,,,0``.
   -  Set ``<mac>`` to the MAC address (BSSID) of an AP/router that is actively
      transmitting on the monitored channel, so that there is real traffic to
      capture.
   -  Currently only 802.11g legacy mode is supported.

**Command**

::

    AT+CSIDMASTART=<mac>[,<buf>][,<mode>][,<ts>][,<interval>][,<ready>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    +CSIDMA:<timestamp>, <port>, <mac>, <rssi>, <noise>, <cfo>, <agc>, <format>, <rate>, <mcs>, <beamformed>, <smoothing>, <nsts>, <nsc>, <csi data...>
    ...

**Parameters**

-  **<mac>**: Target peer MAC address, format ``"00:11:22:33:44:55"`` (mandatory).
-  **<buf>**: Buffer index, range 0 ~ 5.
-  **<mode>**: CSI mode (only honored on full-feature builds, otherwise forced to 0).

   -  0: legacy LTF
   -  1: HT LTF
   -  2: auto

-  **<ts>**: Time control mode (only honored on full-feature builds, otherwise forced to 2).

   -  0: off
   -  1: one-shot
   -  2: periodical
   -  3: FTM trigger

-  **<interval>**: Trigger timestamp / interval in milliseconds.
-  **<ready>**: Buffer ready policy after the ISR.

   -  0: keep the buffer pending; DMA will not overwrite it until it is marked ready again.
   -  1: always mark the buffer ready after the ISR (recommended).

**Example**

::

    // 1. Find the BSSID and channel of the target AP (e.g. by AT+CWJAP?)
    //    +CWJAP:"ssid","b0:7f:b9:ff:73:6e",11,-45,0
    // 2. Arm the capture against the AP BSSID
    AT+CSIDMASTART="b0:7f:b9:ff:73:6e",0

    OK

    // 3. Enter monitor mode on the AP channel to receive its frames
    //    (output=0 suppresses +CWMONITOR hex dumps; only +CSIDMA is needed)
    AT+CWMONITOR=1,11,,,0

    OK

    // 4. CSI reports are reported on the AT channel as the AP transmits
    +CSIDMA:12345678, 0, b0:7f:b9:ff:73:6e, -45, -92, ...
    ...


AT+CSIDMASTOP
-------------------------------------
Function: Stop CSI DMA capture and release resources

Execute command
^^^^^^^^^^^^^^^

**Function**

Stop all CSI DMA buffers. When the last active buffer is stopped, the CSI
worker task is torn down.

**Command**

::

    AT+CSIDMASTOP

**Response**

::

    OK

Set command
^^^^^^^^^^^

**Function**

Stop a single CSI DMA buffer.

**Command**

::

    AT+CSIDMASTOP=<buf>

**Response**

::

    OK

**Parameters**

-  **<buf>**: Buffer index to stop, range 0 ~ 5.


===========================
TCP/IP AT commands
===========================
.. table::
    :widths: 25,75
    :width: 100%
    :align: center

    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | Instruction             | Description                                                                                               |
    +=========================+===========================================================================================================+
    | `AT+CIPV6`_             | Enable/disable the network of Internet Protocol Version 6 (IPv6)                                          |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIFSR`_             | Obtain the local IP address and MAC address                                                               |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPDOMAIN`_         | Resolve a domain name                                                                                     |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPDNS`_            | Query/set DNS server information                                                                          |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MDNS`_              | Start/stop mDNS service advertising or query a LAN mDNS service                                           |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSTATE`_          | Obtain the TCP/UDP/SSL connection information                                                             |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPRECVBUF`_        | Query/set the TCP/UDP/SSL receive buffer size                                                             |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSTART`_          | Establish TCP connection, UDP transmission, or SSL connection                                             |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSTARTEX`_        | Establish TCP connection, UDP transmission, or SSL connection with an automatically assigned ID           |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPTCPOPT`_         | Query/set the socket options                                                                              |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPCLOSE`_          | Close TCP/UDP/SSL connection                                                                              |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSEND`_           | Send data in the normal transmission mode or Wi-Fi passthrough mode                                       |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSENDL`_          | Send long data in parallel in the normal transmission mode                                                |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSENDLCFG`_       | Set the configuration for the command AT+CIPSENDL                                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSENDEX`_         | Send data in the normal transmission mode in expanded ways                                                |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPDINFO`_          | Set "+IPD" message mode                                                                                   |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPMUX`_            | Enable/disable the multiple connections mode                                                              |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSERVER`_         | Delete/create a TCP/SSL server                                                                            |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSERVERMAXCONN`_  | Query/set the maximum connections allowed by a server                                                     |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSTO`_            | Query/set the local TCP server timeout                                                                    |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+SAVETRANSLINK`_     | Set whether to enter Wi-Fi passthrough mode on power-up                                                   |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSNTPCFG`_        | Query/set the time zone and SNTP server                                                                   |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSNTPTIME`_       | Query the SNTP time                                                                                       |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSNTPINTV`_       | Query/set the SNTP time synchronization interval                                                          |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPRECONNINTV`_     | Query/set the TCP/UDP/SSL reconnection interval for the Wi-Fi passthrough mode                            |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPRECVMODE`_       | Query/set socket receiving mode                                                                           |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPRECVDATA`_       | Obtain socket data in passive receiving mode                                                              |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPRECVLEN`_        | Obtain socket data length in passive receiving mode                                                       |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+PING`_              | Ping the remote host                                                                                      |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPEVT`_            | Turn on or off the event message notification prefixed with +CIP.                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CWEVT`_             | Turn on or off the event message notification prefixed with +CW.                                          |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLCCONF`_       | Query/set SSL Clients.                                                                                    |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLSCONF`_       | Query/set SSL Server.                                                                                     |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLSPSK`_        | Query/Set SSL Server Pre-shared Key (PSK) in string format.                                               |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLSPSKHEX`_     | Query/Set SSL Server Pre-shared Key (PSK) in hexadecimal format.                                          |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLCSNI`_        | Query/Set SSL Client Server Name.                                                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLCALPN`_       | Query/set SSL Client Application Layer Protocol Negotiation (ALPN).                                       |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLCPSK`_        | Query/set SSL Client Pre-shared Key (PSK) in string format.                                               |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+CIPSSLCPSKHEX`_     | Query/set SSL Client Pre-shared Key (PSK) in hexadecimal format.                                          |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+IPERF`_             | Test throughput of the device in standalone mode.                                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+IPERFSTOP`_         | Stop test throughput of the device.                                                                       |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+

TCP/IP AT command description
================================

AT+CIPV6
-----------------------------------------------------------------------
Function: Enable/disable the network of Internet Protocol Version 6 (IPv6).

Query command
^^^^^^^^^^^^^^^

**Function**

Query whether IPv6 is enabled.

**Command**

::

    AT+CIPV6?

**Response**

.. container:: screenoutput

 .. code::

    +CIPV6:<enable>

    OK

Set command
^^^^^^^^^^^^^^^

**Function**

Enable/disable IPv6 network.

**Command**

::

    AT+CIPV6=<enable>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<enable>**: status of IPv6 network. Default: 0.

   - 0: disable IPv6 network.
   - 1: enable IPv6 network.


AT+CIFSR
-----------------------------------------------------------------------
Function: Obtain the local IP address and MAC address.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+CIFSR

**Response**

.. container:: screenoutput

 .. code::

    // AP mode
    // AT+CWMODE=2
    +CIFSR:APIP,<"APIP">
    +CIFSR:APIP6LL,<"APIP6LL">
    +CIFSR:APMAC,<"APMAC">

    // STA mode
    // AT+CWMODE=1
    +CIFSR:STAIP,<"STAIP">
    +CIFSR:STAIP6LL,<"STAIP6LL">
    +CIFSR:STAIP6GL,<"STAIP6GL">
    +CIFSR:STAMAC,<"STAMAC">

    OK

**Parameters**

- **<\"APIP\">**: IPv4 address of Wi-Fi softAP interface
- **<”APIP6GL”>**: Global IPv6 address of Wi-Fi softAP interface
- **<\"APMAC\">**: MAC address of Wi-Fi softAP interface
- **<\"STAIP\">**: IPv4 address of Wi-Fi station interface
- **<”STAIP6LL”>**: Link local IPv6 address of Wi-Fi station interface
- **<”STAIP6GL”>**: Global IPv6 address of Wi-Fi station interface
- **<\"STAMAC\">**: MAC address of Wi-Fi station interface

AT+CIPDOMAIN
------------------------------------------------------
Function: Resolve a domain name.

Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPDOMAIN=<"domain name">[,<ip network>]

**Response**

.. container:: screenoutput

 .. code::

    +CIPDOMAIN:<"IP address">

    OK

**Parameters**

-  **<\"domain name\">**: the domain name.
-  **<ip network>**: preferred IP network. Default: 1.

   - 1: preferred resolution of IPv4 address
   - 2: resolve IPv4 address only
   - 3: resolve IPv6 address only

-  **<\"IP address\">**: the resolved IPv4 address or IPv6 address.

**Example**

::

    AT+CWMODE=1                       // set the station mode
    AT+CWJAP="SSID","password"        // access to the internet
    AT+CIPDOMAIN="www.google.com"   // Domain Name Resolution function

    // Domain Name Resolution Function for IPv4 address only
    AT+CIPDOMAIN="www.google.com",2

    // Domain Name Resolution Function for IPv6 address only
    AT+CIPDOMAIN="www.google.com",3

AT+CIPDNS
------------------------------------------------------
Function: Set/query the current DNS server information.

Query command
^^^^^^^^^^^^^^^

**Function**

Query the current DNS server information.

**Command**

::

    AT+CIPDNS?

**Response**

.. container:: screenoutput

 .. code::

    +CIPDNS:<enable>[,<"DNS IP1">][,<"DNS IP2">][,<"DNS IP3">]

    OK

Set command
^^^^^^^^^^^

**Function**

Set DNS server information.

**Command**

::

    AT+CIPDNS=<enable>[,<"DNS IP1">][,<"DNS IP2">][,<"DNS IP3">]

**Response**

.. container:: screenoutput

 .. code::

    OK

or

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

-  **<enable>**: the domain name.

   - 0: Enable automatic DNS server settings from DHCP. Only when the station completes the DHCP process, the DNS server of the station could be updated.
   - 1: Enable manual DNS server settings. If you don't set a value for <DNS IPx>, it will use 208.67.222.222 and 8.8.8.8 by default.

-  **<”DNS IP1”>**: the first DNS server IP address. For the Set command, this parameter only works when you set <enable> to 1, i.e. enable manual DNS settings. If you set <enable> to 1 and a value for this parameter, the AT firmware will return this parameter as the current DNS setting when you run the Query command.

-  **<”DNS IP2”>**: the second DNS server IP address. For the Set command, this parameter only works when you set <enable> to 1, i.e. enable manual DNS settings. If you set <enable> to 1 and a value for this parameter, the AT firmware will return this parameter as the current DNS setting when you run the Query command.

-  **<”DNS IP3”>**: the third DNS server IP address. For the Set command, this parameter only works when you set <enable> to 1, i.e. enable manual DNS settings. If you set <enable> to 1 and a value for this parameter, the AT firmware will return this parameter as the current DNS setting when you run the Query command.


.. note::

    -   The configuration changes will be saved in the NVS area if AT+SYSSTORE=1.
    -   The three parameters can't be set to the same server.
    -   When <enable> is set to 1, the DNS server may change according to the configuration of the router which the device is connected to.


**Example**

::

    AT+CIPDNS=0
    AT+CIPDNS=1,"114.114.114.114"


AT+MDNS
------------------------------------------------------
Function: Start/stop mDNS service advertising or query a LAN mDNS service.

.. note::

    The current implementation supports service advertising and PTR service
    discovery. Direct A/AAAA, SRV-only, and TXT-only query types are not
    supported. The option is disabled by default to keep the default firmware
    footprint small.

Query command
^^^^^^^^^^^^^^^

**Function**

Query the current mDNS advertising state.

**Command**

::

    AT+MDNS?

**Response**

When mDNS advertising is stopped, AT returns:

.. container:: screenoutput

 .. code::

    +MDNS:0

    OK

When mDNS advertising is running, AT returns:

.. container:: screenoutput

 .. code::

    +MDNS:1,<"hostname">,<"service type">,<port>,<"instance">,<"proto">

    OK

Set command
^^^^^^^^^^^

**Function**

Start or stop mDNS service advertising.

**Command**

::

    AT+MDNS=1,<"hostname">,<"service type">,<port>[,<"instance">][,<"proto">][,<txt number>][,<"key">,<"value">][...]
    AT+MDNS=0

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<"hostname">**: Host name advertised as ``<hostname>.local``. The maximum length is 63 bytes.
- **<"service type">**: Service type, such as ``"_http"``. The maximum length is 63 bytes.
- **<port>**: Service port, range 1 ~ 65535.
- **<"instance">**: Optional service instance name. If omitted, the hostname is used.
- **<"proto">**: Optional service protocol. Supported values are ``"_tcp"`` and ``"_udp"``. Default: ``"_tcp"``.
- **<txt number>**: Optional TXT item count, range 1 ~ 5. Each item is configured by a following ``<"key">,<"value">`` pair.

Set command for service query
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Function**

Query PTR records for an mDNS service on the LAN. The command uses the native
lwIP ``mdns_search_service()`` API, so only query type ``0`` (PTR) is supported.
If the mDNS response packet also contains SRV, TXT, A, or AAAA additional
records, AT parses and outputs those records in the same result block.

**Command**

::

    AT+MDNS=2,0,<"service type">,<"proto">[,<timeout ms>][,<max results>]

**Response**

AT returns one or more records, followed by ``OK``:

.. container:: screenoutput

 .. code::

    +MDNS:<data_len>,
    IF=<netif>
    IP=<ip_type>
    PTR=<instance>
    SRV=<hostname>:<port>
    TXT=<key>=<value>
    A=<ip4>
    AAAA=<ip6>

    OK

**Parameters**

- **<type>**: Query type. Only ``0`` (PTR) is supported. Other query types return ``ERROR``.
- **<"service type">**: Service type, such as ``"_http"``. The maximum length is 63 bytes.
- **<"proto">**: Service protocol. Supported values are ``"_tcp"`` and ``"_udp"``.
- **<timeout ms>**: Optional query timeout in milliseconds, range 1000 ~ 180000. Default: 5000.
- **<max results>**: Optional maximum number of returned result blocks, range 1 ~ 255. Default: 1.
- **<data_len>**: Length of the following result block, including line endings.
- **<netif>**: Network interface. ``1``: Wi-Fi Station, ``2``: Wi-Fi SoftAP, ``3``: Ethernet.
- **<ip_type>**: IP address type. ``0``: IPv4, ``1``: IPv6.

**Example**

::

    AT+CWMODE=1
    AT+CWJAP="SSID","password"
    AT+MDNS=1,"at-mdns","_http",80,"web","_tcp",1,"path","/"
    AT+MDNS?
    AT+MDNS=2,0,"_http","_tcp",5000,4
    AT+MDNS=0

**Usage example**

The station must already be connected to the same LAN as the peer
device that publishes or discovers mDNS services.

Verify service advertising:

::

    AT+MDNS=0
    AT+MDNS?
    AT+MDNS=1,"at-mdns-ci","_atmdns",43211,"ci-at-device","_tcp",1,"version","v1"
    AT+MDNS?

Expected key responses:

.. container:: screenoutput

 .. code::

    +MDNS:0

    OK
    +MDNS:1,"at-mdns-ci","_atmdns",43211,"ci-at-device","_tcp"

    OK

On the PC, run:

::

    avahi-browse -r -t _atmdns._tcp

Expected key output:

.. container:: screenoutput

 .. code::

    + <interface> IPv4 ci-at-device _atmdns._tcp local
    = <interface> IPv4 ci-at-device _atmdns._tcp local
       hostname = [at-mdns-ci.local]
       address = [<device-ipv4>]
       port = [43211]
       txt = ["version=v1"]

Stop service advertising after the PC can discover it:

::

    AT+MDNS=0
    AT+MDNS?

Expected key responses:

.. container:: screenoutput

 .. code::

    OK
    +MDNS:0

    OK

Verify PTR query. On the PC, publish an mDNS service:

::

    avahi-publish -s ci-publisher _atmdns._tcp 43212 version=v2

Run:

::

    AT+MDNS=2,0,"_atmdns","_tcp",10000,1

Expected key response:

.. container:: screenoutput

 .. code::

    +MDNS:<data_len>,
    IF=1
    IP=0
    PTR=ci-publisher
    SRV=<pc-hostname>.local:43212
    TXT=version=v2
    A=<pc-ipv4>

    OK

The SRV, TXT, A, and AAAA lines depend on the records included in the received
mDNS response packet. A successful PTR query must return ``PTR=ci-publisher``
and ``OK``. If ``<max results>`` is greater than the number of services found,
AT returns the collected results after ``<timeout ms>`` expires.


AT+CIPSTATE
----------------------------------------------------------------------------------------
Function: Obtain the TCP/UDP/SSL connection information.

Query command
^^^^^^^^^^^^^^^

**Command**

::

    AT+CIPSTATE?

**Response**

When there is a connection, AT returns:

.. container:: screenoutput

 .. code::

    +CIPSTATE:<link ID>,<"type">,<"remote IP">,<remote port>,<local port>,<tetype>

    OK

When there is no connection, AT returns:

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<link ID>**: ID of the connection (0~4), used for multiple connections.
-  **<\"type\">**: string parameter showing the type of transmission: "TCP", "TCPv6", "UDP", "UDPv6", "SSL", or "SSLv6".
-  **<\"remote IP\">**: string parameter showing the remote IPv4 address.
-  **<remote port>**: the remote port number.
-  **<local port>**: the local port number.
-  **<tetype>**:

   -  0: the device runs as a client.
   -  1: the device runs as a server.

AT+CIPRECVBUF
----------------------------------------------------------------------------------------
Function: Query/set the TCP/UDP/SSL receive buffer size.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    // Single connection (AT+CIPMUX=0):
    AT+CIPRECVBUF=<size>

    // Multiple connection (AT+CIPMUX=1):
    AT+CIPRECVBUF=<linkid>,<size>

**Response**

This command needs to be used when there is no connection with this ID; otherwise, it will return an ERROR.

.. container:: screenoutput

 .. code::

    +CIPRECVBUF

    OK

**Parameters**

-  **<linkid>**: ID of the connection (0~4), used for multiple connections.
-  **<size>**: Set the size of the buffer, unit bytes.

.. note::

  - The receive buffer holds data from the remote peer; if the incoming payload exceeds its configured size, excess bytes are dropped.


Query command
^^^^^^^^^^^^^^^

**Command**

::

    // Single connection (AT+CIPMUX=0):
    AT+CIPRECVBUF?

    // Multiple connection (AT+CIPMUX=1):
    AT+CIPRECVBUF=<linkid>?

**Response**


Returns the buffer size for the specified link ID. The default buffer size is 4096 bytes.

.. container:: screenoutput

 .. code::

    +CIPRECVBUF:<size>

    OK


AT+CIPSTART
--------------------------------------------------------------------------------------------
Function: Establish TCP connection, UDP transmission, or SSL connection.

Establish TCP connection
^^^^^^^^^^^^^^^^^^^^^^^^

Set command
""""""""""""

**Command**

::

    // Single connection (AT+CIPMUX=0):
    AT+CIPSTART=<"type">,<"remote host">,<remote port>[,<local_port>][,<mode>][,<keep_alive>][,<"local IP">][,<timeout>]

    // Multiple Connections (AT+CIPMUX=1):
    AT+CIPSTART=<link ID>,<"type">,<"remote host">,<remote port>[,<local_port>][,<mode>][,<keep_alive>][,<"local IP">][,<timeout>]

**Response**

For single connection, it returns:

.. container:: screenoutput

 .. code::

    +CIP:CONNECTED

    OK

For multiple connections, it returns:

.. container:: screenoutput

 .. code::

    +CIP:<link ID>,CONNECTED

    OK

**Parameters**

-  **<link ID>**: ID of network connection (0~4), used for multiple connections.
-  **<\"type\">**: string parameter showing the type of transmission: "TCP","TCPv6","UDP","UDPv6","SSL","SSLv6". Default: "TCP".
-  **<\"remote host\">**: IPv4/IPv6 address,  or domain name of remote host. The maximum length is 64 bytes. If you need to use a domain name and the length of the domain name exceeds 64 bytes, use the AT+CIPDOMAIN command to obtain the IP address corresponding to the domain name, and then use the IP address to establish a connection.
-  **<remote port>**: the remote port number.
-  **<local port>**: Available only for UDP/UDPv6, bound local address.
-  **<mode>**: Available only for UDP/UDPv6, reserved, default is 0. This parameter is reserved for future use and currently has no effect. It may be used in future firmware to select advanced UDP behaviors, but for now, you can ignore it.
-  **<keep_alive>**: It configures the `SO_KEEPALIVE <https://man7.org/linux/man-pages/man7/socket.7.html#SO_KEEPALIVE>`__ option for socket. Unit: second.

   - Range: [0,7200].

     - 0: disable keep-alive function (default).
     - 1 ~ 7200: enable keep-alive function. `TCP_KEEPIDLE <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPIDLE>`_ value is **<keep_alive>**, `TCP_KEEPINTVL <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPINTVL>`_ value is 1, and `TCP_KEEPCNT <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPCNT>`_ value is 3.

   - This parameter of this command is the same as the ``<keep_alive>`` parameter of AT+CIPTCPOPT command. It always takes the value set later by either of the two commands. If it's omitted or not set, the last configured value is used by default.

-  **<\"local IP\">**: the local IPv4 address or IPv6 address that the connection binds. This parameter is useful when you are using multiple network interfaces or multiple IP addresses. By default, it's disabled. If you want to use it, you should specify it first. Null is also valid.

-  **<timeout>**: Command timeout. Unit: milliseconds. Default value: 10000. Range: [0,20000]. When set to 0, the command timeout depends on the network and lwIP protocol stack; when set to a non-zero value, the command will return within the specified timeout.

.. note::

    -   To establish a TCP connection based on an IPv6 network, do as follows:

        -   Make sure that the AP supports IPv6

        -   Set AT+CIPV6=1

        -   Obtain an IPv6 address through the AT+CWJAP command

        -   (Optional) Check whether the device has obtained an IPv6 address using the AT+CIPSTA? command


**Example**

::

    AT+CIPSTART="TCP","192.168.101.110",1000
    AT+CIPSTART="TCP","192.168.101.110",2500,60
    AT+CIPSTART="TCP","192.168.101.110",1000,,"192.168.101.100"
    AT+CIPSTART="TCPv6","test-ipv6.com",80
    AT+CIPSTART="TCPv6","fe80::860d:8eff:fe9d:cd90",1000,,"fe80::411c:1fdb:22a6:4d24"

Query command
""""""""""""""

**Command**

::

    AT+CIPSTART?

**Response**

For single connection, it returns:

.. container:: screenoutput

 .. code::

    +CIPSTART:<link ID>,<"type">,<"ip">,<remote_port>,<local_port>,<tetype>

    OK

**Parameters**

-  **<link ID>**: ID of the connection (0~4), used for multiple connections.
-  **<\"type\">**: string parameter showing the type of transmission: "TCP", "TCPv6", "UDP", "UDPv6", "SSL", or "SSLv6".
-  **<\"remote IP\">**: string parameter showing the remote IPv4 address.
-  **<remote port>**: the remote port number.
-  **<local port>**: the local port number.
-  **<tetype>**:

    -  0: the device runs as a client (created by AT+CIPSTART).
    -  1: the device runs as a server (created by AT+CIPSERVER or remote connection). If a remote client connects to the server, a socket is dedicated to it. Querying AT+CIPSTART? for such a socket (link_ID) will return <tetype> = 1, even though it was not created by CIPSTART.

AT+CIPSTARTEX
--------------------------------------------------------------------------------------------------------------------------------
Function: Establish TCP connection, UDP transmission, or SSL connection with an Automatically Assigned ID.

This command is similar to AT+CIPSTART except that you don't need to assign an ID by yourself in multiple connections mode (AT+CIPMUX=1). The system will assign an ID to the new connection automatically.

[Data Mode Only] +++ : Exit from Data Mode
-------------------------------------------------------------------

Special Execute command
^^^^^^^^^^^^^^^^^^^^^^^^

**Function**

Exit from Data Mode and enter the Command Mode.

**Command**

::

    // Only for data mode
    +++

.. note::

    -  This special execution command consists of three identical ``+`` characters (0x2b ASCII), and no CR-LF appends to the command tail.
    -  Make sure there is more than 20 ms interval before the first ``+`` character, more than 20 ms interval after the third ``+`` character, less than 20 ms interval among the three ``+`` characters. Otherwise, the ``+`` characters will be sent out as normal data.
    -  This command returns no reply.
    -  Please wait for at least one second before sending the next AT command.

AT+CIPTCPOPT
-------------------------------------------------------------
Function: Query/set the socket options.

Query command
^^^^^^^^^^^^^

**Function**

Query current socket options.

**Command**

::

    AT+CIPTCPOPT?

**Response**

.. container:: screenoutput

 .. code::

    +CIPTCPOPT:<link_id>,<so_linger>,<tcp_nodelay>,<so_sndtimeo>,<keepalive>

    OK

Set command
^^^^^^^^^^^

**Command**

::

    // Single TCP connection (AT+CIPMUX=0):
    AT+CIPTCPOPT=[<so_linger>],[<tcp_nodelay>],[<so_sndtimeo>],[<keepalive>]

    // Multiple TCP Connections (AT+CIPMUX=1):
    AT+CIPTCPOPT=<link ID>,[<so_linger>],[<tcp_nodelay>],[<so_sndtimeo>],[<keepalive>]

**Response**

.. container:: screenoutput

 .. code::

    OK

or

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

-  **<link_id>**: ID of the connection (0 ~ max). For multiple connections, if the value is max, it means all connections. By default, max is 5.
-  **<so_linger>**: configure the ``SO_LINGER`` options for the socket (refer to `SO_LINGER description <https://man7.org/linux/man-pages/man7/socket.7.html#SO_LINGER>`_). Unit: second. Default: -1.

   -  = -1: off
   -  = 0: on, linger time = 0
   -  > 0: on, linger time = <so_linger>

-  **<tcp_nodelay>**: configure the ``TCP_NODELAY`` option for the socket (refer to `TCP_NODELAY description <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_NODELAY>`_). Default: 0.

   -  0: disable TCP_NODELAY
   -  1: enable TCP_NODELAY

-  **<so_sndtimeo>**: configure the ``SO_SNDTIMEO`` option for socket (refer to `SO_SNDTIMEO description <https://man7.org/linux/man-pages/man7/socket.7.html#SO_SNDTIMEO>`_). Unit: millisecond. Default: 0.
-  **<keepalive>**: It configures the `SO_KEEPALIVE <https://man7.org/linux/man-pages/man7/socket.7.html#SO_KEEPALIVE>`__ option for socket. Unit: second.

   - Range: [0,7200].

     - 0: disable keep-alive function (default).
     - 1 ~ 7200: enable keep-alive function. `TCP_KEEPIDLE <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPIDLE>`_ value is **<keepalive>**, `TCP_KEEPINTVL <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPINTVL>`_ value is 1, and `TCP_KEEPCNT <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPCNT>`_ value is 3.

   - This parameter of this command is the same as the ``<keepalive>`` parameter of AT+CIPTCPOPT command. It always takes the value set later by either of the two commands. If it's omitted or not set, the last configured value is used by default.
     Setting a low keepalive value in low-power mode will result in increased power consumption.

.. note::

    -  Before configuring these socket options, make sure you fully understand the function of them and the possible impact after configuration.
    -  The SO_LINGER option isn't recommended to be set to a large value. For example, if you set SO_LINGER value to 60, then AT+CIPCLOSE command will block for 60 seconds if the device can't receive TCP FIN packet from the remote TCP peer due to network issues, so the device is unable to respond to any other AT commands. Therefore, it's recommended to keep the default value of the SO_LINGER option.
    -  The TCP_NODELAY option is used for situations with small throughput but high real-time requirements. If this option is enabled, LwIP will speed up TCP transmission, but in a poor network environment, the throughput will be reduced due to retransmission. Therefore, it's recommended to keep the default value of the TCP_NODELAY option.
    -  The SO_SNDTIMEO option is used for situations where the keepalive parameter isn't configured in AT+CIPSTART command. After this option is configured, AT+CIPSEND , AT+CIPSENDL , and AT+CIPSENDEX commands will exit within this timeout, regardless of whether data are sent successfully or not. Here, SO_SNDTIMEO is recommended to be set to 5 ~ 10 seconds.
    -  The SO_KEEPALIVE option is used for actively and regularly detecting whether the connection is disconnected. It's generally recommended to configure this option when AT is used as a TCP server. After this option is configured, additional network bandwidth will be cost. Recommended value of SO_KEEPALIVE should be not less than 60 seconds. Setting a low keepalive value in low-power mode will result in increased power consumption.


AT+CIPCLOSE
-----------------------------------------------------------
Function: Close TCP/UDP/SSL connection.

Set command
^^^^^^^^^^^^^

**Function**

Close TCP/UDP/SSL connection in the multiple connections mode.

**Command**

::

    AT+CIPCLOSE=<link ID>

**Response**

.. container:: screenoutput

 .. code::

    <link ID>,CLOSED

    OK

Execute command
^^^^^^^^^^^^^^^^^

**Function**

Close TCP/UDP/SSL connection in the single connection mode.

::

    AT+CIPCLOSE

**Response**

.. container:: screenoutput

 .. code::

    CLOSED

    OK

**Parameters**

-  **<link ID>**: ID of the connection that you want to close. If you set it to 5, all connections will be closed.

AT+CIPSEND
-----------------------------------------------------------------------------------------------------------------
Function: Send data in the normal transmission mode or Wi-Fi passthrough mode

Set command
^^^^^^^^^^^

**Function**

Set the data length to be sent in the normal transmission mode. AT+CIPSEND returns error if <length> is bigger than 6144. If the number of bytes sent by the host exceeds the declared <length>, the excess portion will be discarded. If the length of data you need to send exceeds 6144 bytes, please use the AT+CIPSENDL command.

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSEND=<length>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSEND=<link ID>,<length>

    // Remote host and port can be set for UDP transmission:
    AT+CIPSEND=[<link ID>,]<length>[,<"remote host">,<remote port>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving serial data. You should enter the data, and when the data length reaches the ``<length>`` value, the transmission of data starts.

If the connection can't be established or is disrupted during data transmission, the system returns:

.. container:: screenoutput

 .. code::

    ERROR

If the data has been successfully sent to the protocol stack (It doesn't mean that the data has been sent to the opposite end), the system returns:

.. container:: screenoutput

 .. code::

    SEND OK

Execute command
^^^^^^^^^^^^^^^

**Function**

Enter the Wi-Fi passthrough mode.

**Command**

::

    AT+CIPSEND

**Response**

.. container:: screenoutput

 .. code::

    OK
    >

or

.. container:: screenoutput

 .. code::

    ERROR

Enter the Wi-Fi Passthrough Mode. This command can only be used for a single connection in the Wi-Fi Passthrough Mode.

**Parameters**

-  **<link ID>**: ID of the connection (0~4), for multiple connections.
-  **<length>**: data length. Maximum: 6144 bytes.
-  **<\"remote host\">**: IPv4 address, or domain name of remote host. It can be set in UDP transmission.
-  **<remote port>**: the remote port number.

.. note::

    - You can use AT+CIPTCPOPT command to configure socket options for each TCP connection. For example, setting <so_sndtimeo> to 5000 will enable TCP send operation to return results within 5 seconds, regardless of success or failure. This can save the time that the MCU waits for AT command response.

AT+CIPSENDL
--------------------------------------------------------------------------------------------------
Function: Send long data in the normal transmission mode.

Set command
^^^^^^^^^^^

**Function**

In normal transmission mode, set the length of data to be sent, then transmit the data to the remote host. AT+CIPSEND does not allow setting a length parameter exceeding 6144, whereas AT+CIPSENDL can be set up to 2^16 - 1. However, it is important to note that data must still be sent in 6144-byte chunks; otherwise, memory corruption may occur.

The difference in command sequence between the two is:

AT+CIPSEND needs to be called repeatedly. For example, when sending 15000 bytes:

::

    AT+CIPSEND=6144

    6144 bytes

    AT+CIPSEND=6144
    SEND OK

    6144 bytes

    AT+CIPSEND=2712
    SEND OK

    2712 bytes
    SEND OK

With AT+CIPSENDL, the process can be simplified to:

::

    AT+CIPSENDL=15000

    6144 bytes
    6144 bytes
    2712 bytes
    SEND OK

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSENDL=<length>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSENDL=<link ID>,<length>

    // Remote host and port can be set for UDP transmission:
    AT+CIPSENDL=[<link ID>,]<length>[,<"remote host">,<remote port>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT enters the Data Mode and AT command port is ready to receive data. You can enter the data now. Once the port receives data, it will be pushed to underlying protocol stack and the transmission starts.

If the transmission starts, the system reports message according to AT+CIPSENDLCFG configuration:

.. container:: screenoutput

 .. code::

    +CIPSENDL:<had sent len>,<port recv len>

If the transmission is cancelled by +++ command, the system returns:

.. container:: screenoutput

 .. code::

    SEND CANCELLED

If not all the data has been sent out, the system finally returns:

.. container:: screenoutput

 .. code::

    SEND FAIL

If the data has been successfully sent to the protocol stack (It doesn't mean that the data has been sent to the opposite end), the system finally returns:

.. container:: screenoutput

 .. code::

    SEND OK

When the connection is disconnected, you can send +++ command to cancel the transmission, then the device will exit from the Data Mode, otherwise, the Data Mode won't end until the AT command port receives all the data of the specified ``<length>``.

**Parameters**

-  **<link ID>**: ID of the connection (0~4), for multiple connections.
-  **<length>**: data length. Maximum: 2 :sup:`31` - 1 bytes.
-  **<\"remote host\">**: IPv4 address, or domain name of remote host. It can be set in UDP transmission.
-  **<remote port>**: the remote port number.
-  **<had sent len>**: the length of data successfully sent to the underlying protocol stack.
-  **<port recv len>**: data length received by AT command port.

.. note::

    - It's recommended to use UART flow control, because if the UART receives data at a faster rate than the network sends, data loss may occur.
    - You can use AT+CIPTCPOPT command to configure socket options for each TCP connection. For example, setting <so_sndtimeo> to 5000 will enable TCP send operation to return results within 5 seconds, regardless of success or failure. This can save the time that the MCU waits for AT command response.

Example
^^^^^^^^

::

    AT+CIPSENDL=24000

    >5000-bytes

    >5000-bytes

    >5000-bytes

    >5000-bytes

    >5000-bytes

    SEND OK

    //Last 1000 bytes (5*5000-24000) will be dropped because exceeding the <length>=24000
    AT+CIPSENDL=24000

    >7000-bytes  (no guarantee that it works, data in the chunk is not droped above 6144, memory can get corrupted and chip get stuck)
    >7000-bytes  (no guarantee that it works, data in the chunk is not droped above 6144, memory can get corrupted and chip get stuck)
    >7000-bytes  (no guarantee that it works, data in the chunk is not droped above 6144, memory can get corrupted and chip get stuck)
    >3000-bytes


AT+CIPSENDLCFG
------------------------------------------------------------------------------------------------------
Function: Set the configuration for the command AT+CIPSENDL.

Query command
^^^^^^^^^^^^^

**Function**

Query the configuration of AT+CIPSENDL

**Command**

::

    AT+CIPSENDLCFG?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSENDLCFG:<report size>,<transmit size>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the configuration of AT+CIPSENDL.

**Command**

::

    AT+CIPSENDLCFG=<report size>[,<transmit size>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<report size>**: report block size for AT+CIPSENDL. Default: 1024. Range: [100,2 :sup:`20`]. For example, set ``<report size>`` to 100, ``<had sent len>`` report sequence in the response of AT+CIPSENDL will be (100, 200, 300, 400, ...). The final ``<had sent len>`` report is always equal to the data length that had been sent out.
-  **<transmit size>**: transmit block size of AT+CIPSENDL. It specifies the size of the data block sent to the underlying protocol stack. Default: 1536. If the data received by the device reaches or exceeds ``<transmit size>`` bytes, the data will be immediately sent in chunks of ``<transmit size>`` bytes. Otherwise, it will wait for 20 milliseconds before being sent.
   The 20ms delay is added for short data to optimize throughput and avoid excessive fragmentation. When small amounts of data arrive, the module waits briefly to see if more data will follow, allowing it to combine multiple small packets into a larger one. This reduces network overhead and improves efficiency. (This parameter does not affect the behavior of the Host).

.. note::

    - For devices with small throughput but high real-time requirements, it is recommended to set a smaller ``<transmit size>``. It is also recommended to set ``TCP_NODELAY`` by AT+CIPTCPOPT command.
    - For devices with large throughput, it is recommended to set a larger ``<transmit size>``.

Example
^^^^^^^^

- if chunks are shorter then ``<report size>`` +CIPSENDL will report each time ``<report size>`` is reached.

- if chunks are bigger then ``<report size>`` some +CIPSENDL be skip.

::

    AT+CIPSENDLCFG=3000,1536

    AT+CIPSENDL=14000

    >5000-bytes

    +CIPSENDL:3000,5000

    >5000-bytes

    +CIPSENDL:9000,10000   //``<had sent len>`` multiple of 3000, and 6000 is skip in such case, only the last multiple (9000) is reported

    >3000-bytes

    +CIPSENDL:12000,13000

    >1000-bytes

    SEND OK

AT+CIPSENDEX
----------------------------------------------------------------------------------------------------
Function: Send data in the normal transmission mode in expanded ways.

Set command
^^^^^^^^^^^

**Function**

Set the data length to be send in the normal transmission mode or use ``\0`` (0x5c, 0x30 ASCII) to trigger data transmission. Do not exceed 6144 bytes in a single transmission, as this may cause the AT module to crash.

The differences from AT+CIPSEND are as follows:

::

    //When we know the length of data to be transmitted
    AT+CIPSEND=20

    20 bytes

    //The length of data to be sent is unknown, just fill in a longer length arbitrarily.
    AT+CIPSENDEX=2000

    20 bytes
    \0       //Here, the transmission has already been triggered, and subsequently sent data will be ignored.
    n bytes

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSENDEX=<length>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSENDEX=<link ID>,<length>

    // Remote host and port can be set for UDP transmission:
    AT+CIPSENDEX=[<link ID>,]<length>[,<"remote host">,<remote port>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving data. You should enter the data of designated length. When the data length reaches the ``<length>`` value, or when the string ``\0`` appears in the data, the transmission starts.

If the connection can't be established or gets disconnected during transmission, the system returns:

.. container:: screenoutput

 .. code::

    ERROR

If the data has been successfully sent to the protocol stack (It doesn't mean that the data has been sent to the opposite end), the system returns:

.. container:: screenoutput

 .. code::

    Recv <length> bytes
    SEND OK

**Parameters**

-  **<link ID>**: ID of the connection (0~4), for multiple connections.
-  **<length>**: data length. Maximum: 6144 bytes.
-  **<\"remote host\">**: IPv4 address, or domain name of remote host. It can be set in UDP transmission.
-  **<remote port>**: remote port can be set in UDP transmission.

.. note::

    - When the requirement of data length is met, or when the string ``\0`` (0x5c, 0x30 in ASCII) appears, the transmission of data starts. Go back to the normal command mode and wait for the next AT command.
    - If the data contains the ``\<any>``, it means that drop backslash symbol and only use ``<any>`` character.
    - When sending ``\0``, please use a backslash to escape it as ``\\0``.
    - You can use AT+CIPTCPOPT command to configure socket options for each TCP connection. For example, setting <so_sndtimeo> to 5000 will enable TCP send operation to return results within 5 seconds, regardless of success or failure. This can save the time that the MCU waits for AT command response.

AT+CIPDINFO
------------------------------------------------------
Function: Set "+IPD" message mode.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CIPDINFO?

**Response**

.. container:: screenoutput

 .. code::

    +CIPDINFO:true

    OK

or

.. container:: screenoutput

 .. code::

    +CIPDINFO:false

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPDINFO=<mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**:

   -  0: doesn't show the remote host and port in "+IPD" messages.
   -  1: show the remote host and port in "+IPD" messages.

**Example**

::

    AT+CIPDINFO=1

AT+CIPMUX
----------------------------------------------------------------
Function: Enable/disable multiple connections.

Query command
^^^^^^^^^^^^^

**Function**

Query the connection type.

**Command**

::

    AT+CIPMUX?

**Response**

.. container:: screenoutput

 .. code::

    +CIPMUX:<mode>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the connection type.

**Command**

::

    AT+CIPMUX=<mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**: connection mode. Default: 0.

   -  0: single connection.
   -  1: multiple connections.

.. note::

    -  This mode can only be changed after all connections are disconnected.
    -  If you want to set the single connection mode when the TCP/SSL server is running, you should delete the server first. (AT+CIPSERVER=0).

**Example**

.. container:: screenoutput

 .. code::

    AT+CIPMUX=1


AT+CIPSERVER
--------------------------------------------------------------
Function: Delete/create a TCP/SSL Server.

Query command
^^^^^^^^^^^^^

**Function**

Query the TCP/SSL server status.

**Command**

::

    AT+CIPSERVER?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSERVER:<mode>[,<port>,<"type">][,<CA enable>][,<keepalive>]

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPSERVER=<mode>[,<param2>][,<"type">][,<CA enable>][,<keepalive>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**:

   -  0: delete a server.
   -  1: create a server.

-  **<param2>**: It means differently depending on the parameter ``<mode>``:

   - If ``<mode>`` is 1, ``<param2>`` represents the port number. Default: 333.
   - If ``<mode>`` is 0, ``<param2>`` represents whether the server closes all connections. Default: 0.

       - 0: shut down the server and keep existing connections.
       - 1: shut down the server and close all connections.

-  **<\"type\">**: server type: "TCP", "TCPv6", "SSL", or "SSLv6". Default: "TCP".

-  **<CA enable>**: This parameter is not currently in use.

-  **<keepalive>**: It configures the `SO_KEEPALIVE <https://man7.org/linux/man-pages/man7/socket.7.html#SO_KEEPALIVE>`__ option for socket. Unit: second.

   - Range: [0,7200].

     - 0: disable keep-alive function (default).
     - 1 ~ 7200: enable keep-alive function. `TCP_KEEPIDLE <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPIDLE>`_ value is **<keepalive>**, `TCP_KEEPINTVL <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPINTVL>`_ value is 1, and `TCP_KEEPCNT <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPCNT>`_ value is 3.

   - This parameter of this command is the same as the ``<keepalive>`` parameter of AT+CIPTCPOPT command. It always takes the value set later by either of the two commands. If it's omitted or not set, the last configured value is used by default.
     Setting a low keepalive value in low-power mode will result in increased power consumption.

.. note::

    -  A TCP/SSL server can only be created when multiple connections are activated (AT+CIPMUX=1).
    -  A server monitor will be created automatically when the server is created. Only one server can be created at most.
    -  When a client is connected to the server, it will take up one connection and be assigned an ID.
    -  Parameters ``<\"type\">`` and ``<CA enable>`` must be omitted when delete a server.

**Example**

::

    // To create a TCP server
    AT+CIPMUX=1
    AT+CIPSERVER=1,80

    // To create an SSL server
    AT+CIPMUX=1
    AT+CIPSERVER=1,443,"SSL",1

    // To delete an server and close all clients
    AT+CIPSERVER=0,1

AT+CIPSERVERMAXCONN
---------------------------------------------------------------------------------------------
Function: Query/set the maximum connections allowed by a server.

Query command
^^^^^^^^^^^^^

**Function**

Obtain the maximum number of clients allowed to connect to the TCP/SSL server.

**Command**

::

    AT+CIPSERVERMAXCONN?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSERVERMAXCONN:<num>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the maximum number of clients allowed to connect to the TCP/SSL server.

**Command**

::

    AT+CIPSERVERMAXCONN=<num>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<num>**: the maximum number of clients allowed to connect to the TCP/SSL server. Range: [1,5]. For how to change the upper limit of this range, please refer to the description of the ``<link ID>`` parameter of the AT+CIPSTART command.

.. note::

   -  You should call the command ``AT+CIPSERVERMAXCONN=<num>`` before creating a server.

**Example**

::

    AT+CIPMUX=1
    AT+CIPSERVERMAXCONN=2
    AT+CIPSERVER=1,80

AT+CIPSTO
-----------------------------------------------------------------------
Function: Query/set the local TCP/SSL server timeout.

Query command
^^^^^^^^^^^^^

**Function**

Query the local TCP/SSL server timeout.

**Command**

::

    AT+CIPSTO?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSTO:<time>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the local TCP/SSL server timeout.

**Command**

::

    AT+CIPSTO=<time>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<time>**: local TCP/SSL server timeout. Unit: second. Range: [0,7200].

.. note::

    -  When a TCP/SSL client doesn't communicate with the server within the ``<time>`` value, the server will terminate this connection.
    -  If you set ``<time>`` to 0, the connection will never timeout. This configuration isn't recommended.
    -  When the client initiates a communication with the server or the server initiate a communication with the client within the set time, the timer will restart. After the timeout expires, the client is closed.
    -  Setting this parameter while in low-power mode will lead to increased power draw.

**Example**

::

    AT+CIPMUX=1
    AT+CIPSERVER=1,1001
    AT+CIPSTO=10

AT+SAVETRANSLINK
----------------------------------------------------------------------------------------------------
Function: Set whether to enter Wi-Fi passthrough mode on power-up.

For TCP/SSL single connection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set command
""""""""""""""

**Command**

::

    AT+SAVETRANSLINK=<mode>,<"remote host">,<remote port>[,<"type">,<keep alive>,<retry count>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**:

   -  0: the device won't enter Wi-Fi passthrough mode on power-up.
   -  1: the device will enter Wi-Fi passthrough mode on power-up.

-  **<\"remote host\">**: string parameter showing the IPv4 address or domain name of remote host.
-  **<remote port>**: the remote port number.
-  **<\"type\">**: string parameter showing the type of transmission: "TCP", "TCPv6", "SSL", or "SSLv6". Default: "TCP".
-  **<keep alive>**: TCP keep-alive interval. Default: 0.
-  **<retry count>**: After power on, the number of retries for connecting to the remote host. Exceeding this count will cause exit from Passthrough Mode. Default value: 10, Range: [1, 255]

   -  0: disable the keep-alive function.
   -  1 ~ 7200: detection interval. Unit: second.

.. note::

    -  This command will save the Wi-Fi Passthrough Mode configuration in the easyflash area. If ``<mode>`` is set to 1, the device will enter the Wi-Fi Passthrough Mode in any subsequent power cycles. The configuration will take effect after the device reboots.
    -  This command is only allowed to run in active receiving mode, and will forcibly switch to active receiving mode after the next reset.
    -  As long as the remote host and port are valid, the configuration will be saved in flash.
    - There are **two alternative methods** to completely exit Wi‑Fi Passthrough Mode:
        1. **Normal method:**
           Use ``+++`` to exit Passthrough Mode, then run ``AT+SAVETRANSLINK=0`` to disable Passthrough Mode on the next reset.
        2. **Drastic method:**
           Reflash the **device AT‑module firmware**.  (Reflashing only the host MCU firmware is not sufficient.)

**Example**

::

    AT+SAVETRANSLINK=1,"192.168.6.110",1002,"TCP"
    AT+SAVETRANSLINK=1,"www.google.com",443,"SSL"
    AT+SAVETRANSLINK=1,"240e:3a1:2070:11c0:55ce:4e19:9649:b75",8080,"TCPv6"
    AT+SAVETRANSLINK=1,"240e:3a1:2070:11c0:55ce:4e19:9649:b75",8080,"SSLv6"

For UDP Transmission
^^^^^^^^^^^^^^^^^^^^

Set command
""""""""""""""

**Command**

::

    AT+SAVETRANSLINK=<mode>,<"remote host">,<remote port>,[<"type">,<local port>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**:

   -  0: the device won't enter Wi-Fi passthrough mode on power-up.
   -  1: the device will enter Wi-Fi passthrough mode on power-up.

-  **<\"remote host\">**: string parameter showing the IPv4 address or domain name of remote host.
-  **<remote port>**: the remote port number.
-  **<\"type\">**: string parameter showing the type of transmission: "UDP" or "UDPv6". Default: "TCP".
-  **<local port>**: local port when UDP Wi-Fi passthrough is enabled on power-up.

.. note::

    -  This command will save the Wi-Fi passthrough mode configuration in the easyflash area. If ``<mode>`` is set to 1, the device will enter the Wi-Fi passthrough mode in any subsequent power cycles. The configuration will take effect after the device reboots.
    -  As long as the remote host and port are valid, the configuration will be saved in flash.

**Example**

::

    AT+SAVETRANSLINK=1,"192.168.6.110",1002,"UDP",1005
    AT+SAVETRANSLINK=1,"240e:3a1:2070:11c0:55ce:4e19:9649:b75",8081,"UDPv6",1005

AT+CIPSNTPCFG
----------------------------------------------------------------------------
Function: Query/set the time zone and the SNTP server.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CIPSNTPCFG?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSNTPCFG:<enable>,<timezone>,<SNTP server1>[,<SNTP server2>,<SNTP server3>]

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPSNTPCFG=<enable>,<timezone>[,<SNTP server1>,<SNTP server2>,<SNTP server3>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<enable>**: configure the SNTP server:

   -  1: the SNTP server is configured.
   -  0: the SNTP server isn't configured.

-  **<timezone>**: support the following two formats:

   -  The first format range is [-12,14]. It marks most of the time zones by offset from Coordinated Universal Time (UTC) in **whole hours** (`UTC–12:00 <https://en.wikipedia.org/wiki/UTC%E2%88%9212:00>`__ to `UTC+14:00 <https://en.wikipedia.org/wiki/UTC%2B14:00>`_).
   -  The second format is ``UTC offset``. The ``UTC offset`` specifies the time value you must add to the UTC time to get a local time value. It has syntax like ``[+|-][hh]mm``. This is negative if the local time zone is on the west of the Prime Meridian and positive if it's on the east. The hour (hh) must be between -12 and 14, and the minute(mm) between 0 and 59. For example, if you want to set the timezone to New Zealand (Chatham Islands) which is in ``UTC+12:45``, you should set the parameter ``<timezone>`` to ``1245``. Please refer to `UTC offset wiki <https://en.wikipedia.org/wiki/Time_zone#List_of_UTC_offsets>`_ for more information.

-  **[<SNTP server1>]**: the first SNTP server.
-  **[<SNTP server2>]**: the second SNTP server.
-  **[<SNTP server3>]**: the third SNTP server.

.. note::

   -  If the three SNTP servers aren't configured, the default server will be used.
   -  For the Query command, ``<timezone>`` parameter in the response may be different from the ``<timezone>`` parameter in Set command. Because the ``<timezone>`` parameter supports the second ``UTC offset`` format, for example, set ``AT+CIPSNTPCFG=1,015``, for Query command, the AT firmware ignores the leading zero of the ``<timezone>`` parameter, and the valid value is ``15``. It doesn't belong to the first format, so it's parsed according to the second ``UTC offset`` format, that's, ``UTC+00:15``, that's, ``timezone`` is 0 in the response.

**Example**

::

    // Enable SNTP server, set timezone to New York of the United States (UTC–05:00)
    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"
    or
    AT+CIPSNTPCFG=1,-500,"0.pool.ntp.org","time.google.com"

    // Enable SNTP server, set timezone to New Zealand (Chatham Islands, UTC+12:45)
    AT+CIPSNTPCFG=1,1245,"0.pool.ntp.org","time.google.com"

AT+CIPSNTPTIME
-----------------------------------------------------
Function: Query the SNTP time.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CIPSNTPTIME?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSNTPTIME:<asctime style time>

    OK

.. note::

   -  The asctime style time is defined at `asctime man page <https://linux.die.net/man/3/asctime>`_.
   -  When the device enters Light-sleep or Deep-sleep mode and then wakes up, the system time may become inaccurate. It's recommended to resend the AT+CIPSNTPCFG command to obtain the new time from the NTP server.

**Example**

::

    AT+CWMODE=1
    AT+CWJAP="AP_TEST","1234567890"
    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"
    AT+CIPSNTPTIME?
    +CIPSNTPTIME:Tue Oct 19 17:47:56 2021

    OK

or

::

    AT+CWMODE=1
    AT+CWJAP="AP_TEST","1234567890"
    AT+CIPSNTPCFG=1,530
    AT+CIPSNTPTIME?
    +CIPSNTPTIME:Tue Oct 19 15:17:56 2021

    OK

AT+CIPSNTPINTV
----------------------------------------------------------------------------------
Function: Query/set the SNTP time synchronization interval.

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+CIPSNTPINTV?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSNTPINTV:<interval second>

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPSNTPINTV=<interval second>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<interval second>**: the SNTP time synchronization interval. Unit: second. Range: [15,4294967].

.. note::

    - It configures interval for synchronization, which means that it sets interval how often the device connects to NTP servers to get new time.

**Example**

::

    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"

    OK

    // synchronize SNTP time every hour
    AT+CIPSNTPINTV=3600

    OK

AT+CIPRECONNINTV
--------------------------------------------------------------------------------------------------------------------------
Function: Query/set the TCP/UDP/SSL reconnection Interval for the Wi-Fi passthrough mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the automatic connect interval for the Wi-Fi passthrough mode.

**Command**

::

    AT+CIPRECONNINTV?

**Response**

.. container:: screenoutput

 .. code::

    +CIPRECONNINTV:<interval>

    OK

Set command
^^^^^^^^^^^

**Function**

Set the automatic reconnecting interval when TCP/UDP/SSL transmission breaks in the Wi-Fi passthrough mode.

**Command**

::

    AT+CIPRECONNINTV=<interval>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<interval>**: the duration between automatic reconnections. Unit: 100 milliseconds. Default: 1. Range: [1,36000].

.. note::

   -  The configuration changes will be saved in the easyflash area if AT+SYSSTORE=1 .

**Example**

::

    AT+CIPRECONNINTV=10


AT+CIPRECVMODE
--------------------------------------------------------------------------------------------------------------------------
Function: Query/set Socket Receiving Mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the socket receiving mode.

**Command**

::

    AT+CIPRECVMODE?

**Response**

.. container:: screenoutput

 .. code::

    +CIPRECVMODE:<mode>

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPRECVMODE=<mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**: the receive mode of socket data. Default: 0.

    - 0: active mode. The AT firmware will send all the received socket data instantly to the host MCU with the header “+IPD”.

    - 1: passive mode. The AT firmware will keep the received socket data in an internal buffer (socket receive window), and wait for the host MCU to read. If the buffer is full, the socket transmission will be blocked for TCP/SSL connections, or data will be lost for UDP connections.

.. note::

   -  The configuration can not be used in the Wi-Fi passthrough mode. If it's a UDP transmission in passive mode, data will be lost when the buffer is full.

   -  When the AT firmware receives socket data in passive mode, it will prompt the following messages in different scenarios:

       For multiple connections mode (AT+CIPMUX=1), the message is +IPD:<link ID>,<len>.

       For single connection mode (AT+CIPMUX=0), the message is +IPD:<len>.

   -  <len> is the total length of socket data in the buffer.

   -  You should read data by running AT+CIPRECVDATA once there is a +IPD reported. Otherwise, the next +IPD won't be reported to the host MCU until the previous +IPD has been read.

   -  When the connection is disconnected, the cache will be cleared.


**Example**

::

    AT+CIPRECVMODE=1



AT+CIPRECVDATA
--------------------------------------------------------------------------------------------------------------------------
Obtain socket data in passive receiving mode.

Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPRECVDATA=<len>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPRECVDATA=<link_id>,<len>

**Response**

.. container:: screenoutput

 .. code::

    +CIPRECVDATA:<actual_len>,<data>

    OK

**Parameters**

-  **<mode>**: the receive mode of socket data. Default: 0.



-  **<link_id>**: connection ID in multiple connections mode.

-  **<len>**: the max value is 0x7fffffff. If the actual length of the received data is less than len, the actual length will be returned.

-  **<actual_len>**: length of the data you actually obtain.

-  **<data>**: the data you want to obtain.

**Example**

::

    AT+CIPRECVMODE=1

    // For example, if host MCU gets a message of receiving 100-byte data in connection with No.0,
    // the message will be "+IPD:0,100".
    // Then you can read those 100-byte data by using the command below.
    AT+CIPRECVDATA=0,100


AT+CIPRECVLEN
--------------------------------------------------------------------------------------------------------------------------
Function: Obtain Socket Data Length in Passive Receiving Mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the length of the entire data buffered for the connection.

**Command**

::

    AT+CIPRECVLEN?

**Response**

.. container:: screenoutput

 .. code::


    // Single connection: (AT+CIPMUX=0)
    +CIPRECVLEN:<data length of link>

    OK

    // Multiple connections: (AT+CIPMUX=1)
    +CIPRECVLEN:<data length of link0>,<data length of link1>,<data length of link2>,<data length of link3>,<data length of link4>

    OK

.. note::

   -  For SSL connections, the data length returned by the AT firmware may be less than the actual data length.

**Example**

::

    AT+CIPRECVLEN?

    // Multiple connections: (AT+CIPMUX=1)
    +CIPRECVLEN:100,0,0,0,0,

    // Single connection: (AT+CIPMUX=0)
    +CIPRECVLEN:100

    OK


AT+PING
-----------------------------------------------
Function: Ping the remote host.

Set command
^^^^^^^^^^^

**Function**

Ping the remote host.

**Command**

::

    AT+PING=<"host">

    AT+PING=<"host">,[<len>,<count>,<interval>,<timeout>]

**Response**

.. container:: screenoutput

 .. code::

    +PING:<time>

    OK

or

.. container:: screenoutput

 .. code::

    +PING:TIMEOUT   // AT returns this response only when the domain name resolution fails or ping times out

    ERROR

**Parameters**

- **<\"host\">**: String parameter showing the host IPv4 or IPV6 address or domain name.
- **<len>**: the length of the ping packet. Unit: byte. Range: (0,65535).
- **<count>**: the count of the ping packet.
- **<interval>**: the interval between ping packets. Unit: millisecond. Range: (0,65535).
- **<timeout>**: the maximum waiting time for a ping response (per packet). Unit: millisecond. Range: (0,65535).
- **<time>**: the response time of ping. Unit: millisecond. Range: (0,1000).

.. note::

    - If the remote host is a domain name string, ping will first resolve the domain name (IPv4 address preferred) from DNS (domain name server) and then ping the remote IP address.
    - Timeout must be greater than or equal to Interval. If Timeout is smaller than Interval, it will be automatically adjusted to be equal to Interval.
    - The default value of Interval is 1000 (milliseconds), and the default value of Timeout is also 1000 (milliseconds).

    For example:

    - AT+PING="192.168.1.1",64,8,,100 → AT+PING="192.168.1.1",64,8,1000,1000
      (Timeout is set to 100, which is smaller than the default Interval of 1000, so Timeout is adjusted to 1000);

    - AT+PING="192.168.1.1",64,8,2000 → AT+PING="192.168.1.1",64,8,2000,2000
      (Interval is set to 2000, which is larger than the default Timeout of 1000, so Timeout is adjusted to 2000).

**Example**

::

    AT+PING="192.168.1.1"
    AT+PING="www.google.com"
    AT+PING="192.168.1.1",64,8,500

    // China Future Internet Engineering Center
    AT+PING="240c::6666"


AT+CIPEVT
-----------------------------------------------
Function: Turn on or off the event message notification prefixed with +CIP.

Set command
^^^^^^^^^^^

**Function**

Turn on or off the event message notification prefixed with +CIP.

**Command**

::

    AT+CIPEVT=<enable>

**Response**

.. container:: screenoutput

 .. code::

    +CIPEVT

    OK

**Parameters**

- **<enable>**: Turn on or off the event message notification prefixed with +CIP.

   -  1: Enable printing of "+CIP" events.
   -  0: Disable printing of "+CIP" events.

Query command
^^^^^^^^^^^^^

**Function**

Query event message notifications with prefix +CIP.

**Command**

::

    AT+CIPEVT?

**Response**

.. container:: screenoutput

 .. code::

    +CIPEVT:<enable>

    OK


AT+CWEVT
-----------------------------------------------
Function: Turn on or off the event message notification prefixed with +CW.

Set command
^^^^^^^^^^^

**Function**

Turn on or off the event message notification prefixed with +CW.

**Command**

::

    AT+CWEVT=<enable>

**Response**

.. container:: screenoutput

 .. code::

    +CWEVT

    OK

**Parameters**

- **<enable>**: Turn on or off the event message notification prefixed with +CW.

   -  1: Enable printing of "+CW" events.
   -  0: Disable printing of "+CW" events.

Query command
^^^^^^^^^^^^^

**Function**

Query event message notifications with prefix +CW.

**Command**

::

    AT+CWEVT?

**Response**

.. container:: screenoutput

 .. code::

    +CWEVT:<enable>

    OK


AT+CIPSSLCCONF
-----------------------------------------------
Function: Query/set SSL clients.

Query command
^^^^^^^^^^^^^

**Function**

Query the configuration of each connection where the device runs as an SSL client.

**Command**

::

    AT+CIPSSLCCONF?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSSLCCONF:<linkid>,<auth_mode>,"<cert_name>","<key_name>","<ca_name>"

    OK

Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCCONF=<auth_mode>[,"<cert_name>"][,"<key_name>"][,"<ca_name>"]

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCCONF=<linkid>,<auth_mode>[,"<cert_name>"][,"<key_name>"][,"<ca_name>"]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<linkid>**: ID of the connection (0~4), used for multiple connections.
- **<auth_mode>**:

    - 0: no authentication. In this case <pki_number> and <ca_number> aren't required.
    - 1: the client provides the client certificate for the server to verify.
    - 2: the client loads CA certificate to verify the server's certificate.
    - 3: mutual authentication.

- **<cert_name>**: The file name of the certificate.
- **<key_name>**: The file name of the private key.
- **<ca_name>**: The file name of CA used by the client to verify the server.

**Example**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCCONF=3,"client_1.crt","client_1.key","ca_1.crt"

    // No auth (AT+CIPMUX=1)
    AT+CIPSSLCCONF=0,0

    // Client auth (AT+CIPMUX=1)
    AT+CIPSSLCCONF=0,1,"client_1.crt","client_1.key",""

    // Server auth (AT+CIPMUX=1)
    AT+CIPSSLCCONF=0,2,"","","ca_1.crt"

    // Both auth (AT+CIPMUX=1)
    AT+CIPSSLCCONF=0,3,"client_1.crt","client_1.key","ca_1.crt"


.. note::

    - If you want this configuration to take effect immediately, run this command before establishing an SSL connection.
    - When downloading AT firmware, it's necessary to download the littlefs file system, and the default file system is mounted to /lfs.
    - If you need to dynamically modify the certificate content, you can refer to the command `AT+FS`_.
    - If <auth_mode> is configured to 2 or 3, in order to check the server certificate validity period,  make sure the device has obtained the current time before sending the AT+CIPSTART command. (You can send AT+CIPSNTPCFG command to configure SNTP and obtain the current time, and send AT+CIPSNPTIME? command to query the current time.)



AT+CIPSSLSCONF
-----------------------------------------------
Function: Query/set SSL Server.

Query command
^^^^^^^^^^^^^

**Function**

Query the configuration where the device runs as an SSL server.

**Command**

::

    AT+CIPSSLSCONF?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSSLSCONF:<auth_mode>,"<cert_name>","<key_name>","<ca_name>"

    OK

Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLSCONF=<auth_mode>[,"<cert_name>"][,"<key_name>"][,"<ca_name>"]

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLSCONF=<linkid>,<auth_mode>[,"<cert_name>"][,"<key_name>"][,"<ca_name>"]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<linkid>**: ID of the connection (0~4), used for multiple connections.
- **<auth_mode>**:

    - 0: no authentication. In this case <pki_number> and <ca_number> aren't required.
    - 1: The server loads the CA certificate to verify the certificate provided by the client.
    - 2: The server provides its own certificate for the client to verify.
    - 3: mutual authentication.

- **<cert_name>**: The file name of the certificate.
- **<key_name>**: The file name of the private key.
- **<ca_name>**: The file name of the CA certificate used by the server to verify the client

**Example**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLSCONF=3,"server_1.crt","server_1.key","ca_1.crt"

    // No auth (AT+CIPMUX=1)
    AT+CIPSSLSCONF=0,0

    // Client auth (AT+CIPMUX=1)
    AT+CIPSSLSCONF=0,2,"","","ca_1.crt"

    // Server auth (AT+CIPMUX=1)
    AT+CIPSSLSCONF=0,1,"server_1.crt","server_1.key",""

    // Both auth (AT+CIPMUX=1)
    AT+CIPSSLSCONF=0,3,"server_1.crt","server_1.key","ca_1.crt"


.. note::

    - If you want this configuration to take effect immediately, run this command before starting the SSL server.
    - When downloading the AT firmware, it's necessary to download the littlefs file system, and the default file system is mounted to /lfs.
    - If you need to dynamically modify the certificate content, you can refer to the command `AT+FS`_ (commands related to file system operations).
    - If <auth_mode> is configured to 1 or 3 (which require verifying the client's certificate), to ensure the validity period of the client's certificate can be checked properly, make sure the device has obtained the current time before starting the SSL server. (You can send the AT+CIPSNTPCFG command to configure the SNTP protocol and obtain the network time, and send the AT+CIPSNPTIME? command to query the device's current time.)



AT+CIPSSLCSNI
-----------------------------------------------
Function: Query/set SSL Client Server Name Indication (SNI).

Query command
^^^^^^^^^^^^^

**Function**

Query the SNI configuration of each connection.

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCSNI?

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCSNI=<linkid>?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSSLCSNI:<link ID>,<"sni">

    OK


Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCSNI=<"sni">

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCSNI=<link ID>,<"sni">

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<linkid>**: ID of the connection (0~4), used for multiple connections.
- **<”sni”>**: the Server Name Indication in ClientHello, this parameter is used to verify the Common Name in the certificate sent by the server.
   The maximum length of sni is 64 bytes.


.. note::

    - If you want this configuration to take effect immediately, run this command before establishing an SSL connection.
    - When downloading AT firmware, it's necessary to download the romfs file system, and the default file system is mounted to /romfs.



AT+CIPSSLCALPN
-----------------------------------------------
Function: Query/set SSL Client Application Layer Protocol Negotiation (ALPN).

Query command
^^^^^^^^^^^^^

**Function**

Query the ALPN configuration of each connection where the device runs as an SSL client.

**Command**

::

    AT+CIPSSLCALPN?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSSLCALPN:<link ID>[,<"alpn">][,<"alpn">][,<"alpn">]

    OK


Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCALPN=<counts>[,<"alpn">][,<"alpn">][,<"alpn">]

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCALPN=<link ID>,<counts>[,<"alpn">][,<"alpn">][,<"alpn">]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<linkid>**: ID of the connection (0~4), used for multiple connections.
- **<counts>**: the number of ALPNs. Range: [0,5].

    - 0: clean the ALPN configuration.
    - [1,5]: set the ALPN configuration.

- **<"alpn">**: A string parameter showing the ALPN in ClientHello. The maximum length of alpn is limited by the command length.


.. note::

    - If you want this configuration to take effect immediately, run this command before establishing an SSL connection.



AT+CIPSSLCPSK
-----------------------------------------------
Function: Query/set SSL client pre-shared key (psk) in string format.

Query command
^^^^^^^^^^^^^

**Function**

Query the PSK configuration of each connection where the device runs as an SSL client.

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCPSK?

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCPSK=<linkid>?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSSLCPSK:<link ID>,<"psk">,<"hint">

    OK


Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCPSK=<"psk">,<"hint">

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCPSK=<link ID>,<"psk">,<"hint">

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<linkid>**: ID of the connection (0~4), used for multiple connections.
- **<"psk">**: The pre-shared key in string format. Maximum length: 32. Please use `AT+CIPSSLCPSKHEX`_ command if your <"psk"> parameter contains \0 characters.
- **<"hint">**: The pre-shared key identity. Maximum length: 32.


.. note::

    - If you want this configuration to take effect immediately, run this command before establishing an SSL connection.


AT+CIPSSLCPSKHEX
-----------------------------------------------
Function: Query/set SSL client pre-shared key (psk) in hexadecimal format.

Query command
^^^^^^^^^^^^^

**Function**

Query the PSK configuration in hexadecimal format of each connection where the device runs as an SSL client.

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCPSKHEX?

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCPSKHEX=<linkid>?

**Response**

.. container:: screenoutput

 .. code::

    +CIPSSLCPSKHEX:<link ID>,<"psk">,<"hint">

    OK


Set command
^^^^^^^^^^^

**Command**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSSLCPSKHEX=<"psk">,<"hint">

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSSLCPSKHEX=<link ID>,<"psk">,<"hint">

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<linkid>**: ID of the connection (0~4), used for multiple connections.
- **<"psk">**: The pre-shared key in string format. Maximum length: 32. Please use `AT+CIPSSLCPSKHEX`_ command if your <"psk"> parameter contains \0 characters.
- **<"hint">**: The pre-shared key identity. Maximum length: 32.


.. note::

    - Similar to the `AT+CIPSSLCPSK`_ command, this command also sets or queries the SSL Client PSK, but its <"psk"> is in hexadecimal format rather than in string format. So, \0 in the <"psk"> parameter means 00.

**Example**

::

    // Single connection: (AT+CIPMUX=0), pre-shared key is "psk", pre-shared key identity is "myhint".
    AT+CIPSSLCPSKHEX="70736b","myhint"

    // Multiple connections: (AT+CIPMUX=1), pre-shared key is "psk", pre-shared key identity is "myhint".
    AT+CIPSSLCPSKHEX=0,"70736b","myhint"



AT+CIPSSLSPSK
-----------------------------------------------
Function: Query/Set SSL Server Pre-shared Key (PSK) in String Format.

Query command
^^^^^^^^^^^^^

**Function**

Query the PSK configuration of each connection where the device runs as an SSL server.

**Command**

::

    AT+CIPSSLSPSK?

**Response**

.. container:: screenoutput

 .. code:: text

    +CIPSSLSPSK:<"psk">,<"hint">

    OK


Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPSSLSPSK=<"psk">,<"hint">

**Response**

.. container:: screenoutput

 .. code:: text

    OK

Parameters
^^^^^^^^^^

- **<"psk">**: The pre-shared key in string format. Maximum length: 32. Please use `AT+CIPSSLSPSKHEX`_ command if your <"psk"> parameter contains \0 characters.
- **<"hint">**: The pre-shared key identity. Maximum length: 32.


.. note::

    - If you want this configuration to take effect immediately, run this command before establishing an SSL server connection.


AT+CIPSSLSPSKHEX
-----------------------------------------------
Function: Query/Set SSL Server Pre-shared Key (PSK) in Hexadecimal Format.

Query command
^^^^^^^^^^^^^

**Function**

Query the PSK configuration in hexadecimal format where the device runs as an SSL server.

**Command**

::

    AT+CIPSSLSPSKHEX?

**Response**

.. container:: screenoutput

 .. code:: text

    +CIPSSLSPSKHEX:<"psk">,<"hint">

    OK


Set command
^^^^^^^^^^^

**Command**

::

    AT+CIPSSLSPSKHEX=<"psk">,<"hint">

**Response**

.. container:: screenoutput

 .. code:: text

    OK

Parameters
^^^^^^^^^^

- **<"psk">**: The pre-shared key in hexadecimal format. Maximum length: 64 (32 bytes in hex). Each byte is represented by two hexadecimal digits.
- **<"hint">**: The pre-shared key identity. Maximum length: 32.


.. note::

    - Similar to the `AT+CIPSSLSPSK`_ command, this command also sets or queries the SSL Server PSK, but its <"psk"> is in hexadecimal format rather than in string format. So, \0 in the <"psk"> parameter means 00.

Example
^^^^^^^^

::

    // PSK in hexadecimal format is "70736b" (equivalent to "psk" in ASCII), pre-shared key identity is "myhint".
    AT+CIPSSLSPSKHEX="70736b","myhint"


AT+IPERF
-----------------------------------------------
Function: Test throughput of the device in standalone mode.

Set command
^^^^^^^^^^^

**Command**

::

    AT+IPERF=<"direct">,<"type">,[<"ipaddr">],[<time>],[<port>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<"direct">**: The direction of iperf, "TX" or "RX".
- **<"type">**: The type of connection, "TCP" or "UDP".
- **<"ipaddr">**: Target ip address. If acting as a client, this parameter must be set.
- **<"time">**: The duration of iperf. Default value 10.(Unit: seconds)
- **<"port">**: The port number of iperf. Default value 5001.


**Example**

::

    AT+IPERF="RX","TCP"

    AT+IPERF="TX","TCP","192.168.1.2",1000

    AT+IPERF="TX","TCP","192.168.1.2",1000,5001


AT+IPERFSTOP
-----------------------------------------------
Function: Stop test throughput of the device.

Set command
^^^^^^^^^^^

**Command**

::

    AT+IPERFSTOP

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    AT+IPERF="TX","TCP","192.168.1.2",1000

    AT+IPERFSTOP


=================
HTTP AT commands
=================
.. table::
    :widths: 25,75
    :width: 100%
    :align: center

    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | Instruction             | Description                                                                                               |
    +=========================+===========================================================================================================+
    | `AT+HTTPCLIENT`_        | Send HTTP Client Request                                                                                  |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPGETSIZE`_       | Get HTTP Resource Size                                                                                    |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPCGET`_          | Get HTTP Resource                                                                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPCPOST`_         | Post HTTP data of specified length                                                                        |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPCPUT`_          | Put HTTP data of specified length                                                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPURLCFG`_        | Set/Get long HTTP URL                                                                                     |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPSSLCFG`_        | Set/Get HTTPS certificate                                                                                 |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPRECVMODE`_      | Query/set socket receiving mode                                                                           |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPRECVDATA`_      | Obtain HTTP data in passive receiving mode                                                                |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPRECVLEN`_       | Obtain HTTP data length in passive receiving mode                                                         |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+HTTPRECVBUF`_       | Query/set the HTTP receive buffer size                                                                    |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+

HTTP AT command description
===========================


AT+HTTPCLIENT
---------------------------
Send HTTP Client Request.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPCLIENT=<linkid>,<opt>,<content-type>,<"url">,[,<"data">]

**Response**

.. container:: screenoutput

 .. code::

    OK

    +HTTPCGET:<linkid>,<size>,<data>

    +HTTPSTATUS:<linkid>,<err_code>


or

.. container:: screenoutput

 .. code::


    +HTTPCPOST:<linkid>,<size>,<data>

    +HTTPSTATUS:<linkid>,<err_code>


or

.. container:: screenoutput

 .. code::


    +HTTPCPUT:<linkid>,<size>,<data>

    +HTTPSTATUS:<linkid>,<err_code>


**Parameters**

- **<linkid>**: The link ID of the HTTP client request. Range:[0, 2]

- **<opt>**: method of HTTP client request.

   - 1: HEAD
   - 2: GET
   - 3: POST
   - 4: PUT

- **<content-type>**: data type of HTTP client request.

   - 0: application/x-www-form-urlencoded
   - 1: application/json
   - 2: multipart/form-data
   - 3: text/xml

- **<url>**: HTTP/HTTPS URL.

- **<data>**:
   If <opt> is a POST request, this parameter holds the data you send to the HTTP server.
   If not, this parameter doesn't exist, which means there is no need to input a comma to indicate this parameter.

- **<err_code>**: Failure reasons

   - 0: File successfully received
   - 1: Unknown error
   - 2: Connection to server failed
   - 3: Failed to resolve server hostname
   - 4: Connection unexpectedly closed by remote server
   - 5: Connection timed out (server didn't respond in time)
   - 6: Server responded with an error code
   - 7: Local memory ERROR
   - 8: Local abort
   - 9: Content length mismatch

.. note::

    - If the length of the entire command containing the URL exceeds 256 bytes, please use the AT+HTTPURLCFG command to preset the URL first and then set the <"url"> parameter of this command to "".

    - If the url parameter isn't null, HTTP client will use it and ignore the host parameter and path parameter; If the url parameter is omitted or null string, HTTP client will use host parameter and path parameter.

    - The command doesn't support redirection. After getting the status code 301 (permanent redirection) or 302 (temporary redirection) from the server, AT won't automatically redirect to the new URL address. You can use some tools to get the actual URL, and then access it using this command.

    - If the length of the entire command containing the <"data"> exceeds 256 bytes, please use the AT+HTTPCPOST command.

    - When the passive mode is enabled, the response will change to +HTTPC:<linkid>,<size>.


**Example**

::

    // HEAD Request
    AT+HTTPCLIENT=0,1,0,"http://httpbin.org/get"

    // GET Request
    AT+HTTPCLIENT=0,2,0,"http://httpbin.org/get"

    // POST Request
    AT+HTTPCLIENT=0,3,0,"http://httpbin.org/post","field1=value1&field2=value2"

    // https HEAD Request
    AT+HTTPCLIENT=0,1,0,"https://httpbin.org/get"


AT+HTTPGETSIZE
---------------------------
Get HTTP resource size.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPGETSIZE=<linkid>,<"url">[,<timeout>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    +HTTPGETSIZE:<linkid>,<size>

    +HTTPSTATUS:<linkid>,<err_code>


**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

- **<url>**: HTTP/HTTPS URL. It's a string parameter and should be enclosed with quotes.

- **<timeout>**: Network timeout. Unit: millisecond. Default: 5000. Range: [0,180000].

- **<size>**: HTTP resource size.


.. note::

    - If the length of the entire command containing the URL exceeds 256 bytes, please use the AT+HTTPURLCFG command to preset the URL first and then set the <"url"> parameter of this command to "".


**Example**

::

    AT+HTTPGETSIZE=0,"https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_92x30dp.png"

    OK

    // Response
    +HTTPGETSIZE:0,1575

    +HTTPSTATUS:0,0



AT+HTTPCGET
---------------------------
Get HTTP resource.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPCGET=<linkid>,<"url">[,<timeout>]

**Response**

.. container:: screenoutput

 .. code::

    OK

    +HTTPCGET:<linkid>,<size>,<data>

    +HTTPSTATUS:<linkid>,<err_code>


**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

- **<url>**: HTTP/HTTPS URL. It's a string parameter and should be enclosed with quotes.

- **<timeout>**: Network timeout. Unit: millisecond. Default: 5000. Range: [0,180000].

- **<size>**: HTTP resource size.


.. note::

    - If the length of the entire command containing the URL exceeds 256 bytes, please use the AT+HTTPURLCFG command to preset the URL first and then set the <"url"> parameter of this command to "".

    - When the passive mode is enabled, the response will change to +HTTPC:<linkid>,<size>.

AT+HTTPCPOST
---------------------------
Post HTTP data of specified length.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPCPOST=<linkid>,<"url">,<length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter the data now. When the requirement of message length determined by the parameter <length> is met, the transmission starts.

If the transmission is successful, AT returns:


.. container:: screenoutput

 .. code::

    SEND OK

Otherwise, it returns:

.. container:: screenoutput

 .. code::

    SEND FAIL


**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

- **<url>**: HTTP/HTTPS URL. It's a string parameter and should be enclosed with quotes.

- **<length>**: HTTP data length to POST. The maximum length is equal to the system allocable heap size.


.. note::

    - If the length of the entire command containing the URL exceeds 256 bytes, please use the AT+HTTPURLCFG command to preset the URL first and then set the <"url"> parameter of this command to "".

    - The default type of content-type is application/x-www-form-urlencoded for this command.

    - When the passive mode is enabled, the response will change to +HTTPC:<linkid>,<size>.


AT+HTTPCPUT
---------------------------
Put HTTP data of specified length.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPCPUT=<linkid>,<"url">,<content_type>,<length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter the data now. When the requirement of message length determined by the parameter <length> is met, the transmission starts.

If the transmission is successful, AT returns:


.. container:: screenoutput

 .. code::

    SEND OK

Otherwise, it returns:

.. container:: screenoutput

 .. code::

    SEND FAIL



**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

- **<url>**: HTTP/HTTPS URL. It's a string parameter and should be enclosed with quotes.

- **<content_type>**: data type of HTTP client request.

   - 0: application/x-www-form-urlencoded
   - 1: application/json
   - 2: multipart/form-data
   - 3: text/xml

- **<length>**: HTTP data length to PUT. The maximum length is equal to the system allocable heap size.

.. note::

    - If the length of the entire command containing the URL exceeds 256 bytes, please use the AT+HTTPURLCFG command to preset the URL first and then set the <"url"> parameter of this command to "".

    - When the passive mode is enabled, the response will change to +HTTPC:<linkid>,<size>.


AT+HTTPURLCFG
---------------------------
Set/Get long HTTP URL.

Query command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPURLCFG=0?

**Response**

.. container:: screenoutput

 .. code::

    +HTTPURLCFG:<linkid>,<url length>,<url>

    OK

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPURLCFG=<linkid>,<url length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving serial data. You should enter the URL now, and when the URL length reaches the <url length> value, the system returns:


.. container:: screenoutput

 .. code::

    SEND OK

Otherwise, it returns:

.. container:: screenoutput

 .. code::

    SEND FAIL


**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

- **<url length>**: HTTP URL length. Unit: byte.

   - 0: clean the HTTP URL configuration.

   - [8,8192]: set the HTTP URL configuration.


- **<url>**: HTTP/HTTPS URL data.



AT+HTTPSSLCFG
---------------------------
Set/Get HTTPS certificate.

Query command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPSSLCFG=0?

**Response**

.. container:: screenoutput

 .. code::

    +HTTPSSLCFG:<linkid>,<scheme>,[<"cert_file">][,<"key_file">][,<"ca_file">]

    OK

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPSSLCFG=<linkid>,<scheme>,[<"cert_file">][,<"key_file">][,<"ca_file">]

**Response**

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

- **<scheme>**:

   - 0: HTTPS no certificate verify.
   - 1: HTTPS verify server certificate.
   - 2: HTTPS provide client certificate.
   - 3: HTTPS verify server certificate and provide client certificate.

- **<"cert_file">**: The file name of the certificate.

- **<"key_file">**: The file name of the private key.

- **<"ca_file">**: The file name of CA used by the client to verify the server.


AT+HTTPRECVMODE
--------------------------------------------------------------------------------------------------------------------------
Function: Query/set HTTP receiving mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the HTTP receiving mode.

**Command**

::

    AT+HTTPRECVMODE?

**Response**

.. container:: screenoutput

 .. code::

    +HTTPRECVMODE:<mode>

    OK

Set command
^^^^^^^^^^^

**Command**

::

    AT+HTTPRECVMODE=<mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**: the receive mode of HTTP data. Default: 0.

    - 0: active mode. The AT firmware will send all the received HTTP data instantly to the host MCU with the header “+HTTPC”.

    - 1: passive mode. The AT firmware will keep the received HTTP data in an internal buffer (HTTP receive buffer), and wait for the host MCU to read. If the buffer is full, the socket transmission will be blocked.

.. note::

   -  This parameter applies to all linkids.

   -  <len> is the total length of socket data in the buffer.

   -  You should read data by running AT+HTTPRECVDATA once there is a +HTTPC reported. Otherwise, the next +HTTPC won't be reported to the host MCU until the previous +HTTPC has been read.

   -  When the connection is disconnected, the cache is still saved.


**Example**

::

    AT+HTTPRECVMODE=1


AT+HTTPRECVBUF
----------------------------------------------------------------------------------------
Function: Query/set the HTTP receive buffer size.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPRECVBUF=<size>


**Response**

.. container:: screenoutput

 .. code::

    +HTTPRECVBUF

    OK

**Parameters**

-  **<size>**: Set the size of the buffer, unit bytes.


Quert Command
^^^^^^^^^^^^^^^

**Command**

::

    AT+HTTPRECVBUF?

**Response**

Return the buffer size, with default buffer size being 15360 (This value won't occupy the heap memory of the system).

.. container:: screenoutput

 .. code::

    +HTTPRECVBUF:<size>

    OK


.. note::

   -  The theoretical upper limit for this value is the size of the lwip heap.


AT+HTTPRECVDATA
--------------------------------------------------------------------------------------------------------------------------
Obtain HTTP data in passive receiving mode.

Set command
^^^^^^^^^^^

**Command**

::

    AT+HTTPRECVDATA=<linkid>,<len>

**Response**

.. container:: screenoutput

 .. code::

    +HTTPRECVDATA:<actual_len>,<data>

    OK

**Parameters**

- **<linkid>**: The link ID of the HTTP client request.

-  **<mode>**: the receive mode of socket data. Default: 0.

-  **<len>**: the max value is 0x7fffffff. If the actual length of the received data is less than len, the actual length will be returned.

-  **<actual_len>**: length of the data you actually obtain.

-  **<data>**: the data you want to obtain.

**Example**

::

    AT+HTTPRECVMODE=1

    // For example, if host MCU gets a message of receiving 100-byte data in connection with No.0,
    // the message will be "+HTTPC:0,100".
    // Then you can read those 100-byte data by using the command below.
    AT+HTTPRECVDATA=0,100


AT+HTTPRECVLEN
--------------------------------------------------------------------------------------------------------------------------
Function: Obtain HTTP data length in passive receiving mode.

Query command
^^^^^^^^^^^^^

**Function**

Query the length of the entire data buffered for the connection.

**Command**

::

    AT+HTTPRECVLEN=0?

**Response**

.. container:: screenoutput

 .. code::


    +HTTPRECVLEN:<linkid>,<data length of link>

    OK

**Example**

::

    AT+HTTPRECVLEN=0?

    +CIPRECVLEN:0,100

    OK

=================
MQTT AT commands
=================
.. table::
    :widths: 25,75
    :width: 100%
    :align: center

    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | Instruction             | Description                                                                                               |
    +=========================+===========================================================================================================+
    | `AT+MQTTUSERCFG`_       | Get/set MQTT user configuration                                                                           |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTCLIENTID`_      | Get/set MQTT client ID                                                                                    |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTUSERNAME`_      | Get/set MQTT username                                                                                     |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTPASSWORD`_      | Get/set MQTT password                                                                                     |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTCONNCFG`_       | Get/set configuration of MQTT connection                                                                  |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTALPN`_          | Get/set MQTT Application Layer Protocol Negotiation (ALPN)                                                |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTSNI`_           | Get/set MQTT Server Name Indication (SNI)                                                                 |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTCONN`_          | Connect to MQTT Brokers                                                                                   |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTPUB`_           | Publish MQTT Messages in string                                                                           |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTPUBRAW`_        | Publish long MQTT messages                                                                                |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTSUB`_           | Subscribe to MQTT topics                                                                                  |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTUNSUB`_         | Unsubscribe from MQTT topics                                                                              |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+
    | `AT+MQTTCLEAN`_         | Close MQTT connections                                                                                    |
    +-------------------------+-----------------------------------------------------------------------------------------------------------+


MQTT AT command description
===========================


AT+MQTTUSERCFG
--------------------------------
Get/set MQTT user configuration.

Query command
^^^^^^^^^^^^^^^

Query MQTT user configuration.

**Command**

::

    AT+MQTTUSERCFG?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTUSERCFG:<LinkID>,<scheme>,<"client_id">,<"username">,<"password">,<"cert_file">,<"key_file">,<"ca_file">

    OK


Set command
^^^^^^^^^^^^^^^

Set MQTT user configuration.

**Command**

::

    AT+MQTTUSERCFG=<LinkID>,<scheme>,<"client_id">,<"username">,<"password">[,<"cert_file">][,<"key_file">][,<"ca_file">]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<LinkID>**: currently only supports link ID 0.

- **<scheme>**:

   - 0: MQTT over TCP.
   - 1: MQTT over TLS (no certificate verify).
   - 2: MQTT over TLS (verify server certificate).
   - 3: MQTT over TLS (provide client certificate).
   - 4: MQTT over TLS (verify server certificate and provide client certificate).

- **<client_id>**: MQTT client ID. Maximum length: 23 bytes.

- **<username>**: The username to login to the MQTT broker. Maximum length: 128 bytes.

- **<password>**: The password to login to the MQTT broker. Maximum length: 128 bytes.

- **<"cert_file">**: The file name of the certificate.

- **<"key_file">**: The file name of the private key.

- **<"ca_file">**: The file name of CA used by the client to verify the server.

.. note::

    - The length of the entire AT command should be less than 256 bytes.

    - If <scheme> is configured to 3, 5, 8, or 10, in order to check the server certificate validity period, please make sure the device has obtained the current time before sending the AT+MQTTCONN command. (You can send AT+CIPSNTPCFG command to configure SNTP and obtain the current time, and send AT+CIPSNPTIME? command to query the current time.)



AT+MQTTCLIENTID
-----------------------
Get/set MQTT client ID.

Query command
^^^^^^^^^^^^^^^

Query MQTT client ID.

**Command**

::

    AT+MQTTCLIENTID?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTCLIENTID:<LinkID>,<"client_id">

    OK

.. note::

    - The maximum length of the Query command doesn't exceed 128 bytes (limited to AT length).


Set command
^^^^^^^^^^^^^^^

Set MQTT client ID.

**Command**

::

    AT+MQTTLONGCLIENTID=<LinkID>,<length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving MQTT Client ID. You should enter the Client ID, and when the Client ID length reaches the <length> value, the system returns:

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: currently only supports link ID 0.

- **<length>**: MQTT client ID length. Range: [1,23].

.. note::

    - The command AT+MQTTUSERCFG can also set MQTT client ID. The differences between the two commands include:

        - You can use AT+MQTTLONGCLIENTID to set a relatively long client ID since there is a limitation on the length of the AT+MQTTUSERCFG command.

        - You should set AT+MQTTLONGCLIENTID after setting the AT+MQTTUSERCFG command.




AT+MQTTUSERNAME
-----------------------
Get/set MQTT username.

Query command
^^^^^^^^^^^^^^^

Query MQTT username.

**Command**

::

    AT+MQTTUSERNAME?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTUSERNAME:<LinkID>,<"user_name">

    OK

.. note::

    - The maximum length of the Query command doesn't exceed 128 bytes (limited to AT length).

Set command
^^^^^^^^^^^^^^^

Set MQTT username.

**Command**

::

    AT+MQTTUSERNAME=<LinkID>,<length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving MQTT username. You should enter the MQTT username, and when the MQTT username length reaches the <length> value, the system returns:

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: currently only supports link ID 0.

- **<length>**: MQTT username length. Range: [1,128].

.. note::

    - The command AT+MQTTUSERCFG can also set MQTT username. The differences between the two commands include:

        - You can use AT+MQTTLONGUSERNAME to set a relatively long username since there is a limitation on the length of the AT+MQTTUSERCFG command.

        - You should set AT+MQTTLONGUSERNAME after setting the command AT+MQTTUSERCFG.


AT+MQTTPASSWORD
-----------------------
Get/set MQTT password.

Query command
^^^^^^^^^^^^^^^

Query MQTT password.

**Command**

::

    AT+MQTTPASSWORD?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTPASSWORD:<LinkID>,<"password">

    OK

.. note::

    - The maximum length of the Query command doesn't exceed 128 bytes (limited to AT length).

Set command
^^^^^^^^^^^^^^^

Set MQTT password.

**Command**

::

    AT+MQTTPASSWORD=<LinkID>,<length>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

This response indicates that AT is ready for receiving MQTT password. You should enter the MQTT password, and when the MQTT password length reaches the <length> value, the system returns:

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: currently only supports link ID 0.

- **<length>**: MQTT password length. Range: [1,128].

.. note::

    - The command AT+MQTTUSERCFG can also set MQTT password. The differences between the two commands include:

        - You can use AT+MQTTLONGPASSWORD to set a relatively long password since there is a limitation on the length of the AT+MQTTUSERCFG command.

        - You should set AT+MQTTLONGPASSWORD after setting the command AT+MQTTUSERCFG.


AT+MQTTCONNCFG
-----------------------------------------
Get/set configuration of MQTT connection.

Query command
^^^^^^^^^^^^^^^

Query configuration of MQTT connection.

**Command**

::

    AT+MQTTCONNCFG?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTCONNCFG:<LinkID>,<keepalive>,<disable_clean_session>,<"lwt_topic">,<"lwt_msg">,<lwt_qos>,<lwt_retain>

    OK


Set command
^^^^^^^^^^^^^^^

Set configuration of MQTT Connection.

**Command**

::

    AT+MQTTCONNCFG=<LinkID>,<keepalive>,<disable_clean_session>,<"lwt_topic">,<"lwt_msg">,<lwt_qos>,<lwt_retain>

**Response**

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: currently only supports link ID 0.

- **<keepalive>**: timeout of MQTT ping. Unit: second. Range [0,7200]. The default value is 0, which will be force-changed to 120 s.

- **<disable_clean_session>**: set MQTT clean session. For more details about this parameter, please refer to the section Clean Session in MQTT Version 3.1.1.

   - 0: enable clean session.
   - 1: disable clean session.

- **<lwt_topic>**: LWT (Last Will and Testament) message topic. Maximum length: 128 bytes.

- **<lwt_msg>**: LWT message. Maximum length: 128 bytes.

- **<lwt_qos>**: LWT QoS, which can be set to 0, 1, or 2. Default: 0.

- **<lwt_retain>**: LWT retain, which can be set to 0 or 1. Default: 0.


AT+MQTTALPN
-----------------------------------------------------------
Get/set MQTT Application Layer Protocol Negotiation (ALPN).

Query command
^^^^^^^^^^^^^^^

Query MQTT Application Layer Protocol Negotiation (ALPN).

**Command**

::

    AT+MQTTALPN?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTALPN:<LinkID>[,<"alpn">][,<"alpn">][,<"alpn">]

    OK


Set command
^^^^^^^^^^^^^^^

Set MQTT Application Layer Protocol Negotiation (ALPN).

**Command**

::

    AT+MQTTALPN=<LinkID>,<alpn_counts>[,<"alpn">][,<"alpn">][,<"alpn">]

**Response**

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<alpn_counts>**: The number of <”alpn”> parameters. Range: [0,5].

   - 0: clean the MQTT ALPN configuration.
   - [1,5]: set the MQTT ALPN configuration.

- **<"alpn">**: You can send more than one ALPN in ClientHello to the server.

.. note::

    - The length of the entire AT command should be less than 256 bytes.

    - MQTT ALPN will be effective only if the MQTT connection is based on TLS or WSS.

    - You should set AT+MQTTALPN after setting the command AT+MQTTUSERCFG.


**Example**

::

    AT+CWMODE=1

    AT+CWJAP="ssid","password"

    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"

    AT+MQTTUSERCFG=0,4,"Bflb","bl","123456789","client_1.crt","client_1.key","ca_1.crt"

    AT+MQTTSNI=0,"server.local"

    AT+MQTTALPN=0,2,"mqtt-ca.cn","mqtt-ca.us"

    AT+MQTTCONN=0,"192.168.31.112",8883,1



AT+MQTTSNI
-------------------------------------------
Get/set MQTT Server Name Indication (SNI).

Query command
^^^^^^^^^^^^^^^

Query MQTT Server Name Indication (SNI).

**Command**

::

    AT+MQTTSNI?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTSNI:<LinkID>,<"sni">

    OK


Set command
^^^^^^^^^^^^^^^

Set MQTT Server Name Indication (SNI).

**Command**

::

    AT+MQTTSNI=<LinkID>,<"sni">

**Response**

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<"sni">**: MQTT Server Name Indication. You can send it in ClientHello to the server.

.. note::

    - The length of the entire AT command should be less than 256 bytes.

    - MQTT SNI will be effective only if the MQTT connection is based on TLS or WSS.

    - You should set AT+MQTTSNI after setting the command AT+MQTTUSERCFG.


**Example**

::

    AT+CWMODE=1

    AT+CWJAP="ssid","password"

    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"

    AT+MQTTUSERCFG=0,4,"Bflb","bl","123456789","client_1.crt","client_1.key","ca_1.crt"

    AT+MQTTSNI=0,"my_specific_prefix.iot.my_aws_region.amazonaws.com"

    AT+MQTTCONN=0,"192.168.31.112",8883,1



AT+MQTTCONN
------------------------
Connect to MQTT Brokers.

Query command
^^^^^^^^^^^^^^^

Query MQTT Brokers that have already been connected.

**Command**

::

    AT+MQTTCONN?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTCONN:<LinkID>,<state>,<scheme><"host">,<port>,<reconnect>

    OK


Set command
^^^^^^^^^^^^^^^

Connect to MQTT Brokers.

**Command**

::

    AT+MQTTCONN=<LinkID>,<"host">,<port>,<reconnect>

**Response**

.. container:: screenoutput

 .. code::

    OK

    +MQTT:CONNECTED,0

.. container:: screenoutput

 .. code::


    +MQTT:DISCONNECTED,0


**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<"host">**: MQTT broker domain. Maximum length: 128 bytes.

- **<port>**: MQTT broker port. Maximum: port 65535.

- **<reconnect>**:

    - 0: MQTT won't reconnect automatically. If MQTT connection established and then disconnected, you can't use this command to reestablish MQTT connection. Please send AT+MQTTCLEAN=0 command to clean MQTT connection first, reconfigure the connection parameters, and then establish a new MQTT connection.
    - 1: MQTT will reconnect automatically. It takes more resources.

- **<state>**:

    - 0: MQTT uninitialized.
    - 1: already set AT+MQTTUSERCFG.
    - 2: already set AT+MQTTCONNCFG.
    - 3: connection disconnected.
    - 4: connection established.
    - 5: connected, but did not subscribe to any topic.
    - 6: connected, and subscribed to MQTT topics.

- **<scheme>**:

   - 0: MQTT over TCP.
   - 1: MQTT over TLS (no certificate verify).
   - 2: MQTT over TLS (verify server certificate).
   - 3: MQTT over TLS (provide client certificate).
   - 4: MQTT over TLS (verify server certificate and provide client certificate).


AT+MQTTPUB
-------------------------------
Publish MQTT Messages in string.

Set command
^^^^^^^^^^^^^^^

**Function**

Publish MQTT messages in string to a defined topic. If the amount of data you publish is relatively large, and the length of a single AT command has exceeded the threshold of 256, please use the AT+MQTTPUBRAW command.

**Command**

::

    AT+MQTTPUB=<LinkID>,<"topic">,<"data">,<qos>,<retain>

**Response**

.. container:: screenoutput

 .. code::

    OK


**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<"topic">**: MQTT topic. Maximum length: 128 bytes.

- **<"data">**: MQTT message in string.

- **<qos>**: QoS of message, which can be set to 0, 1, or 2. Default: 0.

- **<retain>**: retain flag.


.. note::

    - The length of the entire AT command should be less than 256 bytes.

    - This command can't send data \0. If you need to send \0, please use the command AT+MQTTPUBRAW instead.


**Example**

::

    AT+CWMODE=1

    AT+CWJAP="ssid","password"

    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"

    AT+MQTTUSERCFG=0,4,"Bflb","bl","123456789","client_1.crt","client_1.key","ca_1.crt"

    AT+MQTTSNI=0,"server.local"

	AT+MQTTALPN=0,2,"mqtt-ca.cn","mqtt-ca.us"

    AT+MQTTCONN=0,"192.168.31.112",8883,1

    AT+MQTTPUB=0,"topic","\"{\"timestamp\":\"20201121085253\"}\"",0,0  // When sending this command, please pay attention to whether special characters need to be escaped.


AT+MQTTPUBRAW
-------------------------------
Publish long MQTT messages.

Set command
^^^^^^^^^^^^^^^

**Function**

Publish long MQTT messages to a defined topic. If the amount of data you publish is relatively small, and the length of a single AT command isn't greater than the threshold of 256, you also can use the AT+MQTTPUB command.

**Command**

::

    AT+MQTTPUBRAW=<LinkID>,<"topic">,<length>,<qos>,<retain>

**Response**

.. container:: screenoutput

 .. code::

    OK

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter the data now. When the requirement of message length determined by the parameter <length> is met, the transmission starts.

If the transmission is successful, AT returns:

.. container:: screenoutput

 .. code::

    SEND OK

Otherwise, it returns:

.. container:: screenoutput

 .. code::

    SEND FAIL


**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<"topic">**: MQTT topic. Maximum length: 128 bytes.

- **<length>**: The length of an MQTT message. Its maximum length is 1536.

- **<qos>**: QoS of the published message, which can be set to 0, 1, or 2. Default is 0.

- **<retain>**: retain flag.


AT+MQTTSUB
------------------------
Subscribe to MQTT topics.

Query command
^^^^^^^^^^^^^^^
**Function**

Query subscribed MQTT topics.

**Command**

::

    AT+MQTTSUB?

**Response**

.. container:: screenoutput

 .. code::

    +MQTTSUB:<LinkID>,<state>,<"topic1">,<qos>

    +MQTTSUB:<LinkID>,<state>,<"topic2">,<qos>

    +MQTTSUB:<LinkID>,<state>,<"topic3">,<qos>

    ...

    OK


Set command
^^^^^^^^^^^^^^^

**Function**

Subscribe to defined MQTT topics with defined QoS. It supports subscribing to multiple topics.

**Command**

::

    AT+MQTTSUB=<LinkID>,<"topic">,<qos>

**Response**

.. container:: screenoutput

 .. code::

    OK

If the topic has been subscribed before, it will prompt:

.. container:: screenoutput

 .. code::

    +MQTTSUB,ALREADY_SUBSCRIBE

    OK

When AT receives MQTT messages of the subscribed topic, it will prompt:

.. container:: screenoutput

 .. code::

    +MQTT:SUBRECV,<LinkID>,<topic_length>,<data_length>,<"topic">,<data>

**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<state>**:

    - 0: MQTT uninitialized.
    - 1: already set AT+MQTTUSERCFG.
    - 2: already set AT+MQTTCONNCFG.
    - 3: connection disconnected.
    - 4: connection established.
    - 5: connected, but subscribe to no topic.
    - 6: connected, and subscribed to MQTT topics.

- **<topic>**: the topic that's subscribed to.

- **<qos>**: the QoS that's subscribed to.



AT+MQTTUNSUB
----------------------------
Unsubscribe from MQTT topics.

Set command
^^^^^^^^^^^^^^^

**Function**

Unsubscribe the client from defined topics. This command can be called multiple times to unsubscribe from different topics.

**Command**

::

    AT+MQTTUNSUB=<LinkID>,<"topic">

**Response**

.. container:: screenoutput

 .. code::

    OK

If the topic has been subscribed before, it will prompt:

.. container:: screenoutput

 .. code::

    +MQTTUNSUB:NO_UNSUBSCRIBE

    OK


**Parameters**

- **<LinkID>**: Currently only supports link ID 0.

- **<topic>**: MQTT topic. Maximum length: 128 bytes.



AT+MQTTCLEAN
----------------------------
Close MQTT connections.

Set command
^^^^^^^^^^^^^^^

**Function**

Close the MQTT connection and release the resource.

**Command**

::

    AT+MQTTCLEAN=<LinkID>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<LinkID>**: Currently only supports link ID 0.


=========================
BLE AT commands
=========================
.. table::
    :widths: 60,100
    :width: 100%
    :align: center

    +----------------------------+------------------------------------------------------------------------------------+
    |  Instruction               | Description                                                                        |
    +============================+====================================================================================+
    | `AT+BLEINIT`_              | Initialize BLE                                                                     |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEADDR`_              | BLE address                                                                        |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESTATICADDR`_        | BLE static random address                                                          |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLENAME`_              | Set BLE device name                                                                |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESCANPARAM`_         | Set BLE scanning parameters                                                        |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESCAN`_              | Enable BLE scanning                                                                |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESCANRSPDATA`_       | Set BLE SCAN Response data                                                         |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEADVPARAM`_          | Set BLE advertising parameters                                                     |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEADVDATA`_           | Set BLE advertising data                                                           |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEADVSTART`_          | Enable BLE advertising                                                             |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEADVSTOP`_           | Stop BLE advertising                                                               |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLECONN`_              | Establish BLE connection                                                           |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLECONNPARAM`_         | Update BLE connection parameters                                                   |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEDISCONN`_           | End BLE connection                                                                 |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEDATALEN`_           | Set BLE packet length                                                              |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEEXCHANGEMTU`_       | Exchange the size of BLE MTU                                                       |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSSRV`_          | GATTS discover services                                                            |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSSRVCRE`_       | GATTS create services                                                              |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSSRVDEL`_       | GATTS delete services                                                              |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSREGISTER`_     | GATTS services register/unregister                                                 |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSCHAR`_         | GATTS discover characteristics                                                     |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSCHARCRE`_      | GATTS create characteristics                                                       |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSNTFY`_         | GATTS characteristics notify                                                       |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSIND`_          | GATTS characteristics indicate                                                     |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTSRD`_           | GATTS characteristics read                                                         |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTCSRVDIS`_       | GATTC discover services                                                            |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTCCHARDIS`_      | GATTC discover characteristics                                                     |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTCWR`_           | GATTC characteristics write                                                        |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTCRD`_           | GATTC characteristics read                                                         |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTCSUBSCRIBE`_    | GATTC subscribe server                                                             |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEGATTCUNSUBSCRIBE`_  | GATTC unsubscribe server                                                           |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLETXPWR`_             | BLE TX power                                                                       |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECPARAM`_          | Set/query BLE security parameter                                                   |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECCANNEL`_         | BLE cancel pairing                                                                 |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECPASSKEYCONFIRM`_ | BLE pairing passkey confirm                                                        |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECPAIRINGCONFIRM`_ | BLE pairing confirm                                                                |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECPASSKEY`_        | BLE pairing pass entry                                                             |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECGETLTKLIST`_     | Query BLE paired device list                                                       |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECUNPAIR`_         | BLE unpairing                                                                      |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESECSTART`_          | BLE start pairing                                                                  |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEBASINIT`_           | BLE BAS profile init                                                               |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEBASDEINIT`_         | BLE BAS profile deinit                                                             |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEBASLVLGET`_         | GET BLE BAS profile level                                                          |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEBASLVLSET`_         | SET BLE BAS profile level                                                          |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEIASINIT`_           | BLE IAS profile init                                                               |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEIASDEINIT`_         | BLE IAS profile deinit                                                             |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEDISINIT`_           | BLE DIS profile init                                                               |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEDISDEINIT`_         | BLE DIS profile deinit                                                             |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLEDISSET`_            | BLE DIS profile setting                                                            |
    +----------------------------+------------------------------------------------------------------------------------+
    | `AT+BLESETGAPAPPEARANCE`_  | BLE Set GAP APPEARANCE                                                             |
    +----------------------------+------------------------------------------------------------------------------------+

BLE AT command description
==========================


AT+BLEINIT
--------------
Function: BLE initialization.

Query command
^^^^^^^^^^^^^

**Function**

Check the initialization status of BLE.

**Command**

::

    AT+BLEINIT?

**Response**

If BLE is initialized, AT will return:

.. container:: screenoutput

 .. code::

    +BLEINIT:<role>
    OK

If BLE isn't initialized, AT will return:

.. container:: screenoutput

 .. code::

    +BLEINIT:0
    OK

Set command
^^^^^^^^^^^

**Function**

Initialize BLE.

**Command**

::

    AT+BLEINIT=<init>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<init>**:

  * 0: deinit BLE
  * 1: client role
  * 2: server role
  * 3: dual role


AT+BLEADDR
-------------
Function: Query/set the BLE address.

Query command
^^^^^^^^^^^^^

**Function**

Query the BLE device address.

**Command**

::

    AT+BLEADDR?

**Response**

.. container:: screenoutput

 .. code::

    +BLEADDR:<BLE_public_addr>
    OK

Set command
^^^^^^^^^^^

**Function**

Set the BLE device address.

**Command**

::

    AT+BLEADDR=<BLE_public_addr>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <BLE_public_addr>:


AT+BLESTATICADDR
-----------------
Function: Query/set the BLE static random address.

Query command
^^^^^^^^^^^^^

**Function**

Query the BLE static random address.

**Command**

::

    AT+BLESTATICADDR?

**Response**

.. container:: screenoutput

 .. code::

    +BLESTATICADDR:"XX:XX:XX:XX:XX:XX"
    OK

Set command
^^^^^^^^^^^

**Function**

Set the BLE static random address. The static random address will persist and be used for all subsequent BLE operations (advertising, connections).

**Command**

::

    AT+BLESTATICADDR=<static_random_addr>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <static_random_addr>: Static random address, format: "XX:XX:XX:XX:XX:XX"
                        The first byte must have top 2 bits set to 0b11 (range: C0-C7).
                        This ensures the address is identified as a static random address.

**Examples**

::

    AT+BLESTATICADDR="C0:22:33:44:55:66"
    OK
    AT+BLESTATICADDR?
    +BLESTATICADDR:"C0:22:33:44:55:66"
    OK


AT+BLENAME
-------------
Function: Query/set the BLE device name.

Query command
^^^^^^^^^^^^^

**Function**

Query the BLE device name.

**Command**

::

    AT+BLENAME?

**Response**

.. container:: screenoutput

 .. code::

    +BLENAME:<device_name>
    OK

Set command
^^^^^^^^^^^

**Function**

Set the BLE device name.

**Command**

::

    AT+BLENAME=<device_name>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <device_name>: the BLE device name. The maximum length is 29.


AT+BLESCANPARAM
--------------------
Function: Query/set the parameters of BLE scanning.

Query command
^^^^^^^^^^^^^

**Function**

Query the parameters of BLE scanning.

**Command**

::

    AT+BLESCANPARAM?

**Response**

.. container:: screenoutput

 .. code::

    +BLESCANPARAM:<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>
    OK

Set command
^^^^^^^^^^^

**Function**

Set the parameters of BLE scanning.

**Command**

::

    AT+BLESCANPARAM=<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <scan_type>:

  * 0: passive scan
  * 1: active scan

- <own_addr_type>:

  * 0: Public address
  * 1: Random address
  * 2: RPA public address
  * 3: RPA random address

- <filter_policy>:

  * 0: BLE_SCAN_FILTER_ALLOW_ALL
  * 1: BLE_SCAN_FILTER_ALLOW_ONLY_WLST
  * 2: BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR
  * 3: BLE_SCAN_FILTER_ALLOW_WLIST_PRA_DIR

- <scan_interval>: scan interval. It should be more than or equal to the value of <scan_window>. The range of this parameter is [0x0004,0x4000], which corresponds to [4, 16384] . The scan interval equals this parameter multiplied by 0.625 ms, so the range for the actual scan interval is [2.5,10240] ms.

- <scan_window>: scan window. It should be less than or equal to the value of <scan_interval>. The range of this parameter is [0x0004,0x4000], which corresponds to [4, 16384]. The scan window equals this parameter multiplied by 0.625 ms, so the range for the actual scan window is [2.5,10240] ms.


AT+BLESCAN
--------------------
Function: Enable BLE scanning.

Set command
^^^^^^^^^^^

**Function**

Enable/disable scanning.

**Command**

::

    AT+BLESCAN=<enable>

**Response**

.. container:: screenoutput

 .. code::

    +BLE:SCAN:<addr>,<rssi>,<adv_data>,<scan_rsp_data>,<addr_type>
    OK

**Parameters**

- <enable>:

  * 1: enable continuous scanning.
  * 0: disable continuous scanning.

- <addr>: BLE address.

- <rssi>: signal strength.

- <adv_data>: advertising data.

- <scan_rsp_data>: scan response data.

- <addr_type>: the address type of broadcasters.


**Example**

::

    AT+BLEINIT=1    // Role: client
    AT+BLESCAN=1    // start scanning
    AT+BLESCAN=0    // stop scanning


AT+BLESCANRSPDATA
---------------------
Function: Set BLE scan response data.

Set command
^^^^^^^^^^^

**Function**

Set scan response.

**Command**

::

    AT+BLESCANRSPDATA=<scan_rsp_data>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <scan_rsp_data>: scan response data is a HEX string. For example, if you want to set the response data to “02010606095757575757”, the command should be AT+BLESCANRSPDATA="02010606095757575757". BT_DATA_NAME_COMPLETE DATA index must be 2, like AT+BLESCANRSPDATA="02010606095757575757".

  - 02: Command
  - 01: Flag
  - 06: Value
  - 06: Length of data + 1
  - 09: AD type
  - 5757575757: Data in Hex format

**Example**

::

    AT+BLEINIT=2   // Role: server
    AT+BLESCANRSPDATA="02010606095757575757"


AT+BLEADVPARAM
------------------
Function: Query/set parameters of BLE advertising.

Query command
^^^^^^^^^^^^^

**Function**

Query the parameters of advertising.

**Command**

::

    AT+BLEADVPARAM?

**Response**

.. container:: screenoutput

 .. code::

    +BLEADVPARAM:<adv_int_min>,<adv_int_max>,<adv_type>,<channel_map>
    OK

Set command
^^^^^^^^^^^

**Function**

Set the parameters of advertising.

**Command**

::

    AT+BLEADVPARAM=<adv_int_min>,<adv_int_max>,<adv_type>,<channel_map>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <adv_int_min>: minimum advertising interval. The range of this parameter is [0x0020,0x4000] , which corresponds to [32, 16384]. The actual advertising interval equals this parameter multiplied by 0.625 ms, so the range for the actual minimum interval is [20, 10240] ms. It should be less than or equal to the value of <adv_int_max>.

- <adv_int_max>: maximum advertising interval. The range of this parameter is [0x0020,0x4000], which corresponds to [32, 16384]. The actual advertising interval equals this parameter multiplied by 0.625 ms, so the range for the actual maximum interval is [20, 10240] ms. It should be more than or equal to the value of <adv_int_min>.

- <adv_type>:

  * 0: ADV_TYPE_IND
  * 1: ADV_TYPE_SCAN_IND
  * 2: ADV_TYPE_NONCONN_IND

- <channel_map>: channel of advertising.

  * 1: ADV_CHNL_37
  * 2: ADV_CHNL_38
  * 4: ADV_CHNL_39
  * 7: ADV_CHNL_ALL


**Example**

::

    AT+BLEINIT=2   // Role: server
    AT+BLEADVPARAM=160,320,0,7


AT+BLEADVDATA
-------------------
Function: Set BLE advertising data.

Set command
^^^^^^^^^^^

**Function**

Set advertising data.


**Command**

::

    AT+BLEADVDATA=<adv_data>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <adv_data>: advertising data in HEX string. For example, to set the advertising data to “02010606095757575757”, the command should be AT+BLEADVDATA="02010606095757575757". The maximum length is 31 bytes.

  - 02: Command
  - 01: Flag
  - 06: Value
  - 06: Length of data + 1
  - 09: AD type
  - 5757575757: Data in Hex format

**Example**

::

    AT+BLEINIT=2   // Role: server
    AT+BLEADVDATA="02010606095757575757"


AT+BLEADVSTART
---------------------------------
Function: Start BLE advertising.

Execute command
^^^^^^^^^^^^^^^

**Function**

Start advertising.

**Command**

::

    AT+BLEADVSTART

**Response**

.. container:: screenoutput

 .. code::

        OK

**Example**

::

    AT+BLEINIT=2   // Role: server
    AT+BLEADVSTART


AT+BLEADVSTOP
---------------
Function: Stop BLE advertising.

Execute command
^^^^^^^^^^^^^^^

**Function**

Stop advertising.

**Command**

::

    AT+BLEADVSTOP

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    AT+BLEINIT=2   // Role: server
    AT+BLEADVSTART
    AT+BLEADVSTOP


AT+BLECONN
--------------
Function: Establish BLE connection.

Query command
^^^^^^^^^^^^^

**Function**

Query the Bluetooth LE connection.

**Command**

::

    AT+BLECONN?

**Response**

.. container:: screenoutput

 .. code::

    +BLECONN:<conn_index>,<remote_address>

    OK

If the connection has not been established, there will be no <conn_index> and <remote_address> in the response.

Set command
^^^^^^^^^^^

**Function**

Establish the BLE connection.

**Command**

::

    AT+BLECONN=<conn_index>,<remote_address>[,<addr_type>,<timeout>]

**Response**

If the connection is established successfully, it will prompt:

.. container:: screenoutput

 .. code::

   +BLE:CONNECTED:<conn_index>,<remote_address>,<conn_role>
    OK

If the connection fails, it will prompt:

.. container:: screenoutput

 .. code::

    ERROR

If the connection fails due to parameters error or other reasons, it will prompt:

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <remote_address>: remote BLE address.

- <addr_type>: the address type of broadcasters:

  * 0: Public address
  * 1: Random address

- <timeout>: connection req timeout (option). Range: [3,30].

**Example**

::

    AT+BLEINIT=1   // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10


AT+BLECONNPARAM
----------------------
Function: Query/update parameters of BLE connection.

Query command
^^^^^^^^^^^^^

**Function**

Query the parameters of BLE connection.

**Command**

::

    AT+BLECONNPARAM?

**Response**

.. container:: screenoutput

 .. code::

    +BLECONNPARAM:<conn_index>,<min_interval>,<max_interval>,<cur_interval>,<latency>,<timeout>
    OK

Set command
^^^^^^^^^^^

**Function**

Update the parameters of BLE connection.

**Command**

::

    AT+BLECONNPARAM=<conn_index>,<min_interval>,<max_interval>,<latency>,<timeout>

**Response**

.. container:: screenoutput

 .. code::

    OK

If the setting fails, it will prompt the message below:

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <min_interval>: minimum connecting interval. It should be less than or equal to the value of <max_interval>. The range of this parameter is [0x0006,0x0C80], which corresponds to [6,3200]. The actual connecting interval equals this parameter multiplied by 1.25 ms, so the range for the actual minimum interval is [7.5,4000] ms.

- <max_interval>: maximum connecting interval. It should be more than or equal to the value of <min_interval>. The range of this parameter is [0x0006,0x0C80], which corresponds to [6,3200]. The actual connecting interval equals this parameter multiplied by 1.25 ms, so the range for the actual maximum interval is [7.5,4000] ms.

- <cur_interval>: current connecting interval.

- <latency>: latency. Range: [0, 499].

- <timeout>: timeout. The range of this parameter is [0x000A,0x0C80], which corresponds to [10,3200] ms. The actual timeout equals this parameter multiplied by 10 ms, so the range for the actual timeout is [100,32000] ms.


**Example**

::

    AT+BLEINIT=1   // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLECONNPARAM=0,12,14,1,500


AT+BLEDISCONN
-----------------
Function: End BLE connection.

Execute command
^^^^^^^^^^^^^^^^

**Function**

End the BLE connection.

**Command**

::

    AT+BLEDISCONN=<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    OK  // The AT+BLEDISCONN command is received.
    +BLE:DISCONNECTED:<conn_index>,<remote_address>  // The command is successful.

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <remote_address>: remote BLE address.

**Example**

::

    AT+BLEINIT=1   // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEDISCONN=0


AT+BLEDATALEN
---------------
Function: Set the length of BLE data packet.

Query command
^^^^^^^^^^^^^

**Function**

Set the length of BLE data packet.

**Command**

::

    AT+BLEDATALEN=<conn_index>,<tx_octets>,<tx_time>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <tx_octets>: data packet's length. Range: [27,251].

- <tx_time>: data packet transition time: Range: [328, 17040] ms.

.. note::

    The BLE connection has to be established first.

**Example**

::

    AT+BLEINIT=1   // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEDATALEN=0,251,2120


AT+BLEEXCHANGEMTU
-----------------
Function: Exchange BLE MTU length.

Set command
^^^^^^^^^^^

**Function**

Set the length of the maximum transmission unit (MTU).

**Command**

::

    AT+BLEEXCHANGEMTU=<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    OK
    +BLE:MTUSIZE:<conn_index>,<MTU>

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

**Example**

::

    AT+BLEINIT=1   // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEEXCHANGEMTU=0


AT+BLEGATTSSRV
--------------
Function: GATTS discovers services.

Query command
^^^^^^^^^^^^^

**Function**

GATTS discovers services.

**Command**

::

    AT+BLEGATTSSRV?

**Response**

.. container:: screenoutput

 .. code::

    +BLEGATTSSRV:<srv_index>,<srv_uuid>,<srv_type>,<uuid_type>

    OK

**Parameters**

- <srv_index>: service's index starting from 0 to 2

- <srv_uuid>: service's UUID

- <srv_type>: service's type

  * 0: not primary service
  * 1: primary service

- <uuid_type>: UUID's type

  * 0: 16-bit UUID
  * 2: 128-bit UUID

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRV?


AT+BLEGATTSSRVCRE
-----------------------------------------------------------------------
Function: GATTS creates services.

Set command
^^^^^^^^^^^

**Function**

The Generic Attributes Server (GATTS) creates BLE services.

**Command**

::

    AT+BLEGATTSSRVCRE=<srv_index>,<srv_uuid>,<srv_type>,<uuid_type>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <srv_index>: service's index starting from 0 to 2

- <srv_uuid>: service's UUID

- <srv_type>: service's type

  * 0: not primary service
  * 1: primary service

- <uuid_type>: UUID's type

  * 0: 16-bit UUID
  * 2: 128-bit UUID

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVCRE=0,"01020304010203040102030401020304",1,2


AT+BLEGATTSSRVDEL
-----------------------------------------------------------------------
Function: GATTS delete services.

Set command
^^^^^^^^^^^

**Function**

The Generic Attributes Server (GATTS) delete BLE services.

**Command**

::

    AT+BLEGATTSSRVDEL=<srv_index>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <srv_index>: service's index starting from 0 to 2

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVDEL=0

AT+BLEGATTSREGISTER
---------------------
Function: GATTS services register/unregister.

Set command
^^^^^^^^^^^

**Function**

GATTS services register/unregister.

**Command**

::

    AT+BLEGATTSREGISTER=<enable>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <enable>: 1: Register; 0: Unregister

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVCRE=0,"01020304010203040102030401020304",1,2 //create service first
    AT+BLEGATTSCHARCRE=0,0,"01020304010203040102030401020305",2,3,2 //write property(read) (read|write)
    AT+BLEGATTSREGISTER=1 //Register Service

AT+BLEGATTSCHAR
---------------------
Function: GATTS discovers characteristics.

Query command
^^^^^^^^^^^^^

**Function**

GATTS discovers characteristics.

**Command**

::

    AT+BLEGATTSCHAR?

**Response**

.. container:: screenoutput

 .. code::

    +BLEGATTSCHAR:<srv_index>,<char_index>,<char_uuid>,<char_prop>,<char_perm>,<uuid_type>

    OK

**Parameters**

- <srv_index>: service's index starting from 0 to 2

- <char_index>: characteristic's index starting from 0 to 4

- <char_uuid>: characteristic's UUID

- <char_prop>: characteristic's property

  * 2: characteristic read property
  * 4: characteristic write without response property
  * 8: characteristic write with response property
  * 16: characteristic notify property
  * 32: characteristic indicate property

- <char_perm>: characteristic's permissions

  * 1: read permissions
  * 2: write permissions

- <uuid_type>: UUID's type

  * 0: 16-bit UUID
  * 2: 128-bit UUID

AT+BLEGATTSCHARCRE
-----------------------------------------------------------------------
Function: GATTS creates characteristics.

Set command
^^^^^^^^^^^

**Function**

The Generic Attributes Server (GATTS) creates BLE servic's characteristics.

**Command**

::

    AT+BLEGATTSCHARCRE=<srv_index>,<char_index>,<char_uuid>,<char_prop>,<char_perm>,<uuid_type>

**Response**

.. container:: screenoutput

 .. code::

    Recv <length> bytes
    SEND OK

**Parameters**

- <srv_index>: service's index starting from 0 to 2

- <char_index>: characteristic's index starting from 0 to 4

- <char_uuid>: characteristic's UUID

- <char_prop>: characteristic's property

  * 2: characteristic read property
  * 4: characteristic write without response property
  * 8: characteristic write with response property
  * 16: characteristic notify property
  * 32: characteristic indicate property

- <char_perm>: characteristic's permissions

  * 1: read permissions
  * 2: write permissions

- <uuid_type>: UUID's type

  * 0: 16-bit UUID
  * 2: 128-bit UUID

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVCRE=0,"01020304010203040102030401020304",1,2 //create service first
    AT+BLEGATTSCHARCRE=0,0,"01020304010203040102030401020305",2,3,2 //write property(read) (read|write)


AT+BLEGATTSNTFY
-----------------------------------------------------------------------------------
Function: Notify the characteristic value from the server to a client.

Set command
^^^^^^^^^^^

**Function**

Notify the characteristic value from the server to a client.

**Command**

::

    AT+BLEGATTSNTFY=<srv_index>,<char_index>,<length>,<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter data now. When the requirement of data length determined by the parameter <length> is met, the notification starts.

If the data transmission is successful, AT returns:

.. container:: screenoutput

 .. code::

    Recv <length> bytes
    SEND OK

**Parameters**

- <conn_index>: connection index(optional), default conn_index is zero.

- <srv_index>: service's index starting from 0 to 2

- <char_index>: characteristic's index starting from 0 to 4

- <length>: maximum data length is 244.

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVCRE=0,"01020304010203040102030401020304",1,2//create service first
    AT+BLEGATTSCHARCRE=0,0,"01020304010203040102030401020305",8,2,2 //notify property
    AT+BLEGATTSREGISTER=1 //Register Service
    AT+BLEGATTSNTFY=0,0,4,0
    // After the symbol ">" shows, enter the 4-byte data, such as "1234". Then the data will be transmitted automatically.


AT+BLEGATTSIND
-----------------------------------------------------------------------------------
Function: Indicate the characteristic value from the server to a client.

Set command
^^^^^^^^^^^

**Function**

Indicate the characteristic value from the server to a client.

**Command**

::

    AT+BLEGATTSIND=<srv_index>,<char_index>,<length>,<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter data now. When the requirement of data length determined by the parameter <length> is met, the notification starts.

If the data transmission is successful, AT returns:

::

    Recv <length> bytes
    SEND OK

**Parameters**

- <conn_index>: connection index(optional), default conn_index is zero.

- <srv_index>: service's index starting from 0 to 2

- <char_index>: characteristic's index starting from 0 to 4

- <length>: maximum data length is 244.

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVCRE=0,"01020304010203040102030401020304",1,2 //create service first
    AT+BLEGATTSCHARCRE=0,0,"01020304010203040102030401020305",16,2,2 //indicate property
    AT+BLEGATTSREGISTER=1 //Register Service
    AT+BLEGATTSIND=0,0,4,0
    // After the symbol ">" shows, enter the 4-byte data, such as "1234". Then the data will be transmitted automatically.


AT+BLEGATTSRD
-----------------------------------------------------------------------------------
Function: Set the data when client read characteristic from the server.

Set command
^^^^^^^^^^^

**Function**

Set the data when client read characteristic from the server.

**Command**

::

    AT+BLEGATTSRD=<srv_index>,<char_index>,<length>

**Response**

.. container:: screenoutput

 .. code::

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter data now. When the requirement of data length determined by the parameter <length> is met, the notification starts.

If the data transmission is successful, AT returns:

::

    Recv <length> bytes
    SEND OK

**Parameters**

- <srv_index>: service's index starting from 0 to 2

- <char_index>: characteristic's index starting from 0 to 4

- <length>: data length, range 0-244 bytes. When set to 0, it clears the characteristic data (sets read_data_len to 0 and clears the buffer).

.. note::
   When <length> is 0, the command clears the characteristic data immediately without waiting for serial data input. This is useful for resetting the characteristic value to an empty state.

**Example**

::

    AT+BLEINIT=2 // Role: server
    AT+BLEGATTSSRVCRE=0,"01020304010203040102030401020304",1,2 //create service first
    AT+BLEGATTSCHARCRE=0,0,"01020304010203040102030401020305",1,2,2 //read property
    AT+BLEGATTSREGISTER=1 //Register Service
    AT+BLEGATTSRD=0,0,4
    // After the symbol ">" shows, enter the 4-byte data, such as "1234". Then the data will be transmitted automatically when Client read characteristic from the Server.

    // Clear characteristic data:
    AT+BLEGATTSRD=0,0,0
    // This clears the characteristic data buffer and sets length to 0
    // Response: SEND OK


AT+BLEGATTCSRVDIS
-----------------------------------------------------------------------------------
Function: GATTC discover services.

Set command
^^^^^^^^^^^

**Function**

GATTC discover services.

**Command**

::

    AT+BLEGATTCSRVDIS=<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    +BLE:SRV:<conn_index>,<srv_index>,<srv_uuid>,<srv_type>,<start_handle>,<end_handle>
    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <srv_index>: service's index starting from 1.

- <srv_uuid>: service's UUID.

- <srv_type>: service's type

  * 0: not primary service
  * 1: primary service

.. note::

    The BLE connection has to be established first.

**Example**

::

    AT+BLEINIT=1 // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEGATTCSRVDIS=0


AT+BLEGATTCCHARDIS
-----------------------------------------------------------------------------------
Function: GATTC discover characteristics.

Set command
^^^^^^^^^^^

**Function**

GATTC discover characteristics.

**Command**

::

    AT+BLEGATTCCHARDIS=<conn_index>,<srv_index>

**Response**

.. container:: screenoutput

 .. code::

    +BLE:SRVCHAR=<conn_index>,<srv_index>,<char_index>,<char_uuid>,<char_prop>,<char_handle>,<char_value_handle>
    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <srv_index>: service's index starting from 1.

- <char_index>: characteristic's index starting from 1.

- <char_uuid>: characteristic's UUID.

- <char_prop>: characteristic's property

  * 1: characteristic read property
  * 2: characteristic write without response property
  * 4: characteristic write with response property
  * 8: characteristic notify property
  * 16: characteristic indicate property

.. note::

    The BLE connection has to be established first and the service needs to be discovered first.

**Example**

::

    AT+BLEINIT=1 // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEGATTCSRVDIS=0
    AT+BLEGATTCCHARDIS=0,1


AT+BLEGATTCWR
-----------------------------------------------------------------------------------
Function: GATTC writes characteristics.

Set command
^^^^^^^^^^^

**Function**

GATTC writes characteristics.

**Command**

::

    AT+BLEGATTCWR=<conn_index>,<srv_index>,<char_index>,<length>

**Response**

.. container:: screenoutput

 .. code::

    >

The symbol > indicates that AT is ready for receiving serial data, and you can enter data now. When the requirement of data length determined by the parameter <length> is met, the notification starts.

If the data transmission is successful, AT returns:

.. container:: screenoutput

 .. code::

    Recv <length> bytes
    SEND OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <srv_index>: service's index starting from 1.

- <char_index>: characteristic's index starting from 1.

- <length>: maximum data length is 244.

**Example**

::

    AT+BLEINIT=1 // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEGATTCSRVDIS=0
    AT+BLEGATTCCHARDIS=0,1
    AT+BLEGATTCWR=0,1,2,4
    // After the symbol ">" shows, enter the 4-byte data, such as "1234". Then the data will be transmitted automatically.


AT+BLEGATTCRD
-----------------------------------------------------------------------------------
Function: GATTC read characteristics.

Set command
^^^^^^^^^^^

**Function**

GATTC read characteristics.

**Command**

::

    AT+BLEGATTCRD=<conn_index>,<srv_index>,<char_index>

**Response**

.. container:: screenoutput

 .. code::

    +BLE:GATTREAD:<conn_index>,<srv_index>,<char_index>,<length>
    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <srv_index>: service's index starting from 1.

- <char_index>: characteristic's index starting from 1.

- <length>: data length

**Example**

::

    AT+BLEINIT=1 // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEGATTCSRVDIS=0
    AT+BLEGATTCCHARDIS=0,1
    AT+BLEGATTCRD=0,1,1


AT+BLEGATTCSUBSCRIBE
-----------------------------------------------------------------------------------
Function: GATTC subscribe server.

Set command
^^^^^^^^^^^

**Function**

GATTC subscribe server.

**Command**

::

    AT+BLEGATTCSUBSCRIBE=<conn_index>,<ccc_handle>,<value_handle>,<value>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <ccc_handle>: Client Characteristic Configuration Descriptor handle.

- <value_handle>: Characteristic value handle.

- <value>: 1: notification; 2: indication

**Example**

::

    AT+BLEINIT=1 // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEGATTCSUBSCRIBE=0,15,14,1


AT+BLEGATTCUNSUBSCRIBE
-----------------------------------------------------------------------------------
Function: GATTC unsubscribe server.

Set command
^^^^^^^^^^^

**Function**

GATTC unsubscribe server.

**Command**

::

    AT+BLEGATTCUNSUBSCRIBE=<conn_index>,<value_handle>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

- <value_handle>: Characteristic value handle.

**Example**

::

    AT+BLEINIT=1 // Role: client
    AT+BLECONN=0,"11:22:33:44:55:66",0,10
    AT+BLEGATTCUNSUBSCRIBE=0,14

AT+BLETXPWR
-----------------------------------------------------------------------------------
Function: Query/set BLE TX power.

Query command
^^^^^^^^^^^^^

**Function**

Query BLE TX power.

**Command**

::

    AT+BLETXPWR?

**Response**

.. container:: screenoutput

 .. code::

    +BLETXPWR:<tx_power> Range: [0,20].
    OK

Set command
^^^^^^^^^^^

**Function**

Set BLE TX power.

**Command**

::

    AT+BLETXPWR=<tx_power>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <tx_power>: tx power

**Example**

::

    AT+BLEINIT=1 // or AT+BLEINIT=2
    AT+BLETXPWR=10

AT+BLESECPARAM
-----------------------------------------------------------------------------------
Function: Set/query BLE security parameter

Query command
^^^^^^^^^^^^^

**Function**

Query BLE security parameter.

**Command**

::

    AT+BLESECPARAM?

**Response**

.. container:: screenoutput

 .. code::

    +BLESECPARAM:<security parameter>,<security level>,<bond>
    OK

Set command
^^^^^^^^^^^

**Function**

Set BLE security parameter.

**Command**

::

    AT+BLESECPARAM=<security parameter>[,<security level>[,<bond>]]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <security parameter>: security parameter

    - 0: BT_SMP_IO_DISPLAY_ONLY
    - 1: BT_SMP_IO_DISPLAY_YESNO
    - 2: BT_SMP_IO_KEYBOARD_ONLY
    - 3: BT_SMP_IO_NO_INPUT_OUTPUT
    - 4: BT_SMP_IO_KEYBOARD_DISPLAY

- <security level>: security level (optional)
    - BT_SECURITY_L1   1 : Level 1: No encryption and no authentication.
    - BT_SECURITY_L2   2 : Level 2: Encryption and no authentication (no MITM).
    - BT_SECURITY_L3   3 : Level 3: Encryption and authentication (MITM).
    - BT_SECURITY_L4   4 : Level 4: Authenticated Secure Connections and 128-bit key.
    - For the security level parameter, it is generally recommended to set it to BT_SECURITY_L1 or leave it unset. Once the security level is configured, if the AT+BLESECSTART command needs to be used, the sec_level parameter in AT+BLESECSTART must be greater than or equal to the security level set by AT+BLESECPARAM.

- <bond>: whether to store credentials after pairing (optional, default: 1)
    - 0: Pairing only — establish encrypted connection but do **not** save keys (LTK/IRK) to storage. The peer must re-pair after reconnection.
    - 1: Bonding — save keys to persistent storage so the encrypted connection can be restored after reconnection without re-pairing.

**Example**

::

    AT+BLEINIT=1 // or AT+BLEINIT=2
    AT+BLESECPARAM=4         // BT_SMP_IO_KEYBOARD_DISPLAY, bonding (default)
    AT+BLESECPARAM=4,1,0     // BT_SMP_IO_KEYBOARD_DISPLAY, pairing only (no credential storage)

AT+BLESECCANNEL
-----------------------------------------------------------------------------------
Function: BLE cancel pairing.

Set command
^^^^^^^^^^^

**Function**

BLE cancel pairing.

**Command**

::

    AT+BLESECCANNEL=<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

**Example**

::

    AT+BLESECCANNEL=0

AT+BLESECPASSKEYCONFIRM
-------------------------------------------
Function: BLE pairing passkey confirm.

Set command
^^^^^^^^^^^

**Function**

BLE pairing passkey confirm .

**Command**

::


    AT+BLESECPASSKEYCONFIRM=<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

**Example**

::

    AT+BLESECPASSKEYCONFIRM=0

AT+BLESECPAIRINGCONFIRM
-------------------------------------------
Function: BLE pairing passkey confirm.

Set command
^^^^^^^^^^^

**Function**

BLE pairing confirm.

**Command**

::

    AT+BLESECPAIRINGCONFIRM=<conn_index>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].

**Example**

::

    AT+BLESECPAIRINGCONFIRM=0

AT+BLESECPASSKEY
-------------------------------------------
Function: BLE pairing pass entry.

Set command
^^^^^^^^^^^

**Function**

BLE pairing pass entry.

**Command**

::

    AT+BLESECPASSKEY=<conn_index>,<passkey>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].
- <passkey>: pairing passkey. Range: [0,999999].

**Example**

::

    AT+BLESECPASSKEY=0,123456

AT+BLESECGETLTKLIST
-------------------------------------------
Function: Query BLE paired deviceList.


Query command
^^^^^^^^^^^^^

**Function**

Query BLE paired deviceList.

**Command**

::

    AT+BLESECGETLTKLIST?

**Response**

.. container:: screenoutput

 .. code::

    +BTADDR 12:34:56:78:00:11 (public) LTK:01234567890123456789012345678900
    OK

AT+BLESECUNPAIR
-------------------------------------------
Function: BLE unpairing.

Set command
^^^^^^^^^^^

**Function**

BLE unpairing.

**Command**

::

    AT+BLESECUNPAIR=<address>,<addrtype>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <addr>: BLE address.
- <addr_type>: BLE address type.

**Example**

::

    AT+BLESECUNPAIR="12:34:56:78:00:12",0


AT+BLESECSTART
-------------------------------------------
Function: BLE start pairing.

Set command
^^^^^^^^^^^

**Function**

BLE start pairing.

**Command**

::

    AT+BLESECSTART=<conn_index>,<sec_level>

**Response**

.. container:: screenoutput

 .. code::

    OK

If error in pairing

.. container:: screenoutput

 .. code::

    +BLE:PAIRINGFAILED: <addr>

    +BLE:DISCONNECTED:<conn_index>,<addr>

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].
- <sec_level>: Security level. Range: [0,4].

    - 0: BT_SECURITY_L0, Only for BR/EDR special cases, like SDP.
    - 1: BT_SEC_LEVEL_1, No encryption and no authentication.
    - 2: BT_SEC_LEVEL_2, Encryption and no authentication (no MITM).
    - 3: BT_SEC_LEVEL_3, Encryption and authentication (MITM).
    - 4: BT_SEC_LEVEL_4, Authenticated Secure Connections and 128-bit key.

**Example**

::

    AT+BLESECSTART=0,4


AT+BLEBASINIT
-------------------------------------------
**Function**

Initialize the Bluetooth BAS profile.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEBASINIT

**Response**

.. container:: screenoutput

 .. code::

    OK


AT+BLEBASDEINIT
-------------------------------------------
**Function**

Deinitialize the Bluetooth BAS profile.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEBASDEINIT

**Response**

.. container:: screenoutput

 .. code::

    OK


AT+BLEBASLVLGET
-------------------------------------------
Function: Query BLE BAS level.


Query command
^^^^^^^^^^^^^

**Function**

Query BLE BAS level.

**Command**

::

    AT+BLEBASLVLGET?

**Response**

.. container:: screenoutput

 .. code::

    +BLEBASLEVEL:100 (level)
    OK


AT+BLEBASLVLSET
-------------------------------------------------
**Function**

Set BLE BAS level.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEBASLVLSET=<conn idx>,<level>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- <conn_index>: index of BLE connection. Range: [0,9].
- <level>: battery level. Range: [0,100].


**Example**

::

    AT+BLEBASLVLSET=0,100


AT+BLEIASINIT
-------------------------------------------
**Function**

Initialize the Bluetooth IAS profile.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEIASINIT

**Response**

.. container:: screenoutput

 .. code::

    OK


AT+BLEIASDEINIT
-------------------------------------------
**Function**

Deinitialize the Bluetooth IAS profile.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEIASDEINIT

**Response**

.. container:: screenoutput

 .. code::

    OK

AT+BLEDISINIT
-------------------------------------------------
**Function**

Initialize the Bluetooth DIS profile.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEDISINIT=<pnp_vid_src>,<pnp_vid>,<pnp_pid>,<pnp_ver>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<pnp_vid_src>**
    - Range: <0,255>
- **<pnp_vid>**
    - Range: <0,65535>
- **<pnp_pid>**
    - Range: <0,65535>
- **<pnp_ver>**
    - Range: <0,65535>

**Example**

::

    AT+BLEDISINIT=100,101,102,103


AT+BLEDISDEINIT
-------------------------------------------
**Function**

Deinitialize the Bluetooth DIS profile.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEDISDEINIT

**Response**

.. container:: screenoutput

 .. code::

    OK

AT+BLEDISSET
-------------------------------------------------
**Function**

Set the Bluetooth DIS profile.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLEDISSET=<parameter_name>,<parameter_value>,<parameter_len>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<parameter_name>**:
    - "manuf": Manufacturer Name String
    - "model": Model Number String
    - "serial": Serial Number String
    - "fw": Firmware Revision String
    - "hw": Hardware Revision String
    - "sw": Software Revision String
- **<parameter_value>**
- **<parameter_len>** Range: [0,21].


**Example**

::

    AT+BLEDISSET="manuf","BL61x",5

AT+BLESETGAPAPPEARANCE
-------------------------------------------------
**Function**

SET the Bluetooth GAP APPEARANCE

Setup command
^^^^^^^^^^^^^^^

**Command**

::

    AT+BLESETGAPAPPEARANCE=<parameter_value>

**Response**

.. container:: screenoutput

 .. code:: text

    OK

Parameters
^^^^^^^^^^^^^^

- **<parameter_value>** Range: [0,65535].


Example
^^^^^^^^

::

    AT+BLESETGAPAPPEARANCE=0

======================================================================================
Low power AT commands
======================================================================================
.. table::
    :widths: 30,70
    :width: 100%
    :align: center

    +---------------------------+----------------------------------------------------------------------------------------------+
    | Instruction               | Description                                                                                  |
    +===========================+==============================================================================================+
    | `AT+PWR`_                 | Configure power mode                                                                         |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+SLWKIO`_              | Configure GPIO as wakeup source                                                              |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+DEWKIO`_              | Delete GPIO as wakeup source                                                                 |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+SLWKTIMER`_           | Configure timer as wakeup source                                                             |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+GET_AP_DTIM`_         | Query the current DTIM setting obtained by the STA                                           |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+GET_TWT_SUPPORTED`_   | Query whether TWT (Target Wake Time) is supported by the current firmware/platform           |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+PWR_CLEAR`_           | Clear/reset the internal power statistics/counters used by ``PWR_GET``                       |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+PWR_GET`_             | Get the current averaged power value (unit defined by implementation)                        |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+SLWKDTIM`_            | Configure DTIM                                                                               |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+SLCLDTIM`_            | Delete DTIM config                                                                           |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+TWT_PARAM`_           | Setup TWT parameters                                                                         |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+TWT_SLEEP`_           | Enter TWT low power                                                                          |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+TWT_TEARDOWN`_        | Tear down TWT                                                                                |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+TWT_STATUS`_          | Get TWT status                                                                               |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+LP_INTERVAL_GET`_     | Query the station listen interval value for power saving mode                                |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+START_KEEPALIVE`_     | Start sending keepalive frames                                                               |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+STOP_KEEPALIVE`_      | Stop sending keepalive frames                                                                |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+GET_CLOCK`_           | Get the current clock source                                                                 |
    +---------------------------+----------------------------------------------------------------------------------------------+
    | `AT+SET_CLOCK`_           | Set the system clock source                                                                  |
    +---------------------------+----------------------------------------------------------------------------------------------+


Low power AT command description
================================


AT+PWR
-----------------------------------------------------------------------
Function: Configure power mode

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+PWR=<power mode>[,<level>]

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<power mode>**:
   -  0: normal mode.
   -  1: hibernate mode.
   -  2: standby mode.
- **[<level>]**:
   -  In hibernate mode: power level 0-2;
   -  In standby mode: 1 - enable receive broadcast package.

**Example**

::

    AT+PWR=0             // Normal mode
    AT+PWR=1,0           // Hbn mode, level 0.
    AT+PWR=2             // Standby mode
    AT+PWR=2,1           // Standby mode, enable receive broadcast package


AT+SLWKIO
-----------------------------------------------------------------------
Function: Configure GPIO as wakeup source.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+SLWKIO=<wakeup io>,<trigger mode>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<wakeup io>**: GPIO wakeup IO number. Supported values: 16 or 28
- **<trigger mode>**: wakeup trigger mode

   -  0: Wakeup by high level

.. note::

    The current implementation uses a five-wire SPI low-power solution, so the wakeup method isn't configurable and only supports high-level wakeup.


AT+DEWKIO
-----------------------------------------------------------------------
Function: Delete GPIO as wakeup source.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+DEWKIO=<io>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<io>**: GPIO wakeup IO number.


AT+SLWKTIMER
------------------------------------------------------
Function: Configure timer as wakeup source.

Set command
^^^^^^^^^^^

**Command**

::

    AT+SLWKTIMER=<mode>,<timeouts>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<mode>**: 0: standby mode; 1: hibernate mode
-  **<timeouts>**: Set the wakeup duration in milliseconds. One-time wakeup timer, doesn't enter sleep again after waking up (unless a new wakeup timer is configured again)

**Example**

::

    AT+SLWKTIMER=0,5000                       //Standby mode, Wake up once next 5000ms


AT+GET_AP_DTIM
------------------------------------------------------
Function: Query the current DTIM setting obtained by the STA (as reported by firmware).

Query command
^^^^^^^^^^^^^

**Command**

::

    AT+GET_AP_DTIM?

**Response**

.. container:: screenoutput

 .. code::

    +DTIM:<dtim>
    OK

**Return codes**

- ``OK`` — Query succeeded.
- ``ERROR`` — Query failed.

**Example**

::

    AT+GET_AP_DTIM?
    +DTIM:3
    OK

AT+GET_TWT_SUPPORTED
------------------------------------------------------
Function: Query whether TWT (Target Wake Time) is supported by the current firmware/platform.

Query Command
^^^^^^^^^^^^^
**Command**

::

    AT+GET_TWT_SUPPORTED?

**Response**

.. container:: screenoutput

 .. code::

    +TWT_SUPPORT:<support>
    OK

Where ``<support>`` is:
- ``0`` — Not supported
- ``1`` — Supported

**Return codes**

- ``OK`` — Query succeeded.
- ``ERROR`` — Query failed.

**Example**

::

    AT+GET_TWT_SUPPORTED?
    +TWT_SUPPORT:1
    OK

AT+PWR_CLEAR
------------------------------------------------------
Function: Clear/reset the internal power statistics/counters used by ``PWR_GET``.

Execute command
^^^^^^^^^^^^^^^
**Command**

::

    AT+PWR_CLEAR

**Response**

.. container:: screenoutput

 .. code::

    OK

**Return codes**

- ``OK`` — Operation succeeded.

**Example**

::

    AT+PWR_CLEAR
    OK

AT+PWR_GET
------------------------------------------------------
Function: Get the current averaged power value (unit defined by implementation).

Execute command
^^^^^^^^^^^^^^^
**Command**

::

    AT+PWR_GET

**Response**

.. container:: screenoutput

 .. code::

    +AVERAGE PWR:<value>
    OK

**Return codes**

- ``OK`` — Query succeeded.

**Example**

::

    AT+PWR_GET
    +AVERAGE PWR:1969
    OK

AT+SLWKDTIM
------------------------------------------------------
Function: Configure DTIM as wakeup source

Set command
^^^^^^^^^^^

**Command**

::

    AT+SLWKDTIM=<dtim>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **<dtim>**: Set the wakeup DTIM.

  - ``0``: Wake at every beacon (default; best compatibility, lowest latency).
  - ``1``: Wake at every DTIM (standards-compliant power save; no broadcast and multicast packet loss expected).
  - ``2…10``: Wake once every N DTIMs (stronger power saving; increased latency; broadcast and multicast packets may be missed).

.. Note::

   - Standards compliance:

    With ``<dtim>=0`` or ``1``, behavior is 100% standards-compliant and shouldn't lose broadcast and multicast packets by design.

   - Average reception ratio:

    For ``<dtim> ≥ 2``, the STA is awake for roughly ``1/<dtim>`` of DTIM opportunities:

     - ``<dtim>=2``: ~50% received on average.
     - ``<dtim>=3``: ~33% received on average.
     - In general, received ≈ 1/N, missed ≈ 1 − 1/N.

   - Broadcast/Multicast impact:

    With ``<dtim> ≥ 2``, Broadcast/Multicast (for example, ARP, mDNS/LLMNR, discovery protocols) may be missed. Ensure the application tolerates this.

   - AP-defined DTIM:

    The DTIM period is configured by the AP, not by the STA.

    Effective wake interval is approximately:

     ::

         WakeInterval ≈ N × (AP's DTIM period in beacons) × BeaconInterval

     Example (BeaconInterval = 100 ms):

     - AP DTIM = 1, ``<dtim>=3`` → ~ ``3 × 1 × 100 ms = 300 ms``
     - AP DTIM = 2, ``<dtim>=3`` → ~ ``3 × 2 × 100 ms = 600 ms``

   - Wake point restriction:

    When ``<dtim> > 0``, firmware must wake only at DTIM, not at arbitrary beacons.

   - Selection guidance:

     - Low latency/high reliability: use ``0`` or ``1``.
     - Power focused and tolerant of delay/loss: consider ``2…3`` (or higher) after validation.

**Return codes**

- ``OK`` — Command accepted.
- ``ERROR`` — Parameter out of range or invalid state.

**Example**

::

    AT+SLWKDTIM=10                      // Set dtim10 for wakeup.


AT+SLCLDTIM
------------------------------------------------------
Function: Delete DTIM config

Set command
^^^^^^^^^^^

**Command**

::

    AT+SLCLDTIM

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    AT+SLCLDTIM


AT+TWT_PARAM
------------------------------------------------------
Function: Setup TWT parameters.

Set command
^^^^^^^^^^^

**Command**

::

    AT+TWT_PARAM=<setup type>,<flow type>,<exponent>,<wakeup duration>,<wake interval mantissa>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

-  **Setup type (s):** 0: REQUEST, 1: SUGGEST, 2: DEMAND.
-  **Flow type (t):** 0: ANNOUNCED TWT, 1: UNANNOUNCED TWT. Only supports unannounced TWT.
-  **Wake Interval Exponent:** Specifies the wake interval
-  **Wake Duration (n):** Minimum wake duration in microseconds.
-  **Wake Interval Mantissa (m):** Mantissa value for precise interval adjustments.
-  **Service Period (SP):** SP = WakeDuration * 256us.
-  **Wakeup Interval:** Wakeup Interval = wake_int_mantissa << WakeIntervalExponent us.

**Example**

::

    AT+TWT_PARAM=1,1,13,128,1000                       //SP = 32.768 ms, Wake Interval = 8.192 seconds:


.. note::

   -  REQUEST setup type: The target wake time field of the TWT element is set to 0 as the responding STA/AP specifies the target wake time value in this case. Other fields are suggested by requesting STA.
   -  SUGGEST setup type: TWT requesting STA will suggest the target wake time and other parameters during setup. Responding STA may / may not accommodate the suggested value. TWT Connection will still be accepted with values suggested by requesting STA or the values provided by responding STA.
   -  DEMAND setup type: TWT requesting STA will demand the target wake time and other parameters during setup. If responding STA can't accommodate the suggested value, TWT setup will fail.

   - Equation to calculate Service Period:

     ``Service Period (SP) = (<wakeup duration>/10) ms``

     - Example:
       ``AT+TWT_PARAM=1,1,13,128,1000`` → ``SP = (128/10) = 12.8 ms``

   - Equation to calculate TWT Wake Interval:

     ``TWT Wake Interval = ((2^<exponent>)/<wake interval mantissa>) s``

     - Example:
       ``AT+TWT_PARAM=1,1,13,128,1000`` → ``TWT Wake Interval = ((2^12)/1000) = 8.192 s``



AT+TWT_SLEEP
------------------------------------------------------
Function: Enter TWT low power

Set command
^^^^^^^^^^^

**Command**

::

    AT+TWT_SLEEP

**Response**

.. container:: screenoutput

 .. code::

    OK

**Example**

::

    AT+TWT_SLEEP


AT+TWT_TEARDOWN
-----------------------------------------------
Function: Tear down the Target Wake Time (TWT) session.

Set command
^^^^^^^^^^^

**Command**

::

    AT+TWT_TEARDOWN=<neg_type>,<all_twt>[,<flow_id>]

**Response**

.. container:: screenoutput

 .. code::

    OK

or

.. container:: screenoutput

 .. code::

    ERROR

**Parameters**

- **<neg_type>**: TWT negotiation type.

   - 0: Individual negotiation (TWT teardown will be performed for the specified TWT flow).
   - 1: Negotiate TWT teardown on the next wake TBTT (Target Beacon Transmission Time). Currently not supported.

- **<all_twt>**: Specifies whether to tear down all TWT flows.

   - 0: Tear down only the specified TWT flow (specified by <flow_id> parameter).
   - 1: Tear down all existing TWT flows.

- **<flow_id>**: The TWT flow ID to be torn down. This parameter is required when <all_twt> is set to 0.

.. note::

    -  Internal RC errors can result in wake interval inaccuracies (for example, 60 ms deviation over 30 seconds).
    -  External Oscillator Requirement: Testing must be conducted on boards with external crystal oscillators to minimize RTC clock deviation.
    -  Router Compatibility: Use a TWT-compatible router for testing (for example, TP-Link AX5400).
    -  Currently, only Individual negotiation type (0) is supported.
    -  When <all_twt>=1, the <flow_id> parameter isn't needed.
    -  The device must be in TWT mode before using this command.

**Example**

::

    // Tear down a specific TWT flow with ID 1
    AT+TWT_TEARDOWN=0,0,1

    // Tear down all TWT flows
    AT+TWT_TEARDOWN=0,1


AT+TWT_STATUS
-----------------------------------------------------------------------
Function: Query current TWT (Target Wake Time) session status.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+TWT_STATUS?

**Response**

If TWT is active:

.. container:: screenoutput

 .. code::

    +TWT:ACTIVE:<flow_id>,<flow_type>,<flow_type_str>,<wake_int_exp>,<min_wake_dur>,<wake_int_mantissa>

    OK

If no active TWT session:

.. container:: screenoutput

 .. code::

    +TWT:INACTIVE

    OK

**Parameters**

- **<flow_id>**: ID of the TWT flow
- **<flow_type>**: Type of TWT flow (0 for Announced, 1 for Unannounced)
- **<flow_type_str>**: Text description of flow type ("ANNOUNCED" or "UNANNOUNCED")
- **<wake_int_exp>**: Wake interval exponent
- **<min_wake_dur>**: Minimum TWT wake duration
- **<wake_int_mantissa>**: Wake interval mantissa

**Example**

::

    AT+TWT_STATUS?

    +TWT:ACTIVE:0,1,UNANNOUNCED,10,255,512

    OK


AT+LP_INTERVAL_GET
-----------------------------------------------------------------------
Function: Query the station listen interval value for power saving mode.

Query Command
^^^^^^^^^^^^^^^

**Command**

::

    AT+LP_INTERVAL_GET

**Response**

.. container:: screenoutput

 .. code::

    +LP INTERVAL:<interval>

    OK

**Parameters**

-  **<interval>**: Listen interval value, specified as how often the STA wakes up to listen to beacon frames when in power save mode.


AT+START_KEEPALIVE
-----------------------------------------------------------------------
Function: Start sending Null data frames at regular intervals to maintain AP connection.

Set command
^^^^^^^^^^^^^^^

**Command**

::

    AT+START_KEEPALIVE=<period>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<period>**: The interval between Null data frame transmissions, in seconds.

.. note::

    - This command creates a timer that periodically sends Null data frames to the connected AP to prevent connection timeout during periods of inactivity.
    - This can be useful for maintaining connection with APs that have aggressive client timeout settings.
    - The feature will only work when connected to an AP.

**Example**

::

    AT+START_KEEPALIVE=30

    OK

AT+STOP_KEEPALIVE
-----------------------------------------------------------------------
Function: Stop the periodic Null data frame transmission.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+STOP_KEEPALIVE

**Response**

.. container:: screenoutput

 .. code::

    OK

.. note::

    - This command stops the timer created by AT+START_KEEPALIVE.
    - If no keepalive timer is active, the command will still return OK.

**Example**

::

    AT+STOP_KEEPALIVE

    OK


AT+GET_CLOCK
-----------------------------------------------------------------------
Function: Get current clock source.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+GET_CLOCK

**Response**

.. container:: screenoutput

 .. code::

    +GET_CLOCK:<source>

    OK

**Parameters**

- **<source>**: Current clock source ID.

   - 1: RC as the RTC clock source
   - 2: Passive crystal oscillator as the RTC clock source
   - 3: Active crystal oscillator as the RTC clock source

**Example**

::

    AT+GET_CLOCK

    // Response
    +GET_CLOCK:1

    OK

AT+SET_CLOCK
-----------------------------------------------------------------------
Function: Set system clock source.

Execute command
^^^^^^^^^^^^^^^

**Command**

::

    AT+SET_CLOCK=<source>

**Response**

.. container:: screenoutput

 .. code::

    OK

**Parameters**

- **<source>**: Clock source to set.

   - 1: RC as the RTC clock source
   - 2: Passive crystal oscillator as the RTC clock source
   - 3: Active crystal oscillator as the RTC clock source

.. note::

    - Changing the clock source may affect system performance and power consumption.
    - Some clock sources may not be available in certain power modes.

**Example**

::

    AT+SET_CLOCK=1

    // Response
    OK


===========================
Set up secure communication
===========================
This section walks you through embedding certificates into firmware using ROMFS, setting up a local HTTPS server, configuring MQTT services with TLS authentication, and verify MQTT TLS connectivity between devices and HiveMQ brokers.


Embed certificates with ROMFS
=============================

Integrate certificate files into the firmware by embedding them within a ROMFS (Read Only Memory File System) partition. This enables the device to securely access essential credentials such as CA certificates and private keys at runtime, supporting secure operations including authentication and encrypted communication.

1. Create the ROMFS directory and add certificate files

   Create a folder named ``romfs``, and place the required certificate files inside it. The directory structure should look like this:

   .. code-block:: bash

      romfs$
        ca_01.crt
        cert_01.crt
        private_01.key

2. Generate ``romfs.bin`` using the ``genromfs`` tool

   Use the ``genromfs`` tool located in the SDK directory to generate the binary file. To automate this step, add the following command to your ``Makefile``:

   .. code-block:: make

      $(shell ./tools/genromfs -d romfs/ -f ./build/build_out/romfs.bin)

3. Configure the flash settings

   Open the ``flash_prog_cfg.ini`` file and add a new section for the ROMFS partition. Include the path to the generated binary and specify the flash address:

   .. code-block:: ini

      #media factory
      [romfs]
      filedir = ./build/build_out/romfs.bin
      address = 0x378000

   Make sure the ``address`` value matches the media partition address defined in the partition table.

4. Locate the partition table

   Identify the correct partition table file based on your chip configuration. The file will be located in one of the following paths:

   - ``bsp/board/<chip_name>/config/partition_cfg_4M.toml``
   - ``bsp/board/<chip_name>/config_8M/partition_cfg_8M.toml``

5. Flash the firmware including ``romfs.bin``

   Run the firmware flashing command to download the firmware and the ``romfs.bin`` file into the chip:

   .. code-block:: bash

      make flash CHIP=bl616 COMX=<com_port>



Set up a local HTTPS server
===========================

Set up a local HTTPS server using Python to enable secure communication and demonstrate the configuration of modules for various HTTPS authentication modes. This setup supports testing secure HTTP requests (GET, POST, PUT, HEAD) with different client and server authentication configurations.

Create a file named ``https.py`` and paste the following code:

.. code-block:: python

    import http.server
    import ssl
    import sys
    from socketserver import ThreadingMixIn
    import threading

    class httpHandler(http.server.BaseHTTPRequestHandler):
        def do_GET(self):
            response = "Hello World"
            self.send_response(200)
            self.send_header("Content-Type", "text/plain")
            self.send_header("Content-Length", str(len(response)))
            self.end_headers()
            self.wfile.write(response.encode())

        def do_HEAD(self):
            self.send_response(200)
            self.send_header("Content-Type", "text/plain")
            self.send_header("Content-Length", str(len("Hello World")))
            self.end_headers()

        def do_POST(self):
            self.send_response(405)
            self.send_header("Content-Type", "text/plain")
            self.send_header("Content-Length", str(len("POST Method success")))
            self.end_headers()
            self.wfile.write(b"POST Method success")

        def log_message(self, format, *args):
            return

    class ThreadedHTTPServer(ThreadingMixIn, http.server.HTTPServer):
        daemon_threads = True

    class HttpsServer:
        def __init__(self, ip, port, certfile, keyfile, ca_certs=None):
            self.server_address = (ip, port)
            self.certfile = certfile
            self.keyfile = keyfile
            self.ca_certs = ca_certs
            self.httpd = ThreadedHTTPServer(self.server_address, httpHandler)

            # Enable server-side SSL with client certificate validation
            self.httpd.socket = ssl.wrap_socket(
                self.httpd.socket,
                certfile=self.certfile,
                keyfile=self.keyfile,
                server_side=True,
                cert_reqs=ssl.CERT_REQUIRED,  # Require client certificate
                ca_certs=self.ca_certs  # Path to CA certs for client verification
            )
            self.thread = None

        def start_server(self):
            if self.thread is None:
                self.thread = threading.Thread(target=self.httpd.serve_forever)
                self.thread.start()
                print(f"Server started at https://{self.server_address[0]}:{self.server_address[1]}")
            else:
                print("Server is already running.")

        def stop_server(self):
            if self.thread isn't None:
                self.httpd.shutdown()
                self.thread.join()
                self.thread = None
                print("Server stopped.")
            else:
                print("Server isn't running.")

    # Example usage:
    if __name__ == "__main__":
        ip = sys.argv[1]
        port = int(sys.argv[2])
        certfile = sys.argv[3]
        keyfile = sys.argv[4]
        ca_certs = sys.argv[5] if len(sys.argv) > 5 else None  # Optional CA certs for client verification

        server = HttpsServer(ip, port, certfile, keyfile, ca_certs)
        server.start_server()

        try:
            input("Press Enter to stop the server...\n")
        finally:
            server.stop_server()


1. Run the HTTPS server with:

   .. code-block:: bash

      python3 https.py 192.168.31.112 4443 lfs/lfs/server_1.crt lfs/lfs/server_1.key lfs/lfs/ca_1.crt


2. HTTPS two-way authentication:

   .. code-block:: bash

      AT+HTTPSSLCFG=0,3,"client_1.crt","client_1.key","ca_1.crt"

3. HTTPS server authentication:

   Comment out the following line in ``https.py`` to disable client verification:

   .. code-block:: python

      cert_reqs=ssl.CERT_REQUIRED  # Require client certificate

   Then use:

   .. code-block:: bash

      AT+HTTPSSLCFG=0,1,,,"ca_1.crt"

4. HTTPS client authentication:

   .. code-block:: bash

      AT+HTTPSSLCFG=0,2,"client_1.crt","client_1.key"

5. HTTPS no authentication:

   Comment out the following line in ``https.py`` to disable client verification:

   .. code-block:: python

      cert_reqs=ssl.CERT_REQUIRED  # Require client certificate

   Then use:

   .. code-block:: bash

      AT+HTTPSSLCFG=0,0


6. HTTPS HEAD request:

   .. code-block:: bash

      AT+HTTPCLIENT=0,1,0,"https://192.168.31.112:4443/"

7. HTTPS GET request:

   .. code-block:: bash

      AT+HTTPCLIENT=0,2,0,"https://192.168.31.112:4443/"

8. HTTPS POST request:

   .. code-block:: bash

      AT+HTTPCLIENT=0,3,0,"https://192.168.31.112:4443/"

9. HTTPS PUT request:

   .. code-block:: bash

      AT+HTTPCLIENT=0,4,0,"https://192.168.31.112:4443/"



Configure MQTT services with TLS authentication
===============================================

Set up a secure local MQTT broker using Mosquitto and demonstrate the configuration of modules for publishing and subscribing to MQTT topics with various authentication options.

1. Install Mosquitto

   Install the Mosquitto MQTT broker on your system:

   .. code-block:: bash

      sudo apt install mosquitto

2. Set MQTT user password

   Create a password file and set a password for the user ``bflb``:

   .. code-block:: bash

      mosquitto_passwd -c /etc/mosquitto/passwd bflb

   Enter the password twice when prompted. For example:

   .. code-block:: text

      12345678

3. Configure certificates

   Copy the required certificate and key files to the Mosquitto configuration directory:

   .. code-block:: bash

      cp {case_path}/lfs/lfs/server_1.crt /etc/mosquitto/server_1.crt
      cp {case_path}/lfs/lfs/server_1.key /etc/mosquitto/server_1.key
      cp {case_path}/lfs/lfs/ca_1.crt /etc/mosquitto/ca_1.crt

4. Start the MQTT service

   Launch the Mosquitto broker with your configuration file:

   .. code-block:: bash

      mosquitto -c /etc/mosquitto/mosquitto.conf

5. Subscribe to a topic from PC

   Use the following command to subscribe to a topic securely:

   .. code-block:: bash

      mosquitto_sub -h 192.168.31.156 -u bflb -P 12345678 \
      --cafile lfs/lfs/ca_1.crt --cert lfs/lfs/client_1.crt --key lfs/lfs/client_1.key \
      -t "test/topic1" --tls-version tlsv1.2 --insecure

6. MQTT user configuration

   Configure the device with user credentials and certificates:

   .. code-block:: bash

      AT+MQTTUSERCFG=0,4,"BL001","bflb","12345678","client_1.crt","client_1.key","ca_1.crt"

7. Connect to MQTT broker

   Connect the device to the MQTT broker:

   .. code-block:: bash

      AT+MQTTCONN=0,"192.168.31.156",8883,0

8. Publish a message

   Publish a message to a topic from the device:

   .. code-block:: bash

      AT+MQTTPUB=0,"test/topic1","This is a MQTT test",2,0

9. Subscribe to a topic

   Subscribe to a topic from the device:

   .. code-block:: bash

      AT+MQTTSUB=0,"test/topic2",0
10. Publish a message from PC

    Use the following command to publish a message to the broker from your PC:

    .. code-block:: bash

       mosquitto_pub -h 192.168.31.156 -p 8883 -u bflb -P 12345678 \
       --cafile lfs/lfs/ca_1.crt --cert lfs/lfs/client_1.crt --key lfs/lfs/client_1.key \
       -t "test/topic2" -m "Hello MQTT" --tls-version tlsv1.2 --insecure



Verify MQTT TLS connectivity between devices and HiveMQ brokers
===================================================================
Verify MQTT TLS connectivity between devices and HiveMQ brokers using both serverless and starter plans. Demonstrate the configuration of one-way and two-way TLS authentication, set up Server Name Indication (SNI), and perform publish/subscribe operations to validate secure communication.

Verification steps for the serverless version
---------------------------------------------

1. Download the broker Root CA certificate from HiveMQ (``isrgrootx1.pem``) and place it in:

   .. code-block:: bash

      examples/spi_wifi/lfs/lfs/isrgrootx1.pem

2. Rebuild the project and flash the image:

   .. code-block:: bash

      cd examples/spi_wifi/
      make

   Flash the image using the firmware flashing command for your chip.

3. Create a broker in the HiveMQ web console under the serverless plan. Set credentials:

   - Username: ``BL_test_0``
   - Password: ``BL123456``
   - Permissions: Publish & Subscribe

   Refer to HiveMQ documentation for detailed steps.

4. Connect the device to the Wi-Fi AP:

   .. code-block:: bash

      AT+CWMODE=1
      AT+CWJAP="8E88",""

5. Configure MQTT user settings (one-way TLS):

   .. code-block:: bash

      AT+MQTTUSERCFG=0,2,"bflb","BL_test_0","BL123456","","","isrgrootx1.pem"

6. Configure MQTT SSL/TLS SNI:

   .. code-block:: bash

      AT+MQTTSNI=0,"8f3ef71da5bf445eb0669e56eecef396.s1.eu.hivemq.cloud"

7. Configure MQTT connection properties:

   .. code-block:: bash

      AT+MQTTCONNCFG=0,120,0,"","",0,0

8. Initiate MQTT connection:

   .. code-block:: bash

      AT+MQTTCONN=0,"8f3ef71da5bf445eb0669e56eecef396.s1.eu.hivemq.cloud",8883,0

9. Subscribe to all topics:

   .. code-block:: bash

      AT+MQTTSUB=0,"#",0

10. Publish a topic message:

    .. code-block:: bash

       AT+MQTTPUB=0,"/test/bflb","What's up guys",0,0

Verification steps for the starter version
------------------------------------------

1. Download the broker Root CA certificate (``isrgrootx1.pem``) and place it in:

   .. code-block:: bash

      examples/spi_wifi/lfs/lfs/isrgrootx1.pem

2. Rebuild the project and flash the image:

   .. code-block:: bash

      cd examples/spi_wifi/
      make

   Flash the image using the firmware flashing command for your chip.

3. Create a broker in the HiveMQ web console under the starter plan. Set credentials:

   - Username: ``BL_test_0``
   - Password: ``BL123456``
   - Permissions: Publish & Subscribe
   - Add ``ca_1.crt`` as the Root CA for verifying device certificates

   Refer to HiveMQ documentation for detailed steps.

4. Connect the device to the Wi-Fi AP:

   .. code-block:: bash

      AT+CWMODE=1
      AT+CWJAP="8E88",""

5. Configure MQTT user settings (two-way TLS):

   .. code-block:: bash

      AT+MQTTUSERCFG=0,4,"changeme","BL_test_0","BL123456","client_1.crt","client_1.key","isrgrootx1.pem"

6. Configure MQTT SSL/TLS SNI:

   .. code-block:: bash

      AT+MQTTSNI=0,"indigosquash-2lk628.a02.usw2.aws.hivemq.cloud"

7. Configure MQTT connection properties:

   .. code-block:: bash

      AT+MQTTCONNCFG=0,120,0,"","",0,0

8. Initiate MQTT connection:

   .. code-block:: bash

      AT+MQTTCONN=0,"indigosquash-2lk628.a02.usw2.aws.hivemq.cloud",8883,0

9. Subscribe to all topics:

   .. code-block:: bash

      AT+MQTTSUB=0,"#",0

10. Publish a topic message:

    .. code-block:: bash

       AT+MQTTPUB=0,"/test/bflb","What's up guys",0,0
