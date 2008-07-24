#!/usr/bin/env python

__author__ = "Henry Work and Jeremy Fishman"
__version__ = "$Revision: 0.1 $"

# outputEKF -- this takes a log file created on t3h dog and runs it through
# the EKF residing in pycode/NewEKFLocalize.py and then pipes standard out
# into a new text file which can then be viewed in java's WorldController
# --can handle both log file format with old and new goals

import sys
import EKF
import Constants
from TypeDefs import *

NUM_ELEMENTS = 21

(TIME_INDEX,
 FRAME_INDEX,
 ODO_X_INDEX,
 ODO_Y_INDEX,
 ODO_H_INDEX,
 BY_DIST_INDEX,
 BY_BEARING_INDEX,
 YB_DIST_INDEX,
 YB_BEARING_INDEX,
 YGLP_DIST_INDEX,
 YGLP_BEARING_INDEX,
 YGRP_DIST_INDEX,
 YGRP_BEARING_INDEX,
 BGLP_DIST_INDEX,
 BGLP_BEARING_INDEX,
 BGRP_DIST_INDEX,
 BGRP_BEARING_INDEX,
 BALL_DIST_INDEX,
 BALL_BEARING_INDEX,
 PWM_VALUE_INDEX,
 HEAD_PAN_INDEX) = range(NUM_ELEMENTS)

OUTFILE = "out.tmp.log"

class LandmarkDebug:
    def __init__(self):
	self.used_landmarks = []
	self.ball = self.Ball()
	self.my = MyInfo()
	self.sensorValues = range(Constants.NUM_PY_SENSORS)
	self.lastJoints = [0]*(Constants.HEAD_PAN+1)
        self.goalieInGoal = False

    def addUsedLandmark(self,landmark, isCorner=False):
	'''adds used landmark to list'''
        if not isCorner:
            self.used_landmarks.append(landmark.id)
        else:
            # TODO,FIXME: Hard coded ID for the new type of corner
            # I will eventually remove all the old constants concerning corners
            # and add this new thing as the only type of corner landmark.
            self.used_landmarks.append(-1)
	self.used_landmarks.append(landmark.x)
	self.used_landmarks.append(landmark.y)
	self.used_landmarks.append(landmark.dist)
	self.used_landmarks.append(landmark.bearing)

    def getUsedLandmarks(self):
	return self.used_landmarks

    def resetUsedLandmarks(self):
	self.used_landmarks = []

    def reportBallSeen(self):
        #self.time_of_last_ball_sighting = self.getTime()
        return

    def sign(self, value):
	if value < 0:
	    return -1
	else:
	    return 1

    class Ball:
	def __init__(self):
	    self.framesOn = 0

def main(inFileName):
    # reads from input file
    infile = file(inFileName, 'r')

    # opens output file
    outfile = open(OUTFILE, 'w')
    outfile.write("AIBO\n")
    print "outfile: ", OUTFILE

    # saves stdout
    saveout = sys.stdout

    # redirects stdout
    sys.stdout = outfile

    # instantiates debug class for used landmarks
    landmark_debug = LandmarkDebug()

    # reads all lines in input file, stores to list 'lines'
    lines = infile.readlines()

    basic_info = lines[0].split(" ")
    team_color = int(basic_info[0])
    player_number = int(basic_info[1])
    landmark_debug.my.playerNumber = player_number

    # Figure out init stats for the filter
    initStats = lines[1].split(" ")

    # instantiates ekf filter class
    test_filter = EKF.EKFLocalize(float(initStats[0]),
                                  float(initStats[1]),
                                  float(initStats[2]),
                                  float(initStats[3]),
                                  float(initStats[4]),
                                  float(initStats[5]),
                                  float(initStats[6]),
                                  float(initStats[7]),
                                  float(initStats[8]),
                                  float(initStats[9]),
                                  float(initStats[10]),
                                  float(initStats[11]),
                                  float(initStats[12]),
                                  float(initStats[13]),
                                  float(initStats[14]),
                                  landmark_debug)

    # set up landmarks
    myGoalLeftPost = FieldObject(Constants.LANDMARK_MY_GOAL_LEFT_POST_X,
				      Constants.LANDMARK_MY_GOAL_LEFT_POST_Y,
				      Constants.LANDMARK_MY_GOAL_LEFT_POST)
    myGoalRightPost = FieldObject(Constants.LANDMARK_MY_GOAL_RIGHT_POST_X,
				       Constants.LANDMARK_MY_GOAL_RIGHT_POST_Y,
				       Constants.LANDMARK_MY_GOAL_RIGHT_POST)
    oppGoalLeftPost = FieldObject(Constants.LANDMARK_OPP_GOAL_LEFT_POST_X,
				       Constants.LANDMARK_OPP_GOAL_LEFT_POST_Y,
				       Constants.LANDMARK_OPP_GOAL_LEFT_POST)
    oppGoalRightPost = FieldObject(Constants.LANDMARK_OPP_GOAL_RIGHT_POST_X,
				   Constants.LANDMARK_OPP_GOAL_RIGHT_POST_Y,
				   Constants.LANDMARK_OPP_GOAL_RIGHT_POST)

    leftBeacon = FieldObject(Constants.LANDMARK_LEFT_BEACON_X,
				  Constants.LANDMARK_LEFT_BEACON_Y,
				  Constants.LANDMARK_LEFT_BEACON)
    rightBeacon = FieldObject(Constants.LANDMARK_RIGHT_BEACON_X,
				   Constants.LANDMARK_RIGHT_BEACON_Y,
				   Constants.LANDMARK_RIGHT_BEACON)

    # Set lines to be the remaining lines
    lines = lines[2:]

    for line in lines:
	log = line.split(" ")
	# this is for merged logs with camera lines
	# stops the filter for this line
	if log[0] == 'camera:':
	    outfile.write(line)
	    continue

	# this will catch logs that have been messed up
	if len(log) < NUM_ELEMENTS:
	    raise TypeError("outputEKF expected at least %i log values, got %i" % (NUM_ELEMENTS, len(log)), line)

        if log[0] == "INGOAL":
            landmark_debug.goalieInGoal = True
            log = log[1:]
        else:
            landmark_debug.goalieInGoal = False

	time_since_start = int(log[TIME_INDEX])
	frame_number = int(log[FRAME_INDEX])

	# Get our new sensor data to our fake brain
	landmark_debug.sensorValues[Constants.PAN_PWM] = \
	    float(log[PWM_VALUE_INDEX])
	landmark_debug.lastJoints[Constants.HEAD_PAN] = \
	    float(log[HEAD_PAN_INDEX])

	# -Odometry data
	x_change = float(log[ODO_X_INDEX])
	y_change = float(log[ODO_Y_INDEX])
	a_change = float(log[ODO_H_INDEX])

	# Feed the log information to the offline filter
	# -Odometry information
	test_filter.updateOdometry(x_change, y_change, a_change)

	landmark_debug.resetUsedLandmarks()

	# -Post information
	by_dist = float(log[BY_DIST_INDEX])
	by_bearing = float(log[BY_BEARING_INDEX])
	yb_dist = float(log[YB_DIST_INDEX])
	yb_bearing = float(log[YB_BEARING_INDEX])
	yglp_dist = float(log[YGLP_DIST_INDEX])
	yglp_bearing = float(log[YGLP_BEARING_INDEX])
	ygrp_dist = float(log[YGRP_DIST_INDEX])
	ygrp_bearing = float(log[YGRP_BEARING_INDEX])
	bglp_dist = float(log[BGLP_DIST_INDEX])
	bglp_bearing = float(log[BGLP_BEARING_INDEX])
	bgrp_dist = float(log[BGRP_DIST_INDEX])
	bgrp_bearing = float(log[BGRP_BEARING_INDEX])

	# setup landmarks based on team color
	if team_color == Constants.TEAM_BLUE: # blue team object sorting
	    myGoalLeftPost.dist = bglp_dist
	    myGoalLeftPost.bearing = bglp_bearing
	    myGoalRightPost.dist = bgrp_dist
	    myGoalRightPost.bearing = bgrp_bearing
	    oppGoalLeftPost.dist = yglp_dist
	    oppGoalLeftPost.bearing = yglp_bearing
	    oppGoalRightPost.dist = ygrp_dist
	    oppGoalRightPost.bearing = ygrp_bearing
	    leftBeacon.dist = yb_dist
	    leftBeacon.bearing = yb_bearing
	    rightBeacon.dist = by_dist
	    rightBeacon.bearing = by_bearing
	else:
	    myGoalLeftPost.dist = yglp_dist
	    myGoalLeftPost.bearing = yglp_bearing
	    myGoalRightPost.dist = ygrp_dist
	    myGoalRightPost.bearing = ygrp_bearing
	    oppGoalLeftPost.dist = bglp_dist
	    oppGoalLeftPost.bearing = bglp_bearing
	    oppGoalRightPost.dist = bgrp_dist
	    oppGoalRightPost.bearing = bgrp_bearing
	    leftBeacon.dist = by_dist
	    leftBeacon.bearing = by_bearing
	    rightBeacon.dist = yb_dist
	    rightBeacon.bearing = yb_bearing

	myGoalLeftPost.certainty = Constants.SURE
	myGoalRightPost.certainty = Constants.SURE
	oppGoalLeftPost.certainty = Constants.SURE
	oppGoalRightPost.certainty = Constants.SURE
	leftBeacon.certainty = Constants.SURE
	rightBeacon.certainty = Constants.SURE

	myGoalLeftPost.distCertainty = Constants.BOTH_SURE
	myGoalRightPost.distCertainty = Constants.BOTH_SURE
	oppGoalLeftPost.distCertainty = Constants.BOTH_SURE
	oppGoalRightPost.distCertainty = Constants.BOTH_SURE
	leftBeacon.distCertainty = Constants.BOTH_SURE
	rightBeacon.distCertainty = Constants.BOTH_SURE

	# add landmarks into filter
	if myGoalLeftPost.dist > 0:
	    test_filter.sawSpecificLandmark(myGoalLeftPost)
	if myGoalRightPost.dist > 0:
	    test_filter.sawSpecificLandmark(myGoalRightPost)
	if oppGoalLeftPost.dist > 0:
	    test_filter.sawSpecificLandmark(oppGoalLeftPost)
	if oppGoalRightPost.dist > 0:
	    test_filter.sawSpecificLandmark(oppGoalRightPost)
	if leftBeacon.dist > 0:
	    test_filter.sawSpecificLandmark(leftBeacon)
	if rightBeacon.dist > 0:
	    test_filter.sawSpecificLandmark(rightBeacon)

	ball_dist = float(log[BALL_DIST_INDEX])
	ball_bearing = float(log[BALL_BEARING_INDEX])

	if ball_dist > 0:
	    landmark_debug.ball.framesOn += 1
	    test_filter.sawBall(ball_dist,ball_bearing)
	else:
	    landmark_debug.ball.framesOn = 0
	    test_filter.ballNotSeen()

	# -Corner information
        corners = []
	corner_raw_data = log[NUM_ELEMENTS:]

        while len(corner_raw_data):
            possibilities = []
            if corner_raw_data.pop(0) == 'c':
                dist = float(corner_raw_data.pop(0))
                bearing = float(corner_raw_data.pop(0))
                c = [0]*2
                c[0] = int(corner_raw_data.pop(0))
                c[1] = int(corner_raw_data.pop(0))
                possibilities.append(c)
            else:
                dist = float(corner_raw_data.pop(0))
                bearing = float(corner_raw_data.pop(0))
                while(len(corner_raw_data) > 0 and
                      (corner_raw_data[0] != 'a' and
                       corner_raw_data[0] != 'c')):
                    a = [0]*2
                    a[0] = int(corner_raw_data.pop(0))
                    a[1] = int(corner_raw_data.pop(0))
                    possibilities.append(a)

            corner = Corner(possibilities, dist, bearing)
	    #create a bool called blueTeam that checks whether myGoal is yellow
            blueTeam = True
            if team_color == Constants.TEAM_RED:
                blueTeam = False

	    #if blueTeam:
	    #	print "debug: BLUE_TEAM"
	    #else:
 	    #	print "debug: RED_TEAM"

# 	    # convert vision types to constant based on team color
#             # narrowly abstract types
#             if corner.visionID == Constants.BLUE_GOAL_T:
#                 if blueTeam:
#                     corner.visionID = Constants.MY_GOAL_T
#                 else:
#                     corner.visionID = Constants.OPP_GOAL_T
#             elif corner.visionID == Constants.YELLOW_GOAL_T:
#                 if blueTeam:
#                     corner.visionID = Constants.OPP_GOAL_T
#                 else:
#                     corner.visionID = Constants.MY_GOAL_T
#             elif corner.visionID == Constants.BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.MY_GOAL_RIGHT_L_OR_OPP_GOAL_LEFT_L
#                 else:
#                     corner.visionID = Constants.MY_GOAL_LEFT_L_OR_OPP_GOAL_RIGHT_L
#             elif corner.visionID == Constants.BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.MY_GOAL_LEFT_L_OR_OPP_GOAL_RIGHT_L
#                 else:
#                     corner.visionID = Constants.MY_GOAL_RIGHT_L_OR_OPP_GOAL_LEFT_L
#             elif corner.visionID == Constants.BLUE_GOAL_OUTER_L:
#                 if blueTeam:
#                     corner.visionID = Constants.MY_GOAL_OUTER_L
#                 else:
#                     corner.visionID = Constants.OPP_GOAL_OUTER_L
#             elif corner.visionID == Constants.YELLOW_GOAL_OUTER_L:
#                 if blueTeam:
#                     corner.visionID = Constants.OPP_GOAL_OUTER_L
#                 else:
#                     corner.visionID = Constants.MY_GOAL_OUTER_L

#             #
#             # goalie corner check
#             #
# 		    '''
#             if self.limitCornerInformation and self.my.playerNumber == 1:
#               if corner.visionID == Constants.BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L:
#                   if blueTeam:
#                       corner.visionID = Constants.BLUE_GOAL_RIGHT_L
#                   else:
#                       corner.visionID = Constants.YELLOW_GOAL_LEFT_L
#             elif corner.visionID == Constants.BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.BLUE_GOAL_LEFT_L
#                 else:
#                     corner.visionID = Constants.YELLOW_GOAL_RIGHT_L
# 		    '''
#             ##### specific corners  #####

#             ## BLUE GOAL 
#             ## bg left, right Corner L's
#             # assign blue corner left L to my corner left L or 
# 	    # opp corner right L
#             # note: as far as I know, this type cannot be recognized, but 
# 	    # instead inferred -- ie, if you see a corner, 
#             # how do you see a goal too?
#             elif corner.visionID == Constants.BLUE_CORNER_LEFT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_MY_CORNER_LEFT_L
#                 else:
#                     corner.visionID = Constants.LANDMARK_OPP_CORNER_RIGHT_L # tricky
#             elif corner.visionID == Constants.BLUE_CORNER_RIGHT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_MY_CORNER_RIGHT_L
#                 else:
#                     corner.visionID = Constants.LANDMARK_OPP_CORNER_LEFT_L # tricky

#             ## bg left, right goal L's
#             # assign blue goal left L corner to my goal left L or opp goal right L
#             elif corner.visionID == Constants.BLUE_GOAL_LEFT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_MY_GOAL_LEFT_L
#                 else:
#                     corner.visionID = Constants.LANDMARK_OPP_GOAL_RIGHT_L #tricky
#             # assign blue goal right L corner to my goal right L or opp goal left L
#             elif corner.visionID == Constants.BLUE_GOAL_RIGHT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_MY_GOAL_RIGHT_L
#                 else:
#                     corner.visionID = Constants.LANDMARK_OPP_GOAL_LEFT_L #tricky
#             # assume center left is respective to blue team,
#             # so keep orientation unless on red team
#             elif corner.visionID == Constants.CENTER_YB_T:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_CENTER_LEFT_T
#                 else:
#                     corner.visionID = Constants.LANDMARK_CENTER_RIGHT_T
#             # assume center right is respective to blue team, so keep orientation
#             # unless on red team
#             elif corner.visionID == Constants.CENTER_BY_T:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_CENTER_RIGHT_T
#                 else:
#                     corner.visionID = Constants.LANDMARK_CENTER_LEFT_T

#             ## YELLOW GOAL 
#             ## yg left, right Corner L's
#             # assign yellow corner left L to my corner left L or opp corner right L
#             # note: as far as I know, this type cannot be recognized, but instead
#             # inferred -- ie, if you see a corner, how do you see a goal too?
#             elif corner.visionID == Constants.YELLOW_CORNER_LEFT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_OPP_CORNER_RIGHT_L
#                 else:
#                     corner.visionID = Constants.LANDMARK_MY_CORNER_LEFT_L
#             elif corner.visionID == Constants.YELLOW_CORNER_RIGHT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_OPP_CORNER_LEFT_L # tricky
#                 else:
#                     corner.visionID = Constants.LANDMARK_MY_CORNER_RIGHT_L

#             ## yg left, right goal T's
#             # assign yellow goal left T to my goal left T or opp goal right T
#             elif corner.visionID == Constants.YELLOW_GOAL_LEFT_T:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_OPP_GOAL_RIGHT_T #tricky    
#                 else:
#                     corner.visionID = Constants.LANDMARK_MY_GOAL_LEFT_T

#             # assign yellow goal right T corner to my goal right L or opp goal left L
#             elif corner.visionID == Constants.YELLOW_GOAL_RIGHT_T:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_OPP_GOAL_LEFT_T #tricky   
#                 else:
#                     corner.visionID = Constants.LANDMARK_MY_GOAL_RIGHT_T

#             ## yg left, right goal L's
#             # assign yellow goal left L corner to my goal left L or opp goal right L
#             elif corner.visionID == Constants.YELLOW_GOAL_LEFT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_OPP_GOAL_RIGHT_L #tricky
#                 else:
#                     corner.visionID = Constants.LANDMARK_MY_GOAL_LEFT_L

#             # assign yellow goal right L corner to my goal right L or opp goal left L
#             elif corner.visionID == Constants.YELLOW_GOAL_RIGHT_L:
#                 if blueTeam:
#                     corner.visionID = Constants.LANDMARK_OPP_GOAL_LEFT_L #tricky
#                 else:
#                     corner.visionID = Constants.LANDMARK_MY_GOAL_RIGHT_L

	    #corner_parsed_data.append(corner.visionID)
	    #corner_parsed_data.append(corner.dist)
	    #corner_parsed_data.append(corner.bearing)

	    # add corner to cornersList
            corners.append(corner)

        test_filter.sawCorners(corners)
	# Get the offline filters estimates
#	test_filter.changePreviousBallInformation()
	computed_x_estimate = test_filter.getXEst()
	computed_y_estimate = test_filter.getYEst()
	computed_h_estimate = test_filter.getHeadingEst()
	computed_x_uncert = test_filter.getXUncert()
	computed_y_uncert = test_filter.getYUncert()
	computed_h_uncert = test_filter.getHeadingUncert()
	ball_x_est = test_filter.getBallXEst()
	ball_y_est = test_filter.getBallYEst()
	ball_x_uncert = test_filter.getBallXUncert()
	ball_y_uncert = test_filter.getBallYUncert()
	ball_x_velocity_est = test_filter.getBallXVelocityEst()
	ball_y_velocity_est = test_filter.getBallYVelocityEst()
	ball_x_velocity_uncert = test_filter.getBallXVelocityUncert()
	ball_y_velocity_uncert = test_filter.getBallYVelocityUncert()

	# Record to the outupt log all of the logged vision information and
	# the offline filter's results
	output_string_data_list = [
	    time_since_start,
	    frame_number,
	    team_color,
	    player_number,
	    computed_x_estimate,
	    computed_y_estimate,
	    computed_h_estimate,
	    computed_x_uncert,
	    computed_y_uncert,
	    computed_h_uncert,
	    ball_x_est,
	    ball_y_est,
	    ball_x_uncert,
	    ball_y_uncert,
	    ball_x_velocity_est,
	    ball_y_velocity_est,
	    ball_x_velocity_uncert,
	    ball_y_velocity_uncert,
	    ball_dist,
	    ball_bearing,
	    x_change,
	    y_change,
	    a_change]

	output_string_data_list += landmark_debug.getUsedLandmarks()

        #output_string_data_list += corner_parsed_data

	#for corner_data_list in corners_seen_list:
	#    output_string_data_list += corner_data_list
	#output_string_data_list = [repr(data) for data in 
	#			   output_string_data_list]
	list = [float(elem) for elem in output_string_data_list]
	string = getListAsShortFloatString(list)

	#outfile.write(' '.join(output_string_data_list) + '\n')
	outfile.write(string + '\n')

    # directs stdout back to where it should be
    sys.stdout = saveout

def getListAsShortFloatString(list):
    '''returns a nicely formatted list of shortened floats'''
    return " ".join([("%g" % (elem)) for elem in list])

if __name__=='__main__':
    sys.exit(main(*sys.argv[1:]))
