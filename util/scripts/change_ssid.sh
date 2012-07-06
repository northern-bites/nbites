#!/bin/sh

MAL=mal.local
JAYNE=jayne.local
RIVER=river.local
WASH=wash.local
ZOE=zoe.local

WPASUPCONF=../config/wpa_supplicant.conf
TMPCONF=../config/tmpconf
DOALL=0

if [ $2 ]; then 
    echo $1 | grep -i "mal" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="nao@$MAL"
    fi
    echo $1 | grep -i "jayne" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="nao@$JAYNE"
    fi
    echo $1 | grep -i "river" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="nao@$RIVER"
    fi
    echo $1 | grep -i "wash" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="nao@$WASH"
    fi
    echo $1 | grep -i "zoe" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="nao@$ZOE"
    fi
    echo $1 | grep -i "all" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="nao@"
	DOALL=1
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

if [ $DOALL -eq 1 ]; then
    scp $WPASUPCONF $ROBOT$MAL:/etc/wpa_supplicant/wpa_supplicant.conf
    scp $WPASUPCONF $ROBOT$JAYNE:/etc/wpa_supplicant/wpa_supplicant.conf
    scp $WPASUPCONF $ROBOT$RIVER:/etc/wpa_supplicant/wpa_supplicant.conf
    scp $WPASUPCONF $ROBOT$WASH:/etc/wpa_supplicant/wpa_supplicant.conf
    scp $WPASUPCONF $ROBOT$ZOE:/etc/wpa_supplicant/wpa_supplicant.conf
else
    scp $WPASUPCONF $ROBOT:/etc/wpa_supplicant/wpa_supplicant.conf
fi