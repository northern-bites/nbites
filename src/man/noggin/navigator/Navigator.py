from math import fabs
from ..util import FSA
from . import NavStates
from . import PlaybookPositionStates
from . import ChaseStates
from . import PFKStates
from . import NavConstants as constants
from . import NavTransitions as navTrans
from . import NavHelper as helper
from objects import RelLocation, RobotLocation
from ..kickDecider import kicks

DEBUG_DESTINATION = True

class Navigator(FSA.FSA):
    def __init__(self,brain):
        """it gets you where you want to go"""

        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(NavStates)
        self.currentState = 'stopped'
        self.doingSweetMove = False
        self.setName('Navigator')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
        self.stateChangeColor = 'cyan'
        self.justKicked = False

        # Goto controls
        self.dest = RobotLocation(0, 0, 0)
        self.newDestination = False

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
        self.destType = constants.BALL
        self.switchTo('goToPosition')

    def kickPosition(self, kick):
        """
        It will position the robot at the ball using self.kick to determine the x,y
        offset and the final heading.

        This state will aggresively omni-walk, so it's probably best if we don't call
        it until we're near the ball.
        """
        self.kick = kick

        if (self.currentState is not 'destWalking' or
            self.destType is not constants.BALL):

            self.destType = constants.BALL
            self.switchTo('goToPosition')

    def kickPositionDest(self, kick):
        if self.currentState is 'destWalking':
            return

        ball = self.brain.ball

        # slow down as we get near the ball (max 80% speed)
        if ball.dist < 30:
            gain = min(.8, (0.4 + (ball.dist / 30)) * .8)
        else:
            gain = .8

        # TODO later?
        #self.destTheta = self.kick.heading - self.brain.my.h

        if DEBUG_DESTINATION:
            print 'Ball rel X: {0} Y: {1} ball bearing: {2}' \
                  .format(ball.relX, ball.relY, ball.bearing)

        self.setDest(ball.relX - self.kick.x_offset -2, # HACK!!!
                     ball.relY - self.kick.y_offset,
                     ball.bearing,
                     gain)

    def positionPlaybook(self):
        """robot will walk to the x,y,h from playbook using a mix of omni,
        straight walks and spins"""

        self.destType = constants.PLAYBOOK_DEST
        self.switchTo('goToPosition')

    def goTo(self,dest):
        self.dest = dest
        self.destType = constants.GO_TO_DEST
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

    def setDest(self, x, y, theta, gain):
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

        self.newDestination = True
        self.switchTo('destWalking')

    # Have we reached our destination?
    def isAtPosition(self):
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
        self.newDestination = True
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
        self.newDestination = False


    # Choose our next position based on our mode of locomotion:
    #      - Playbook
    #      - GoTo Dest
    #      - Ball
    def getDestination(self):
        if self.destType is constants.PLAYBOOK_DEST:
            return self.brain.play.getPosition()

        elif self.destType is constants.GO_TO_DEST:
            return self.dest

        elif self.destType is constants.BALL:
            return self.brain.ball
#             kick_x = kick_y = 0
#             if self.kick is not None:
#                 kick_x = self.kick.x_offset
#                 kick_y = self.kick.y_offset
#             return RelLocation(self.brain.my,
#                                self.brain.ball.relX - kick_x,
#                                self.brain.ball.relY - kick_y,
#                                self.brain.ball.bearing)


