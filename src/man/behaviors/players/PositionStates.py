import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
from ..navigator import Navigator
import noggin_constants as NogginConstants
from objects import Location, RelRobotLocation
from ..playbook import PBConstants
from . import BoxPositionConstants as BPConstants
from . import SharedTransitions

def playbookPosition(player):
    """
    Super State for Non Chasers
    """
    if player.gameState == 'gameReady':
        return player.goNow('positionReady')
    else:
        return player.goNow('positionPlaying')

def positionReady(player):
    """
    Game Ready positioning
    """
    if player.firstFrame():
        if player.usingBoxPositions:
            if(player.brain.gameController.ownKickOff 
               and not player.isDefender):
                player.kickoffPosition = BPConstants.ourKickoff
            elif not player.isDefender:
                player.kickoffPosition = BPConstants.theirKickoff

            player.brain.nav.goTo(player.kickoffPosition, 
                                  precision = Navigator.PLAYBOOK,
                                  speed = Navigator.QUICK_SPEED, 
                                  avoidObstacles = True, 
                                  fast = False, pb = False)
        else:
            player.brain.nav.positionPlaybook()
        player.brain.tracker.repeatBasicPan() # TODO Landmarks

    if (player.brain.nav.isAtPosition()): #or
#        SharedTransitions.atRobotLocation(player.kickoffPosition, 
#                                          Navigator.GENERAL_AREA)):
        player.brain.tracker.trackBall()
        #player.brain.nav.stand()
        return player.stay()

    if (not player.brain.nav.isAtPosition() and
        player.brain.time - player.timeReadyBegan > 38):
        print "IT'S BEEN TOO LONG!"
        return player.goNow('readyFaceMiddle')

    return player.stay()

def readyFaceMiddle(player):
    """
    If we didn't make it to our position, find the middle of the field
    """
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)
        player.stand()
        readyFaceMiddle.startedSpinning = False
        readyFaceMiddle.done = False

    centerField = Location(NogginConstants.CENTER_FIELD_X,
                           NogginConstants.CENTER_FIELD_Y)

    if player.brain.nav.isStopped() and not readyFaceMiddle.startedSpinning:
        readyFaceMiddle.startedSpinning = True
        spinDir = player.brain.loc.spinDirToPoint(centerField)
        player.setWalk(0,0,spinDir*0.3)

    elif (not readyFaceMiddle.done and readyFaceMiddle.startedSpinning and
        ((player.brain.ygrp.on and
          player.brain.ygrp.distance > NogginConstants.MIDFIELD_X + 200) or
        (player.brain.yglp.on and
         player.brain.yglp.distance > NogginConstants.MIDFIELD_X + 200))):
        print "Found a post at {0} or {1}".format(player.brain.ygrp.distance,
                                                  player.brain.yglp.distance)
        readyFaceMiddle.done = True
        player.brain.tracker.repeatBasicPan()
        player.stopWalking()

    return player.stay()

readyFaceMiddle.done = False
readyFaceMiddle.startedSpinning = False

def positionPlaying(player):
    """
    Game Playing positioning
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.usingBoxPositions:
            return player.goLater('positionAtHome')

    if player.brain.play.isChaser() and transitions.shouldChaseBall(player):
        return player.goLater('chase')

    if transitions.shouldFindBallPosition(player):
        #return player.goLater('findBall')
        pass

    if player.brain.locUncert > 75:
        # Find some goalposts (preferably close ones) and walk toward them.
        pass

    return player.stay()

def hackWalkForward(player):
    if player.firstFrame():
        player.brain.nav.walkTo(RelRobotLocation(200, 0, 0))
        player.brain.tracker.lookToAngle(-30)
        return player.stay()
    elif player.brain.nav.isStanding():
        return player.goNow('positionPlaying')

    return player.stay()
