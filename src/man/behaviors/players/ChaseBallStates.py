"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
from ..navigator import Navigator
from ..kickDecider import HackKickInformation as hackKick
from ..kickDecider import kicks
from objects import RelRobotLocation
from math import fabs
import noggin_constants as nogginConstants

def chase(player):
    """
    Super State to determine what to do from various situations
    """
    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    else:
        return player.goNow('spinToBall')

def spinToBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        player.brain.nav.stand()

    if transitions.shouldFindBall(player):
        return player.goLater('chase')

    if transitions.shouldStopSpinningToBall(player):
        return player.goNow('approachBall')
    else:
        spinDir = player.brain.my.spinDirToPoint(player.brain.ball.loc)
        if fabs(player.brain.ball.loc.bearing) > constants.CHANGE_SPEED_THRESH:
            speed = Navigator.GRADUAL_SPEED
        else:
            speed = Navigator.SLOW_SPEED
        player.setWalk(0,0,spinDir*speed)
        return player.stay()

def approachBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if player.shouldKickOff:
            player.brain.nav.chaseBall(Navigator.QUICK_SPEED)
        else:
            player.brain.nav.chaseBall()

    if (transitions.shouldFindBall(player) or
        transitions.shouldSpinToBall(player)):
        return player.goLater('chase')

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
        player.inKickingState = True
        if player.shouldKickOff:
            if player.brain.ball.loc.relY > 0:
                player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK
            else:
                player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
            player.shouldKickOff = False
            return player.goNow('positionForKick')
        else:
            return player.goNow('prepareForKick')
    else:
        return player.stay()

def prepareForKick(player):
    if player.firstFrame():
        prepareForKick.hackKick = hackKick.KickInformation(player.brain)
        player.orbitDistance = player.brain.ball.loc.dist
        player.brain.tracker.performKickPanFixedPitch(prepareForKick.hackKick.shouldKickPanRight())
        player.brain.nav.stand()
        return player.stay()

    prepareForKick.hackKick.collectData()

    if player.brain.ball.loc.dist > 40:
        # Ball has moved away. Go get it!
        player.inKickingState = False
        return player.goLater('chase')

    # If loc is good, stop pan ASAP and do the kick
    # Loc is currently never accurate enough @summer 2012
    #  Might have to do it anyway if comm is always down.

    # If hackKickInfo has enough information already, prematurely end pan and kick.
    if player.brain.tracker.isStopped() or \
            prepareForKick.hackKick.hasEnoughInformation():
        prepareForKick.hackKick.calculateDataAverages()
        if hackKick.DEBUG_KICK_DECISION:
            print str(prepareForKick.hackKick)
        player.kick = prepareForKick.hackKick.shoot()
        if hackKick.DEBUG_KICK_DECISION:
            print str(player.kick)
        return player.goNow('orbitBall')

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():

        if hackKick.DEBUG_KICK_DECISION:
            print "Orbiting at angle: ",player.kick.h

        if player.kick.h == 0:
            return player.goNow('positionForKick')

        # Reset from pre-kick pan to straight, then track the ball.
        player.brain.tracker.lookStraightThenTrackFixedPitch()
        player.brain.nav.orbitAngle(player.orbitDistance, player.kick.h)

    elif player.brain.nav.isStopped():
        player.shouldOrbit = False
        player.kick.h = 0
        if player.kick == kicks.ORBIT_KICK_POSITION:
            return player.goNow('prepareForKick')
        else:
            player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
            return player.goNow('positionForKick')

    if (transitions.shouldFindBallKick(player) or
        transitions.shouldCancelOrbit(player)):
        player.inKickingState = False
        return player.goLater('chase')

    return player.stay()

def positionForKick(player):
    """
    Get the ball in the sweet spot
    """
    if (transitions.shouldApproachBallAgain(player) or
        transitions.shouldRedecideKick(player)):
        player.inKickingState = False
        return player.goLater('chase')

    ballLoc = player.brain.ball.loc
    kick_pos = player.kick.getPosition()
    positionForKick.kickPose = RelRobotLocation(ballLoc.relX - kick_pos[0],
                                                ballLoc.relY - kick_pos[1],
                                                0)

    #only enque the new goTo destination once
    if player.firstFrame():
        # Safer when coming from orbit in 1 frame. Still works otherwise, too.
        player.brain.tracker.lookStraightThenTrackFixedPitch()
        player.brain.nav.goTo(positionForKick.kickPose,
                              Navigator.PRECISELY,
                              Navigator.GRADUAL_SPEED,
                              False,
                              Navigator.ADAPTIVE)
    else:
        player.brain.nav.updateDest(positionForKick.kickPose)

    if transitions.shouldFindBallKick(player) and player.counter > 15:
        player.inKickingState = False
        return player.goLater('chase')

    if (transitions.ballInPosition(player, positionForKick.kickPose) or
        player.brain.nav.isAtPosition()):
        player.brain.nav.stand()
        return player.goNow('kickBallExecute')

    return player.stay()

def lookAround(player):
    """
    Nav is stopped. We want to look around to get better loc.
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.stopHeadMoves() # HACK so that tracker goes back to stopped.
        player.brain.tracker.repeatBasicPanFixedPitch()

    # Make sure we leave this state...
    if player.brain.ball.vis.frames_off > 200:
        return player.goLater('chase')

    if player.brain.tracker.isStopped() and player.counter > 2:
            player.brain.tracker.trackBallFixedPitch()
            player.brain.kickDecider.decideKick()
            if transitions.shouldOrbit(player) and not player.penaltyKicking:
                print "Don't have a kick, orbitting"
                return player.goNow('orbitBall')
            else:
                return player.goLater('chase')

    return player.stay()
