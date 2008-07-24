#!/bin/sh

PATH=/sbin:/bin:/usr/sbin:/usr/bin

DAEMON=/usr/sbin/wpa_supplicant
CONFIG="/etc/wpa_supplicant.conf"
PNAME="wpa_supplicant"
INTERFACE="eth1"
LOG_FILE="/var/log/wireless.log"

# insane defaults
if [ -n "$OPTIONS" ]; then
  WPA_OPTIONS="-Bw -c $CONFIG $OPTIONS"
else
  WPA_OPTIONS="-Bw -c $CONFIG -i $INTERFACE"
fi
DHCP_OPTIONS="-i $INTERFACE --retries 6 -n"

test -f /etc/default/wpa && . /etc/default/wpa

if [ ! -f $CONFIG ]; then
	echo "No configuration file found, not starting."
	exit 1
fi

test -f $DAEMON || exit 0

case "$1" in
	start)
		echo -n "Resetting wireless interface ($INTERFACE): "
		ifdown $INTERFACE >/dev/null >/dev/null 2>&1
		ifup $INTERFACE >/dev/null >$LOG_FILE 2>&1
                if [ $? -eq 0 ]; then
                  echo "done."
                else
                  echo "failed."
                  exit 1
                fi

		echo -n "Starting wpa_supplicant: "
		start-stop-daemon -S -b -x $DAEMON -- $WPA_OPTIONS >>$LOG_FILE 2>&1
                if [ $? -eq 0 ]; then
                  echo "done."
                else
                  echo "failed."
                  exit 1
                fi

		#udhcpc $DHCP_OPTIONS
		;;
	stop)
		echo -n "Stopping wpa_supplicant: "
		start-stop-daemon -K -n $PNAME >>$LOG_FILE 2>&1
                if [ $? -eq 0 ]; then
                  echo "done."
                else
                  echo "failed."
                  exit 1
                fi

		echo -n "Shutting down wireless interface ($INTERFACE): "
		ifdown $INTERFACE >$LOG_FILE 2>&1
                if [ $? -eq 0 ]; then
                  echo "done."
                else
                  echo "failed."
                fi
		;;
	reload|force-reload)
		echo -n "Reloading wpa_supplicant: "
		killall -HUP $PNAME
		echo "done."
		;;
	restart)
		echo -n "Resetting wireless interface ($INTERFACE): "
		ifdown $INTERFACE >/dev/null 2>&1
		ifup $INTERFACE >$LOG_FILE 2>&1
                if [ $? -eq 0 ]; then
                  echo "done."
                else
                  echo "failed."
                fi

		echo -n "Restarting wpa_supplicant: "
		start-stop-daemon -K -n $PNAME >>$LOG_FILE 2>&1
		sleep 1
		start-stop-daemon -S -b -x $DAEMON -- $WPA_OPTIONS >>$LOG_FILE 2>&1
                if [ $? -eq 0 ]; then
                  echo "done."
                else
                  echo "failed."
                fi

		#udhcpc $DHCP_OPTIONS
		;;
	*)
		echo "Usage: $0 {start|stop|restart|reload|force-reload}" >&2
		exit 1
		;;
esac

exit 0
