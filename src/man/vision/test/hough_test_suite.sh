#!/bin/bash

if [ $# != 2 ]; then
    echo "Usage: ./hough_test_suite.sh <test image directory> <OffflineVision exec. directory>"
    exit
fi

# First argument is the directory containing test images
IMAGE_DIR=$1

# Second argument gives location of OfflineVision executable
BIN_DIR=$2

ITERATIONS=200

for folder in "zero" "one" "couple" "clutter"
do
    OUT_FILE=$folder"_timing_results.txt"
    echo $OUT_FILE
    NUM_IMAGES=`find $IMAGE_DIR/$folder/ -name *.frm | wc -l`
    $BIN_DIR/OfflineVision $IMAGE_DIR/$folder 1 $NUM_IMAGES $ITERATIONS > $OUT_FILE
done

# echo "Done running tests, creating graphs!"

# Create graphs
# R CMD BATCH hough_graphs.R

echo "Done!"
