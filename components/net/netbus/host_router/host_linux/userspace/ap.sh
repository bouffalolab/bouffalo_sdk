##  Copyright (C) Bouffalo Lab 2016-2023
##  SPDX-License-Identifier: Apache-2.0

if [ $EVENT = "CARD_INSERTED" ]; then
    # card inserted
    echo "Card inserted" > dev/kmsg
    blctl start_ap testme
    ifconfig bleth0 192.168.169.1 netmask 255.255.255.0

elif [ $EVENT = "CARD_DEAD" ]; then
    # card dead
    # TODO: reset card here
    echo "Card dead!" > /dev/kmsg

fi
