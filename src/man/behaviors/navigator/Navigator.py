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

DEBUG_MOTION_STATUS = False

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

            Transition.CountTransition(navTrans.shouldDodge,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : NavStates.dodge

            }

        NavStates.dodge.transitions = {
            Transition.CountTransition(navTrans.doneDodging,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : NavStates.briefStand
            }

        NavStates.atPosition.transitions = {
            self.locRepositionTransition : NavStates.goToPosition
            }

    def run(self):
        FSA.FSA.run(self)

    def performSweetMove(self, move):
        """
        Do a sweet ass move. Goes to stopped after done.
        """
        NavStates.scriptedMove.sweetMove = move
        self.switchTo('scriptedMove')

    def positionPlaybook(self):
        """
        Calls goTo on the playbook position
        """
        self.goTo(self.brain.play.getPositionCoord(), precision = PRECISELY,
                  speed = QUICK_SPEED, avoidObstacles = True, fast = True, pb = True)

    def chaseBall(self, speed = FAST_SPEED, fast = False):
        """
        Calls goTo on ball, which should be a RobotLocation.

        Theoretically walks into the ball, so make sure to switch the behavior beforehand.
        """
        self.goTo(self.brain.ball, CLOSE_ENOUGH, speed, True, fast = fast)

    def goTo(self, dest, precision = GENERAL_AREA, speed = FULL_SPEED,
             avoidObstacles = False, adaptive = False, fast = False, pb = False):
        """
        General go to method.
        Ideal for going to a field position, or for going to a relative location
        that we can track/see.

        Goes to atPosition after done. At position will switch back to goingTo if
        the dest changes are we're not at the dest anymore (considering precision)

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
        is and the speed paramater is interpreted as the maximum speed; only affects
        the non-fast walk, since the fast walk adapts its velocities dynamically anyway.
        Don't use it the estimates to the target are good.

        @param avoidObstacle: uses obstacle avoidance

        @param fast: books it using velocity walk; Best if dest is straight ahead!
        Use it to look like a baller on the field.

        @param pb: Set true if playbook positioning so we switch from fast to odometry
        walk when in the general area of our target. This allows us to ignore playbook's
        requested heading until we are actually close to the (x, y) position, so we can
        walk fast to the destination then correct heading once we get there.
        """

        # Debug prints for motion status (seeking the walking not walking bug)
        if DEBUG_MOTION_STATUS:
            status = self.brain.interface.motionStatus
            print "DEBUG_MOTION_STATUS in nav.goTo():"
            print "Standing:       " + str(status.standing)
            print "body_is_active: " + str(status.body_is_active)
            print "walk_is_active: " + str(status.walk_is_active)
            print "head_is_active: " + str(status.head_is_active)
            print "calibrated:     " + str(status.calibrated)

        self.updateDest(dest, speed)
        NavStates.goToPosition.precision = precision
        NavStates.goToPosition.avoidObstacles = avoidObstacles
        NavStates.goToPosition.adaptive = adaptive
        NavStates.goToPosition.fast = fast
        NavStates.goToPosition.pb = pb

        if self.currentState is not 'goToPosition':
            self.switchTo('goToPosition')

    def updateDest(self, dest, speed = KEEP_SAME_SPEED):
        """  Update the destination we're headed to   """
        NavStates.goToPosition.dest = dest
        if speed is not KEEP_SAME_SPEED:
            NavStates.goToPosition.speed = speed

    def walkTo(self, walkToDest, speed = FULL_SPEED):
        """
        Walks to a RelRobotLocation while checking odometry to see if
        we reached the destination.
        Great for close destinations (since odometry gets bad over time) in
        case loc is bad.
        Doesn't avoid obstacles! (that would make it very confused and odometry
        very bad, especially if we're being pushed).
        Switches to standing at the end.
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
        Orbits a point at a certain radius for a certain angle using walkTo commands.
        Splits the command into multiple smaller commands
        Don't rely on it too much since it depends on the odometry of strafes
        and turns which slips a lot
        It will orbit in steps, each orbit taking ~30 degrees (more like 45
        when I test it out)
        TODO: please change this to just use a velocity walk -Octavian
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

    def walk(self, x, y, theta):
        """
        Starts a new velocity walk command.
        Does nothing if it the velocities the same as the current velocities.
        """
        NavStates.walking.speeds = (x, y, theta)
        self.switchTo('walking')

    def stand(self):
        """
        Make the robot stand. Standing should be the default action when we're not
        walking/executing a sweet move.
        """
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

    def getXSpeed(self):
        return NavStates.walking.speeds[0]

    def getYSpeed(self):
        return NavStates.walking.speeds[1]

    def getHSpeed(self):
        return NavStates.walking.speeds[2]

    def setXSpeed(self, x):
        NavStates.walking.speeds = (x, self.getYSpeed(), self.getHSpeed())

    def setYSpeed(self, y):
        NavStates.walking.speeds = (self.getXSpeed(), y, self.getHSpeed())

    def setHSpeed(self, h):
        NavStates.walking.speeds = (self.getXSpeed(), self.getYSpeed(), h)

