#!/bin/bash
wifi_cards_total=$(iwconfig 2>&1 | grep "IEEE 802.11" | grep -v "mon" | cut -d' ' -f1)
wifi_cards=""
for wifi_card in $wifi_cards_total; do
    if udevadm info -e | grep -A4 $wifi_card | grep ID_BUS | grep -q usb; then
        wifi_cards="$wifi_cards $wifi_card"
    fi
done

for card in $wifi_cards;
do
    mac="$mac $(cat /sys/class/net/$card/address)"
done    

for eif in $(ifconfig -a | grep Ethernet | cut -f1 -d' '); do
    if ! ifconfig -a $eif | egrep -q "([0-9]{1,3}\.){3}[0-9]{1,3}"; then
        dhclient $eif &
    fi
done
ulimit -c 1000

./rtt $mac 0.0.0.0
read -p "press enter to exit"
