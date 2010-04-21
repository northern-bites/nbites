import time

from .VisualObject import VisualObject
from .. import NogginConstants as Constants
from ..util.MyMath import (getRelativeVelocityX,
                          getRelativeVelocityY,
                          getRelativeX,
                          getRelativeY)


class Ball(VisualObject):
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
        VisualObject.__init__(self)

        (self.angleX,
         self.angleY,
         self.confidence,
         self.elevation,
         self.prevFramesOn,
         self.prevFramesOff,
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
         self.lastSeenBearing) = [0]*Constants.NUM_TOTAL_BALL_VALUES

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
            if not self.on:
                self.prevFramesOff = self.framesOff
        else:
            if self.on:
                self.prevFramesOn = self.framesOn

        # Now update to the new stuff
        VisualObject.updateVision(self, visionBall)
        self.elevation = visionBall.elevation
        self.confidence = visionBall.confidence

        if self.dist > 0:
            self.reportBallSeen()
            self.relX = getRelativeX(self.dist, self.bearing)
            self.relY = getRelativeY(self.dist, self.bearing)
        else:
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
        self.locDist = my.dist(self)
        self.locBearing = my.getRelativeBearing(self)
        self.locRelX = getRelativeX(self.locDist, self.locBearing)
        self.locRelY = getRelativeY(self.locDist, self.locBearing)
        self.relVelX = getRelativeVelocityX(my.h, self.velX, self.velY)
        self.relVelY = getRelativeVelocityY(my.h, self.velX, self.velY)

    def __str__(self):
        """returns string with all class variables"""
        return (" loc: (%g,%g) uncert: (%g,%g) sd: %g vel (%g,%g) dist: %g \
        bearing: %g \n \
        vision: elevation: %g" %
                (self.x,self.y,self.uncertX,self.uncertY,self.sd,
                 self.velX,self.velY, self.locDist, self.locBearing, self.elevation)
                + VisualObject.__str__(self) )
