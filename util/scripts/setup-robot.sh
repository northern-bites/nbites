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

# Make nbites folders
echo "Making folders for nbites content..."
mkdir -p nbites/audio nbites/log
chown -R nao nbites
rm -rf behaviors
rm -rf recordings

# Put audio files in their place
echo "Moving audio files..."
mv *.wav nbites/audio

# Move the libraries
echo "Moving libraries..."
mv libboost_python-mt.so /usr/lib/
mv libprotobuf.so.7 /usr/lib/libprotobuf.so.7

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
mv init.d/* /etc/init.d/
rmdir init.d/
mv wpa_supplicant.conf /etc/wpa_supplicant/

#switch connman to wpa_supplicant
echo "Setting up networking..."
rc-update delete connman boot
rc-update add /etc/init.d/utwireless boot
rc-update add /etc/init.d/utwired boot

# Move the autoload files
echo "Moving autoload.ini files into place..."
mv sys_autoload.ini /etc/naoqi/autoload.ini
mv usr_autoload.ini naoqi/preferences/autoload.ini

# Change the password
echo "Change the password..."
passwd nao

echo "This robot should be good to go!"
echo "You should now DELETE this script to avoid clutter."
