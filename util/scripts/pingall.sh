#!/bin/sh

MAL=mal.local
JAYNE=jayne.local
RIVER=river.local
WASH=wash.local
ZOE=zoe.local
ROBOT=""
L=3

if [ $1 ]; then
    echo $1 | grep "[^1-5]" > /dev/null 2>&1
    if [ "$?" -eq "1" ]; then
	L=$1
    fi
fi

echo "pinging hosts with nmap -T$L... \n"

HOST=$MAL
ROBOT="mal"
nmap -sP -T$L $HOST 2>&1 | grep "Host is up"
if [ "$?" -eq 0 ]; then
    echo "$ROBOT is up at $HOST."
else
    echo "ERROR: $HOST unreachable, $ROBOT is down!"
fi
echo "\n"

HOST=$JAYNE
ROBOT="jayne"
nmap -sP -T$L $HOST 2>&1 | grep "Host is up"
if [ "$?" -eq 0 ]; then
    echo "$ROBOT is up at $HOST."
else
    echo "ERROR: $HOST unreachable, $ROBOT is down!"
fi
echo "\n"

HOST=$RIVER
ROBOT="river"
nmap -sP -T$L $HOST 2>&1 | grep "Host is up"
if [ "$?" -eq 0 ]; then
    echo "$ROBOT is up at $HOST."
else
    echo "ERROR: $HOST unreachable, $ROBOT is down!"
fi
echo "\n"

HOST=$WASH
ROBOT="wash"
nmap -sP -T$L $HOST 2>&1 | grep "Host is up"
if [ "$?" -eq 0 ]; then
    echo "$ROBOT is up at $HOST."
else
    echo "ERROR: $HOST unreachable, $ROBOT is down!"
fi
echo "\n"

HOST=$ZOE
ROBOT="zoe"
nmap -sP -T$L $HOST 2>&1 | grep "Host is up"
if [ "$?" -eq 0 ]; then
    echo "$ROBOT is up at $HOST."
else
    echo "ERROR: $HOST unreachable, $ROBOT is down!"
fi