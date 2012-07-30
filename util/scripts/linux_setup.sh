#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./linux_setup.sh [naoqi-version]"
	exit 1
fi

PACKAGES="build-essential cmake git-core \
python2.7-dev emacs cmake-curses-gui ccache curl aptitude \
ant qt4-dev-tools"

echo "Are you on 64-bit linux? (y/n)"
read IS64BIT

if [ $IS64BIT == 'y' ]; then
    echo "64 bit Linux is NOT SUPPORTED!"
    echo "The Northern Bites code base depends on too many 32-bit libraries."
    echo "Please switch to 32-bit, or set up your system manually."
    echo "Exiting."
    exit 1
fi

echo "What version of Ubuntu are you on? (example: 12.04)"
read VERSION

if [[ $VERSION != '10.10' && $VERSION != '11.04' && $VERSION != '11.10' && $VERSION != '12.04' ]]; then

    echo "That version is not supported."
    echo "If you are very sure of what you are doing, you may continue."
    echo "Otherwise, please switch to Ubuntu 10.10 through 12.04."
    echo "Abort? (y/n)"
    read ABORT
    if [ $ABORT == 'y' ]; then
	echo "Exiting."
	exit 1
    fi
fi

echo "Do you want to run the deprecated Java tool on this computer? (y/n)"
read WANTJAVA

if [ $WANTJAVA == 'y' ]; then

    if [ $VERSION == '10.10' ]; then
	echo "Downloading Java. Accept the license by pressing TAB!"
	sudo add-apt-repository ppa:sun-java-community-team/sun-java6
	sudo apt-get update
	sudo apt-get install sun-java6-jdk
	sudo update-java-alternatives -s java-6-sun
    elif [ $VERSION == '11.04' ]; then
	echo "Downloading Java. Accept the license by pressing TAB!"
	sudo add-apt-repository ppa:ferramroberto/java
	sudo apt-get update
	sudo apt-get install sun-java6-jdk
    elif [ $VERSION == '11.10' ]; then
	echo "Downloading Java. Accept the license by pressing TAB!"
	sudo add-apt-repository ppa:ferramroberto/java
	sudo apt-get update
	sudo apt-get install sun-java6-jdk
    else
	echo "You will need to install Sun Java manually."
	echo ""
    fi
fi

echo "Downloading awesome free stuff!"
sudo apt-get install $PACKAGES

echo "Downloading and unpacking NBites files"

naoqi_version=$1
robocup=robocup.bowdoin.edu:/mnt/research/robocup
nbites_dir=$PWD/../..
lib_dir=$nbites_dir/lib

naoqi=naoqi-sdk-$naoqi_version-linux32.tar.gz
atom=nbites-atom-toolchain-$naoqi_version.tar.gz
geode=nbites-geode-toolchain-$naoqi_version.tar.gz

naoqi_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$naoqi
atom_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$atom
geode_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$geode

naoqi_local=$lib_dir/naoqi-sdk-$naoqi_version-linux32
atom_local=$lib_dir/atomtoolchain
geode_local=$lib_dir/geodetoolchain

ext=ext-nbites-linux32.tar.gz

ext_robocup=$robocup/software/$ext

echo ""
echo "What's your Bowdoin username?"
read USER_NAME

echo "Downloading NaoQi"
mkdir -p $lib_dir
rsync -v $USER_NAME@$naoqi_robocup $lib_dir/

echo "Downloading Atom toolchain"
rsync -v $USER_NAME@$atom_robocup $lib_dir/

echo "Downloading Geode toolchain"
rsync -v $USER_NAME@$geode_robocup $lib_dir/

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xzf $naoqi
rm $naoqi

mkdir $atom_local
tar -xzf $atom -C $atom_local --strip-components 1

mkdir $geode_local
tar -xzf $geode -C $geode_local --strip-components 1
popd

echo "Downloading external components"

rsync -v $USER_NAME@$ext_robocup $nbites_dir/

echo "Unpacking ext"
pushd $nbites_dir
tar -xzf $ext
rm $ext
popd

echo "Setting up git stuff ..."
./git_setup.sh

echo "Setting up bash stuff ..."

nbites_bash=$nbites_dir/util/scripts/nbites.bash

echo "export NBITES_DIR=$nbites_dir" >> $nbites_bash
echo "export AL_DIR=$naoqi_local" >> $nbites_bash
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$nbites_dir/ext/lib" >> $nbites_bash
echo "export PATH=$nbites_dir/ext/bin:$PATH" >> $nbites_bash
echo "Done! The last step is just to add the following line:"
echo "source $nbites_bash"
echo "to your .bashrc (which is in your home directory)"

echo "Would you like this to be done automatically? (y/n)"
read AUTO

if [ $AUTO == 'y' ]; then
    echo "#added by linux-setup.sh for RoboCup purposes" >> ~/.bashrc
    echo "source $nbites_bash" >> ~/.bashrc
    echo "You're good to go!"
else
    echo "Add the line manually, and you'll be all set up!"
fi

echo "NOW RESTART THE TERMINAL SO THAT .bashrc GETS RUN!"
