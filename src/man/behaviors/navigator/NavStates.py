import NavConstants as constants
import NavHelper as helper
import NavTransitions as navTrans
from collections import deque
from objects import RobotLocation, RelRobotLocation
from ..util import Transition
from math import fabs
from random import random

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

    #if nav.counter % 10 is 0:
    #    print "going to " + str(relDest)
    #    print "ball is at {0}, {1}, {2} ".format(nav.brain.ball.loc.relX,
    #                                             nav.brain.ball.loc.relY,
    #                                             nav.brain.ball.loc.bearing)

    if goToPosition.pb:
        # Calc dist to dest
        dist = helper.getDistToDest(nav.brain.loc, goToPosition.dest)
        if goToPosition.fast and dist < 140:
            goToPosition.fast = False
            goToPosition.dest = nav.brain.play.getPosition()
        elif not goToPosition.fast and dist > 160:
            goToPosition.fast = True
            goToPosition.dest = nav.brain.play.getPositionCoord()

    if goToPosition.fast:
        # So that fast mode works for objects of type RobotLocation also
        if isinstance(goToPosition.dest, RobotLocation) and not goToPosition.close:
            fieldDest = RobotLocation(goToPosition.dest.x, goToPosition.dest.y, 0)
            relDest = nav.brain.loc.relativeRobotLocationOf(fieldDest)
            relDest.relH = nav.brain.loc.getRelativeBearing(fieldDest)

        HEADING_ADAPT_CUTOFF = 103
        DISTANCE_ADAPT_CUTOFF = 10

        MAX_TURN = .5

        BOOK_IT_TURN_THRESHOLD = 23
        BOOK_IT_DISTANCE_THRESHOLD = 50

        if relDest.relH >= HEADING_ADAPT_CUTOFF:
            velH = MAX_TURN
        elif relDest.relH <= -HEADING_ADAPT_CUTOFF:
            velH = -MAX_TURN
        else:
            velH = helper.adaptSpeed(relDest.relH,
                                    HEADING_ADAPT_CUTOFF,
                                    MAX_TURN)

        if relDest.relX >= DISTANCE_ADAPT_CUTOFF:
            velX = goToPosition.speed
        elif relDest.relX <= -DISTANCE_ADAPT_CUTOFF:
            velX = -goToPosition.speed
        else:
            velX = helper.adaptSpeed(relDest.relX,
                                    DISTANCE_ADAPT_CUTOFF,
                                    goToPosition.speed)

        if relDest.relY >= DISTANCE_ADAPT_CUTOFF:
            velY = goToPosition.speed
        elif relDest.relY <= -DISTANCE_ADAPT_CUTOFF:
            velY = -goToPosition.speed
        else:
            velY = helper.adaptSpeed(relDest.relY,
                                    DISTANCE_ADAPT_CUTOFF,
                                    goToPosition.speed)

        if fabs(relDest.dist) > BOOK_IT_DISTANCE_THRESHOLD:
            goToPosition.close = False
            if fabs(relDest.relH) > BOOK_IT_TURN_THRESHOLD:
                if relDest.relH > 0: velH = MAX_TURN
                if relDest.relH < 0: velH = -MAX_TURN
                velX = 0
                velY = 0
                goToPosition.bookingIt = False
            else:
                velY = 0
                goToPosition.bookingIt = True
        else:
            goToPosition.close = True

        goToPosition.speeds = (velX, velY, velH)
        helper.setSpeed(nav, goToPosition.speeds)

    else:
        if goToPosition.adaptive:
            #reduce the speed if we're close to the target
            speed = helper.adaptSpeed(relDest.dist,
                                    constants.ADAPT_DISTANCE,
                                    goToPosition.speed)
        else:
            speed = goToPosition.speed

        helper.setDestination(nav, relDest, speed)

    return Transition.getNextState(nav, goToPosition)

goToPosition.speed = "speed gain from 0 to 1"
goToPosition.dest = "destination, can be any type of location"
goToPosition.adaptive = "adapts the speed to the distance of the destination"
goToPosition.precision = "how precise we want to be in moving"

goToPosition.speeds = ''
goToPosition.bookingIt = False
goToPosition.close = False

# State where we are moving away from an obstacle
def dodge(nav):
    order = [0, 1, -1, 2, -2, 3, -3, 4]
    if nav.firstFrame():
        # dodge.positions[0] is position.NONE, so direction numbers are their own index
        for i in range(len(order)):
            temp = getIndex(int(dodge.targetDest) + order[i])
            # if there is no obstacle in this direction
            if not dodge.positions[temp]:
                print "DODGE TO ", dodge.DDirects[temp]
                dodge.dest = RelRobotLocation(constants.DGE_DESTS[temp-1][0],
                                              constants.DGE_DESTS[temp-1][1],
                                              constants.DGE_DESTS[temp-1][2])
                break

    # TODO the worst hack I have ever written, sorry -- Josh Imhoff
    dest2 = RelRobotLocation(dodge.dest.relX + random(),
                             dodge.dest.relY + random(),
                             dodge.dest.relH + random())
    helper.setDestination(nav, dest2, 0.5)
    return Transition.getNextState(nav, dodge)

def getIndex(num):
    if num <=8 and num >= 1:
        return num
    elif num > 8:
        return num - 8
    else:
        return num + 8

# Quick stand to stabilize from the dodge.
def briefStand(nav):
    if nav.firstFrame():
        helper.stand(nav)

    if nav.counter > 3:
        return nav.goLater('goToPosition')

    return nav.stay()

def destinationWalkingTo(nav):
    """
    State to be used for destination walking.
    """
    if nav.firstFrame():
        destinationWalkingTo.enqueAZeroVector = False

    if len(destinationWalkingTo.destQueue) > 0:
        dest = destinationWalkingTo.destQueue.popleft()
        helper.setDestination(nav, dest,
                              destinationWalkingTo.speed,
                              destinationWalkingTo.kick)
        destinationWalkingTo.enqueAZeroVector = True
    elif destinationWalkingTo.enqueAZeroVector:
        helper.setDestination(nav, RelRobotLocation(0,0,0),
                              destinationWalkingTo.speed,
                              destinationWalkingTo.kick)
        destinationWalkingTo.enqueAZeroVector = False

    return nav.stay()

destinationWalkingTo.destQueue = deque()
destinationWalkingTo.speed = 0

def walkingTo(nav):
    """
    State to be used for odometry walking.
    """
    if nav.firstFrame():
        helper.stand(nav)
        return nav.stay()

    # TODO why check standing?
    if nav.brain.interface.motionStatus.standing:
        if len(walkingTo.destQueue) > 0:
            dest = walkingTo.destQueue.popleft()
            helper.setOdometryDestination(nav, dest, walkingTo.speed)
        else:
            return nav.goNow('standing')

    return nav.stay()

walkingTo.destQueue = deque()
walkingTo.speed = 0

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used for velocity walking.
    """
    helper.setSpeed(nav, walking.speeds)

    return Transition.getNextState(nav, walking)

walking.speeds = constants.ZERO_SPEEDS     # current walking speeds
walking.transitions = {}

### Stopping States ###
def stopped(nav):
    return nav.stay()

def atPosition(nav):
    """
    Switches back if we're not at the destination anymore.
    """
    if nav.firstFrame():
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

    if (nav.counter % 300 == 0):
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
