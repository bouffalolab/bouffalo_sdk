##  Copyright (C) Bouffalo Lab 2016-2023
##  SPDX-License-Identifier: Apache-2.0

if [ $EVENT = "IP_UPDATE" ]; then
    echo IP $BF_IP > /dev/kmsg
    ifconfig bleth0 $BF_IP netmask $BF_MASK
    echo GW $BF_GW > /dev/kmsg
    route add default gw $BF_GW
    echo "nameserver $BF_DNS1" > /etc/resolv.conf
    if [ "$BF_DNS2" != "0.0.0.0" ]; then
        echo "nameserver $BF_DNS2" >> /etc/resolv.conf
    fi

elif [ $EVENT = "DISCONNECT" ]; then
    echo "Removing address" > /dev/kmsg
    ifconfig bleth0 0.0.0.0
elif [ $EVENT = "CARD_INSERTED" ]; then
    # card inserted
    echo "Card inserted" > /dev/kmsg

elif [ $EVENT = "CARD_DEAD" ]; then
    # card dead
    # TODO: reset card here
    echo "Card dead!" > /dev/kmsg

fi
