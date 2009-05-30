import time

from . import NogginConstants as Constants
from .util.MyMath import (dist,
                          getRelativeBearing,
                          getRelativeVelocityX,
                          getRelativeVelocityY,
                          getRelativeX,
                          getRelativeY,
                          sub180Angle)


class MyInfo:
    """
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
    """
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
        self.teamColor = Constants.TEAM_BLUE
        self.penalized = False
        self.kicking = False

    def updateLoc(self, loc):
        if self.teamColor == Constants.TEAM_BLUE:
            self.x = loc.x
            self.y = loc.y
            self.h = loc.h
        else:
            self.x = Constants.FIELD_GREEN_WIDTH - loc.x
            self.y = Constants.FIELD_GREEN_HEIGHT - loc.y
            self.h = sub180Angle(loc.h - 180)
        self.uncertX = loc.xUncert
        self.uncertY = loc.yUncert
        self.uncertH = loc.hUncert

    def __str__(self):
        return ("name: %s #%d on team: %d color: %s @ (%g,%g,%g) uncert: (%g,%g,%g)" %
                (self.name, self.playerNumber, self.teamNumber,
                 Constants.teamColorDict[self.teamColor], self.x, self.y,
                 self.h, self.uncertX, self.uncertY, self.uncertH))

class Ball:
    """
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
    """
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
         self.lastSeenDist,
         self.lastSeenBearing,
         self.on) = [0]*Constants.NUM_TOTAL_BALL_VALUES

        self.updateVision(visionBall)

    def updateVision(self,visionBall):
        """update method gets list of vision updated information"""
        # Hold our history
        self.lastVisionDist = self.dist
        self.lastVisionBearing = self.bearing
        self.lastVisionCenterX = self.centerX
        self.lastVisionCenterY = self.centerY
        self.lastVisionAngleX = self.angleX
        self.lastVisionAngleY = self.angleY

        if self.dist > 0:
            self.lastSeenBearing = self.bearing
            self.lastSeenDist = self.dist

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

            self.reportBallSeen()
            if not self.on:
                self.prevFramesOff = self.framesOff
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
            self.relX = getRelativeX(self.dist, self.bearing)
            self.relY = getRelativeY(self.dist, self.bearing)
        else:
            self.angleX = 0
            self.angleY = 0
            if self.on:
                self.prevFramesOn = self.framesOn
            self.on = False
            self.framesOff += 1
            self.framesOn = 0
            self.relX = 0.0
            self.relY = 0.0

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
        return (time.time() - self.lastTimeSeen)

    def updateLoc(self, loc, my):
        """
        Update all of our inforamtion pased on the newest localization info
        """
        # Get latest estimates
        if my.teamColor == Constants.TEAM_BLUE:
            self.x = loc.ballX
            self.y = loc.ballY
            self.velX = loc.ballVelX
            self.velY = loc.ballVelY
        else:
            self.x = Constants.FIELD_GREEN_WIDTH - loc.ballX
            self.y = Constants.FIELD_GREEN_HEIGHT - loc.ballY
            self.velX = -loc.ballVelX
            self.velY = -loc.ballVelY

        self.uncertX = loc.ballXUncert
        self.uncertY = loc.ballYUncert
        self.uncertVelX = loc.ballVelXUncert
        self.uncertVelY = loc.ballVelYUncert
        self.sd = self.uncertX * self.uncertY

        # Determine other values
        self.locDist = dist(my.x, my.y, self.x, self.y)
        self.locBearing = getRelativeBearing(my.x, my.y, my.h,
                                             self.x, self.y)
        self.locRelX = getRelativeX(self.locDist, self.locBearing)
        self.locRelY = getRelativeY(self.locDist, self.locBearing)
        self.relVelX = getRelativeVelocityX(my.h, self.velX, self.velY)
        self.relVelY = getRelativeVelocityY(my.h, self.velX, self.velY)

    def __str__(self):
        """returns string with all class variables"""
        return ("vision dist: %g bearing: %g elevation: %g center: (%d,%d) aX/aY: %g/%g, framesOn: %d framesOff: %d on: %s\n loc: (%g,%g) uncert: (%g,%g) sd: %g vel (%g,%g) dist: %g bearing: %g" %
                (self.dist, self.bearing, self.elevation,
                 self.centerX, self.centerY, self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on,
                 self.x,self.y,self.uncertX,self.uncertY,self.sd,
                 self.velX,self.velY, self.locDist, self.locBearing))

class FieldObject:
    """
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
    """
    def __init__(self, visionInfos, visionName):
        """initialization all values for FieldObject() class"""
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
        """updates class variables with new vision information"""
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
        """returns string with all class variables"""
        return ("%s, %s at (%d,%d): dist: %g bearing: %g center: ""(%d,%d) w/h: %g/%g aX/aY: %g/%g, framesOn: %d framesOff: %d on: %s" %
                (Constants.visionObjectTuple[self.visionId],
                 Constants.landmarkTuple[self.localId],
                 self.x, self.y, self.dist, self.bearing,
                 self.centerX, self.centerY,
                 self.width, self.height, self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on))

class Corner:
    """
    Class for one single corner, contains:
    -id -- this is an unique identifier for every type of corner
    -dist -- distance from center of body to corner in cm
    -bearing -- angle on x-axis from center of body to corner in degrees
    """
    INVALID_FIELD_POSITION = -1

    def __init__(self, visionCorner):
        """init method for corner class."""
        self.x = self.INVALID_FIELD_POSITION
        self.y = self.INVALID_FIELD_POSITION
        self.updateVision(visionCorner)

    # updates corner info from vision
    def updateVision(self,visionCorner):
        self.dist = visionCorner.dist
        self.bearing = visionCorner.bearing
        self.possibilities = visionCorner.possibilities

    def getData(self):
        """returns [possibilities, dist, bearing]"""
        return [self.possiblities,self.dist,self.bearing]

    def __str__(self):
        """returns string of all values in class -- can just print corner"""
        return ("possibilities: %g dist: %g bearing: %g" % (self.possibilities,
                                                            self.dist,
                                                            self.bearing))

class Line:
    """
    class for one single line, contains:
    -x1,y1 -- left coordinate on image screen
    -x2,y2 -- right coordinate on image screen
    -slope -- slope of line. remember that (0,0) is top-left corner of image
    -length -- length of the line
    """
    def __init__(self, visionLine):
        """init method"""
        self.x1 = visionLine.x1
        self.y1 = visionLine.y1
        self.x2 = visionLine.x2
        self.y2 = visionLine.y2
        self.slope = visionLine.slope
        self.length = visionLine.length

    def __str__(self):
        return ("p1: (%g,%g) p2: (%g,%g) slope: %g length: %g" %
                (self.x1,self.y1,self.x2,self.y1,self.slope,self.length))

class Crossbar:
    """
    Crossbar class, what used to be the backstop class
    """
    def __init__(self, visionInfos, visionName):
        """initialization all values for FieldObject() class"""
        # Things to be filled out later
        self.framesOn = 0
        self.framesOff = 0
        self.on = False

        # Setup the data from vision
        self.visionId = visionName
        self.updateVision(visionInfos)

    def updateVision(self, visionInfos):
        """updates class variables with new vision information"""
        self.x = visionInfos.x
        self.y = visionInfos.y
        self.angleX = visionInfos.angleX
        self.angleY = visionInfos.angleY
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.focDist = visionInfos.focDist
        self.distance = visionInfos.distance
        self.bearing = visionInfos.bearing
        self.elevation = visionInfos.elevation
        self.leftOpening = visionInfos.leftOpening
        self.rightOpening = visionInfos.rightOpening
        self.shoot = visionInfos.shoot

        # obj is in this frame
        if self.distance > 0:
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
        # obj not in this frame
        else:
            self.on = False
            self.framesOff += 1
            self.framesOn = 0

class Packet:
    """
    Class for one single packet, contains:
    -teamHeader -- header string for nBites (should only receive our own)
    -teamNumber -- team number (should only receive our own)
    -playerNumber -- player number of the owner of the packet
    -timeStamp -- milliseconds since gameController switched state to STATE_PLAY
    -playerX -- x in cm of the packet owner's position on the field
    -playerY -- y in cm of the packet owner's position on the field
    -playerH -- heading of packet owner in degrees, range == [-179..0..180]
    -uncertX -- uncertainty in x direction, self
    -uncertY -- uncertainty in y direction, self
    -uncertH -- uncertainty in heading, self
    -ballX -- estimate of ball x coordinate in cm on field
    -ballY -- estimate of ball y coordinate in cm on field
    -ballUncertX -- estimate of uncertainty in x-axis of the ball's position
    -ballUncertY -- estimate of uncertainty in y-axis of the ball's position
    -ballDist -- distance from player to ball in cm.  set to special values for
    kicking, grabbing, etc
    -role -- current role used in special situations in playbook and Coop
    -subrole -- current sub role used in special situations in playbook and Coop

    can pass packet list into constructor, else it'll default to all zeros
    """
    def __init__(self,new_packet=[0]*17):
        (self.teamNumber,
         self.playerNumber,
         self.color,
         self.playerX,
         self.playerY,
         self.playerH,
         self.uncertX,
         self.uncertY,
         self.uncertH,
         self.ballX,
         self.ballY,
         self.ballUncertX,
         self.ballUncertY,
         self.ballDist,
         self.role,
         self.calledSubRole,
         self.chaseTime) = new_packet

        self.teamNumber = int(self.teamNumber)
        self.playerNumber = int(self.playerNumber)
        self.color = int(self.color)
        self.playerX = float(self.playerX)
        self.playerY = float(self.playerY)
        self.playerH = float(self.playerH)
        self.uncertX = float(self.uncertX)
        self.uncertY = float(self.uncertY)
        self.uncertH = float(self.uncertH)
        self.ballX = float(self.ballX)
        self.ballY = float(self.ballY)
        self.ballUncertX = float(self.ballUncertX)
        self.ballUncertY = float(self.ballUncertY)
        self.ballDist = float(self.ballDist)
        self.role = float(self.role)
        self.calledSubRole = float(self.calledSubRole)
        self.chaseTime = float(self.chaseTime)

    def update(self,update_packet=[0]*17): #=[0]*16
        """update packet with a list of new values"""
        # error checking if passed list is not right size
        if len(update_packet) != 17:
            raise ValueError("Packet().update got list of size:" +
                     str(len(update_packet)) + " but should have size 17 ")
        # else, update values
        (self.teamNumber,
         self.playerNumber,
         self.color,
         self.playerX,
         self.playerY,
         self.playerH,
         self.uncertX,
         self.uncertY,
         self.uncertH,
         self.ballX,
         self.ballY,
         self.ballUncertX,
         self.ballUncertY,
         self.ballDist,
         self.role,
         self.calledSubRole,
         self.chaseTime) = update_packet

    def __str__(self):
        """returns string with all important values"""
        return ("#%d role: %d subRole: %d chaseTime: %g; loc: (%g,%g,%g) uncert: (%g,%g,%g) ball - loc: (%g,%g) uncert: (%g,%g) dist: (%g)" %
            (self.playerNumber,
             self.role, self.calledSubRole, self.chaseTime,
             self.playerX, self.playerY, self.playerH,
             self.uncertX, self.uncertY, self.uncertH,
             self.ballX, self.ballY, self.ballUncertX,
             self.ballUncertY, self.ballDist))

class Sonar:
    """
    Holds the data from the ultrasound sonar sensors
    """
    def __init__(self):
        # Class constants
        self.UNKNOWN_VALUE = "unknown distance"
        self.MIN_DIST = 0.0 # minimum readable distance in cm
        self.MAX_DIST = 255.0 # maximum readable distance in cm

        self.lastDist = self.UNKNOWN_VALUE
        self.lastMode = None

        self.LLdist = self.UNKNOWN_VALUE
        self.RRdist = self.UNKNOWN_VALUE
        self.LRdist = self.UNKNOWN_VALUE
        self.RLdist = self.UNKNOWN_VALUE

    def updateSensors(self, sensors, modes):
        """
        Update the sonar info from the most recent sensors
        """
        self.lastDist = sensors.ultraSoundDistance
        self.lastMode = sensors.ultraSoundMode

        if (self.lastDist < self.MIN_DIST or
            self.lastDist > self.MAX_DIST):
            self.lastDist = self.UNKNOWN_VALUE
        if self.lastMode == modes.LL:
            self.LLdist = self.lastDist
        elif self.lastMode == modes.RR:
            self.RRdist = self.lastDist
        elif self.lastMode == modes.LR:
            self.LRdist = self.lastDist
        elif self.lastMode == modes.RL:
            self.RLdist = self.lastDist

    def __str__(self):
        return ("Last reading of " + str(self.lastMode) +
                " has distance of " + str(self.lastDist) +
                " \n" +
                "LL dist: " + str(self.LLdist) + "\n" +
                "RR dist: " + str(self.RRdist) + "\n" +
                "LR dist: " + str(self.LRdist) + "\n" +
                "RL dist: " + str(self.RLdist))
