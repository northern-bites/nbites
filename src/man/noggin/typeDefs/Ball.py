from objects import Location
import noggin_constants as Constants
from ..util.MyMath import (getRelativeVelocityX,
                           getRelativeVelocityY,
                           getRelativeX,
                           getRelativeY,
                           sub180Angle)

FRAMES_AFTER_LOST_BALL_TO_USE_VISION = 7

class Ball(Location):
    """
    Class for holding all current Ball information, contains:

    -dist -- best distance, either vis or loc
    -bearing -- best bearing, either vis or loc
    -x,y -- (x,y) coordinate of ball on field via ekf
    -uncertX,uncertY -- uncertainty in x,y axis from ekf
    -velX,velY -- velocity in x,y axis of ball from ekf
    -locDist -- euclidian distance between (x,y) of self to (x,y) of ball (ekf)
    -locBearing -- relative bearing between self (x,y) and ball (x,y) via ekf
    -lastRel keeps track of the last relX and Y values
    -dx and dy are the difference between lastRel and Rel values

    Others accessible through ball.vis: vision dist and bearing, angleX,
    angleY, elevation, confidence, radius, heat, on, and framesOn/Off.
    -dist, bearing -- self-explanatory
    -angleX,angleY -- angles of center (x,y) to focal point in degrees
    -elevation -- angle in y-axis from center of body to ball
    -on -- simple bool if ball is on vision frame or not
    -framesOn -- # of consecutive frames the ball has been recognized in vision
    -framesOff -- # of consecutive frames the ball has been not recognized
    """
    def __init__(self, visionBall):
        Location.__init__(self, 0.0, 0.0)

        self.vis = visionBall

        (self.uncertX,
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
         self.lastRelX,
         self.lastRelY,
         self.dx,
         self.dy,
         self.endY,
         self.accX,
         self.accY,
         self.uncertAccX,
         self.uncertAccY,
         self.relAccX,
         self.relAccY) = [0]*Constants.NUM_TOTAL_BALL_VALUES

    def updateLoc(self, loc, my):
        """
        Update all of our inforamtion pased on the newest localization info
        """
        globalX = loc.ballX
        globalY = loc.ballY
        globalVelX = loc.ballVelX
        globalVelY = loc.ballVelY
        globalAccX = loc.ballAccX
        globalAccY = loc.ballAccY

        # Get latest estimates
        if my.teamColor == Constants.teamColor.TEAM_BLUE:
            self.x = globalX
            self.y = globalY
            self.velX = globalVelX
            self.velY = globalVelY
            self.accX = globalAccX
            self.accY = globalAccY
        else:
            self.x = Constants.FIELD_GREEN_WIDTH - globalX
            self.y = Constants.FIELD_GREEN_HEIGHT - globalY
            self.velX = -globalVelX
            self.velY = -globalVelY
            self.accX = -globalAccX
            self.accY = -globalAccY

        self.uncertX = loc.ballXUncert
        self.uncertY = loc.ballYUncert
        self.uncertVelX = loc.ballVelXUncert
        self.uncertVelY = loc.ballVelYUncert
        self.uncertAccX = loc.ballAccXUncert
        self.uncertAccY = loc.ballAccYUncert
        self.sd = self.uncertX * self.uncertY

        # Determine other values
        self.locDist = loc.ballDistance
        self.locBearing = loc.ballBearing
        self.relVelX = loc.ballRelVelX
        self.relVelY = loc.ballRelVelY
        self.relAccX = loc.ballRelAccX
        self.relAccY = loc.ballRelAccY

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
        self.heading = my.headingTo(self)

        if self.vis.framesOn > 1:
            self.dx = self.lastRelX - self.relX
            self.dy = self.lastRelY - self.relY

        # calculation for the goalie to figure out
        # what the y value of the ball will be when it
        # gets to the goalie
        # TODO: use new ball information
        if(self.dx != 0):
            self.endY = self.relY - (self.dy*(self.relX/self.dx))

