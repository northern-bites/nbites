#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./linux_setup.sh [naoqi-version]"
	exit 1
fi

PACKAGES="build-essential cmake git-core \
python2.7-dev emacs cmake-curses-gui ccache curl aptitude \
ant qt4-dev-tools python-pyparsing"

echo "Are you on 64-bit linux? (y/n)"
read IS64BIT

if [ $IS64BIT == 'y' ]; then
    echo ""
    echo "64 bit Linux is NOT SUPPORTED!"
    echo "The Northern Bites code base depends on too many 32-bit libraries."
    echo "Please switch to 32-bit or set up your system manually."
    echo "Exiting."
    exit 1
fi

echo ""
echo "What version of Ubuntu are you on? (example: 12.04)"
read VERSION

if [[ $VERSION != '11.10' && $VERSION != '12.04' ]]; then

    echo ""
    echo "That version is NOT SUPPORTED."
    echo "Some packages will not be the right version or may not exist."
    echo "--------------------------------------------------------------"
    echo "If you are very sure of what you are doing, you may continue and"
    echo "configure broken packages manually."
    echo "Otherwise, please switch to Ubuntu 11.10 or 12.04."
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
sudo apt-get install $PACKAGES

naoqi_version=$1
robocup=robocup.bowdoin.edu:/mnt/research/robocup
nbites_dir=$PWD/../..
lib_dir=$nbites_dir/lib

naoqi=naoqi-sdk-$naoqi_version-linux32.tar.gz
atom=nbites-atom-toolchain-$naoqi_version.tar.gz

naoqi_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$naoqi
atom_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$atom

naoqi_local=$lib_dir/naoqi-sdk-$naoqi_version-linux32
atom_local=$lib_dir/atomtoolchain

ext=ext-nbites-linux32.tar.gz

ext_robocup=$robocup/software/$ext

echo ""
echo "What's your Bowdoin username?"
read USER_NAME

echo ""
echo "Downloading and unpacking NBites files."

echo "Downloading NaoQi"
mkdir -p $lib_dir
rsync -v $USER_NAME@$naoqi_robocup $lib_dir/

echo "Downloading Atom toolchain"
rsync -v $USER_NAME@$atom_robocup $lib_dir/

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xzf $naoqi
rm $naoqi

mkdir $atom_local
tar -xzf $atom -C $atom_local --strip-components 1

popd

echo "Downloading external components"

rsync -v $USER_NAME@$ext_robocup $nbites_dir/

echo "Unpacking external components"
pushd $nbites_dir
tar -xzf $ext
rm $ext
popd

echo ""
echo "Configuring git."
./git_setup.sh

echo ""
echo "Configuring bash."

nbites_bash=$nbites_dir/util/scripts/nbites.bash

echo "export NBITES_DIR=$nbites_dir" >> $nbites_bash
echo "export AL_DIR=$naoqi_local" >> $nbites_bash
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$nbites_dir/ext/lib" >> $nbites_bash
echo "export PATH=$nbites_dir/ext/bin:$nbites_dir/src/serenity:$PATH" >> $nbites_bash

echo ""
echo "Done! The last step is just to add the following line:"
echo "source $nbites_bash"
echo "to your .bashrc (which is in your home directory)"

echo "Would you like this to be done automatically? (y/n)"
read AUTO

if [ $AUTO == 'y' ]; then
    echo "" >> ~/.bashrc
    echo "#added by linux-setup.sh for RoboCup purposes" >> ~/.bashrc
    echo "source $nbites_bash" >> ~/.bashrc
    echo ""
    echo "You're good to go!"
else
    echo ""
    echo "Add the line manually, and you'll be all set up!"
fi

echo "NOW RESTART THE TERMINAL SO THAT .bashrc GETS RUN!"
