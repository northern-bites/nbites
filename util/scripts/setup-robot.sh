# To be run on the robot AS ROOT

if [ $# -ne 2 ] ; then
    echo "Usage: ./setup-robot <new-hostname> <old-or-new>"
    exit 1
fi

if [ x$(whoami) != xroot ]; then
    echo "You must be root to run this script."
    exit 1
fi

if [ $2 != 'old' -a $2 != 'new' ]; then
    echo "The second parameter needs to be 'old' or 'new'"
    exit 1
fi

HOSTNAME=$1
TYPE=$2

# Make nbites folders
echo "Making folders for nbites content..."
mkdir -p nbites/audio nbites/frames nbites/log
chown -R nao nbites
rm -rf behaviors
rm -rf recordings

# Put audio files in their place
echo "Moving audio files..."
mv *.wav nbites/audio

# Move the libraries
echo "Moving libraries..."
mv libboost_python-mt.so /usr/lib/
mv libprotobuf.so.7 /usr/lib/
if [ $TYPE == 'old' ]; then
    mv lxv4l2.ko /lib/modules/2.6.29.6-rt24-aldebaran-rt/kernel/drivers/media/video/lxv4l2/
elif [ $TYPE == 'new' ]; then
    rm lxv4l2.ko
fi

# Set the hostname
echo "Setting the hostname to $HOSTNAME..."
echo $HOSTNAME > /etc/hostname

# Set up the profile
echo "Adding nbites config to profile..."
echo "# NBites Config below:" >> /etc/profile
echo "alias naolog='tail -f /home/nao/nbites/log/nblog'" >> /etc/profile
echo "ulimit -S -c unlimited" >> /etc/profile

# Move the etc config
echo "Moving etc files into place..."
mv init_stuff/* /etc/init.d/
rmdir init_stuff/

# Move the autoload files
echo "Moving autoload.ini files into place..."
mv sys_autoload.ini /etc/naoqi/autoload.ini
mv usr_autoload.ini naoqi/preferences/autoload.ini

# Change the password
echo "Change the password..."
passwd nao

echo "This robot should be good to go!"
echo "You should now DELETE this script to avoid clutter."