
from . import NavStates
from .util import FSA
from .util import MyMath
import man.motion as motion
import math

LOC_IS_ACTIVE_H  = 400
CLOSE_ENOUGH_XY = 25.0
CLOSE_ENOUGH_H = 25.0
ALMOST_CLOSE_ENOUGH_H = 45.0

HEADING_NEAR_THRESH = 10.
HEADING_MEDIUM_THRESH = 30.

HEADING_NEAR_SCALE = 0.3
HEADING_MEDIUM_SCALE = 0.6
HEADING_FAR_SCALE = 1.0

class Navigator(FSA.FSA):
    def __init__(self,brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(NavStates)
        self.currentState = 'stopped'
        self.setName('Navigator')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
        self.stateChangeColor = 'cyan'

        self.lastDestX = 0
        self.lastDestY = 0
        self.lastDestH = 0
        self.destX= 0
        self.destY= 0
        self.destH = 0
        self.walkX = 0
        self.walkY = 0
        self.walkTheta = 0

    def goTo(self,x,y,h=None):
        self.destH = h
        self.destX,self.destY = x,y
        self.switchTo('spinToWalkHeading')

    def setWalk(self, x, y, theta):
        """
        Sets a new walk command
        Returns False if it is the same as the current walk
        True otherwise
        """
        if (self.walkX == x and self.walkY == y and
            self.walkTheta == theta):
            return False

        self.walkX = x
        self.walkY = y
        self.walkTheta = theta

        self.updatedTrajectory = True
        return True

    def setSpeed(self,x,y,theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        walk = motion.WalkCommand(x=x,y=y,theta=theta)
        self.brain.motion.setNextWalkCommand(walk)

    def atDestination(self):
        """
        Returns true if we are at an (x, y) close enough to the one we want
        """
#         self.printf("X diff is " + str(self.brain.my.x - self.destX))
#         self.printf("Y diff is " + str(self.brain.my.y - self.destY))
        return (abs(self.brain.my.x - self.destX) < CLOSE_ENOUGH_XY
                and abs(self.brain.my.y - self.destY) < CLOSE_ENOUGH_XY)

    def atHeading(self, targetHeading = None):
        """
        Returns true if we are at a heading close enough to what we want
        """
        if targetHeading is None:
            targetHeading = self.destH
        hDiff = abs(MyMath.sub180Angle(self.brain.my.h - targetHeading))
        self.printf("H diff is " + str(hDiff))
        return abs(hDiff) < CLOSE_ENOUGH_H and \
            self.brain.my.uncertH < LOC_IS_ACTIVE_H


    def notAtHeading(self, targetHeading= None):
        if targetHeading is None:
            targetHeading = self.destH
        hDiff = abs(MyMath.sub180Angle(self.brain.my.h - targetHeading))
        #self.printf("H diff is " + str(hDiff))
        return abs(hDiff) > ALMOST_CLOSE_ENOUGH_H and \
            self.brain.my.uncertH < LOC_IS_ACTIVE_H


    def getRotScale(self, headingDiff):
        absHDiff = abs(headingDiff)
        if absHDiff < HEADING_NEAR_THRESH:
            return HEADING_NEAR_SCALE
        elif absHDiff < HEADING_MEDIUM_THRESH:
            return HEADING_MEDIUM_SCALE
        else:
            return HEADING_FAR_SCALE


    def isStopped(self):
        return self.currentState == 'stopped'
