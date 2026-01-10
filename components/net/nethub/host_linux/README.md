# nethub_host

This project implements both kernel and userspace components:

- **Kernel part**: Compiles to generate a .ko file, virtualizing multiple interfaces (SDIO, USB, SPI) into ethernet, netlink, and tty channels
- **Userspace part**:
  - AT command application based on tty channel, mainly implementing device connection, scanning, query, and other functions
  - netlink_app demonstrates user private data transmission through netlink_client

## 1. Software Architecture and Features

![image](docs/images/image-nethubarch.png)

- **Host Driver**: Virtualizes SDIO, USB, SPI and other interfaces into ethernet and tty channels
- **Device Control**: Connect to specified WiFi network, query WiFi status, scan available WiFi, OTA firmware upgrade

## 2. Quick Start

- Compile hd_sdio.ko and application firmware

  ```bash
  ./build.sh build
  ```

- Load hd_sdio.ko

  ```bash
  sudo ./build.sh load
  ```

- Connect to network and configure static IP (this script is for testing only, please refer to the detailed instructions below)

  ```bash
  ./userspace/connect_and_setip [password]
  ```

- Control functions supported through AT channel

  - WiFi Connection

    ```bash
    # Connect to WiFi
    ./userspace/easyat/src/easyat connect MyWiFi password123

    # Connect to open network
    ./userspace/easyat/src/easyat connect OpenWiFi
    ```

  - WiFi Disconnection

    ```bash
    ./userspace/easyat/src/easyat disconnect_ap
    ```

  - Query Status

    ```bash
    ./userspace/easyat/src/easyat get_link_status
    ```

  - Scan Networks

    ```bash
    ./userspace/easyat/src/easyat wifi_scan
    ```

  - OTA Firmware Upgrade

    ```bash
    ./userspace/easyat/src/easyat ota firmware.bin.ota
    ```

- Send data to device through netlink channel

  ```bash
  sudo ./userspace/netlink_ctrl/netlink_app
  help
  send user 12345678
  ```

  ![image](docs/images/image-netlinkapp.png)

## 3. Notes and FAQ

- Q1: What interfaces will appear on the host after hardware connection and loading the host driver?

  After loading, at least 2 interfaces will appear on the host:

  - **hd_eth0** [Required] - Network card interface, equivalent to STA to ethernet conversion
  - **/dev/ttyhd0** [Required] - TTY control channel, commands such as connection and disconnection are sent and received through this channel, protocol is standard AT commands
  - ~~**/dev/ttyhd1**~~ ~~[Deprecated] - Original TTY data channel (replaced by netlink method)~~
  - Provides **netlink** driver for userspace, facilitating private data transmission and reception

- Q2: Which side runs the DHCP client, host or device?

  Runs on the **device** side. The host obtains the IP address through a query mechanism. After connecting to WiFi, a GOTIP event will be received, then the host updates the IP address through queries. Refer to the `connect_and_setip` script implementation in the host.

- Q3: Can users customize the packet filter on the device side? What are the default rules?

  **Yes, customizable**. Users can complete filter customization by re-implementing the `eth_input_filter` function on the device side.

  **Default filtering rules:**

  - **ARP packets**: Bidirectional transmission
  - **DHCPv4 and ICMPv4 packets**: By default handled by device side, will not be delivered to host side
  - **Port filtering**: Users can complete address space filtering by defining `CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN` and `CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX`
  - **Other packets**: By default handled by host side

- Q4: What are the relationships and differences between nethub, hd_sdio, easyat, and connect_and_setip?

  - **nethub**: Firmware software running on the device side, implementing WiFi Station to ethernet protocol conversion, supporting packet filtering and forwarding rule management
  - **hd_sdio**: Kernel driver module (ko file), responsible for SDIO communication between host and device
  - **easyat**: AT command encapsulation library for user device control, providing convenient device operation interfaces. Developers can directly send and receive AT protocols bypassing this library
  - **connect_and_setip**: Network configuration example tool, implementing the complete process of connecting to a specified AP, obtaining IP information, and setting static IP
  - **Architecture relationship**:

    ```txt
    User Application Layer
        ↓ connect_and_setip (example tool)
        ↓ easyat (AT command library)
    Control Channel Layer (/dev/ttyhd0)
        ↓ hd_sdio (kernel driver)
    Physical Layer (SDIO interface)
        ↓ nethub (device firmware)
    ```
  - **Data flow**:

    - `nethub` is device-side firmware, handling WiFi protocol conversion and data forwarding
    - `hd_sdio` is the underlying communication driver, responsible for physical connection between host and device
    - `easyat` is userspace control library, encapsulating AT command operations
    - `connect_and_setip` is application layer example, showing complete usage flow

- Q5: What are the special considerations for host development?

  - **Network configuration requirements**:

    Since the current solution designs the DHCP client on the device side, the host side needs to follow the following configuration requirements:

    - **Must disable DHCP service on hd_eth0**: Use static IP configuration method
    - **Static IP configuration steps**:

      - Use easyat command to query link status: `./userspace/easyat/src/easyat get_link_status`
      - Configure static IP address on host side based on query results (using the same IP as device side)
      - Ensure host side and device side use the same IP address
    - **Configuration example**:

      ```txt
      # 1. Query link status to obtain IP information
      ./userspace/easyat/src/easyat get_link_status

      # 2. Configure static IP based on query results (example)
      sudo ip addr add 192.168.1.100/24 dev hd_eth0
      sudo ip link set hd_eth0 up
      ```

      ⚠️ **Important reminder**: If DHCP service is enabled on the host side, it will conflict with the DHCP client on the device side, causing network connection abnormalities.

## 4. Features to be Implemented

- [ ] Hot-plug SDIO functionality (theoretically supported, pending stress testing confirmation)
- [ ] hd_sdio.ko dynamic load/unload functionality (theoretically supported, pending stress testing confirmation)
- [ ] Performance optimization and stability improvement
- [ ] [Optional] AT control channel can be disabled (users can implement control through userspace netlink module)
- [ ] [Optional] AP control channel also virtualizes **hd_eth1** to host
- [ ] Add USB and SPI interface support
