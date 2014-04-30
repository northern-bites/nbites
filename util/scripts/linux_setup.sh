#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./linux_setup.sh [naoqi-version]"
	exit 1
fi

OLDPACKAGES="libboost1.48-dev libboost-python1.48-dev"

PACKAGES="build-essential cmake git-core \
emacs cmake-curses-gui ccache aptitude \
qt4-dev-tools python-pyparsing libeigen3-dev"

BITS=`uname -m`

if [ $BITS == 'x86_64' ]; then
    echo ""
    echo "64 bit Linux is NOT SUPPORTED!"
    echo "The Northern Bites code base depends on too many 32-bit libraries."
    echo "Please switch to 32-bit or set up your system manually."
    echo "Exiting."
    exit 1
fi

VERSION=`lsb_release -a 2>/dev/null | grep 'Release:' | grep -o '[0-9]\+.[0-9]\+'`

if [[ $VERSION != '12.04' && $VERSION != '14.04' ]]; then

    echo ""
    echo "That version is NOT SUPPORTED."
    echo "Some packages will not be the right version or may not exist."
    echo "--------------------------------------------------------------"
    echo "If you are very sure of what you are doing, you may continue and"
    echo "configure broken packages manually."
    echo "Otherwise, please switch to Ubuntu 14.04 or 12.04."
    echo ""
    echo "Abort? (y/n)"
    read ABORT
    if [ $ABORT == 'y' ]; then
	echo "Exiting."
	exit 1
    fi
fi

echo ""
echo "Downloading and installing software!"
echo "..."

# Certain packages have to be installed from the 12.04 repo and frozen at that version
# First add the precise main repo to sources.list & update
sudo mkdir /etc/apt/sources.list.d 2>/dev/null
sudo touch /etc/apt/sources.list.d/precise.list
echo -e "deb http://us.archive.ubuntu.com/ubuntu/ precise main restricted universe" | sudo tee -a /etc/apt/sources.list.d/precise.list 1>/dev/null
echo -e "deb-src http://us.archive.ubuntu.com/ubuntu/ precise main restricted universe" | sudo tee -a /etc/apt/sources.list.d/precise.list 1>/dev/null
sudo apt-get update

# then install specifically from that repo and freeze the packages
sudo apt-get -y -t=precise install $OLDPACKAGES
sudo apt-mark hold $OLDPACKAGES

sudo apt-get -y install $PACKAGES

naoqi_version=$1
robocup=http://robocup.bowdoin.edu/public
nbites_dir=$PWD/../..
lib_dir=$nbites_dir/lib

naoqi=naoqi-sdk-$naoqi_version-linux32.tar.gz
atom=nbites-atom-toolchain-$naoqi_version.tar.gz

naoqi_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$naoqi
atom_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$atom

naoqi_local=$lib_dir/naoqi-sdk-$naoqi_version-linux32
atom_local=$lib_dir/atomtoolchain

echo ""
echo "Downloading and unpacking NBites files."

echo "Downloading NaoQi"
mkdir -p $lib_dir
wget $naoqi_robocup -P $lib_dir/

echo "Downloading Atom toolchain"
wget $atom_robocup -P $lib_dir/

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xzf $naoqi
rm $naoqi

mkdir $atom_local
tar -xzf $atom -C $atom_local --strip-components 1
rm $atom

popd

echo ""
echo "Configuring git."
./git_setup.sh

echo ""
echo "Configuring bash."

nbites_bash=$nbites_dir/util/scripts/nbites.bash

echo "export NBITES_DIR=$nbites_dir" >> $nbites_bash
echo "export AL_DIR=$naoqi_local" >> $nbites_bash

echo "" >> ~/.bashrc
echo "#added by linux-setup.sh for RoboCup purposes" >> ~/.bashrc
echo "source $nbites_bash" >> ~/.bashrc
echo ""

echo ""
echo "One last IMPORTANT step: Restart your terminal. Then you're good to go!"
