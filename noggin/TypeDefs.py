
from math import fabs, radians, sin, cos
import time

from . import NogginConstants as Constants
from .util.MyMath import (dist,
                          getRelativeBearing,
                          getRelativeVelocityX,
                          getRelativeVelocityY,
                          getRelativeX,
                          getRelativeY)


class MyInfo:
    '''
    Class just to store important information about me, containing
    -x -- x coordinate on the field, according to ekf
    -y -- y coordinate on the field, according to ekf
    -h -- heading on the field, according to ekf
    -uncertX -- uncertainty in the x axis, accoridng to ekf
    -uncertY -- uncertainty in the y axis, accoridng to ekf
    -uncertH -- uncertainty in heading, accoridng to ekf
    -name -- integer relating to one of the 12 nBites Aibos (see Constants.py)
    -number -- player number on team
    -teamNumber -- team number according to GameController
    -teamColor -- team color , either TEAM_BLUE or TEAM_RED (see Constants.py)
    '''
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.h = 0.0
        self.uncertX = 0.0
        self.uncertY = 0.0
        self.uncertH = 0.0
        self.name = 0
        self.playerNumber = 3
        self.teamNumber = 0
        self.teamColor = Constants.TEAM_RED
        self.penalized = False
        self.kicking = False

    def updateLoc(self, ekf):
        self.x = ekf.getXEst()
        self.y = ekf.getYEst()
        self.h = ekf.getHeadingEst()
        self.uncertX = ekf.getXUncert()
        self.uncertY = ekf.getYUncert()
        self.uncertH = ekf.getHeadingUncert()

    def __str__(self):
        return ("name: %s #%d on team: %d color: %s @ (%g,%g,%g) uncert: (%g,%g,%g)" %
		(self.name, self.playerNumber, self.teamNumber,
		 Constants.teamColorDict[self.teamColor], self.x, self.y,
		 self.h, self.uncertX, self.uncertY, self.uncertH))

class Ball:
    '''
    Class for holding all current Ball information, contains:

    -centerX,centerY -- center (x,y) coordinates of ball on image screen
    -angleX,angleY -- angles of center (x,y) to focal point in degrees
    -dist -- distance from center of the body to ball in cms
    -bearing -- angle in x-axis from center of body to ball
    -elevation -- angle in y-axis from center of body to ball
    -framesOn -- # of consecutive frames the ball has been recognized in vision
    -framesOff -- # of consecutive frames the ball has been not recognized
    NOTE: if framesOn > 0, framesOff == 0, and vice versa
    -x,y -- (x,y) coordinate of ball in cms of dog on field via ekf
    -uncertX,uncertY -- uncertainty in x,y axis from ekf
    -velX,velY -- velocity in x,y axis of ball from ekf
    -locDist -- euclidian distance between (x,y) of self to (x,y) of ball (ekf)
    -locBearing -- relative bearing between self (x,y) and ball (x,y) via ekf
    -on -- simple bool if ball is on vision frame or not
    '''
    def __init__(self, visionBall):
        (self.centerX,
         self.centerY,
         self.width,
         self.height,
         self.angleX,
         self.angleY,
         self.focDist,
         self.confidence,
         self.dist,
         self.bearing,
         self.elevation,
         self.framesOn,
         self.framesOff,
         self.prevFramesOn,
         self.prevFramesOff,
         self.x,
         self.y,
         self.uncertX,
         self.uncertY,
         self.sd,
         self.velX,
         self.velY,
         self.uncertVelX,
         self.uncertVelY,
         self.locDist,
         self.locBearing,
         self.locRelX,
         self.locRelY, # loc based
         self.relX, # vision based
         self.relY,
         self.relVelX,
         self.relVelY,
         self.lastVisionDist,
         self.lastVisionBearing,
         self.lastVisionCenterX,
         self.lastVisionCenterY,
         self.lastVisionAngleX,
         self.lastVisionAngleY,
         self.lastTimeSeen,
         self.on) = [0]*Constants.NUM_TOTAL_BALL_VALUES

        self.updateVision(visionBall)

    def updateVision(self,visionBall):
        '''update method gets list of vision updated information'''
        # Hold our history
        self.lastVisionDist = self.dist
        self.lastVisionBearing = self.bearing
        self.lastVisionCenterX = self.centerX
        self.lastVisionCenterY = self.centerY
        self.lastVisionAngleX = self.angleX
        self.lastVisionAngleY = self.angleY

        # Now update to the new stuff
        self.centerX = visionBall.centerX
        self.centerY = visionBall.centerY
        self.width = visionBall.width
        self.height = visionBall.height
        self.focDist = visionBall.focDist
        self.dist = visionBall.dist
        self.bearing = visionBall.bearing
        self.elevation = visionBall.elevation
        self.confidence = visionBall.confidence

        # set angleX, angleY so that we don't create c->python object overhead
        if self.dist > 0:
            self.angleX = (((Constants.IMAGE_WIDTH/2.-1) - self.centerX)/
                           Constants.IMAGE_ANGLE_X)
            self.angleY = (((Constants.IMAGE_HEIGHT/2.-1) - self.centerY)/
                           Constants.IMAGE_ANGLE_Y)

            # records last known visual center x, ys and stuff
            self.lastVisionCenterX = self.centerX
            self.lastVisionCenterY = self.centerY
            self.lastVisionAngleX = self.angleX
            self.lastVisionAngleY = self.angleY
            self.lastVisionDist = self.dist
            self.lastVisionBearing = self.bearing
            self.reportBallSeen()
            if not self.on:
            self.prevFramesOff = self.framesOff
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
        else:
            self.angleX = 0
            self.angleY = 0
            if self.on:
            self.prevFramesOn = self.framesOn
            self.on = False
            self.framesOff += 1
            self.framesOn = 0

            self.relX = getRelativeX(self.dist, self.bearing)
            self.relY = getRelativeY(self.dist, self.bearing)

    def reportBallSeen(self):
        """
        Reset the time since seen.  Happens when we see a ball or when
        a teammate tells us he did.
        """
        self.lastTimeSeen = time.time()

    def timeSinceSeen(self):
        """
        Update the time since we last saw a ball
        """
        return time.time() - self.lastTimeSeen

    def updateLoc(self, ekf):
        """
        Update all of our inforamtion pased on the newest localization info
        """
        # Get latest estimates
        self.x = ekf.getBallXEst()
        self.y = ekf.getBallYEst()
        self.uncertX = ekf.getBallXUncert()
        self.uncertY = ekf.getBallYUncert()
        self.sd = ekf.getBallSD()
        self.velX = ekf.getBallXVelocityEst()
        self.velY = ekf.getBallYVelocityEst()
        self.uncertVelX = ekf.getBallXVelocityUncert()
        self.uncertVelY = ekf.getBallYVelocityUncert()

        # Determine other values
        self.locDist = dist(ekf.getXEst(), ekf.getYEst(), self.x, self.y)
        self.locBearing = getRelativeBearing(ekf.getXEst(), ekf.getYEst(),
                                             ekf.getHeadingEst(),
                                             self.x, self.y)
        self.locRelX = getRelativeX(self.locDist, self.locBearing)
        self.locRelY = getRelativeY(self.locDist, self.locBearing)
        self.relVelX = getRelativeVelocityX(ekf.getHeadingEst(),
                                            self.velX, self.velY)
        self.relVelY = getRelativeVelocityY(ekf.getHeadingEst(),
                                            self.velX, self.velY)

    def __str__(self):
        '''returns string with all class variables'''
        return ("vision dist: %g bearing: %g elevation: %g center: (%d,%d) aX/aY: %g/%g, framesOn: %d framesOff: %d on: %s\n loc: (%g,%g) uncert: (%g,%g) sd: %g vel (%g,%g) dist: %g bearing: %g" %
                (self.dist, self.bearing, self.elevation,
                 self.centerX, self.centerY, self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on,
                 self.x,self.y,self.uncertX,self.uncertY,self.sd,
                 self.velX,self.velY, self.locDist, self.locBearing))

class FieldObject:
    '''
    FieldObject is a class for field landmarks, storing visual information
    -landmarks:
    -yglp,ygrp,bglp,bgrp (yellow and blue goal left and right posts)
    -myGoalLeftPost,myGoalRightPost,oppGoalLeftPost,oppGoalRightPost
    -yellow-blue beacon, blue-yellow beacon

    contains:
    -centerX,centerY -- center (x,y) coordinates of object on image screen
    -width,height -- width and height of blob in pixels
    -angleX,angleY -- angles of center (x,y) to focal point in degrees
    -dist -- distance from center of the body to ball in cms
    -bearing -- angle in x-axis from center of body to field object
    -elevation -- angle in y-axis from center of body to field object
    -framesOn -- # of consecutive frames the obj has been recognized in vision
    -framesOff -- # of consecutive frames the obj has been not recognized
    -on -- simple bool if field object is on vision frame or not
    -x,y -- coordinates of the landmark on the field
    '''
    def __init__(self, visionInfos, visionName):
        '''initialization all values for FieldObject() class'''
        # Things to be filled out later
        self.x = 0
        self.y = 0
        self.localId = 0 # name based around team color
        self.locDist = 0
        self.locBearing = 0
        self.angleX = 0
        self.angleY = 0
        self.framesOn = 0
        self.framesOff = 0
        self.on = False

        # Setup the data from vision
        self.visionId = visionName
        self.updateVision(visionInfos)

    def associateWithRelativeLandmark(self, relativeLandmark):
        """
        Method to set the properties of the object relative to the team color
        """
        self.x = relativeLandmark[0]
        self.y = relativeLandmark[1]
        self.localId = relativeLandmark[2]

    def updateVision(self, visionInfos):
        '''updates class variables with new vision information'''
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.focDist = visionInfos.focDist
        self.dist = visionInfos.dist
        self.bearing = visionInfos.bearing
        self.certainty = visionInfos.certainty
        self.distCertainty = visionInfos.distCertainty

        # set angleX, angleY so that we don't create c->python object overhead
        if self.dist > 0:
            self.angleX = (((Constants.IMAGE_WIDTH/2-1) - self.centerX)/
                           Constants.IMAGE_ANGLE_X)
            self.angleY = (((Constants.IMAGE_HEIGHT/2-1) - self.centerY)/
                           Constants.IMAGE_ANGLE_Y)
        else:
            self.angleX = 0
            self.angleY = 0

        # obj is in this frame
        if self.dist > 0:
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
        # obj not in this frame
        else:
            self.on = False
            self.framesOff += 1
            self.framesOn = 0

    def __str__(self):
        '''returns string with all class variables'''
        return ("%s, %s at (%d,%d): dist: %g bearing: %g center: ""(%d,%d) w/h: %g/%g aX/aY: %g/%g, framesOn: %d framesOff: %d on: %s" %
                (Constants.visionObjectTuple[self.visionId],
                 Constants.landmarkTuple[self.localId],
                 self.x, self.y, self.dist, self.bearing,
                 self.centerX, self.centerY,
                 self.width, self.height, self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on))

class Corner:
    '''
    Class for one single corner, contains:
    -id -- this is an unique identifier for every type of corner
    -dist -- distance from center of body to corner in cm
    -bearing -- angle on x-axis from center of body to corner in degrees
    '''
    INVALID_FIELD_POSITION = -1

    def __init__(self, visionCorner):
        '''init method for corner class.'''
        self.x = self.INVALID_FIELD_POSITION
        self.y = self.INVALID_FIELD_POSITION
        self.updateVision(visionCorner)
        self.locID = None

    # updates corner info from vision
    def updateVision(self,visionCorner):
        #self.visionId = visionCorner.id
        self.dist = visionCorner.dist
        self.bearing = visionCorner.bearing
        self.possibilities = visionCorner.possibilities
        self.locID = None

    def getData(self):
        '''returns [possibilities, dist, bearing]'''
        return [self.possiblities,self.dist,self.bearing]

    def __str__(self):
        '''returns string of all values in class -- can just print corner'''
        return ("possibilities: %g dist: %g bearing: %g" % (self.possibilities,
                                                            self.dist,
                                                            self.bearing))

class Line:
    '''
    class for one single line, contains:
    -x1,y1 -- left coordinate on image screen
    -x2,y2 -- right coordinate on image screen
    -slope -- slope of line. remember that (0,0) is top-left corner of image
    -length -- length of the line
    '''
    def __init__(self, visionLine):
        '''init method'''
        self.x1 = visionLine.x1
        self.y1 = visionLine.y1
        self.x2 = visionLine.x2
        self.y2 = visionLine.y2
        self.slope = visionLine.slope
        self.length = visionLine.length

    def __str__(self):
        return ("p1: (%g,%g) p2: (%g,%g) slope: %g length: %g" %
                (self.x1,self.y1,self.x2,self.y1,self.slope,self.length))


