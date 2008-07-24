#!/usr/bin/env python
"""
Script to output false data to a log file to be used for ball velocity
testing
    *Change values to BALL_DIST_VALUES and BALL_BEARING_VALUES to test various
     ball velocities
"""

__author__ = "Tucker Hermans"

import Constants
import outputEKF
import sys
from math import sqrt,atan2

NUM_ELEMENTS = 20

# Fake values of ball dist and angle
BALL_DIST_VALUES = []
BALL_BEARING_VALUES = []

ANGLE = 0
INIT_DIST = 10

for i in range(100):
     BALL_BEARING_VALUES.append(ANGLE)
     if i % 10 != 0:
 	BALL_DIST_VALUES.append(INIT_DIST+i)
     else:
 	BALL_DIST_VALUES.append(0)
#for i in range(300):
#    BALL_BEARING_VALUES.append(0)
#    BALL_DIST_VALUES.append(0)
#for i in range(800):
#    BALL_DIST_VALUES.append(150)
#    BALL_BEARING_VALUES.append(135)
#for i in range(30):
#    BALL_BEARING_VALUES.append(0)
#    BALL_DIST_VALUES.append(0)
for i in range(60):
    BALL_BEARING_VALUES.append(ANGLE)
    BALL_DIST_VALUES.append(INIT_DIST + 100)

for i in range(150):
   BALL_BEARING_VALUES.append(ANGLE+.5*100 - .5*i)
   if i % 15 != 0:
	BALL_DIST_VALUES.append(0)
   else:
	BALL_DIST_VALUES.append(0)
    
    
time_index = 0
frames_index = 1
x_change_index = 2
y_change_index = 3
a_change_index = 4
by_dist_index = 5
by_bearing_index = 6
yb_dist_index = 7
yb_bearing_index = 8
yglp_dist_index = 9
yglp_bearing_index = 10
ygrp_dist_index = 11
ygrp_bearing_index = 12
bglp_dist_index = 13
bglp_bearing_index = 14
bgrp_dist_index = 15
bgrp_bearing_index = 16
ball_dist_index = 17
ball_bearing_index = 18
#num_corners_index = 19

# Where the dog is on the field
DOG_X = Constants.CENTER_FIELD_X
DOG_Y = Constants.CENTER_FIELD_Y
DOG_H = Constants.OPP_GOAL_HEADING

# Odometry values
X_CHANGE = 0
Y_CHANGE = 0
A_CHANGE = 0

# Calculate distance to objects
BY_DIST = abs(Constants.LANDMARK_RIGHT_BEACON_X - DOG_X)
BY_BEARING = 90.

YB_DIST = abs(Constants.LANDMARK_LEFT_BEACON_X - DOG_X)
YB_BEARING = -90.

class LogFileLineMaker:
    def __init__(self):
        """
        Initialize the static values of the file
        """
        self.staticValues = list(range(NUM_ELEMENTS))
        self.staticValues[x_change_index] = X_CHANGE
        self.staticValues[y_change_index] = Y_CHANGE
        self.staticValues[a_change_index] = A_CHANGE
        
        self.staticValues[by_dist_index] = BY_DIST
        self.staticValues[by_bearing_index] = BY_BEARING
        self.staticValues[yb_dist_index] = YB_DIST
        self.staticValues[yb_bearing_index] = YB_BEARING
        
        self.staticValues[yglp_dist_index] = 0
        self.staticValues[yglp_bearing_index] = 0
        self.staticValues[ygrp_dist_index] = 0
        self.staticValues[ygrp_bearing_index] = 0
        self.staticValues[bgrp_dist_index] = 0
        self.staticValues[bgrp_bearing_index] = 0
        self.staticValues[bglp_dist_index] = 0
        self.staticValues[bglp_bearing_index] = 0

    def makeLine(self, time, frame, ballDist, ballBearing):
        """
        Makes a line based on ballX, ballY, frame count, and time count
        """
        newLine = self.staticValues[:] # Make a new copy of the list
        newLine[time_index] = time
        newLine[frames_index] = frame
        newLine[ball_dist_index] = ballDist
        newLine[ball_bearing_index] = ballBearing

        return newLine

# Number of miliseconds a frame
FRAME_MILLI_COUNT = 33
        
def main(*args):
   # simple input
    if len(args) != 1:
	print "USAGE: ./makeBallInfo.py [FILE]"
	return

#    print "Enter name of file to save data to:"

    outputFolder = './fakeBallPositions/'
    outFileName = str(args[0])
#    outFileName = sys.stdin.readline()

    filename = outputFolder+outFileName
    # opens output file
    outfile = open(filename, 'w')
    print "outfile:",outputFolder + outFileName

    # Setup initial values for the log file
    frameStart = 1000
    timeStart = 1
    myMaker = LogFileLineMaker()

    # In case we have mismatched ball data lists, we can still debug
    if len(BALL_DIST_VALUES) < len(BALL_BEARING_VALUES):
        frameCount = len(BALL_DIST_VALUES)
    else:
        frameCount = len(BALL_BEARING_VALUES)
    
    # First write the team color and player number to the file
    outfile.write("0 3\n")

    # For 100 frams have constant data, so that the dog can recover from restart
    for j in range(50):
        newLine = myMaker.makeLine(frameStart + j,
                                   timeStart + j*FRAME_MILLI_COUNT,
                                   INIT_DIST,ANGLE)
        
        for ind,val in enumerate(newLine):
            if ind == len(newLine)-1:
                outfile.write("%s\n" % (val))
            else:
                outfile.write("%s " % (val))
                
    for frame in range(frameCount):
        newLine = myMaker.makeLine(frameStart + (frame+100),
                                   timeStart + (frame+100)*FRAME_MILLI_COUNT,
                                   BALL_DIST_VALUES[frame],
                                   BALL_BEARING_VALUES[frame])
        
        for ind,val in enumerate(newLine):
            if ind == len(newLine)-1:
                outfile.write("%s\n" % (val))
            else:
                outfile.write("%s " % (val))

        
    outfile.close()
    print "Running outputEKF.py"
    outputEKF.main(outputFolder+outFileName)

if __name__ == '__main__':
    sys.exit(main(*sys.argv[1:]))
