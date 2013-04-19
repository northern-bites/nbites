#!/bin/sh

MAL=mal.local
JAYNE=jayne.local
RIVER=river.local
WASH=wash.local
ZOE=zoe.local

echo "|  mal  | jayne | river | wash  |  zoe  |"

while(true)
do
    DONTCARE=$(nmap -sP -T3 $MAL 2>&1 | grep "Host is up")
    if [ "$?" -eq 0 ]; then
	MALUP="  up   "
    else
	MALUP=" DOWN  "
    fi

    DONTCARE=$(nmap -sP -T3 $JAYNE 2>&1 | grep "Host is up")
    if [ "$?" -eq 0 ]; then
	JAYNEUP="  up   "
    else
	JAYNEUP=" DOWN  "
    fi

    DONTCARE=$(nmap -sP -T3 $RIVER 2>&1 | grep "Host is up")
    if [ "$?" -eq 0 ]; then
	RIVERUP="  up   "
    else
	RIVERUP=" DOWN  "
    fi

    DONTCARE=$(nmap -sP -T3 $WASH 2>&1 | grep "Host is up")
    if [ "$?" -eq 0 ]; then
	WASHUP="  up   "
    else
	WASHUP=" DOWN  "
    fi

    DONTCARE=$(nmap -sP -T3 $ZOE 2>&1 | grep "Host is up")
    if [ "$?" -eq 0 ]; then
	ZOEUP="  up   "
    else
	ZOEUP=" DOWN  "
    fi

    echo "|$MALUP|$JAYNEUP|$RIVERUP|$WASHUP|$ZOEUP|"
done

