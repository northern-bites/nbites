from math import fabs
from ..util import FSA
from . import NavStates
from . import NavConstants as constants
from . import NavHelper as helper
from man.noggin.typeDefs.Location import RobotLocation

class Navigator(FSA.FSA):
    def __init__(self,brain):
        """it gets you where you want to go"""

        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(NavStates)
        self.currentState = 'stopped'
        self.setName('Navigator')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
        self.stateChangeColor = 'cyan'

        # Goto controls
        self.dest = RobotLocation(0, 0, 0)

        # Walk controls
        self.currentGait = None
        self.walkX = 0
        self.walkY = 0
        self.walkTheta = 0
        self.orbitDir = 0
        self.angleToOrbit = 0

    def performSweetMove(self, move):
        self.sweetMove = move
        self.switchTo('doingSweetMove')

    def positionPlaybook(self, dest):
        self.dest = dest

        if not self.currentState == 'positioningPlaybook':
            self.switchTo('positioningPlaybook')

    def positionReady(self, dest):
        self.dest = dest

        if not self.currentState == 'positioningReady':
            self.switchTo('positioningReady')

    def omniGoTo(self, dest):
        self.dest = dest
        self.switchTo('omniWalkToPoint')

    def goTo(self,dest):
        self.dest = dest

        if not self.currentState == 'spinToWalkHeading' and \
                not self.currentState == 'walkStraightToPoint' and \
                not self.currentState == 'spinToFinalHeading':
            if not helper.atHeadingGoTo(self.brain.my, self.dest.h):
                self.switchTo('spinToWalkHeading')
            elif helper.atHeadingGoTo(self.brain.my, self.dest.h):
                self.switchTo('walkStraightToPoint')

    def stop(self):
        if self.currentState =='stop':
            pass
        else:
            self.switchTo('stop')

    def isStopped(self):
        return self.currentState == 'stopped'

    def movingOmni(self):
        return self.currentState == 'omniWalkToPoint'

    def orbit(self, orbitDir):

        # If the orbit direction is the same ignore the command
        if (self.orbitDir == orbitDir):
            self.updatedTrajectory = False
            return

        self.orbitDir = orbitDir
        self.walkX = 0
        self.walkY = self.orbitDir*constants.ORBIT_STRAFE_SPEED
        self.walkTheta = self.orbitDir*constants.ORBIT_SPIN_SPEED
        self.updatedTrajectory = True

        self.switchTo('orbitPoint')

    def orbitAngle(self, angleToOrbit):

        if (self.angleToOrbit == angleToOrbit):
            self.updatedTrajectory = False
            return

        self.angleToOrbit = angleToOrbit
        self.updatedTrajectory = True

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
            self.updatedTrajectory = False
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
        self.walkX, self.walkY, self.walkTheta = 0, 0, 0
        self.stepX = x
        self.stepY = y
        self.stepTheta = theta
        self.numSteps = numSteps
        self.switchTo('stepping')

#######SHOULD NOT BE CALLED BY ANYTHING OUTSIDE NAVIGATOR FOLDER#######
