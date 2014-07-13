# To be run on the robot AS ROOT
if [ $# -ne 1 ] ; then
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

# Make nbites folders
echo "Making folders for nbites content..."
mkdir -p nbites/audio nbites/log
chown -R nao nbites

# Put audio files in their place
echo "Moving audio files..."
mv $FOLDER/audio/* nbites/audio

# Move the libraries
echo "Moving libraries..."
mv $FOLDER/libboost_python.so.1.48.0 /usr/lib/
mv $FOLDER/libprotobuf.so.7 /usr/lib/

# Put the camera driver in place
echo "Moving camera driver..."
mv $FOLDER/mt9m114.ko /lib/modules/2.6.33.9-rt31-aldebaran-rt/kernel/drivers/media/video/

# Set the hostname
echo "Setting the hostname to $HOSTNAME..."
echo $HOSTNAME > /etc/hostname

# Set up the profile
echo "Adding nbites config to profile..."
echo "# NBites Config below:" >> /etc/profile
echo "alias naolog='tail -f /home/nao/nbites/log/nblog'" >> /etc/profile
echo "alias done='sudo shutdown -h now && exit'" >> /etc/profile
echo "ulimit -S -c unlimited" >> /etc/profile

# Move the etc config
echo "Moving etc files into place..."
mv $FOLDER/naoqi-init /etc/init.d/naoqi

# Move the autoload files
echo "Moving autoload.ini files into place..."
mv $FOLDER/sys_autoload.ini /etc/naoqi/autoload.ini
mv $FOLDER/usr_autoload.ini naoqi/preferences/autoload.ini

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

# Replacing connman with wpa_supplicant
echo "Replacing connman with wpa_supplicant"
rc-config delete connman boot
rc-config delete naopathe default
rc-config delete lighttpd default
rc-config add nbwired boot
rc-config add nbwireless boot

# Change the password
echo "Change the password..."
passwd nao

rm -rf $FOLDER
rm -rf $WIFI_CONFIG

echo "This robot should be good to go!"
echo "You should now DELETE this script to avoid clutter."
