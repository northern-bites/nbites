#!/bin/bash

# Liberally modeled off of linux_setup.sh

if [ $# -ne 1 ]; then
    echo "usage: ./update_naoqi.sh [naoqi-version]"
    exit 1
fi

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

echo "Downloading NaoQi"
wget $naoqi_robocup -P $lib_dir/

echo "Downloading Atomtoolchain"
wget $atom_robocup -P $lib_dir/

echo "Unpacking NaoQi"

pushd $lib_dir
tar -xzf $naoqi
rm $naoqi

echo "Unpacking Atomtoolchain"

mv $atom_local oldAtom
mkdir $atom_local
tar -xzf $atom -C $atom_local --strip-components 1
rm $atom
echo "Sudo privileges required to remove old atomtoolchain"
sudo rm -rf oldAtom

popd

scripts=$nbites_dir/util/scripts/
mv $scripts/nbites.bash bashBackup

sed "2 c\export AL_DIR=$naoqi_local" bashBackup >> nbites.bash

rm $scripts/bashBackup

echo "Restart terminal for changes to take effect"
