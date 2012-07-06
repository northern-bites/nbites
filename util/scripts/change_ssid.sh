#!/bin/sh

MAL=mal.local
JAYNE=jayne.local
RIVER=river.local
WASH=wash.local
ZOE=zoe.local

ROBOT="nao@"
WPASUPCONF=../config/wpa_supplicant.conf
TMPCONF=../config/tmpconf

if [ $2 ]; then 
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

else
    echo "Usage: ./change_ssid [robot_name] [network_ssid] [password_optional]"
    echo "You may substitute \"all\" for the robot name to copy the file to all reachable robots"
    exit 2
fi

SSID_LINE=`awk '$0 ~ str{print NR}' str="ssid" $WPASUPCONF`
PSK_LINE=`awk '$0 ~ str{print NR}' str="psk" $WPASUPCONF`

sed "${SSID_LINE}s/.*/  ssid\=\"$2\"/" $WPASUPCONF > $TMPCONF
mv $TMPCONF $WPASUPCONF

if [ $3 ]; then
    sed "${PSK_LINE}s/.*/  psk\=\"$3\"/" $WPASUPCONF > $TMPCONF
    mv $TMPCONF $WPASUPCONF
else
    sed "${PSK_LINE}s/.*/  psk\=\"\"/" $WPASUPCONF > $TMPCONF
    mv $TMPCONF $WPASUPCONF
fi

scp $WPASUPCONF $ROBOT:/etc/wpa_supplicant/wpa_supplicant.conf