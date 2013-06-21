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

    if player.brain.time - player.timeReadyBegan > 35:
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

    centerField = Location(NogginConstants.CENTER_FIELD_X,
                           NogginConstants.CENTER_FIELD_Y)

    if nav.isStopped() and not readyFaceMiddle.startedSpinning:
        readyFaceMiddle.startedSpinning = True
        spinDir = player.brain.loc.spinDirToPoint(centerField)
        player.setWalk(0,0,spinDir*constants.FIND_BALL_SPIN_SPEED)

    targetH = player.brain.loc.headingTo(centerField)

    if ((targetH - 10 < player.brain.loc.h < targetH + 10) or
        (player.brain.ygrp.on and
         player.brain.ygrp.distance > 0.5*NogginConstants.MIDFIELD_X) or
        (player.brain.yglp.on and
         player.brain.yglp.distance > 0.5*NogginConstants.MIDFIELD_X)):
        player.stopWalking()

    return player.stay()

readyFaceMiddle.startedSpinning = False

def positionPlaying(player):
    """
    Game Playing positioning
    """
    if player.firstFrame():
        player.brain.nav.positionPlaybook(Navigator.PLAYBOOK)
        player.brain.tracker.repeatBasicPan() # TODO Landmarks

    if player.brain.ball.on and player.brain.ball.distance < 100:
        player.brain.tracker.trackBall()
    else:
        player.brain.tracker.repeatBasicPan() # TODO Landmarks

    if player.brain.nav.isAtPosition():
        player.brain.tracker.trackBall()

    if player.brain.play.isChaser() and transitions.shouldChaseBall(player):
        return player.goLater('chase')

    if transitions.shouldFindBallPosition(player):
        return player.goLater('findBall')

    if player.brain.locUncert > 75:
        # Find some goalposts (preferably close ones) and walk toward them.
        pass

    return player.stay()
