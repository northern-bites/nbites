#!/usr/bin/env python

# EKF offline log handler for the Nao

import sys

# Nao things
import EKF
import NogginConstants as Constants
from TypeDefs import *

OUTFILE = "out.tmp.log"
NUM_ELEMENTS = 21
DEBUG_OUTPUTTING = True
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
    
class LandmarkDebug:
    def __init__(self):
	self.used_landmarks = []
	self.ball = self.Ball()
	self.my = MyInfo()
        # Nao stuff
        self.motion = self.Motion()
        self.emptyVision = self.Vision(0.,0.,0,0)
        self.setupFieldObjects()

    def addUsedLandmark(self,landmark, isCorner=False):
	'''adds used landmark to list'''
	self.used_landmarks.append(landmark.visionId)
	self.used_landmarks.append(landmark.x)
	self.used_landmarks.append(landmark.y)
	self.used_landmarks.append(landmark.dist)
	self.used_landmarks.append(landmark.bearing)

    def getUsedLandmarks(self):
	return self.used_landmarks

    def resetUsedLandmarks(self):
	self.used_landmarks = []
	
    def sign(self, value):
	if value < 0:
	    return -1
	else:
	    return 1

    def setupFieldObjects(self):
        self.yglp = FieldObject(self.emptyVision, Constants.VISION_YGLP)
        self.ygrp = FieldObject(self.emptyVision, Constants.VISION_YGRP)
        self.bglp = FieldObject(self.emptyVision, Constants.VISION_BGLP)
        self.bgrp = FieldObject(self.emptyVision, Constants.VISION_BGRP)
        self.by = FieldObject(self.emptyVision, Constants.VISION_BY)
        self.yb = FieldObject(self.emptyVision, Constants.VISION_YB)

        self.makeFieldObjectsRelative()

        
    def makeFieldObjectsRelative(self):
        """
        Builds a list of fieldObjects based on their relative names to the robot
        Needs to be called when team color is determined
        """
        # Blue team setup
        if self.my.teamColor == Constants.TEAM_BLUE:
            # Yellow goal
            self.oppGoalRightPost = self.yglp
            self.oppGoalLeftPost = self.ygrp
            self.yglp.associateWithRelativeLandmark(
                Constants.LANDMARK_OPP_GOAL_RIGHT_POST)
            self.ygrp.associateWithRelativeLandmark(
                Constants.LANDMARK_OPP_GOAL_LEFT_POST)
            # Blue Goal
            self.myGoalLeftPost = self.bglp
            self.myGoalRightPost = self.bgrp
            self.bglp.associateWithRelativeLandmark(
                Constants.LANDMARK_MY_GOAL_LEFT_POST)
            self.bgrp.associateWithRelativeLandmark(
                Constants.LANDMARK_MY_GOAL_RIGHT_POST)

        # Yellow team setup
        else:
            # Yellow goal
            self.myGoalLeftPost = self.yglp
            self.myGoalRightPost = self.ygrp
            self.yglp.associateWithRelativeLandmark(
                Constants.LANDMARK_MY_GOAL_LEFT_POST)
            self.ygrp.associateWithRelativeLandmark(
                Constants.LANDMARK_MY_GOAL_RIGHT_POST)
            # Blue Goal
            self.oppGoalRightPost = self.bglp
            self.oppGoalLeftPost = self.bgrp
            self.bglp.associateWithRelativeLandmark(
                Constants.LANDMARK_OPP_GOAL_RIGHT_POST)
            self.bgrp.associateWithRelativeLandmark(
                Constants.LANDMARK_OPP_GOAL_LEFT_POST)

        # Build a list of all of the field objects with respect to team color
        self.myFieldObjects = [self.myGoalLeftPost, self.myGoalRightPost,
                               self.oppGoalLeftPost, self.oppGoalRightPost]

    class Motion:
        def __init__(self):
            self.currentHeadYaw = 0

        def getHeadYaw(self):
            return self.currentHeadYaw

    class Ball:
        def __init__(self):
            self.framesOn = 0

    class Vision:
        def __init__(self, visDist, visBearing, cert, distCert):
            self.centerX = 0
            self.centerY = 0
            self.width = 0
            self.height = 0
            self.focDist = 0
            self.dist = visDist
            self.bearing = visBearing
            self.certainty = cert
            self.distCertainty = distCert

def main(inFileName):
    # reads from input file
    infile = file(inFileName, 'r')

    # opens output file
    outfile = open(OUTFILE, 'w')
    outfile.write("NAO\n")
    print "outfile: ", OUTFILE

    # saves stdout
    saveout = sys.stdout
    
    # redirects stdout
    sys.stdout = outfile

    # instantiates debug class for used landmarks
    landmark_debug = LandmarkDebug()

    # reads all lines in input file, stores to list 'lines'
    lines = infile.readlines()

    # Figure out team color and player number
    basic_info = lines[0].split(" ")
    team_color = int(basic_info[0])
    player_number = int(basic_info[1])
    landmark_debug.my.playerNumber = player_number

    # Figure out init stats for the filter
    initStats = lines[1].split(" ")

    # instantiates ekf filter class
    test_filter = EKF.EKF(float(initStats[0]),
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
                          landmark_debug)        

    # Now we start the output for the standard info lines
    lines = lines[2:]

    for line in lines:
	log = line.split(" ")

	# this will catch logs that have been messed up
	if len(log) < NUM_ELEMENTS:
	    raise TypeError("outputEKF expected at least %i log values, got %i" 
                            % (NUM_ELEMENTS, len(log)), line)
   
	time_since_start = int(log[TIME_INDEX])
	frame_number = int(log[FRAME_INDEX])

	# Get our new sensor data to our fake brain
        landmark_debug.currentHeadYaw = float(log[HEAD_PAN_INDEX])

	# -Odometry data
	f_change = float(log[ODO_X_INDEX])
	l_change = float(log[ODO_Y_INDEX])
	a_change = float(log[ODO_H_INDEX])

	# Feed the log information to the offline filter
	# -Odometry information
        test_filter.updateOdometry((f_change, l_change, a_change))
	
	landmark_debug.resetUsedLandmarks()

	# Post information
	landmark_debug.yglp.updateVision(
            landmark_debug.Vision(float(log[YGLP_DIST_INDEX]),
                                  float(log[YGLP_BEARING_INDEX]),
                                  Constants.SURE,
                                  Constants.BOTH_SURE))

	landmark_debug.ygrp.updateVision(
            landmark_debug.Vision(float(log[YGRP_DIST_INDEX]),
                                  float(log[YGRP_BEARING_INDEX]),
                                  Constants.SURE,
                                  Constants.BOTH_SURE))
	landmark_debug.bgrp.updateVision(
            landmark_debug.Vision(float(log[BGRP_DIST_INDEX]),
                                  float(log[BGRP_BEARING_INDEX]),
                                  Constants.SURE,
                                  Constants.BOTH_SURE))
	landmark_debug.bglp.updateVision(
            landmark_debug.Vision(float(log[BGLP_DIST_INDEX]),
                                  float(log[BGLP_BEARING_INDEX]),
                                  Constants.SURE,
                                  Constants.BOTH_SURE))
	    
        # Now update the filter
        for landmark in landmark_debug.myFieldObjects:
            if landmark.dist > 0.:
                test_filter.sawSpecificLandmark(landmark)

	ball_dist = float(log[BALL_DIST_INDEX])
	ball_bearing = float(log[BALL_BEARING_INDEX])

	if ball_dist > 0:
	    landmark_debug.ball.framesOn += 1
	    test_filter.sawBall(ball_dist, ball_bearing)
	else:
	    landmark_debug.ball.framesOn = 0
	    test_filter.ballNotSeen()

	# -Corner information
	#number_of_corners_seen = int(log[NUM_CORNERS_INDEX])
	corner_raw_data = log[NUM_ELEMENTS:]
	#corner_parsed_data = []
	for corner in range(len(corner_raw_data)/3):
	    cid = int(corner_raw_data[3*corner])
	    dist = float(corner_raw_data[3*corner+1])
	    bearing = float(corner_raw_data[3*corner+2])

	    corner = Corner([cid,dist,bearing])

	    #create a bool called blueTeam that checks whether myGoal is yellow
            blueTeam = True
            if team_color == Constants.TEAM_RED:
                blueTeam = False

	    # convert vision types to constant based on team color
            # narrowly abstract types
            if corner.id == Constants.BLUE_GOAL_T:
                if blueTeam:
                    corner.id = Constants.MY_GOAL_T
                else:
                    corner.id = Constants.OPP_GOAL_T
            elif corner.id == Constants.YELLOW_GOAL_T: 
                if blueTeam:
                    corner.id = Constants.OPP_GOAL_T
                else:
                    corner.id = Constants.MY_GOAL_T
            elif corner.id == Constants.BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L:
                if blueTeam:
                    corner.id = Constants.MY_GOAL_RIGHT_L_OR_OPP_GOAL_LEFT_L
                else:
                    corner.id = Constants.MY_GOAL_LEFT_L_OR_OPP_GOAL_RIGHT_L
            elif corner.id == Constants.BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L:
                if blueTeam:
                    corner.id = Constants.MY_GOAL_LEFT_L_OR_OPP_GOAL_RIGHT_L
                else:
                    corner.id = Constants.MY_GOAL_RIGHT_L_OR_OPP_GOAL_LEFT_L
            elif corner.id == Constants.BLUE_GOAL_OUTER_L:
                if blueTeam:
                    corner.id = Constants.MY_GOAL_OUTER_L
                else:
                    corner.id = Constants.OPP_GOAL_OUTER_L
            elif corner.id == Constants.YELLOW_GOAL_OUTER_L:        
                if blueTeam:
                    corner.id = Constants.OPP_GOAL_OUTER_L
                else:
                    corner.id = Constants.MY_GOAL_OUTER_L

            ##### specific corners  #####

            ## BLUE GOAL 
            ## bg left, right Corner L's
            # assign blue corner left L to my corner left L or 
	    # opp corner right L
            # note: as far as I know, this type cannot be recognized, but 
	    # instead inferred -- ie, if you see a corner, 
            # how do you see a goal too?
            elif corner.id == Constants.BLUE_CORNER_LEFT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_MY_CORNER_LEFT_L
                else:
                    corner.id = Constants.LANDMARK_OPP_CORNER_RIGHT_L # tricky
            elif corner.id == Constants.BLUE_CORNER_RIGHT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_MY_CORNER_RIGHT_L
                else:
                    corner.id = Constants.LANDMARK_OPP_CORNER_LEFT_L # tricky

            ## bg left, right goal L's
            # assign blue goal left L corner to my goal left L or opp goal right L
            elif corner.id == Constants.BLUE_GOAL_LEFT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_MY_GOAL_LEFT_L
                else:
                    corner.id = Constants.LANDMARK_OPP_GOAL_RIGHT_L #tricky
            # assign blue goal right L corner to my goal right L or opp goal left L
            elif corner.id == Constants.BLUE_GOAL_RIGHT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_MY_GOAL_RIGHT_L
                else:
                    corner.id = Constants.LANDMARK_OPP_GOAL_LEFT_L #tricky
            # assume center left is respective to blue team,
            # so keep orientation unless on red team
            elif corner.id == Constants.CENTER_YB_T:
                if blueTeam:
                    corner.id = Constants.LANDMARK_CENTER_LEFT_T
                else:
                    corner.id = Constants.LANDMARK_CENTER_RIGHT_T
            # assume center right is respective to blue team, so keep orientation
            # unless on red team
            elif corner.id == Constants.CENTER_BY_T:
                if blueTeam:
                    corner.id = Constants.LANDMARK_CENTER_RIGHT_T
                else:
                    corner.id = Constants.LANDMARK_CENTER_LEFT_T

            ## YELLOW GOAL 
            ## yg left, right Corner L's
            # assign yellow corner left L to my corner left L or opp corner right L
            # note: as far as I know, this type cannot be recognized, but instead
            # inferred -- ie, if you see a corner, how do you see a goal too?
            elif corner.id == Constants.YELLOW_CORNER_LEFT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_OPP_CORNER_RIGHT_L
                else:
                    corner.id = Constants.LANDMARK_MY_CORNER_LEFT_L
            elif corner.id == Constants.YELLOW_CORNER_RIGHT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_OPP_CORNER_LEFT_L # tricky
                else:
                    corner.id = Constants.LANDMARK_MY_CORNER_RIGHT_L

            ## yg left, right goal T's
            # assign yellow goal left T to my goal left T or opp goal right T
            elif corner.id == Constants.YELLOW_GOAL_LEFT_T:
                if blueTeam:
                    corner.id = Constants.LANDMARK_OPP_GOAL_RIGHT_T #tricky    
                else:
                    corner.id = Constants.LANDMARK_MY_GOAL_LEFT_T

            # assign yellow goal right T corner to my goal right L or opp goal left L
            elif corner.id == Constants.YELLOW_GOAL_RIGHT_T:
                if blueTeam:
                    corner.id = Constants.LANDMARK_OPP_GOAL_LEFT_T #tricky   
                else:
                    corner.id = Constants.LANDMARK_MY_GOAL_RIGHT_T

            ## yg left, right goal L's
            # assign yellow goal left L corner to my goal left L or opp goal right L
            elif corner.id == Constants.YELLOW_GOAL_LEFT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_OPP_GOAL_RIGHT_L #tricky
                else:
                    corner.id = Constants.LANDMARK_MY_GOAL_LEFT_L

            # assign yellow goal right L corner to my goal right L or opp goal left L
            elif corner.id == Constants.YELLOW_GOAL_RIGHT_L:
                if blueTeam:
                    corner.id = Constants.LANDMARK_OPP_GOAL_LEFT_L #tricky
                else:
                    corner.id = Constants.LANDMARK_MY_GOAL_RIGHT_L

	    #corner_parsed_data.append(corner.id)
	    #corner_parsed_data.append(corner.dist)
	    #corner_parsed_data.append(corner.bearing)
		    
	    # add corner to filter
	    test_filter.sawCorner(corner)

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
	    f_change,
	    l_change,
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
