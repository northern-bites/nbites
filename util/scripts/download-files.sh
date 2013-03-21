
# Script to download the extra libraries needed to run the Northern Bites man package on the Aldebaran Nao

if [ $# -ne 1 ]; then
    echo "Usage: ./download-libs.sh <Bowdoin username>"
    exit 1
fi

USERNAME=$1
ROBOCUP=robocup.bowdoin.edu:/mnt/research/robocup

lib_dir=../../lib
audio_dir=../audio

echo "Copying libraries from RoboCup."
rsync -vr USERNAME@$ROBOCUP/lib/* $lib_dir/

rm $audio_dir/*

echo "Copying audio files from RoboCup."
rsync -rv $1@$robocup/audio_for_robot/* $audio_dir/
