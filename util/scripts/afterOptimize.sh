#!/bin/bash
# Script to take the PSO pickle file off a robot and put it on our
# server. Run this after you run the walk optimizer around.
#
# @see beforeOptimize.sh
# @date May 2011
# @author Nathan Merritt

USAGE="./afterOptimize.sh <robot ip address>"

ROBOT_IP=$1

if [ "$1" = "" ]; then
    echo "No Robot IP entered"
    echo $USAGE
    exit 1
fi

# modified for Istanbul
# commented out lines should be re-enabled at Bowdoin

#ROBOCUP_DIR=/mnt/research/robocup
ROBOCUP_DIR=/home/nbites/
PSO_DIR=gait-pso/swarms
PICKLE="PSO_pGaitLearner.pickle"

# fetch the gait file from the robot
FETCH_GAIT="scp nao@$ROBOT_IP:/home/nao/gaits/$PICKLE /tmp/"

#echo "Type username for RoboCup server, followed by [ENTER]"
#read username
username=nbites

echo "Fetching gait..."
echo $FETCH_GAIT
eval $FETCH_GAIT || exit 1

#SERVER=$username@robocup.bowdoin.edu
SERVER=$username@eisbaer.local

# back up the old gait file
NOW=`date "+%m_%d_%H_%M"`  # month_day_hour_minute

PICKLE_PATH="$ROBOCUP_DIR/$PSO_DIR/$PICKLE"
BACKUP="ssh $SERVER 'cp $PICKLE_PATH $PICKLE_PATH.$NOW'"

echo "Backing up the old swarm pickle file"
echo $BACKUP
eval $BACKUP || exit 1

# and put the gait onto the server
PUT_GAIT="scp /tmp/$PICKLE $SERVER:$ROBOCUP_DIR/$PSO_DIR/"

echo "Putting gait pickle back onto our server"
echo $PUT_GAIT
eval $PUT_GAIT || exit 1

echo "All done! Thanks for running the walk optimizer!"


