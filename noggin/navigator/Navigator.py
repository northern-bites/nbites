from math import fabs
import man.motion as motion
from ..util import FSA
from . import NavStates
from . import NavMath
from . import NavConstants as constants

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
        self.NavMath = NavMath

    def omniGoTo(self, dest):
        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX, self.destY, self.destH = dest
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
            if not NavMath.atHeadingGoTo(self, self.destH):
                self.switchTo('spinToWalkHeading')
            elif NavMath.atHeadingGoTo(self, self.destH):
                self.switchTo('walkStraightToPoint')

    def stop(self):
        if self.currentState =='stop':
            pass
        else:
            self.switchTo('stop')

    def isStopped(self):
        return self.currentState == 'stopped'

    def orbit(self, orbitDir):
        self.orbitDir = orbitDir
        self.switchTo('orbitPoint')

    def orbitAngle(self, angleToOrbit):
        self.angleToOrbit = angleToOrbit
        self.switchTo('orbitPointThruAngle')

    def walk(self, x, y, theta):
        """
        Starts a new walk command
        Does nothing if it is the same as the current walk
        Switches to it otherwise
        """
        # Make sure we stop
        if (x == 0 and y == 0 and theta == 0):
            if self.walkX == 0 and \
                   self.walkY == 0 and \
                   self.walkTheta == 0:
                return
        # If the walk changes are really small, then ignore them
        elif (fabs(self.walkX - x) < constants.FORWARD_EPSILON and
            fabs(self.walkY - y) < constants.STRAFE_EPSILON and
            fabs(self.walkTheta - theta) < constants.SPIN_EPSILON):
            return

        self.walkX = x
        self.walkY = y
        self.walkTheta = theta

        self.updatedTrajectory = True
        self.switchTo('walking')

    def takeSteps(self, x, y, theta, numSteps):
        """
        Set the step commands
        """
        self.stepX = x
        self.stepY = y
        self.stepTheta = theta
        self.numSteps = numSteps
        self.switchTo('stepping')

#######SHOULD NOT BE CALLED BY ANYTHING OUTSIDE NAVIGATOR FOLDER#######
    def setSpeed(self,x,y,theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        self.walkX, self.walkY, self.walkTheta = x, y, theta
        walk = motion.WalkCommand(x=x,y=y,theta=theta)
        self.brain.motion.setNextWalkCommand(walk)

    def step(self,x,y,theta,numSteps):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        steps = motion.StepCommand(x=x,y=y,theta=theta,numSteps=numSteps)
        self.brain.motion.sendStepCommand(steps)
