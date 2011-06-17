from math import fabs
from ..util import FSA
from . import NavStates
from . import PlaybookPositionStates
from . import ChaseStates
from . import PFKStates
from . import NavConstants as constants
from . import NavTransitions as navTrans
from . import NavHelper as helper
from man.noggin.typeDefs.Location import RobotLocation

class Navigator(FSA.FSA):
    def __init__(self,brain):
        """it gets you where you want to go"""

        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(NavStates)
        self.addStates(PlaybookPositionStates)
        self.addStates(ChaseStates)
        self.addStates(PFKStates)
        self.currentState = 'stopped'
        self.setName('Navigator')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
        self.stateChangeColor = 'cyan'
        self.justKicked = False

        # Goto controls
        self.dest = RobotLocation(0, 0, 0)

        # Walk controls
        self.walkX = 0
        self.walkY = 0
        self.walkTheta = 0
        self.orbitDir = 0
        self.angleToOrbit = 0
        self.curSpinDir = 0

        self.shouldAvoidObstacleLeftCounter = 0
        self.shouldAvoidObstacleRightCounter = 0

    def performSweetMove(self, move):
        """
        Navigator function to do the sweet move
        """
        self.sweetMove = move
        self.brain.player.stopWalking()
        helper.executeMove(self.brain.motion, self.sweetMove)
        self.switchTo('doingSweetMove')

    def dribble(self):
        self.switchTo('dribble')

    def chaseBall(self):
        """
        robot will walk to the ball with it centered at his feet.
        if no ball is visible, localization will be usedn
        """
        self.switchTo('walkSpinToBall')

    def kickPosition(self, kick):
        """
        state to align on the ball once we are near it
        """
        self.kick = kick
        self.switchTo('pfk_all')

    def positionPlaybook(self):
        """robot will walk to the x,y,h from playbook using a mix of omni,
        straight walks and spins"""

        self.switchTo('playbookWalk')

    def goTo(self,dest):
        self.dest = dest

        if not self.currentState == 'spinToWalkHeading' and \
               not self.currentState == 'walkStraightToPoint' and \
               not self.currentState == 'spinToFinalHeading':
            if not navTrans.atHeadingGoTo(self.brain.my, self.dest.h):
                self.switchTo('spinToWalkHeading')
            elif navTrans.atHeadingGoTo(self.brain.my, self.dest.h):
                self.switchTo('walkStraightToPoint')

    def stop(self):
        if ((self.currentState =='stop' or self.currentState == 'stopped')
            and not self.justKicked):
            pass
        else:
            self.switchTo('stop')

    def isStopped(self):
        return self.currentState == 'stopped'

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

        if (self.angleToOrbit == angleToOrbit and \
                self.currentState == 'orbitPointThruAngle'):
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
