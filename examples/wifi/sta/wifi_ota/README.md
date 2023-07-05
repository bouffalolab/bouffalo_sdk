# wifi6

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Program Flash

```
make flash COMX=xxx ## xxx is your com name
```

## How use wifi OTA test


```
              ))                                 ____________
    \|/      )))         \|/  \|/               |            |
     |______           ___|____|___             |  server pc |
    |       |         |            |            |____________|
    | BL616 |         |   Router   |#<-------->#/            /#
    |_______|         |____________|           /____________/#
   192.168.1.3         192.168.1.1              192.168.1.2
                      SSID: BL_TEST
                    Password:12345678
```

### Start simple OTA server

On Linux server pc run netcat command, listen localhost 3365 port, if client connected send OTA FW to BL616

```bash
$ cd build/build_out
$ cat wifi_ota_bl616.bin.ota|nc -nvl 3365
```

Or run an http server using the python command in the directory where the OTA bin file is located.
like this:

```bash
$ cd build/build_out
$ python -m http.server 3365
```

### Connect wifi

If you use automatic networking, you need `AUTO_CONNECT_WIFI` to `1` to enable automatic connection, and you need to configure your wifi ssid and password correctly in main.c.
When this program is downloaded to the chip, it will automatically search for the network after starting by pressing the reset button. If the networking is successful, it will automatically send OTA command for OTA.

```c
/* config your wifi ssid and password */
static const uint8_t wifi_sta_connet[] = "wifi_sta_connect BL_TEST 12345678\r";
/* config your OTA server and port */
static const uint8_t wifi_ota_test[] = "wifi_ota_test 192.168.1.2 3365\r";

```

If automatic networking is not configured, then you will need to manually enter networking commands and OTA commands into the BL616's serial port.
On BL616 board, using <wifi_sta_connect> command connect your WiFi router, and start OTA command

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_ota_test 192.168.1.2 3365

```

if connect success, Linux server pc have send data to BL616.


## OTA use partition

This OTA case use default partition `bsp/board/bl616dk/config/partition_cfg_4M.toml`;
The addresses in the partition table are divided into Address0(A zone) and Address1(B zone), and when the firmware is OTA, it is also the address that the upgraded firmware should be written from the partition table, and the upgrade of the new firmware Boot2 is also based on the active partition in the partition table to get the latest application firmware to start running.

## partition Flash layout

In general IOT application scenarios, the Flash layout is as follows:

```c
/*
************************
 ____________________________   0x0
|      BOOT2 bootheader      |
|----------------------------|
|          Padding           |
|----------------------------|  0x2000
|           BOOT2            |
|----------------------------|
|          Padding           |
|----------------------------|  0xE000
|         Partition0         |
|----------------------------|  0xF000
|         Partition1         |
|----------------------------|  0x10000
|       FW A bootheader      |
|----------------------------|  0x11000
|         FW A zone          |
|----------------------------|
|          Padding           |
|----------------------------|  0x210000
|       FW B bootheader      |
|----------------------------|  0x211000
|         FW B zone          |
|----------------------------|
|          Padding           |
|----------------------------|  0x378000
|         Media zone         |
|----------------------------|
|          Padding           |
|----------------------------|
|           .....            |
|----------------------------|
|          others            |
|____________________________|

*/
```

1. Flash's starting 4K position is Bootheader of Boot2, which is used to analyze and run Boot2 for the first level Bootrom of the chip.
2. Flash 0x2000-0xDFFF is Boot2, and the main functions of Boot2 are as follows:

    2.1 According to the partition table, start the latest firmware.

    2.2 The latest firmware is abnormal, roll back to the old firmware to run.

    2.3 Support compressing firmware, you can decompress the compressed mirror and write it to Flash, and then start running.

    2.4 Support IAP function, you can upgrade firmware through UART/USB.

3. 0xE000-0xEFFF of Flash is partition table 0, 0xF000-0xFFFF of Flash is partition table 1.
4. 0x10000-0x10FFF of Flash is the BootHeader of the application firmware A, which is used to analyze and run the application firmware for the second-level Boot:Boot2 of the chip.
5. 0x110000-0x20FFFF of Flash is the entity of area B of the application firmware, which is the content of the Bin file output after compiling the SDK
6. 0x210000-0x210FFF of Flash is the BootHeader of the application firmware B, which is used to analyze and run the application firmware for the second-level Boot:Boot2 of the chip.
7. 0x210FFF-0x377FFF of Flash is the entity of area B of the application firmware, which is the content of the Bin file output after compiling the SDK.
8. Flash's 0x378000-0x3E8FFF is a Media partition, which stores files used by the application.

## This case OTA flow

1. Get the partition table，check partition;
2. If the currently active partition is partition FW A, then Erase FW B zone.
3. Connect OTA server.
4. Check the OTA Header sent by the server; if check is successful, continue to get the OTA data.
5. Calculates whether the hash value of the OTA data is correct.
6. If the hash value is calculated incorrectly, the OTA process is exited and the partition table and currently active partition flags are not updated. If hash is correct, then partition table and current active partition flags are updated, and the `age++` indicates the number of OTA updates.
7. reboot system.
8. If the OTA case run crashes, there will be Watch Dog for system reset; Watch Dog starts running from the system and will reset after 30s without OTA success.

## Dump OTA log

The changes in the partition table before and after OTA are shown below:

OTA before:

```bash
Active PT:0,Age 0
======= PtTable_Config @0x62ff7a68=======
magicCode 0x54504642; version 0x0000; entryCnt 8; age 0; crc32 0x39B2C2F4
 idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age
[00]   16     0        0          Boot2   0x00000000   0x00000000   0x0000e000   0x00000000   0
[01]   00     0        0             FW   0x00010000   0x00210000   0x00200000   0x00168000   0
[02]   10     0        0            mfg   0x00210000   0x00000000   0x00168000   0x00000000   0
[03]   02     0        0          media   0x00378000   0x00000000   0x00071000   0x00000000   0
[04]   03     0        0            PSM   0x003e9000   0x00000000   0x00008000   0x00000000   0
[05]   04     0        0            KEY   0x003f1000   0x00000000   0x00002000   0x00000000   0
[06]   05     0        0           DATA   0x003f3000   0x00000000   0x00005000   0x00000000   0
[07]   06     0        0        factory   0x003f8000   0x00000000   0x00008000   0x00000000   0
```

OTA after:

```bash
Active PT:1,Age 1
======= PtTable_Config @0x62ff7cbc=======
magicCode 0x54504642; version 0x0000; entryCnt 8; age 1; crc32 0x810EA591
 idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age
[00]   16     0        0          Boot2   0x00000000   0x00000000   0x0000e000   0x00000000   0
[01]   00     0        1             FW   0x00010000   0x00210000   0x00200000   0x00168000   1
[02]   10     0        0            mfg   0x00210000   0x00000000   0x00168000   0x00000000   0
[03]   02     0        0          media   0x00378000   0x00000000   0x00071000   0x00000000   0
[04]   03     0        0            PSM   0x003e9000   0x00000000   0x00008000   0x00000000   0
[05]   04     0        0            KEY   0x003f1000   0x00000000   0x00002000   0x00000000   0
[06]   05     0        0           DATA   0x003f3000   0x00000000   0x00005000   0x00000000   0
[07]   06     0        0        factory   0x003f8000   0x00000000   0x00008000   0x00000000   0
```