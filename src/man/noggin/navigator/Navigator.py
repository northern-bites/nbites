from math import fabs
from ..util import FSA
from . import NavStates
from . import PlaybookPositionStates
from . import ChaseStates
from . import PFKStates
from . import NavConstants as constants
from . import NavTransitions as navTrans
from . import NavHelper as helper
from objects import RobotLocation

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
        self.doingSweetMove = False
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
        self.destX = 0
        self.destY = 0
        self.destTheta = 0
        self.destGain = 1

        self.newDestination = False

        self.shouldAvoidObstacleLeftCounter = 0
        self.shouldAvoidObstacleRightCounter = 0

        self.resetSpeedMemory()
        self.resetDestMemory()

    def performSweetMove(self, move):
        """
        Navigator function to do the sweet move
        """
        self.sweetMove = move
        self.brain.player.stopWalking()
        self.resetSpeedMemory()
        self.resetDestMemory()
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
        self.switchTo('goToPosition')

    def stop(self):
        if ((self.currentState =='stop' or self.currentState == 'stopped')
            and not self.justKicked):
            pass
        else:
            self.switchTo('stop')

    def isStopped(self):
        return self.currentState == 'stopped'

    def orbitAngle(self, angleToOrbit):

        if (self.angleToOrbit == angleToOrbit and \
                self.currentState == 'orbitPointThruAngle'):
            return

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
        self.walkX = x
        self.walkY = y
        self.walkTheta = theta

        self.switchTo('walking')

    def setDest(self, x, y, theta):
        """
        Sets a new destination
        Always does something, since destinations are relative and time sensitive
        """
        self.destX = x
        self.destY = y
        self.destTheta = theta

        self.newDestination = True
        self.switchTo('destWalking')

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


    # Have we reached our destination?
    def isAtPositition(self):
        return self.currentState is 'atPosition'

    #################################################################
    #             ::Walk speed memory::                             #
    #                                                               #
    #         DO NOT SET IN THE NORMAL NAVIGATOR BEHAVIOR           #
    #         ONLY TO BE USED IN NAV HELPER SPEED SETTERS           #
    #               AND HERE.                                       #
    #                                                               #
    #      self.lastXSpeed, self.lastYSpeed, self.lastThetaSpeed    #
    #      self.lastDestX,  self.lastDestY,  self.lastDestTheta     #
    #           self.lastDestGain                                   #
    #################################################################

    # Is the given vector equal to our current values
    def speedVectorsEqual(self, x, y, theta):
        return (fabs(self.lastSpeedX - x) < constants.FORWARD_EPSILON and
                fabs(self.lastSpeedY - y) < constants.STRAFE_EPSILON and
                fabs(self.lastSpeedTheta - theta) < constants.SPIN_EPSILON)

    # Update the speed values and reset the others
    def updateSpeeds(self, x, y, theta):
        self.lastSpeedX, self.lastSpeedY, self.lastSpeedTheta = x,y,theta
        self.resetDestMemory()

    def updateDests(self, x, y, theta, gain):
        self.lastDestX, \
            self.lastDestY, \
            self.lastDestTheta, \
            self.lastDestGain= x,y,theta, gain
        self.resetSpeedMemory()

    def resetSpeedMemory(self):
        self.lastSpeedX,  \
            self.lastSpeedY,\
            self.lastSpeedTheta  = (constants.WALK_VECTOR_INIT,)*3

    def resetDestMemory(self):
        self.lastDestX, \
            self.lastDestY, \
            self.lastDestTheta, \
            self.lastDestGain = (constants.WALK_VECTOR_INIT,)*4

