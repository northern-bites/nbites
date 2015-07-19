import NavConstants as constants
import NavHelper as helper
import NavTransitions as navTrans
import Navigator as Navigator
from collections import deque
from objects import RobotLocation, RelRobotLocation
from ..util import *
from math import fabs, degrees, copysign
from random import random

def scriptedMove(nav):
    """
    State that we stay in while doing sweet moves
    """
    if nav.firstFrame():
        helper.executeMove(nav, scriptedMove.sweetMove)
        return nav.stay()

    if not nav.brain.interface.motionStatus.body_is_active:
        return nav.goNow('stopped')

    return nav.stay()

scriptedMove.sweetMove = None

def kickEngine(nav):
    """
    State that we stay in while calling kick engine
    """
    if nav.firstFrame():
        helper.executeKickEngine(nav, kickEngine.kickType)
        return nav.stay()

    helper.stand(nav)
    return nav.stay()

kickEngine.kickType = None

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

    goToPosition.speed = nav.velocity
    if fabs(nav.requestVelocity - nav.velocity) > Navigator.SPEED_CHANGE:
        nav.velocity += copysign(Navigator.SPEED_CHANGE, (nav.requestVelocity - nav.velocity))

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

    if navTrans.shouldDodge(nav):
        return nav.goNow('dodge')
        
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
    if nav.firstFrame():
        nav.dodging = True

        if nav.brain.ball.vis.frames_off < 5:
            nav.brain.tracker.trackBall()
        else:
            nav.brain.tracker.trackObstacle(dodge.obstaclePosition)

        ## SET UP the dodge direction based on where the obstacle is
        # if directly in front of us, move back and to one side based on
        # where the goToPosition dest is
        dodge.speed = Navigator.BRISK_SPEED

        obstacleInfo = constants.OBS_DICT[dodge.obstaclePosition]
        helper.createAndSendWalkVector(nav, 
                                        dodge.speed*obstacleInfo[0], 
                                        dodge.speed*obstacleInfo[1], 
                                        0)
        print "Dodging ", obstacleInfo[2], " Obstacle"

    if navTrans.doneDodging(nav):
        nav.dodging = False
        nav.brain.tracker.repeatBasicPan()
        return nav.goLater('briefStand')

    return nav.stay()

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

    destinationWalkingTo.speed = nav.velocity
    if fabs(nav.requestVelocity - nav.velocity) > Navigator.SPEED_CHANGE:
            nav.velocity += copysign(Navigator.SPEED_CHANGE, (nav.requestVelocity - nav.velocity))

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

def locationsMatch(odom, dest):
    if (abs(odom.relX - dest.relX) < constants.ODOM_CLOSE_ENOUGH) \
    and (abs(odom.relY - dest.relY) < constants.ODOM_CLOSE_ENOUGH) \
    and (abs(odom.relH - degrees(dest.relH)) < constants.ODOM_CLOSE_ENOUGH):
        return True

    # print ("Not true; relx diff:", abs(odom.relX - dest.relX))
    # print ("relly diff: ", abs(odom.relY - dest.relY))
    # print ("relh diff:", abs(odom.relH - dest.relH))
    # print ("degres", degrees(dest.relH), "normal: ", dest.relH)
    # if nav.counter % 30 == 0:
    #     print ("odomH: ", odom.relH, "odomDeg", degrees(odom.relH), "destH:", dest.relH, "destH deg:", degrees(dest.relH))

    return False

def walkingTo(nav):
    """
    State to be used for odometry walking.
    """
    if nav.firstFrame():
        print ("Resetting odometry!")
        nav.brain.interface.motionRequest.reset_odometry = True
        nav.brain.interface.motionRequest.timestamp = int(nav.brain.time * 1000)
        print ("MY dest: ", nav.destination.relX, nav.destination.relY, nav.destination.relH)
        helper.stand(nav)
        return nav.stay()


    walkingTo.currentOdo = RelRobotLocation(nav.brain.interface.odometry.x,
                         nav.brain.interface.odometry.y,
                         nav.brain.interface.odometry.h)

    # TODO why check standing?
    if nav.brain.interface.motionStatus.standing:

        if len(walkingTo.destQueue) > 0:
            dest = walkingTo.destQueue.popleft()
            # dest.relH = 0
            helper.setOdometryDestination(nav, dest, walkingTo.speed)
            # helper.setDestination(nav, dest, walkingTo.speed)
            print ("MY dest: ", dest.relX, dest.relY, dest.relH)

    if locationsMatch(nav.destination, walkingTo.currentOdo):
        return nav.goNow('standing')

    if nav.counter % 30 == 0:
        print "Current odo:"
        print ("x:", walkingTo.currentOdo.relX)
        print ("y:", walkingTo.currentOdo.relY)
        print ("h:", walkingTo.currentOdo.relH)

    return nav.stay()

walkingTo.destQueue = deque()
walkingTo.speed = 0

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used for velocity walking.
    """
    helper.setSpeed(nav, walking.speeds)

    if navTrans.shouldDodge(nav):
        return nav.goNow('dodge')

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
