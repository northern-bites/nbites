#!/bin/sh

###############################################################################
# Script to configure wireless on a new Nao.
# See wireless/README for details
###############################################################################



DEST=/etc
SOURCE=wireless
SCRIPT_NAME=configure-wireless
OPTIONS="<robot-ip>"

if [ "$1"  == "" ]; then
    echo "Usage: ./$SCRIPT_NAME $OPTIONS"
    exit 1
fi


#echo "$1:/etc/init.d/wireless stop"
#ssh root@$1 '/etc/init.d/wireless stop'

TARGET=wireless.sh
SUBFOLDER=/init.d
echo "Copying $TARGET to $DEST$SUBFOLDER"
scp -r $SOURCE/$TARGET root@$1:$DEST$SUBFOLDER/

TARGET=interfaces
SUBFOLDER=""
echo "Copying $TARGET to $DEST$SUBFOLDER"
scp -r $SOURCE/$TARGET root@$1:$DEST$SUBFOLDER/

TARGET=modutils
SUBFOLDER=""
echo "Copying $TARGET to $DEST$SUBFOLDER"
scp -r $SOURCE/$TARGET root@$1:$DEST$SUBFOLDER/


#cofigure and restart wirelss
echo "$1:update-modules"
echo "$1:/etc/init.d/wireless start"

ssh root@$1 'update-modules && /etc/init.d/wireless start'
