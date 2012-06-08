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
FAST_SPEED = 0.8
MEDIUM_SPEED = 0.6
CAREFUL_SPEED = 0.4
SLOW_SPEED = 0.2
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
        self.setPrintFunction(self.brain.out.printf)
        self.stateChangeColor = 'cyan'

        self.shouldAvoidObstacleLeftCounter = 0
        self.shouldAvoidObstacleRightCounter = 0
        
        #transitions
        #@todo: move this to the actual transitions file?
        self.atLocPositionTransition = Transition.CountTransition(navTrans.atDestination)
        self.locRepositionTransition = Transition.CountTransition(navTrans.notAtLocPosition, 
                                                                  Transition.SOME_OF_THE_TIME,
                                                                  Transition.LOW_PRECISION)
        self.walkingToTransition = Transition.CountTransition(navTrans.walkedEnough,
                                                              Transition.ALL_OF_THE_TIME,
                                                              Transition.INSTANT)
        
        NavStates.goToPosition.transitions = { NavStates.atPosition: self.atLocPositionTransition }
        NavStates.atPosition.transitions = { NavStates.goToPosition: self.locRepositionTransition }
        NavStates.walkingTo.transitions = { NavStates.standing: self.walkingToTransition }

    def run(self):
        FSA.FSA.run(self)

    def performSweetMove(self, move):
        """
        Navigator function to do the sweet move
        """
        NavStates.scriptedMove.sweetMove = move
        self.switchTo('scriptedMove')

    def positionPlaybook(self):
        self.goTo(self.brain.play.getPosition())
        
    def chaseBall(self):
        self.goTo(self.brain.ball.loc, CLOSE_ENOUGH, FULL_SPEED)

    def goTo(self, dest, precision = GENERAL_AREA, speed = FULL_SPEED, adaptive = False):
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
        @param precision: a tuple of deltaX, deltaY, deltaH for how close you want to get 
        to the location
        @param adaptive: if true, then the speed is adapted to how close the target is
        and the speed paramater is interpreted as the maximum speed
        """
        
        self.updateDest(dest, speed)
        NavStates.goToPosition.precision = precision
        NavStates.goToPosition.adaptive = adaptive
        
        if self.currentState is not 'goToPosition':
            self.switchTo('goToPosition')

    def updateDest(self, dest, speed = KEEP_SAME_SPEED):
        """  Update the destination we're headed to   """
        NavStates.goToPosition.dest = dest
        if speed is not KEEP_SAME_SPEED:
            NavStates.goToPosition.speed = speed

    def walkTo(self, walkToDest, precision = CLOSE_ENOUGH, speed = FULL_SPEED):
        """
        Walks to a RelRobotLocation
        Checks for the destination using odometry
        Great for close destinations (since odometry gets bad over time) in 
        case loc is bad
        Doesn't avoid obstacles! (that would make odometry very bad, especially if we're 
        being pushed)
        Switches to standing at the end
        @todo: Calling this again before the other walk is done does some weird stuff
        """
        if not isinstance(walkToDest, RelRobotLocation):
            raise TypeError, "walkToDest must be a RelRobotLocation"
        
        NavStates.walkingTo.dest = walkToDest
        NavStates.walkingTo.speed = speed
        NavStates.walkingTo.precision = precision
        
        #reset the counter to make sure walkingTo.firstFrame() is true on entrance
        #in case we were in walkingTo before as well
        self.counter = 0
        self.switchTo('walkingTo')

    def stop(self):
        if self.currentState not in ['stop', 'stopped']:
            self.switchTo('stop')

    def orbitAngle(self, radius, angle):
        """ 
        Orbits a point at a certain radius for a certain angle using walkTo
        WARNING: as of now angles that are greater than 90 degrees
        are iffy since the robot will try to cut a straight-ish path to that
        destination; a solution would be to enque several smaller
        walkTo's or something
        """
        self.walkTo(helper.getOrbitLocation(radius, angle), CLOSE_ENOUGH, SLOW_SPEED)

        

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
        self.switchTo('standing')

    # informative methods
    def isAtPosition(self):
        return self.currentState is 'atPosition'
    
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
        