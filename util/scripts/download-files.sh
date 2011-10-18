
# Script to download the extra libraries needed to run the Northern Bites man package on the Aldebaran Nao

if [ $# -ne 1 ]; then
    echo "usage: ./download-libs.sh <Bowdoin username>"
    exit 1
fi

username=$1
robocup=robocup.bowdoin.edu:/mnt/research/robocup

lib_dir=../../lib
audio_dir=../audio

echo "Copying libraries from RoboCup."
rsync -vr $1@$robocup/lib/* $lib_dir/

echo "Copying audio files from RoboCup."
rsync -rv $1@$robocup/random/audio/* $audio_dir/
