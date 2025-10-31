# OpenThread command line example
OpenThread porting is over `components/wireless/lmac154` module. 

This is an example for OpenThread command line example.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile
- BL616/BL618
    - Build for OpenThread FTD example
        ```shell
        make CHIP=bl616 BOARD=bl616dk OT_FTD=1
        ```
    
    - Build for OpenThread MTD example
        ```shell
        make CHIP=bl616 BOARD=bl616dk OT_FTD=0
        ```

- OpenThread feature options

  Please free modify OpenThread feature options in `openthread-core-proj-config.h`.

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## How to run

By default, command line example has Bouffalo SDK command set and uses `otc` to execute OpenThread Command.

Prepare two boards with command line example programmed.

### steps

- Create an OpenThread with on board as below

  ```shell
  # Initalize active dataset with commands otc dataset init new and otc dataset commit active
  > bouffalolab />otc dataset init new
  dataset init new
  Done
  > bouffalolab />otc dataset commit active
  dataset commit active
  Done
  
  # Get active dataset as hex string format with command otc dataset active -x
  > bouffalolab />otc dataset active -x
  dataset active -x
  0e080000000000000000000300000f35060004001fffe00208dead00beef00cafe0708fddead00beef0000051000112233445566778899aabbccddeeff030a4f70656e54687265616404109ce7b658f9eb6d53275154280792d3df0c0402a0f7f801026677
  Done
  
  # Show active dataset with commands otc dataset init active and otc dataset
  > bouffalolab />otc dataset init active 
  dataset init active
  Done
  > bouffalolab />otc dataset
  dataset
  Active Timestamp: 0
  Channel: 15
  Channel Mask: 0x07fff800
  Ext PAN ID: dead00beef00cafe
  Mesh Local Prefix: fdde:ad00:beef:0::/64
  Network Key: 00112233445566778899aabbccddeeff
  Network Name: OpenThread
  PAN ID: 0x6677
  PSKc: 9ce7b658f9eb6d53275154280792d3df
  Security Policy: 672 onrc 0
  Done
  
  # Start Thread network with commands otc ifconfig up and otc thread start
  > bouffalolab />otc ifconfig up
  ifconfig up
  Done
  > bouffalolab />otc thread start
  thread start
  Done
  
  # Check the Thread state with command otc state
  # May take a while, please try more to check Thread network created and becomes to leader
  > bouffalolab />otc state
  state
  leader
  Done
  
  # Show IP addresses with command otc ipaddr
  bouffalolab />otc ipaddr
  ipaddr
  fdde:ad00:beef:0:0:ff:fe00:fc00
  fdde:ad00:beef:0:0:ff:fe00:8400
  fdde:ad00:beef:0:341c:558f:b78d:71f9
  fe80:0:0:0:7062:2b0c:b541:e9be
  Done
  ```

- Configure another board to attach the Thread network above

  ```shell
  # Configure active dataset with command otc dataset set active 
  bouffalolab />otc dataset set active 0e080000000000000000000300000f35060004001fffe00208dead00beef00cafe0708fddead00beef0000051000112233445566778899aabbccddeeff030a4f70656e54687265616404109ce7b658f9eb6d53275154280792d3df0c0402a0f7f801026677
  dataset set active 0e080000000000000000000300000f35060004001fffe00208dead00beef00cafe0708fddead00beef0000051000112233445566778899aabbccddeeff030a4f70656e54687265616404109ce7b658f9eb6d53275154280792d3df0c0402a0f7f801026677
  Done
  
  # Show active dataset with commands otc dataset init active and otc dataset
  > bouffalolab />otc dataset init active 
  dataset init active
  Done
  > bouffalolab />otc dataset
  dataset
  Active Timestamp: 0
  Channel: 15
  Channel Mask: 0x07fff800
  Ext PAN ID: dead00beef00cafe
  Mesh Local Prefix: fdde:ad00:beef:0::/64
  Network Key: 00112233445566778899aabbccddeeff
  Network Name: OpenThread
  PAN ID: 0x6677
  PSKc: 9ce7b658f9eb6d53275154280792d3df
  Security Policy: 672 onrc 0
  Done
  
  # Start Thread network with commands otc ifconfig up and otc thread start
  > bouffalolab />otc ifconfig up
  ifconfig up
  Done
  > bouffalolab />otc thread start
  thread start
  Done
  
  # Check the Thread state with command otc state
  # After a while, this board attaches and becomes router role.
  bouffalolab />otc state
  state
  router
  Done
  
  # Ping IP address of the lead with command otc ping
  bouffalolab />otc ping fdde:ad00:beef:0:0:ff:fe00:fc00
  ping fdde:ad00:beef:0:0:ff:fe00:fc00
  bouffalolab />16 bytes from fdde:ad00:beef:0:0:ff:fe00:fc00: icmp_seq=1 hlim=64 time=14ms
  1 packets transmitted, 1 packets received. Packet loss = 0.0%. Round-trip min/avg/max = 14/14.0/14 ms.
  Done
  ```
  

### Used command list

- otc dataset init new, only creates an new dataset 
- otc dataset init active, loads active dataset from Thread stack, if it has one
- otc dataset commit active, commits created dataset to Thread stack as active dataset
- otc dataset, displays created dataset or loaded dataset as human readable format
- otc dataset active -x, displays active dataset of Thread stack as hex string format, if it has one
- otc dataset set active, sets hex string to Thread stack as active dataset
- otc ifconfig up, only enable Thread interface, not start Thread stack
- otc thread start, start Thread stack, which will try to attach to Thread network specified by active dataset.
- otc state, displays the state of Thread state
  - Disable, Thread stack is not started
  - Detached, Thread stack is started and trying to attached a Thread network
  - Leader, attached and play as a leader role, there is only one leader in a Thread network partition
  - Router, attached and play as a router role
  - Child, attached and play as a child role
