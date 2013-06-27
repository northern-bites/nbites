import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
from ..navigator import Navigator
import noggin_constants as NogginConstants
from objects import Location

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
        player.brain.nav.positionPlaybook()
        player.brain.tracker.repeatBasicPan() # TODO Landmarks

    if player.brain.nav.isAtPosition():
        player.brain.tracker.trackBall()
        return player.stay()

    if (not player.brain.nav.isAtPosition() and
        player.brain.time - player.timeReadyBegan > 38):
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
        player.brain.nav.positionPlaybook()
        player.brain.tracker.trackBall()

    if player.brain.play.isChaser() and transitions.shouldChaseBall(player):
        return player.goLater('chase')

    if transitions.shouldFindBallPosition(player):
        return player.goLater('findBall')

    if player.brain.locUncert > 75:
        # Find some goalposts (preferably close ones) and walk toward them.
        pass

    return player.stay()
