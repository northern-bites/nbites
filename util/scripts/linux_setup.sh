#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./linux_setup.sh [ naoqi-version ]"
	exit 1
fi

PACKAGES="build-essential cmake git-core \
python2.6-dev emacs cmake-curses-gui ccache curl aptitude \
ant qt4-dev-tools libmpfr1ldbl"

echo "Are you on 64-bit linux?(y/n)"
read IS64BIT
echo "What version of Ubuntu are you on? (example: 11.04)"
read VERSION

if [ $IS64BIT == 'y' ]; then
  PACKAGES="$PACKAGES g++-4.4-multilib"
fi

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
else
    echo "That version is not supported. Please use 10.10 or 11.04. However, \
if you are very sure of what you're doing, you can \
finish running the script and install Sun Java manually when it is done."
    echo ""
    echo "Continue? (y/n)"
    read CONTINUE
    if [ $CONTINUE == 'n' ]; then
	echo "Exiting."
	exit 1
    fi
fi

echo "Downloading awesome free stuff!"
sudo apt-get install $PACKAGES

echo "Downloading and unpacking NBites files"

naoqi_version=$1
robocup=robocup.bowdoin.edu:/mnt/research/robocup
nbites_dir=$PWD/../..
lib_dir=$nbites_dir/lib

naoqi=naoqi-sdk-$naoqi_version-linux-nbites.tar.gz
naoqi_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$naoqi
naoqi_local=$lib_dir/naoqi-sdk-$naoqi_version-linux

ext=ext-nbites-linux.tar.gz
ext_robocup=$robocup/software/$ext

echo ""
echo "What's your Bowdoin username?"
read USER_NAME

echo "Downloading NaoQi"
mkdir -p $lib_dir
rsync -v $USER_NAME@$naoqi_robocup $lib_dir/

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xzf $naoqi
rm $naoqi
popd

if [ $IS64BIT == 'y' ]; then

ctc=linux-x64-crosstoolchain.tar.gz
ctc_robocup=$robocup/software/nao/cross_compiler_stuff/$ctc

echo "Downloading the CTC"
rsync -v $USER@$ctc_robocup $naoqi_local/

echo "Unpacking the CTC"

pushd $naoqi_local
tar -xzf $ctc
rm $ctc
popd
fi

echo "Downloading external components"

rsync -v $USER_NAME@$ext_robocup $nbites_dir/

echo "Unpacking ext"
pushd $nbites_dir
tar -xzf $ext
rm $ext
popd

echo "Setting up bash stuff ..."

nbites_bash=$nbites_dir/util/scripts/nbites.bash

echo "export NBITES_DIR=$nbites_dir" >> nbites_bash
echo "export AL_DIR=$naoqi_local" >> nbites_bash
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$nbites_dir/ext/lib" >> nbites_bash
echo "export PATH=$nbites_dir/ext/bin:$PATH" >> nbites_bash
echo "Done! The last step is just to add the following line:"
echo "source $nbites_bash"
echo "to your .bashrc (which is in your home directory)"

echo "Would you like this to be done automatically? (y/n)"
read AUTO

if [ $AUTO == 'y' ]; then
    echo "source $nbites_bash" >> ~/.bashrc
else
    echo "Add the line manually, and you'll be all set up!"
fi
