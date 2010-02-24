from math import fabs
from ..util import FSA
from . import NavStates
from . import NavMath
from . import NavConstants as constants
from . import NavHelper as helper

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
        self.destX= 0
        self.destY= 0
        self.destH = 0

        # Walk controls
        self.currentGait = None

        self.helper = helper.NavHelper(self)

    def performSweetMove(self, move):
        self.sweetMove = move
        self.switchTo('doingSweetMove')

    def positionReady(self, dest):
        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX, self.destY, self.destH = dest
        self.switchTo('positioningReady')

    def positionPlaybook(self, dest):
        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX, self.destY, self.destH = dest
        self.switchTo('positioningPlaybook')

    def omniGoTo(self, dest):
        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX, self.destY, self.destH = dest
        self.switchTo('omniWalkToPoint')

    def goTo(self,dest):

        if len(dest) == 2:
            self.destX, self.destY = dest
            self.destH = 0.0
        elif len(dest) == 3:
            self.destX, self.destY, self.destH = dest

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
            self.printf("!!!!!! USE player.stopWalking() NOT walk(0,0,0)!!!!!")
            return
        # If the walk changes are really small, then ignore them
        if (fabs(self.walkX - x) < constants.FORWARD_EPSILON and
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
