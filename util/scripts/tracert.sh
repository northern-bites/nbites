#/bin/sh

MAL=192.168.16.102
JAYNE=192.168.16.103
RIVER=192.168.16.104
WASH=192.168.16.105
ZOE=192.168.16.106
ROBOT="127.0.0.1"
TRACE=0
SUM=0

if [ $1 ]; then 
    echo $1 | grep -i "mal" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$MAL"
    fi
    echo $1 | grep -i "jayne" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$JAYNE"
    fi
    echo $1 | grep -i "river" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$RIVER"
    fi
    echo $1 | grep -i "wash" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$WASH"
    fi
    echo $1 | grep -i "zoe" >/dev/null 2>&1
    if [ "$?" -eq "0" ]; then
	ROBOT="$ZOE"
    fi
else
    #echo "you didn't specify a host to trace to"
    #exit 2
    echo "tracerting to $ROBOT"
fi


for i in {1..5}
do
    traceroute --sport=21600 -U $ROBOT 112 -m 2 | grep ms | sed 's/[^0-9. ]*//g' | awk '{print $3, $4, $5}' #| awk '{sum=sum+$1} END {print sum}')
    #SUM=`expr $TRACE + expr $SUM`
done
#SUM=$SUM/i
#echo "average latency: $SUM ms"