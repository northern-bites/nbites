from ..util import FSA
from . import NavStates
from . import NavConstants as constants
from . import NavTransitions as navTrans
from . import NavHelper as helper
from objects import RobotLocation, RelRobotLocation
from ..kickDecider import kicks
from ..util import Transition

#speed gains
FULL_SPEED = 1.0
HASTY_SPEED = 0.9
FAST_SPEED = 0.8
QUICK_SPEED = 0.7
BRISK_SPEED = 0.6
MEDIUM_SPEED = 0.5
GRADUAL_SPEED = 0.4
CAREFUL_SPEED = 0.3
SLOW_SPEED = 0.2
SLUGGISH_SPEED = 0.1
KEEP_SAME_SPEED = -1
#walk speed adapt
ADAPTIVE = True
#goTo precision
GENERAL_AREA = (5.0, 5.0, 20)
CLOSE_ENOUGH = (3.5, 3.5, 10)
PRECISELY = (1.0, 1.0, 5)
#directions - left is positive (in terms of rotation or y) and right is negative
LEFT = 1
RIGHT = -LEFT


DEBUG_DESTINATION = False

class Navigator(FSA.FSA):
    """it gets you where you want to go"""

    def __init__(self, brain):
        FSA.FSA.__init__(self, brain)
        self.brain = brain
        self.addStates(NavStates)
        self.currentState = 'stopped'
        self.setName('Navigator')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'cyan'

        #transitions
        #@todo: move this to the actual transitions file?
        self.atLocPositionTransition = Transition.CountTransition(navTrans.atDestination)
        self.locRepositionTransition = Transition.CountTransition(navTrans.notAtLocPosition,
                                                                  Transition.SOME_OF_THE_TIME,
                                                                  Transition.LOW_PRECISION)

        NavStates.goToPosition.transitions = {
            self.atLocPositionTransition : NavStates.atPosition,

            Transition.CountTransition(navTrans.shouldDodgeLeft,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : NavStates.avoidLeft,

            Transition.CountTransition(navTrans.shouldDodgeRight,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : NavStates.avoidRight
            }

        NavStates.avoidLeft.transitions = {
            Transition.CountTransition(navTrans.doneDodging,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : NavStates.briefStand
            }

        NavStates.avoidRight.transitions = {
            Transition.CountTransition(navTrans.doneDodging,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : NavStates.briefStand
            }

        NavStates.atPosition.transitions = {
            self.locRepositionTransition : NavStates.goToPosition
            }

    def run(self):
        FSA.FSA.run(self)

    def performSweetMove(self, move):
        """
        Navigator function to do the sweet move
        """
        NavStates.scriptedMove.sweetMove = move
        self.switchTo('scriptedMove')

    def positionPlaybook(self):
        self.goTo(self.brain.play.getPosition(), avoidObstacles = True)

    def chaseBall(self, speed = FULL_SPEED):
        ball = self.brain.ball
        ballRelRobotLocation = RelRobotLocation(ball.rel_x,
                                                ball.rel_y,
                                                ball.bearing_deg)
        self.goTo(ballRelRobotLocation, CLOSE_ENOUGH, speed, True)

    def goTo(self, dest, precision = GENERAL_AREA, speed = FULL_SPEED, avoidObstacles = False, adaptive = False):
        """
        General go to method
        Ideal for going to a field position, or for going to a
        relative location that we can track/see

        @param dest: must be a Location, RobotLocation, RelLocation
        or RelRobotLocation.
        If you want to update the destination, you can just modify the instance
        you passed to goTo (so for example if you passed ball.loc as a destination,
        then you wouldn't have to do anything since the ball's position updates
        automatically).
        Alternatively, you can use updateDest to change the destination.
        This is especially important if dest is a relative location,
        since there's no way for the robot to keep track of how close it is to the
        location, so if you don't update it it will keep walking to that destination
        indefinitely
        @param speedGain: controls how fast the robot does the goTo; use provided
        constants for some good ballparks
        @param precision: a tuple of deltaX, deltaY, deltaH for how close
        you want to get to the location
        @param adaptive: if true, then the speed is adapted to how close the target
        is and the speed paramater is interpreted as the maximum speed
        """

        self.updateDest(dest, speed)
        NavStates.goToPosition.precision = precision
        NavStates.goToPosition.avoidObstacles = avoidObstacles
        NavStates.goToPosition.adaptive = adaptive

        if self.currentState is not 'goToPosition':
            self.switchTo('goToPosition')

    def updateDest(self, dest, speed = KEEP_SAME_SPEED):
        """  Update the destination we're headed to   """
        NavStates.goToPosition.dest = dest
        if speed is not KEEP_SAME_SPEED:
            NavStates.goToPosition.speed = speed

    def walkTo(self, walkToDest, speed = FULL_SPEED):
        """
        Walks to a RelRobotLocation
        Checks if reached the destination using odometry
        Great for close destinations (since odometry gets bad over time) in
        case loc is bad
        Doesn't avoid obstacles! (that would make it very confused and odometry
        very bad, especially if we're being pushed)
        Switches to standing at the end
        @todo: Calling this again before the other walk is done does some weird stuff
        """
        if not isinstance(walkToDest, RelRobotLocation):
            raise TypeError, "walkToDest must be a RelRobotLocation"

        NavStates.walkingTo.destQueue.clear()

        NavStates.walkingTo.destQueue.append(walkToDest)
        NavStates.walkingTo.speed = speed

        #reset the counter to make sure walkingTo.firstFrame() is true on entrance
        #in case we were in walkingTo before as well
        self.switchTo('walkingTo')

    def stop(self):
        """
        This is the same as standing because to end a walk
        we just make it stand
        """
        if self.currentState not in ['stopped', 'stand', 'standing']:
            self.stand()

    def orbitAngle(self, radius, angle):
        """
        Orbits a point at a certain radius for a certain angle using walkTo
        Splits the command into multiple smaller commands
        Don't rely on it too much since it depends on the odometry of strafes
        and turns which slip a lot
        It will orbit in steps, each orbit taking ~30 degrees (more like 45
        when I test it out)
        """

        NavStates.walkingTo.destQueue.clear()

        #@todo: make this a bit nicer or figure out a better way to do it
        # split it up in 15 degree moves; good enough approximation for small radii
        for k in range(0, abs(angle) / 15):
            if angle > 0:
                NavStates.walkingTo.destQueue.append(RelRobotLocation(0.0, radius / 6, 0.0))
                NavStates.walkingTo.destQueue.append(RelRobotLocation(0.0, 0.0, -15))
            else:
                NavStates.walkingTo.destQueue.append(RelRobotLocation(0.0, -radius / 6, 0.0))
                NavStates.walkingTo.destQueue.append(RelRobotLocation(0.0, 0.0, 15))

        NavStates.walkingTo.speed = FAST_SPEED
        self.switchTo('walkingTo')
        #self.walkTo(helper.getOrbitLocation(radius, angle), speed)
        #self.walk(0, .75, -.5)


    def walk(self, x, y, theta):
        """
        Starts a new walk command
        Does nothing if it is the same as the current walk
        Switches to it otherwise
        """
        NavStates.walking.speeds = (x, y, theta)
        self.switchTo('walking')

    def stand(self):
        """
        Make the robot stand; Standing should be the default action when we're not
        walking/executing a sweet move
        """
        if self.currentState not in ['stand', 'standing']:
            self.switchTo('stand')

    # informative methods
    def isAtPosition(self):
        return self.currentState is 'atPosition'

    def isStanding(self):
        return self.currentState in ['standing', 'stand']

    def isStopped(self):
        return self.currentState in ['stopped', 'standing']

    def spinDirection(self):
        """ Returns LEFT or RIGHT depending on navigation direction """

        #@todo: put in a threshold since a relative destination
        # will never  have heading 0; I don't know how important that is
        if self.currentState is 'goToPosition':
            if NavStates.goToPosition.deltaDest.relH > 0:
                return LEFT
            elif NavStates.goToPosition.deltaDest.relH < 0:
                return RIGHT
            else:
                return 0

        if self.currentState is 'walkingTo':
            if NavStates.walkingTo.deltaDest.relH > 0:
                return LEFT
            elif NavStates.walkingTo.deltaDest.relH < 0:
                return RIGHT
            else:
                return 0

        if self.currentState is 'walking':
            if NavStates.walking.speeds[2] > 0:
                return LEFT
            elif NavStates.walking.speeds[2] < 0:
                return RIGHT
            else:
                return 0

    def isSpinningLeft(self):
        return self.spinDirection() == LEFT

    def isSpinningRight(self):
        return self.spinDirection() == RIGHT
