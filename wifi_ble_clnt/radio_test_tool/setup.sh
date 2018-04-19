#!/bin/bash

ERR_NOPRIV=1
ERR_HOSTAPD=2
ERR_IFCONFIG=3
ERR_AIRMON=4
ERR_DNS=5

# make sure user run as root 
if [ $(whoami) != 'root' ]
then 
    echo "Please run as root"
    exit $ERR_NOPRIV
fi

for mon in $(ifconfig | grep mon | cut -d' ' -f 1)
do 
    airmon-ng stop $mon
done

#detecting sheild box
nr_shield_box=$(ls /dev/ttyUSB* 2>&- | wc -w)

# Auto generate hostapd and dnsmasq configuration
#wifi_cards=$(ifconfig | grep wlan | cut -d' ' -f 3)
wifi_cards_total=$(iwconfig 2>&1 | grep "IEEE 802.11" | grep -v "mon" | cut -d' ' -f1)
wifi_cards=""
for wifi_card in $wifi_cards_total; do
    if udevadm info -e | grep -A4 $wifi_card | grep ID_BUS | grep -q usb; then
        wifi_cards="$wifi_cards $wifi_card"
    fi
done
nr_wifi_cards=$(echo $wifi_cards | wc -w)

if [ $nr_shield_box -lt $nr_wifi_cards ]
then
    echo "Warning: only $nr_shield_box shield box connected, but equipped with $nr_wifi_cards WiFi cards"
fi

if [ -z "$wifi_cards" ] 
then 
    echo "no wifi card detected."
    exit -1
fi


i=0
hostapd_conf=""
rm -f /etc/dnsmasq.conf

for card in $wifi_cards
do
    rm -f /etc/hostapd/hostapd$i.conf
    cat >> /etc/hostapd/hostapd$i.conf << EOF
ctrl_interface=/var/run/hostapd
ctrl_interface_group=0
interface=$card
driver=nl80211
ssid=miio_default
hw_mode=g
channel=6
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=3
wpa_passphrase=0x82562647
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
EOF
    hostapd_conf="$hostapd_conf /etc/hostapd/hostapd$i.conf"

    echo "no-resolv" >> /etc/dnsmasq.conf
    echo "interface=$card" >> /etc/dnsmasq.conf
    echo "dhcp-range=192.168.$i.3,192.168.$i.200,12h" >> /etc/dnsmasq.conf
    echo "dhcp-authoritative" >> /etc/dnsmasq.conf
    echo "" >> /etc/dnsmasq.conf
    i=$((i+1))
done

sed -i "s%DAEMON_CONF=.*%DAEMON_CONF=\"$hostapd_conf\"%g" /etc/init.d/hostapd

# take over wlan from network manager
nmcli nm wifi off >/dev/null 2>&1
nmcli n off >/dev/null 2>&1 

rfkill unblock wlan

# start hostapd
echo "start hostapd"
/etc/init.d/hostapd restart
if [ $? -ne 0 ]
then 
    echo "failed to start hostapd"
    exit $ERR_HOSTAPD
fi

systemctl disable NetworkManager
systemctl stop NetworkManager
systemctl disable ModemManager
systemctl stop ModemManager
systemctl disable avahi-daemon
systemctl stop avahi-daemon
# stop dns server
pkill dnsmasq

if pgrep dnsmasq >> /dev/null
then
    echo "Failed to kill dnsmasq, fatal error!!"
    exit $ERR_DNS
fi

echo "" > /var/lib/misc/dnsmasq.leases

# stop rngd daemon
sudo pkill -9 rngd

# config wlan as AP
i=0
for card in $wifi_cards
do 
    ifconfig $card 192.168.$i.1/24 up
    if [ $? -ne 0 ]
    then 
        echo "failed to run ifconfig on $card"
        exit $ERR_IFCONFIG
    fi
    #start airmon-ng 
    airmon-ng start $card 6 
    if [ $? -ne 0 ]
    then 
        echo "failed to run monitor $card"
        exit $ERR_AIRMON
    fi
    i=$((i+1))
done

sleep 5

if [[ $(ifconfig | grep mon | wc -l) -ne $i ]]
then 
    echo "failed to start monitor interface"
    exit $ERR_AIRMON
fi

unlink /tmp/rt_cmd_interface 2>&1 > /dev/null

exit 0
