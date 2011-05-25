#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./linux_setup_x64.sh [ naoqi-version ]"
	exit 1
fi

echo "Downloading awesome free stuff! Also accept the Sun Java licence by \
pressing TAB!"

sudo add-apt-repository ppa:sun-java-community-team/sun-java6
sudo apt-get update
sudo apt-get install build-essential cmake git-core sun-java6-jdk \
python2.6-dev emacs cmake-curses-gui ccache curl aptitude \
g++-4.4-multilib
sudo update-java-alternatives -s java-6-sun

echo "Downloading and unpacking NBites files"

naoqi_version=$1
robocup=robocup.bowdoin.edu:/mnt/research/robocup
nbites_dir=$PWD/../..
lib_dir=$nbites_dir/lib

naoqi=naoqi-sdk-$naoqi_version-linux-nbites.tar.gz
naoqi_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$naoqi
naoqi_local=$lib_dir/naoqi-sdk-$naoqi_version-linux

ctc=linux-x64-crosstoolchain.tar.gz
ctc_robocup=$robocup/software/nao/cross_compiler_stuff/$ctc

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
tar -xvzf $naoqi
popd


echo "Downloading the CTC"
rsync -v $USER@$ctc_robocup $naoqi_local/

echo "Unpacking the CTC"

pushd $naoqi_local
tar -xvzf $ctc
popd

echo "Downloading external components"

rsync -v $USER@$ext_robocup $nbites_dir/

echo "Unpacking ext"
pushd $nbites_dir
tar -xvzf $ext
popd

echo "Setting up bash stuff ..."

echo "source $PWD/nbites.bash" >> ~/.bashrc
echo "export NBITES_DIR=$nbites_dir" >> ~/.bashrc
echo "export AL_DIR=$naoqi_local" >> ~/.bashrc
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/$nbites_dir/ext/lib" >> ~/.bashrc
echo "export PATH=$PATH:$nbites_dir/ext/bin" >> ~/.bashrc

