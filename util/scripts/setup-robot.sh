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

# Change the password
echo "Change the password..."
passwd nao

rm -rf $FOLDER

echo "This robot should be good to go!"
echo "You should now DELETE this script to avoid clutter."
