# To be run on the robot AS ROOT
if [ $# -ne 2 ]; then
    echo "Usage: ./setup-robot <new-hostname>"
    exit 1
fi

if [ x$(whoami) != xroot ]; then
    echo "You must be root to run this script."
    exit 1
fi

HOSTNAME=$1
FOLDER=nao_files
WIFI_CONFIG=wireless_config

# Move the config files for wpa_supplicant
echo "Moving config files for wpa_supplicant"
mv $WIFI_CONFIG/wpa_supplicant.conf /etc/wpa_supplicant
mv $WIFI_CONFIG/nbwired /etc/init.d
mv $WIFI_CONFIG/nbwireless /etc/init.d
chmod +x /etc/init.d/nbwired
chmod +x /etc/init.d/nbwireless

# Edit nbwired so as to have a unique link local IP
# TODO should pull IPs from CommDef.h
if [ "$HOSTNAME" = wash ]; then
    sed -i 's/ROBOT/9/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = river ]; then
    sed -i 's/ROBOT/10/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = jayne ]; then
    sed -i 's/ROBOT/11/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = simon ]; then
    sed -i 's/ROBOT/12/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = inara ]; then
    sed -i 's/ROBOT/13/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = kaylee ]; then
    sed -i 's/ROBOT/14/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = vera ]; then
    sed -i 's/ROBOT/15/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = mal ]; then
    sed -i 's/ROBOT/16/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = zoe ]; then
    sed -i 's/ROBOT/17/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = ringo ]; then
    sed -i 's/ROBOT/18/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = beyonce ]; then
    sed -i 's/ROBOT/19/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = ozzy ]; then
    sed -i 's/ROBOT/20/g' /etc/init.d/nbwired
elif [ "$HOSTNAME" = avril ]; then
    sed -i 's/ROBOT/21/g' /etc/init.d/nbwired
else
    echo "Robot does not have a valid hostname. Link local will not be configured correctly"
fi

# Edit nbwireless so as to have a static IP
# TODO should pull IPs from CommDef.h
if [ "$HOSTNAME" = wash ]; then
    sed -i 's/ROBOT/9/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = river ]; then
    sed -i 's/ROBOT/10/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = jayne ]; then
    sed -i 's/ROBOT/11/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = simon ]; then
    sed -i 's/ROBOT/12/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = inara ]; then
    sed -i 's/ROBOT/13/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = kaylee ]; then
    sed -i 's/ROBOT/14/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = vera ]; then
    sed -i 's/ROBOT/15/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = mal ]; then
    sed -i 's/ROBOT/16/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = zoe ]; then
    sed -i 's/ROBOT/17/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = ringo ]; then
    sed -i 's/ROBOT/18/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = beyonce ]; then
    sed -i 's/ROBOT/19/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = ozzy ]; then
    sed -i 's/ROBOT/20/g' /etc/init.d/nbwireless
elif [ "$HOSTNAME" = avril ]; then
    sed -i 's/ROBOT/21/g' /etc/init.d/nbwireless
else
    echo "Robot does not have a valid hostname. Link local will not be configured correctly"
fi

# Replacing connman with wpa_supplicant
if [ $2 == "connman" ]; then
    echo "Replacing connman with wpa_supplicant"
    mv $FOLDER/sys_autoload.ini /etc/naoqi/autoload.ini
    rc-config delete connman boot
    rc-config delete naopathe default
    rc-config delete lighttpd default
    rc-config add nbwired boot
    rc-config add nbwireless boot
fi

rm -rf $FOLDER
rm -rf $WIFI_CONFIG

echo "This robot should be good to go!"
echo "You should now DELETE this script to avoid clutter."
