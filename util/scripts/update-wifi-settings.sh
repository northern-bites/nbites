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

WIFI_CONFIG=wireless_config
FOLDER=nao_files

chmod +x setup-wifi.sh

if [[ $DOALL -eq 1 ]]; then
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@wash.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@river.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@jayne.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@simon.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@inara.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@kaylee.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@vera.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@mal.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@zoe.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@ringo.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@beyonce.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@ozzy.local:
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@avril.local:
else
    scp -r $FOLDER $WIFI_CONFIG setup-wifi.sh $ROBOT_UNAME@$ROBOT:
fi

echo "Now run setup-wifi.sh ON THE ROBOT(S)."
