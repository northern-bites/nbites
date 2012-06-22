#!/bin/sh

#MAL=192.168.16.102
#JAYNE=192.168.16.103
#RIVER=192.168.16.104
#WASH=192.168.16.105
#ZOE=192.168.16.106
MAL=mal.local
JAYNE=jayne.local
RIVER=river.local
WASH=wash.local
ZOE=zoe.local

ROBOT="nao@"
WPASUPCONF=../../wireless/wpa_supplicant.conf


if [ $1 ]; then 
    echo $1 | grep -i "mal" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$ROBOT$MAL"
    fi
    echo $1 | grep -i "jayne" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$ROBOT$JAYNE"
    fi
    echo $1 | grep -i "river" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$ROBOT$RIVER"
    fi
    echo $1 | grep -i "wash" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$ROBOT$WASH"
    fi
    echo $1 | grep -i "zoe" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$ROBOT$ZOE"
    fi

    scp $WPASUPCONF $ROBOT:/etc/wpa_supplicant/
else
    echo "Usage: ./change_ssid [robot_name]"
    exit 2
fi

