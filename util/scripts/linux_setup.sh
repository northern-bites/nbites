#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./linux_setup.sh [ naoqi-version ]"
	exit 1
fi

echo "Downloading awesome free stuff! Also accept the Sun Java licence by \
pressing TAB!"

PACKAGES="build-essential cmake git-core sun-java6-jdk \
python2.6-dev emacs cmake-curses-gui ccache curl aptitude \
ant qt4-dev"

echo "Are you on 64-bit linux?(y/n)"
read IS64BIT
echo "What version of Ubuntu are you on? (example: 11.04)"
read VERSION

if [ $IS64BIT == 'y' ]; then
  PACKAGES="$PACKAGES g++-4.4-multilib"
fi

if [ $VERSION == '10.10' ]; then
    sudo add-apt-repository ppa:sun-java-community-team/sun-java6
elif [ $VERSION == '11.04' ]; then
    sudo add-apt-repository ppa:ferramroberto/java
else
    echo "That version is not supported. Please use 10.10 or 11.04"
    exit 1
fi
sudo apt-get update
sudo apt-get install $PACKAGES
sudo update-java-alternatives -s java-6-sun

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
prsync -v $USER_NAME@$naoqi_robocup $lib_dir/

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xvzf $naoqi
rm $naoqi
popd

if [ $IS64BIT == 'y' ]; then

ctc=linux-x64-crosstoolchain.tar.gz
ctc_robocup=$robocup/software/nao/cross_compiler_stuff/$ctc

echo "Downloading the CTC"
rsync -v $USER@$ctc_robocup $naoqi_local/

echo "Unpacking the CTC"

pushd $naoqi_local
tar -xvzf $ctc
rm $ctc
popd
fi

echo "Downloading external components"

rsync -v $USER_NAME@$ext_robocup $nbites_dir/

echo "Unpacking ext"
pushd $nbites_dir
tar -xvzf $ext
rm $ext
popd

echo "Setting up bash stuff ..."

echo "export NBITES_DIR=$nbites_dir" >> nbites.bash
echo "export AL_DIR=$naoqi_local" >> nbites.bash
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$nbites_dir/ext/lib" >> ~/nbites.bash
echo "export PATH=$PATH:$nbites_dir/ext/bin" >> ~/nbites.bash
echo "Done! The last step is just to add the following line"
echo "source $PWD/nbites.bash"
echo "to your .bashrc (which is in your home directory)"
