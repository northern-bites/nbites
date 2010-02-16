
from . import NavStates
from ..util import FSA
from ..util import MyMath
import NavConstants as constants
import man.motion as motion

from math import fabs

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

        # Goto controls
        self.lastDestX = 0
        self.lastDestY = 0
        self.lastDestH = 0
        self.destX= 0
        self.destY= 0
        self.destH = 0
        self.oScale = -1
        self.hScale = -1

        # Walk controls
        self.walkX = 0
        self.walkY = 0
        self.walkTheta = 0
        self.currentGait = None
        self.movingOmni = False

        # Step controls
        self.stepX = 0
        self.stepY = 0
        self.stepTheta = 0

        self.orbitDir = None

    def chaseTarget(self, target, heading=0.0):
        '''
        will chase a target(ball, robot, person?)
        heading is desired final heading (NOT IMPLEMENTED!)
        '''
        self.brain.CoA.setRobotDribbleGait(motion)
        self.destX = target.x
        self.destY = target.y
        self.destH = 0.0
        self.movingOmni = False
        self.switchTo('chaseToPoint')

    #NOT SAFE FOR USE
    def dribbleGoTo(self, dest):
        '''
        we\'ll dribble the ball! (to give destination)
        '''
        pass
        self.brain.CoA.setRobotDribbleGait(motion)

    def omniGoTo(self, dest):
        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX,self.destY, self.destH = dest
        self.movingOmni = True
        self.switchTo('omniWalkToPoint')

    def goTo(self,dest):

        self.movingOmni = False
        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX,self.destY, self.destH = dest

        if not self.currentState == 'spinToWalkHeading' and \
                not self.currentState == 'walkStraightToPoint' and \
                not self.currentState == 'spinToFinalHeading':
            if not self.atHeadingGoTo(self.destH):
                self.switchTo('spinToWalkHeading')
            elif self.atHeadingGoTo(self.destH):
                self.switchTo('walkStraightToPoint')

    def setWalk(self, x, y, theta):
        """
        Sets a new walk command
        Returns False if it is the same as the current walk
        True otherwise
        """
        # Make sure we stop
        if (x == 0 and y == 0 and theta == 0):
            if self.walkX == 0 and self.walkY == 0 and self.walkTheta == 0:
                return False
        # If the walk changes are really small, then ignore them
        elif (fabs(self.walkX - x) < constants.FORWARD_EPSILON and
            fabs(self.walkY - y) < constants.STRAFE_EPSILON and
            fabs(self.walkTheta - theta) < constants.SPIN_EPSILON):
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
        self.walkX, self.walkY, self.walkTheta = x, y, theta
        walk = motion.WalkCommand(x=x,y=y,theta=theta)
        self.brain.motion.setNextWalkCommand(walk)

    def setSteps(self, x, y, theta, numSteps):
        """
        Set the step commands
        """
        self.stepX = x
        self.stepY = y
        self.stepTheta = theta
        self.numSteps = numSteps

    def step(self,x,y,theta,numSteps):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        steps = motion.StepCommand(x=x,y=y,theta=theta,numSteps=numSteps)
        self.brain.motion.sendStepCommand(steps)


    def atDestination(self):
        """
        Returns true if we are at an (x, y) close enough to the one we want
        """
#         self.printf("X diff is " + str(self.brain.my.x - self.destX))
#         self.printf("Y diff is " + str(self.brain.my.y - self.destY))
        return (abs(self.brain.my.x - self.destX) < constants.CLOSE_ENOUGH_XY
                and abs(self.brain.my.y - self.destY) < constants.CLOSE_ENOUGH_XY)

    def atDestinationCloser(self):
        """
        Returns true if we are at an (x, y) close enough to the one we want
        """
#         self.printf("X diff is " + str(self.brain.my.x - self.destX))
#         self.printf("Y diff is " + str(self.brain.my.y - self.destY))
        return (abs(self.brain.my.x - self.destX) < constants.CLOSER_XY
                and abs(self.brain.my.y - self.destY) < constants.CLOSER_XY)

    def atDestinationGoalie(self):
        return (abs(self.brain.my.x - self.destX) < constants.GOALIE_CLOSE_X
                and abs(self.brain.my.y - self.destY) < constants.GOALIE_CLOSE_Y)

    def atHeadingGoTo(self,targetHeading):
        hDiff = abs(MyMath.sub180Angle(self.brain.my.h - targetHeading))
        #self.printf("H diff is " + str(hDiff))
        return hDiff < constants.AT_HEADING_GOTO_DEG


    def atHeading(self, targetHeading = None):
        """
        Returns true if we are at a heading close enough to what we want
        """
        if targetHeading is None:
            targetHeading = self.destH
        hDiff = abs(MyMath.sub180Angle(self.brain.my.h - targetHeading))
        #self.printf("H diff is " + str(hDiff))
        return hDiff < constants.CLOSE_ENOUGH_H and \
            self.brain.my.uncertH < constants.LOC_IS_ACTIVE_H

    def notAtHeading(self, targetHeading= None):
        if targetHeading is None:
            targetHeading = self.destH
        hDiff = abs(MyMath.sub180Angle(self.brain.my.h - targetHeading))
        #self.printf("H diff is " + str(hDiff))
        return hDiff > constants.ALMOST_CLOSE_ENOUGH_H and \
            self.brain.my.uncertH < constants.LOC_IS_ACTIVE_H

    def getRotScale(self, headingDiff):
        absHDiff = abs(headingDiff)
        if absHDiff < constants.HEADING_NEAR_THRESH:
            return constants.HEADING_NEAR_SCALE
        elif absHDiff < constants.HEADING_MEDIUM_THRESH:
            return constants.HEADING_MEDIUM_SCALE
        else:
            return constants.HEADING_FAR_SCALE

    def getOScale(self):
        dist = MyMath.dist(self.brain.my.x, self.brain.my.y,
                           self.destX, self.destY)
        if dist < constants.POSITION_NEAR_THRESH:
            return constants.POSITION_NEAR_SCALE
        elif dist < constants.HEADING_MEDIUM_THRESH:
            return constants.POSITION_MEDIUM_THRESH
        else:
            return constants.POSITION_FAR_SCALE

    def isStopped(self):
        return self.currentState == 'stopped'

    def orbit(self, orbitDir):
        self.orbitDir = orbitDir
        self.switchTo('orbitPoint')

    def orbitAngle(self, angleToOrbit):
        self.angleToOrbit = angleToOrbit
        self.switchTo('orbitPointThruAngle')
