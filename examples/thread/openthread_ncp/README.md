# OpenThread NCP/RCP example

OpenThread porting is over `components/wireless/lmac154` module. 

This is an example for OpenThread NCP and RCP example.

  - Communicate with HOST side with UART baud-rate 460800 setting
  - Demonstrate OpenThread NCP/RCP with `pyspinel`, `Wireshark` and `ot-br-posix`.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618
    ```shell
    make CHIP=bl616 BOARD=bl616dk
    ```

    - Build for OpenThread RCP example
        ```shell
        make CHIP=bl616 BOARD=bl616dk OT_NCP=0
        ```
    
    - Build for OpenThread NCP example with FTD
        ```shell
        make CHIP=bl616 BOARD=bl616dk OT_NCP=1 OT_FTD=1
        ```
    
    - Build for OpenThread NCP example with MTD
        ```shell
        make CHIP=bl616 BOARD=bl616dk OT_NCP=1 OT_FTD=0
        ```

- OpenThread feature options

  Please free modify OpenThread feature options in `openthread-core-proj-config.h`.

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Test with `pyspinel`

Pyspinel, available on the [pyspinel GitHub repository](https://github.com/openthread/pyspinel), is a Python CLI for the [Spinel](https://openthread.io/platforms/co-processor#spinel_protocol) protocol, used to configure and manage [OpenThread NCPs or RCPs](https://openthread.io/platforms/co-processor). 

> Please refer to [official guide](https://openthread.google.cn/guides/pyspinel) for more detail. 
> **Note:** Pyspinel is only supported for NCP and RCP builds of OpenThread on Linux or MacOS.

- Setup environment and  clone  `pyspinel` 

  ```shell
  # Install python modules
  pip3 install --user pyserial ipaddress
  
  # Clone pyspinel
  git clone https://github.com/openthread/pyspinel.git
  ```

- Run with `pyspinel` 

  ```shell
  # Run with UART device /dev/ttyACM0 and baud-rate 460800
  python3 spinel-cli.py -b 460800 -u /dev/ttyACM0
  
  # Check the device version
  spinel-cli > version
  OpenThread/7e32165be; BL616; Jan 13 2024 00:57:10
  Done
  
  # Check the device mode
  spinel-cli > mode
  rdn
  Done
  ```
  More operations are available with NCP as below.
  
- Attach to a Thread network.

  - Setup a Thread network with a OpenThread command line example

    ```shell
    # Setup active dataset for Thread network as below:
    # Channel: 15
    # PAN ID: 0x6677
    # Network Key: 00112233445566778899aabbccddeeff
    otc dataset set active 0e080000000000000000000300000f35060004001fffe00208dead00beef00cafe0708fddead00beef0000051000112233445566778899aabbccddeeff030a4f70656e54687265616404109ce7b658f9eb6d53275154280792d3df0c0402a0f7f801026677
    
    # Enable interface and start Thread network.
    otc ifconfig up
    otc thread start
    ```

    After this OpenThread command line becomes leader, then operates under `pyspinel` command line

  - Attach NCP to Thread network PAN ID: 0x6677 and ping Leader

    ```shell
    # Configure NCP Thread network information
    spinel-cli > panid 0x6677
    Done
    spinel-cli > networkkey 00112233445566778899aabbccddeeff
    Done
    spinel-cli > channel 15
    Done
    
    # Enable interface and attch to Thread network PAN ID: 0x6677
    spinel-cli > ifconfig up
    Done
    spinel-cli > thread start
    Done
    
    # After a while, NCP attaches to Thread network.
    spinel-cli > state
    child
    Done
    
    # Show IP addresses
    spinel-cli > ipaddr
    fdde:ad00:beef::ff:fe00:801
    fdde:ad00:beef:0:341c:558f:b78d:71f9
    fe80::7062:2b0c:b541:e9be
    Done
    
    # Ping IP address of Leader
    spinel-cli > ping fdde:ad00:beef:0:0:ff:fe00:fc00
    56 bytes from fdde:ad00:beef::ff:fe00:fc00: icmp_seq=30577 hlim=64 time=83ms
    ```


## Configure Wireshark as sniffer with RCP

Wireshark also uses `pyspinel` to communicate with OpenThread RCP, please refer to two pages as below to setup

- [Install and Configure Wireshark](https://openthread.google.cn/guides/pyspinel/wireshark)
- [Packet Sniffing using Extcap](https://openthread.google.cn/guides/pyspinel/sniffer-extcap)

## OpenThread Border Router with RCP

Please refer to these two official guides as below to setup OpenThread Border Router

- [OpenThread Border Router Build and Configuration](https://openthread.google.cn/guides/border-router/build)
- [OpenThread Border Router Web GUI](https://openthread.google.cn/guides/border-router/web-gui)

Since `ot-br-posix` uses UART baud-rate 115200 by default, it needs change `/etc/default/otbr-agent` configuration to use UART baud-rate 460800
```shell
# Default settings for otbr-agent. This file is sourced by systemd

# Options to pass to otbr-agent
OTBR_AGENT_OPTS="-I wpan0 -B wlp4s0 spinel+hdlc+uart:///dev/ttyACM0?uart-baudrate=460800 trel://wlp4s0"
OTBR_NO_AUTO_ATTACH=0
```
