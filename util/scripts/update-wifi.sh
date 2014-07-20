#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: ./update-wifi-settings <robot-address> <username>"
    exit 1
fi

if [[ $1 == "all" ]]; then
    ROBOT_UNAME="nao"
    DOALL=1
else
    ROBOT=$1
    ROBOT_UNAME=$2
fi

WPA_SUPPLICANT_CONFIG=wireless_config/wpa_supplicant.conf

if [[ $DOALL -eq 1 ]]; then
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@wash.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@river.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@jayne.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@simon.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@inara.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@kaylee.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@vera.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@mal.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@zoe.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@ringo.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@beyonce.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@ozzy.local:/etc/wpa_supplicant
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@avril.local:/etc/wpa_supplicant
else
    scp $WPA_SUPPLICANT_CONFIG $ROBOT_UNAME@$ROBOT:/etc/wpa_supplicant
fi

echo "Done!"
