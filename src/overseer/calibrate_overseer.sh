#!/bin/bash

SRC_BASE=$(pwd)
AUSTINVILLA_PACKAGE=$SRC_BASE/austinvilla
ROS_PACKAGE_PATH=$ROS_PACKAGE_PATH:$AUSTINVILLA_PACKAGE

roslaunch ground_truth calibrate.launch

