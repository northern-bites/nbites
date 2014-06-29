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

# Move the config files for wpa supplicant
echo "Moving config files for wpa_supplicant"
mv $FOLDER/wpa_supplicant.conf /etc/wpa_supplicant
mv $FOLDER/nbwired /etc/init.d
mv $FOLDER/nbwireless /etc/init.d
chmod +x /etc/init.d/nbwired
chmod +x /etc/init.d/nbwireless

# Removing connman and web interface
rc-config delete connman boot
rc-config delete naopathe default
rc-config delete lighttpd default
rc-config add utwired boot
rc-config add utwireless boot

# Change the password
echo "Change the password..."
passwd nao

rm -rf $FOLDER

echo "This robot should be good to go!"
echo "You should now DELETE this script to avoid clutter."
