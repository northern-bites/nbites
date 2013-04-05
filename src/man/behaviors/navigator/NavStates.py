import NavConstants as constants
import NavHelper as helper
import NavTransitions as navTrans
from collections import deque
from objects import RobotLocation, RelRobotLocation
from ..util import Transition
from math import fabs

DEBUG = False

def scriptedMove(nav):
    '''State that we stay in while doing sweet moves'''
    if nav.firstFrame():
        helper.executeMove(nav, scriptedMove.sweetMove)
        return nav.stay()

    if not nav.brain.interface.motionStatus.body_is_active:
        return nav.goNow('stopped')

    return nav.stay()

scriptedMove.sweetMove = None

def goToPosition(nav):
    """
    Go to a position set in the navigator. General go to state.  Goes
    towards a location on the field stored in dest.
    The location can be a RobotLocation, Location, RelRobotLocation, RelLocation
    Absolute locations get transformed to relative locations based on current loc
    For relative locations we use our bearing to that point as the heading
    """
    relDest = helper.getRelativeDestination(nav.brain.loc, goToPosition.dest)
    goToPosition.deltaDest = relDest # cache it for later use

#    if nav.counter % 10 is 0:
#        print "going to " + str(relDest)
#        print "ball is at {0}, {1}, {2} ".format(nav.brain.ball.loc.relX,
#                                                 nav.brain.ball.loc.relY,
#                                                 nav.brain.ball.loc.bearing)
    if goToPosition.adaptive and relDest.relX >= 0:
        #reduce the speed if we're close to the target
        speed = helper.adaptSpeed(relDest.dist,
                                 constants.ADAPT_DISTANCE,
                                 goToPosition.speed)
    else:
        speed = goToPosition.speed

#    print "distance {0} and speed {1}".format(relDest.dist, speed)

    #if y-distance is small, ignore it to avoid strafing
    #strafelessDest = helper.getStrafelessDest(relDest)
    helper.setDestination(nav, relDest, speed)

#    if navTrans.shouldAvoidObstacle(nav):
#        return nav.goLater('avoidObstacle')

    return Transition.getNextState(nav, goToPosition)

goToPosition.speed = "speed gain from 0 to 1"
goToPosition.dest = "destination, can be any type of location"
goToPosition.deltaDest = "how much we have left to travel to location (or rel destination)"
goToPosition.adaptive = "adapts the speed to the distance of the destination"
goToPosition.precision = "how precise we want to be in moving"

def avoidLeft(nav):
    if nav.firstFrame():
        avoidDest = RelRobotLocation(0, 25, 0)
        helper.setOdometryDestination(nav, avoidDest)
        return nav.stay()

    return Transition.getNextState(nav, avoidLeft)

def avoidRight(nav):
    if nav.firstFrame():
        avoidDest = RelRobotLocation(0, -25, 0)
        helper.setOdometryDestination(nav, avoidDest)
        return nav.stay()

    return Transition.getNextState(nav, avoidRight)

def briefStand(nav):
    if nav.firstFrame():
        helper.stand(nav)

    if nav.counter > 3:
        return nav.goLater('goToPosition')

    return nav.stay()

def walkingTo(nav):
    """
    Walks to a relative location based on odometry
    """
    if nav.firstFrame():
        helper.stand(nav)
        return nav.stay()

    if nav.brain.interface.motionStatus.standing:
        if len(walkingTo.destQueue) > 0:
            dest = walkingTo.destQueue.popleft()
            helper.setOdometryDestination(nav, dest, walkingTo.speed)
            return nav.stay()
        else:
            return nav.goNow('standing')

    return nav.stay()

walkingTo.destQueue = deque()
walkingTo.speed = 0

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """

    if ((walking.speeds != walking.lastSpeeds)
        or not nav.brain.interface.motionStatus.walk_is_active):
        helper.setSpeed(nav, walking.speeds)
    walking.lastSpeeds = walking.speeds

    return Transition.getNextState(nav, walking)

walking.speeds = constants.ZERO_SPEEDS     # current walking speeds
walking.lastSpeeds = constants.ZERO_SPEEDS # useful for knowing if speeds changed
walking.transitions = {}

### Stopping States ###
def stopped(nav):
    return nav.stay()

def atPosition(nav):
    if nav.firstFrame():
        nav.brain.speech.say("At Position")
        helper.stand(nav)

    return Transition.getNextState(nav, atPosition)

def stand(nav):
    """
    Transitional state between walking and standing
    So we can give new walk commands before we complete
    the stand if desired
    """
    if nav.firstFrame():
        helper.stand(nav)
        return nav.stay()

    if not nav.brain.interface.motionStatus.walk_is_active:
        return nav.goNow('standing')

    return nav.stay()

def standing(nav):
    """
    Complete walk standstill
    """
    return nav.stay()
