PACKAGES="build-essential cmake curl libqt4-dev"

echo "Downloading awesome free stuff!"
sudo apt-get install $PACKAGES

EXT=ext-nbites-linux32.tar.gz
ROBOCUP_SOFTWARE=http://robocup.bowdoin.edu/public/software

echo "Downloading the external dependencies"
curl -O $ROBOCUP_SOFTWARE/$EXT
tar -xzf $EXT