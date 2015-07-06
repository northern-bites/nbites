#!/bin/bash
echo "kdone:"
echo "Sending SIGTERM to [hal, naoqi-bin]..."
killall -v naoqi-bin
killall -v hal
sleep 2
echo "Sending SIGKILL to [hal, naoqi-bin]..."
killall -v -9 naoqi-bin
killall -v -9 hal
echo "Remaining nao user processes:"
ps -u nao
sudo shutdown -h now && exit
