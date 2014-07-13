#!/bin/sh

if [ $# -ne 2 ]; then
    echo "Usage: ./update-wifi-settings <robot-address> <username>"
    exit 1
fi

ROBOT=$1
ROBOT_UNAME=$2
WPA_CONF=wireless_config/wpa_supplicant.conf

scp $WPA_CONF $ROBOT_UNAME@$ROBOT:/etc/wpa_supplicant/wpa_supplicant.conf
