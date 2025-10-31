WIFI TCP
====================

This section mainly introduces the use of BL616/BL618 wifi and how to make TCP requests.

Preparation before Development
-----------------------------
Same as :ref:`wifi_http`

Hardware Connection
-----------------------------

none

Software Implementation
-----------------------------

For more detailed code, please refer to examples/wifi/sta/wifi_tcp

Compile and Program
-----------------------------

- Compilation, the Makefile file has already configured other parameters ( CHIP and BOARD ), and does not need to be filled in by the user.

.. code-block:: bash

    $ cd examples/wifi/sta/wifi_tcp
    $ make

- Program

Reference :ref:`get_started`

Experimental phenomena
-----------------------------

- Open the serial port terminal software, press the reset key, and output the following log. Press the Enter key to display the words bouffalolab />, similar to the Linux terminal.

    .. figure:: img/wifi_tcp1.png
       :align: center
       :scale: 50%

- Enter wifi_sta_connect ssid pwd to connect to the network, where ssid is the connected ap name and pwd is the connected ap password

    .. figure:: img/wifi_tcp2.png
       :align: center
       :scale: 50%

- After the wifi connection is successful, the assigned IP address will be printed out and CONNECTED will be prompted.

    .. figure:: img/wifi_tcp3.png
       :align: center
       :scale: 45%

If you want to test TCP TX, please use the following instructions for testing.

- The TCP server needs to be initialized on the PC side of the same LAN using the iperf instruction

    .. figure:: img/wifi_tcp4.png
       :align: center
       :scale: 50%

- Enter wifi_tcp_test Server IP port to connect to the tcp server

    .. figure:: img/wifi_tcp5.png
       :align: center
       :scale: 50%

If you want to test TCP RX, please use the following instructions for testing.

- Enter wifi_tcp_echo_test port to start a TCP server

    .. figure:: img/wifi_tcp6.png
       :align: center
       :scale: 50%

- The client can connect to the TCP server of the module through the iperf command

    .. figure:: img/wifi_tcp7.png
       :align: center
       :scale: 50%
