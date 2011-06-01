#!/bin/bash
# Script to fetch the latest PSO pickle file from our server
# and install it onto your robot. Run this before you 'make install'
# pGaitLearner.
#
# This will work best if you have already set up automatic robot login
# using the setup-autologin script in this directory.
#
# @date May 2011
# @author Nathan Merritt

USAGE="./beforeOptimize.sh <robot ip address>"

ROBOT_IP=$1

if [ "$1" = "" ]; then
    echo "No Robot IP entered"
    echo $USAGE
    exit 1
fi

echo "Type username for RoboCup server, followed by [ENTER]"
read username

SERVER=$username@robocup.bowdoin.edu
ROBOCUP_DIR=/mnt/research/robocup
PSO_DIR=gait-pso/swarms
PICKLE="PSO_pGaitLearner.pickle"

# fetch the gait file from our server
FETCH_GAIT="scp ${SERVER}:$ROBOCUP_DIR/$PSO_DIR/$PICKLE /tmp/"

echo "Fetching gait..."
echo $FETCH_GAIT
$FETCH_GAIT

# make /home/nao/gaits folder on the robot
MAKE_DIR="ssh nao@$ROBOT_IP 'mkdir /home/nao/gaits/'"

echo $MAKE_DIR
$MAKE_DIR

# and put the gait onto the robot
PUT_GAIT="scp /tmp/$PICKLE nao@$ROBOT_IP:/home/nao/gaits/"

echo "Putting gait pickle onto robot"
echo $PUT_GAIT
$PUT_GAIT

echo "All done! Go ahead and 'make install' now and run the optimizer"
echo "Remember to run afterOptimize.sh when you're finished"
