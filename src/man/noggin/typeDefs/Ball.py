from .VisualObject import VisualObject
from .. import NogginConstants as Constants
from ..util.MyMath import (getRelativeVelocityX,
                           getRelativeVelocityY,
                           getRelativeX,
                           getRelativeY,
                           sub180Angle)

FRAMES_AFTER_LOST_BALL_TO_USE_VISION = 7

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
    -lastRel keeps track of the last relX and Y values
    -dx and dy are the difference between lastRel and Rel values
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
         self.bearing,
         self.dist,
         self.uncertVelX,
         self.uncertVelY,
         self.heading,
         self.locDist,
         self.locBearing, # loc based
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
         self.lastRelX,
         self.lastRelY,
         self.dx,
         self.dy,
         self.endY,
         self.lastSeenDist,
         self.lastSeenBearing,
         self.heat) = [0]*Constants.NUM_TOTAL_BALL_VALUES

        self.updateVision(visionBall)

    def updateVision(self,visionBall):
        """update method gets list of vision updated information"""
        # Hold our history
        self.lastVisionDist = self.visDist
        self.lastVisionBearing = self.visBearing
        self.lastVisionCenterX = self.centerX
        self.lastVisionCenterY = self.centerY
        self.lastVisionAngleX = self.angleX
        self.lastVisionAngleY = self.angleY

        # ball was on last frame (visDist already updated, self.on not)
        if self.visDist > 0:
            self.lastSeenBearing = self.visBearing
            self.lastSeenDist = self.visDist
            if not self.on: # ball wasn't on last frame
                self.prevFramesOff = self.framesOff
        else:
            if self.on:
                self.prevFramesOn = self.framesOn

        # Now update to the new stuff
        # self.on updated in visualObject
        VisualObject.updateVision(self, visionBall)
        self.elevation = visionBall.elevation
        self.confidence = visionBall.confidence
        self.heat = visionBall.heat

    def updateLoc(self, loc, my):
        """
        Update all of our inforamtion pased on the newest localization info
        """
        globalX = loc.ballX
        globalY = loc.ballY
        globalVelX = loc.ballVelX
        globalVelY = loc.ballVelY

        # Get latest estimates
        if my.teamColor == Constants.TEAM_BLUE:
            self.x = globalX
            self.y = globalY
            self.velX = globalVelX
            self.velY = globalVelY
        else:
            self.x = Constants.FIELD_GREEN_WIDTH - globalX
            self.y = Constants.FIELD_GREEN_HEIGHT - globalY
            self.velX = -globalVelX
            self.velY = -globalVelY

        self.uncertX = loc.ballXUncert
        self.uncertY = loc.ballYUncert
        self.uncertVelX = loc.ballVelXUncert
        self.uncertVelY = loc.ballVelYUncert
        self.sd = self.uncertX * self.uncertY

        # Determine other values
        self.locDist = loc.ballDistance
        self.locBearing = loc.ballBearing
        self.relVelX = loc.ballRelVelX
        self.relVelY = loc.ballRelVelY

        self.locRelX = loc.ballRelX
        self.locRelY = loc.ballRelY

    def updateBestValues(self, my):
        self.bearing  = self.locBearing
        self.dist     = self.locDist

        self.lastRelX = self.relX
        self.lastRelY = self.relY

        self.relX     = self.locRelX
        self.relY     = self.locRelY

        # uses my.x, my.y which are loc determined to get heading
        self.heading = my.headingTo(self, forceCalc=True)

        if not self.on:
            self.dx = self.lastRelX - self.relX
            self.dy = self.lastRelY - self.relY

        # calculation for the goalie to figure out
        # what the y value of the ball will be when it
        # gets to the goalie
        if(self.dx != 0):
            self.endY = self.relY - (self.dy*(self.relX/self.dx))

    def __str__(self):
        """returns string with all class variables"""
        return (" loc: (%g,%g) uncert: (%g,%g) sd: %g vel (%g,%g) dist: %g bearing: %g vision: elevation: %g" %
                (self.x,self.y,self.uncertX,self.uncertY,self.sd,
                 self.velX,self.velY, self.dist, self.bearing, self.elevation)
                + VisualObject.__str__(self) )
