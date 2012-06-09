import NavConstants as constants
import NavHelper as helper
import NavTransitions as navTrans
from objects import RobotLocation, RelRobotLocation
from ..util import Transition
from math import fabs

DEBUG = False

def scriptedMove(nav):
    '''State that we stay in while doing sweet moves'''
    if nav.firstFrame():
        helper.executeMove(nav.brain.motion, scriptedMove.sweetMove)
        return nav.stay()

    if not nav.brain.motion.isBodyActive():
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
        
    relDest = helper.getRelativeDestination(nav.brain.my, goToPosition.dest)
    goToPosition.deltaDest = relDest # cache it for later use
        
#    if nav.counter % 10 is 0:
#        print "going to " + str(relDest)
#        print "ball is at {0}, {1}, {2} ".format(nav.brain.ball.loc.relX, 
#                                                 nav.brain.ball.loc.relY, 
#                                                 nav.brain.ball.loc.bearing)

    if goToPosition.adaptive:
        speed = helper.adaptSpeed(relDest.dist, constants.ADAPT_DISTANCE, goToPosition.speed)
    else:
        speed = goToPosition.speed

#    print "distance {0} and speed {1}".format(relDest.dist, speed)

    #reduce the speed if we're close to the target

    #if y-distance is small, ignore it to avoid strafing
    strafelessDest = helper.getStrafelessDest(relDest)
    helper.setDestination(nav, strafelessDest, speed)

#    if navTrans.shouldAvoidObstacle(nav):
#        return nav.goLater('avoidObstacle')

    return Transition.getNextState(nav, goToPosition)

goToPosition.speed = "speed gain from 0 to 1"
goToPosition.dest = "destination, can be any type of location"
goToPosition.deltaDest = "how much we have left to travel to location (or rel destination)"
goToPosition.adaptive = "adapts the speed to the distance of the destination"
goToPosition.precision = "how precise we want to be in moving"

def walkingTo(nav):
    """
    Walks to a relative location based on odometry
    """
    loc = nav.brain.loc
    dest = walkingTo.dest
        
    if nav.firstFrame():
        #@todo: make a method that returns odometry as a tuple in PyLoc?
        walkingTo.startingOdometry = (loc.lastOdoX, loc.lastOdoY, loc.lastOdoTheta)
    
    deltaOdo = helper.getDeltaOdometry(loc, walkingTo.startingOdometry)
    walkingTo.deltaDest = dest - (deltaOdo.relX, deltaOdo.relY, deltaOdo.relH)
#    print "Delta dest {0}".format(walkingTo.deltaDest)
#    print str(dest)
#    print str(deltaOdo)
    #walk the rest of the way
    helper.setDestination(nav, walkingTo.deltaDest, walkingTo.speed)
    
    return Transition.getNextState(nav, walkingTo)
    
walkingTo.dest = RelRobotLocation(0, 0, 0) 
walkingTo.deltaDest = RelRobotLocation(0, 0, 0) # how much do we have left to walk
walkingTo.speed = 0
walkingTo.precision = (0, 0, 0)

# WARNING: avoidObstacle could possibly go into our own box
def avoidObstacle(nav):
    """
    If we detect something in front of us, dodge it
    """

    if nav.firstFrame():
        nav.brain.speech.say("Avoid obstacle")

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    # store previous state here, b/c lastDiffState gets
    # replaced when we perform 'goNow'
    nav.preAvoidState = nav.lastDiffState

    if (avoidLeft and avoidRight):
        return nav.goNow('avoidFrontObstacle')
    elif avoidLeft:
        return nav.goNow('avoidLeftObstacle')
    elif avoidRight:
        return nav.goNow('avoidRightObstacle')
    else:
        return nav.goLater(nav.lastDiffState)


def avoidFrontObstacle(nav):
    # Backup
    # strafe away from the closer one?
    # strafe towards dest?

    # ever a good time to backup?
    # we'll probably want to go forward again and most obstacle
    # are moving, so pausing might make more sense

    # TODO figure this out maybe potential field will fix this for us???
    if nav.firstFrame():
        nav.doneAvoidingCounter = 0
        nav.printf(nav.brain.sonar)
        nav.printf("Avoid by backup");
        helper.setSpeed(nav, (constants.DODGE_BACK_SPEED, 0, 0))

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    if (avoidLeft and avoidRight):
        nav.doneAvoidingCounter -= 1
        nav.doneAvoidingCounter = max(0, nav.doneAvoidingCounter)
        return nav.stay()

    elif avoidRight:
        return nav.goLater('avoidRightObstacle')

    elif avoidLeft:
        return nav.goLater('avoidLeftObstacle')

    else:
        nav.doneAvoidingCounter += 1

    if nav.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        nav.shouldAvoidObstacleRight = 0
        nav.shouldAvoidObstacleLeft = 0
        return nav.goLater(nav.preAvoidState)

    return nav.stay()

def avoidLeftObstacle(nav):
    """
    dodges right if we only detect something to the left of us
    """
    if nav.firstFrame():
        nav.doneAvoidingCounter = 0
        nav.printf(nav.brain.sonar)
        nav.printf("Avoid by right dodge");
        helper.setSpeed(nav, (0, constants.DODGE_RIGHT_SPEED, 0))

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    if (avoidLeft and avoidRight):
        return nav.goLater('avoidFrontObstacle')
    elif avoidRight:
        return nav.goLater('avoidRightObstacle')
    elif avoidLeft:
        nav.doneAvoidingCounter -= 1
        nav.doneAvoidingCounter = max(0, nav.doneAvoidingCounter)
        return nav.stay()
    else:
        nav.doneAvoidingCounter += 1

    if nav.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        nav.shouldAvoidObstacleRight = 0
        nav.shouldAvoidObstacleLeft = 0
        return nav.goLater(nav.preAvoidState)

    return nav.stay()

def avoidRightObstacle(nav):
    """
    dodges left if we only detect something to the left of us
    """
    if nav.firstFrame():
        nav.doneAvoidingCounter = 0
        nav.printf(nav.brain.sonar)
        nav.printf("Avoid by left dodge");
        helper.setSpeed(nav, (0, constants.DODGE_LEFT_SPEED, 0))

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    if (avoidLeft and avoidRight):
        return nav.goLater('avoidFrontObstacle')
    elif avoidLeft:
        return nav.goLater('avoidLeftObstacle')
    elif avoidRight:
        nav.doneAvoidingCounter -= 1
        nav.doneAvoidingCounter = max(0, nav.doneAvoidingCounter)
        return nav.stay()
    else:
        nav.doneAvoidingCounter += 1

    if nav.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        nav.shouldAvoidObstacleRight = 0
        nav.shouldAvoidObstacleLeft = 0
        return nav.goLater(nav.preAvoidState)

    return nav.stay()


# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """
    
    if (walking.speeds != walking.lastSpeeds) or not nav.brain.motion.isWalkActive():
        helper.setSpeed(nav, walking.speeds)
    walking.lastSpeeds = walking.speeds
    
    return Transition.getNextState(nav, walking)

walking.speeds = constants.ZERO_SPEEDS     # current walking speeds
walking.lastSpeeds = constants.ZERO_SPEEDS # useful for knowing if speeds changed
walking.transitions = {}

### Stopping States ###
def stop(nav):
    """
    Wait until the walk is finished.
    """
    if nav.firstFrame():
        # stop walk vectors
        helper.stand(nav)
        
    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()

def stopped(nav):
    return nav.stay()

def atPosition(nav):
    if nav.firstFrame():
        nav.brain.speech.say("At Position")
        helper.stand(nav)
    
    return Transition.getNextState(nav, atPosition)

def standing(nav):
    if nav.firstFrame():
        helper.stand(nav)
        
    return nav.stay()
