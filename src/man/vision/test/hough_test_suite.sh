#!/bin/bash

if [ $# != 2 ]; then
    echo "Usage: ./hough_test_suite.sh <test image directory> <OffflineVision exec. directory>"
    exit
fi

# First argument is the directory containing test images
IMAGE_DIR=$1

# Second argument gives location of OfflineVision executable
BIN_DIR=$2

ITERATIONS=500

for folder in "zero" "one" "couple" "clutter"
do
    NUM_IMAGES=`find $IMAGE_DIR/$folder/ -name *.NBFRM | wc -l`
    echo $NUM_IMAGES
    $BIN_DIR/OfflineVision $IMAGE_DIR/$folder 1 $NUM_IMAGES $ITERATIONS
done



