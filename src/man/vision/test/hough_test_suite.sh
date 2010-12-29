#!/bin/bash

# First argument is the directory containing test images
IMAGE_DIR=$1

# Second argument gives location of OfflineVision executable
BIN_DIR=$2

ITERATIONS=500

for folder in "zero" "one" "couple"
do
    $BIN_DIR/OfflineVision $IMAGE_DIR/$folder 1 3 $ITERATIONS
done

$BIN_DIR/OfflineVision $IMAGE_DIR/"clutter" 1 6 $ITERATIONS

