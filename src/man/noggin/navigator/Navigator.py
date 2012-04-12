from math import fabs
from ..util import FSA
from . import NavStates
from . import NavConstants as constants
from . import NavTransitions as navTrans
from . import NavHelper as helper
from objects import RobotLocation
from ..kickDecider import kicks
from ..util import Transition 

DEBUG_DESTINATION = False

class Navigator(FSA.FSA):
    """it gets you where you want to go"""

    def __init__(self,brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(NavStates)
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

        self.destX = 0
        self.destY = 0
        self.destTheta = 0
        self.destGain = 1
        self.nearDestination = False

        self.shouldAvoidObstacleLeftCounter = 0
        self.shouldAvoidObstacleRightCounter = 0

        self.resetSpeedMemory()
        self.resetDestMemory()

        self.destType = None
        self.kick = None
        
        self.atLocPositionTransition = Transition.CountTransition(navTrans.atLocPosition)
        self.locRepositionTransition = Transition.CountTransition(navTrans.notAtLocPosition, 
                                                                  Transition.SOME_OF_THE_TIME,
                                                                  Transition.LOW_PRECISION)
        
        NavStates.goToPosition.transitions = { NavStates.atPosition: self.atLocPositionTransition }
        NavStates.atPosition.transitions = { NavStates.goToPosition: self.locRepositionTransition }

    def run(self):
        if self.destType is constants.BALL:
            if navTrans.shouldSwitchPFKModes(self):
                if self.kick is None:
                    self.kick = kicks.ORBIT_KICK_POSITION

                self.kickPositionDest(self.kick)

        FSA.FSA.run(self)

    def performSweetMove(self, move):
        """
        Navigator function to do the sweet move
        """
        self.sweetMove = move
        self.destType = None

        self.brain.player.stopWalking()
        self.resetSpeedMemory()
        self.resetDestMemory()

        helper.executeMove(self.brain.motion, self.sweetMove)
        self.switchTo('doingSweetMove')

    def positionPlaybook(self):
        self.goTo(self.brain.play.getPosition())

    def goTo(self, dest, speedGain = 1.0,
             precision = constants.CLOSE_ENOUGH,
             accountForLocUncertainty = True):
        
        self.dest = dest
        self.destGain = speedGain
        self.destPrecision = precision
        self.accountForLocUncertainty = True
        if self.currentState is not 'goToPosition':
            self.switchTo('goToPosition')

    def stop(self):
        if self.currentState not in ['stop', 'stopped']:
            self.switchTo('stop')

    def isStopped(self):
        return self.currentState == 'stopped'

    def orbitAngle(self, angleToOrbit):
        if (self.currentState == 'orbitPointThruAngle'):
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

    def stand(self):
        """
        Make the robot stand; Standing should be the default action when we're not 
        walking/executing a sweet move
        """
        helper.stand(self)
        self.switchTo('standing')
        

    def setDest(self, x, y, theta, gain=1.0):
        """
        Sets a new destination
        Always does something, since destinations are relative and time sensitive
        """
        if DEBUG_DESTINATION:
            print 'Set new destination of ({0}, {1}, {2}, gain={3})' \
                  .format(self.destX, self.destY, self.destTheta, self.destGain)
            self.brain.speech.say("New destination")

        self.destX = x
        self.destY = y
        self.destTheta = theta
        self.destGain = gain

        self.updateDests(x, y, theta, gain)

        self.switchTo('destWalking')

    # Have we reached our destination?
    def isAtPosition(self):
        return self.currentState is 'atPosition'


    # TODO: put in helper or something not accessible to other FSAs
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

    def updateSpeeds(self, x, y, theta):
        """
        Update the speed values and reset the others
        """
        self.lastSpeedX, self.lastSpeedY, self.lastSpeedTheta = x,y,theta
        self.resetDestMemory()

    def updateDests(self, x, y, theta, gain):
        """
        Update the last destination we were sent to
        """
        self.lastDestX, \
            self.lastDestY, \
            self.lastDestTheta, \
            self.lastDestGain= x,y,theta, gain
        self.resetSpeedMemory()
        self.nearDestination = False

    def resetSpeedMemory(self):
        """
        Reset our idea of the last walk vector we had
        """
        self.lastSpeedX,  \
            self.lastSpeedY,\
            self.lastSpeedTheta  = (constants.WALK_VECTOR_INIT,)*3

    def resetDestMemory(self):
        """
        Reset our idea of the last walk destination vector we had
        """
        self.lastDestX, \
            self.lastDestY, \
            self.lastDestTheta, \
            self.lastDestGain = (constants.WALK_VECTOR_INIT,)*4


    # Choose our next position based on our mode of locomotion:
    #      - Playbook
    #      - GoTo Dest
#    #      - Ball
#    def getDestination(self):
#        if self.destType is constants.PLAYBOOK_DEST:
#            return self.brain.play.getPosition()
#
#        elif self.destType is constants.GO_TO_DEST:
#            return self.dest
#
#        elif self.destType is constants.BALL:
#            return self.brain.ball.loc
#
#        # This can happen when setDest is called
#        else:          # destType is None
#            return self.brain.my


