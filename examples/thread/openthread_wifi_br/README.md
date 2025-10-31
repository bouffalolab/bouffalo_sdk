# OpenThread Border Router Over Wi-Fi Infrastructure network

Please refer to document [Introduction](./Introduction.md) for solution introduction.

**Limitations**

- This solution may have not enough capability to communicate with SSED.
- Higher DTIM in AP may lead higher latency to communicate between Thread network and Wi-Fi network.

OTBR with Ethernet infrastructure does't have above two limitations.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## How to run

### Hardwares

- One board, named OTBR, which flashes this example.

- One board, named OTCLI, which flashes openthread command line example.

- Test PC machine

  - Description on operations and commands are based on Linux-base operating system.
  - `Windows`, `Macos` and etc may work with this example.  

- An Wi-Fi AP router

  - Must have IPv4 DHCP function

  - IPv6 DHCP is not a must

    Since current OTBR implementation only supports stateless IPv6 address. Some legacy Wi-Fi routers in the field may be not good compatible with this IPv6 address allocation, so please disable IPv6 function in these kind of routers to try this example.

### Steps

- Open OTBR UART, power up or reset the board, then type following command to connect OTBR to Wi-Fi AP.

    ```shell
    # connect to Wi-Fi AP with command wifi_sta_connect
    bouffalolab />wifi_sta_connect <SSID> <PASSWORD>  
    ```
    > OTBR will acquire IPv4 address at its Wi-Fi interface after it connects to Wi-Fi AP successfully;  before that, RF is almost not available for M154/Thread.
    >
    > After IPv4 address assigned at Wi-Fi interface. OTBR starts/attaches Thread network, and plays Thread Border Routing forwarding.

- After OTBR gets IPv4 address from Wi-Fi AP, check Thread state becomes Leader, this may take minute, try more. Then, go to next step.

  ```shell
  # Get Thread state with command otc state.
  # Try more times in minute to check Thread state becomes to leader
  bouffalolab />otc state
  state
  leader
  Done
  ```

- Get dataset of Thread network from OTBR

  ```shell
  # Get active dataset as hex string format with command otc dataset active -x
  bouffalolab />otc dataset active -x
  dataset active -x
  0e080000000000010000000300001735060004001fffe002086e0f33c1aa1076d30708fd563c8a2072356f05101f5be8fb42b487278a27619c7a0f2470030f4f70656e5468726561642d3765363501027e6504107ccd496e4c41be0651547fe30355ff330c0402a0f7f8
  Done
  ```
  > hex string of dataset has all information of Thread network, include channel, panid network, etc. Copy that for step to attach OTCLI to OTBR network.
  >


- Get IPv6 OMR prefix information from OTBR

  - OMR prefix belongs Thread network domain
  
  - IPv6 addresses using OMR prefix which are allocated by OTBR to Thread device are available to be reached from infrastructure network.
  
  ```shell
  # Get OMR prefix with command otc br omrprefix
  bouffalolab />otc br omrprefix
  br omrprefix
  Local: fd41:20be:97bf:1::/64
  Favored: fd41:20be:97bf:1::/64 prf:low
  Done
  ```
  
- Get IPv6 Onlink prefix information from OTBR

    - Onlink prefix belongs Infrastructure network domain
    
    - IPv6 addresses using Onlink prefix are available to be reached from Thread network.
    
    ```shell
    # Get onlink prefix with command otc br onlinkprefix
    bouffalolab />otc br onlinkprefix
    br onlinkprefix
    Local: fd19:5f1c:33c6:41d5::/64
    Favored: fd19:5f1c:33c6:41d5::/64
    Done
    ```
    
- Open OTCLI UART, input the following commands to make OTCLI attach to OTBR Thread network.

    ```shell
    # Configure active dataset with command otc dataset set active 
    bouffalolab />otc dataset set active <Thread network credential hex string in last step>
    Done
    
    # Start Thread network with commands otc ifconfig up and otc thread start
    bouffalolab />otc ifconfig up
    ifconfig up
    Done
    
    bouffalolab />otc thread start
    thread start
    Done
    
    # Get Thread state with command otc state.
    # Try more times in seconds to check Thread state becomes to child/router
    bouffalolab />otc state
    state
    router
    Done
    
    # Get IP addresses with command otc ipaddr
    bouffalolab />otc ipaddr
    ipaddr
    fdef:ca56:3b51:5bf4:0:ff:fe00:6000
    fd41:20be:97bf:1:c017:4b66:f036:eb1a
    fdef:ca56:3b51:5bf4:a79:f4bc:cd40:d915
    fe80:0:0:0:bc2a:4222:ded0:7032
    Done
    ```

- Connectivity test between Thread device and PC station

  - PC station reaches Thread device

    IPv6 address fd41:20be:97bf:1:c017:4b66:f036:eb1a has prefix same as OMR prefix shown at OTBR as above, which is available to be access by a PC station in same AP router. `ping`  command from that PC station can reach this IPv6 address.

  - Thread device reaches PC station

    - Get IPv6 address with onlink prefix on target Host machine online linux shell terminal
    
      ```shell
      weiyin@weiyin-PC:~/workspace/tools/scripts$ ip a
      1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
          link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
          inet 127.0.0.1/8 scope host lo
             valid_lft forever preferred_lft forever
          inet6 ::1/128 scope host 
             valid_lft forever preferred_lft forever
      2: wlp4s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
          link/ether d8:12:65:1c:2b:f1 brd ff:ff:ff:ff:ff:ff
          inet 192.168.1.16/24 brd 192.168.1.255 scope global dynamic noprefixroute wlp4s0
             valid_lft 86393sec preferred_lft 86393sec
          inet6 fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9/64 scope global dynamic noprefixroute 
             valid_lft 1791sec preferred_lft 1791sec
          inet6 fe80::2a6f:2e93:1783:ea3e/64 scope link noprefixroute 
             valid_lft forever preferred_lft forever
      ```
      
      IPv6 address fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9 at interface wlp4s0 has prefx same as online prefix showd above.
      
    - Ping target host machine from OTCLI
    
      ```shell
      bouffalolab />otc ping fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9
      ping fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9
      bouffalolab />16 bytes from fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9: icmp_seq=1 hlim=63 time=813ms
      1 packets transmitted, 1 packets received. Packet loss = 0.0%. Round-trip min/avg/max = 813/813.0/813 ms.
      Done
      ```
    
  
- UDP message test


    - Send message from PC to OTCLI
    
      Execute commands to open socket and bind port
    
      ```she
      bouffalolab />otc udp open
      udp open
      Done
      > bouffalolab />otc udp bind -u 0::0 12345
      udp bind -u 0::0 12345
      Done
      ```
    
      Connect to OTCLI UDP port at PC station and send message `OTCLI` to OTCLI
    
      ```shell
      nc -u fd38:be15:d904:1:111e:b964:7a76:b285 12345
      OTCLI
      ```
      
      Message received at Thread OTCLI
      
      ```shell
      bouffalolab />6 bytes from                  fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9 12345 OTCLI
      ```
      
      close UDP socket at Thread OTCLI
      
      ```shell
      bouffalolab />otc udp close
      udp close
      Done
      ```
      
    -  Send message from OTCLI to PC
    
      Execute command to open udp socket and bind port at PC station
    
      ```shell
      nc -lu 0::0 12345
      ```
    
      Send message to PC at OTCLI
    
      ```shell
      bouffalolab />otc udp open
      udp open
      Done
      > bouffalolab />otc udp connect fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9 12345
      udp connect fd19:5f1c:33c6:41d5:f037:6204:503a:a6f9 12345
      Done
      > bouffalolab />otc udp send hello
      udp send hello
      Done
      bouffalolab />otc udp close
      udp close
      Done
      ```
    
      Message received at PC station
    
      ```shell
      nc -lu 0::0 12345
      hello
      ```

