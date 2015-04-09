#!/bin/bash

# Liberally modeled off of linux_setup.sh

if [ $# -ne 1 ]; then
    echo "usage: ./update_naoqi.sh [naoqi-version]"
    exit 1
fi

naoqi_version=$1
server=dover
robocup=/mnt/research/robocup
nbites_dir=$PWD/../..
lib_dir=$nbites_dir/lib

naoqi=naoqi-sdk-$naoqi_version-linux32.tar.gz
atom=nbites-atom-toolchain-$naoqi_version.tar.gz

naoqi_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$naoqi
atom_robocup=$robocup/software/nao/NaoQi/$naoqi_version/$atom

naoqi_local=$lib_dir/naoqi-sdk-$naoqi_version-linux32
atom_local=$lib_dir/atomtoolchain

echo "Downloading NaoQi & Atom toolchain"
echo "Please enter your Bowdoin username"
read username

scp -r $username@$server:$software $lib_dir

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xzf $naoqi_version/$naoqi -C .

echo "Unpacking Atomtoolchain"

mv $atom_local oldAtom
mkdir $atom_local
tar -xzf $naoqi_version/$atom -C $atom_local --strip-components 1

rm -r $naoqi_version

echo "Sudo privileges required to remove old atomtoolchain"
sudo rm -rf oldAtom

popd

scripts=$nbites_dir/util/scripts/
mv $scripts/nbites.bash bashBackup

sed "2 c\export AL_DIR=$naoqi_local" bashBackup >> nbites.bash

rm $scripts/bashBackup

echo "Restart terminal for changes to take effect"
