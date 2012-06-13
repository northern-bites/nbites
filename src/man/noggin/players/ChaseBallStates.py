"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import GoalieTransitions as goalTran
from ..navigator import Navigator
from ..playbook.PBConstants import GOALIE
import man.motion.HeadMoves as HeadMoves
import man.noggin.kickDecider.HackKickInformation as hackKick
import man.noggin.kickDecider.kicks as kicks
from objects import RelRobotLocation
from math import fabs

def chase(player):
    """
    Super State to determine what to do from various situations
    """
    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    if player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')

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
            speed = Navigator.CAREFUL_SPEED
        else:
            speed = Navigator.SLOW_SPEED
        player.setWalk(0,0,spinDir*speed)
        return player.stay()

def approachBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        player.brain.nav.chaseBall()

    if (transitions.shouldFindBall(player) or
        transitions.shouldSpinToBall(player)):
        return player.goLater('chase')

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
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
        player.brain.tracker.performWidePanFixedPitch()
        player.brain.nav.stand()
        return player.stay()

    prepareForKick.hackKick.collectData()

    if player.brain.tracker.isStopped():
        prepareForKick.hackKick.calculateDataAverages()
        print str(prepareForKick.hackKick)
        player.kick = prepareForKick.hackKick.shoot()
        print str(player.kick)
        return player.goNow('orbitBall')

    return player.stay()


def positionForKick(player):
    """
    Get the ball in the sweet spot
    """
    if player.penaltyKicking and player.brain.ball.loc.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')

    if (transitions.shouldApproachBallAgain(player) or
        transitions.shouldRedecideKick(player)):
        return player.goLater('chase')

    ballLoc = player.brain.ball.loc
    kick_pos = player.kick.getPosition()
    positionForKick.kickPose = RelRobotLocation(ballLoc.relX - kick_pos[0],
                                                ballLoc.relY - kick_pos[1],
                                                0)

    #only enque the new goTo destination once
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        player.inKickingState = False
        player.brain.nav.goTo(positionForKick.kickPose,
                              Navigator.PRECISELY,
                              Navigator.CAREFUL_SPEED,
                              Navigator.ADAPTIVE)
    else:
        player.brain.nav.updateDest(positionForKick.kickPose)

    # most of the time going to chase will kick back to here, lets us reset
    if transitions.shouldFindBallKick(player) and player.counter > 15:
        player.inKickingState = False
        return player.goNow('findBall')

    #if transitions.shouldKick(player):
    if (transitions.ballInPosition(player, positionForKick.kickPose) or
        player.brain.nav.isAtPosition()):
#        if transitions.shouldOrbit(player):
#            return player.goNow('lookAround')
#        else:
        player.brain.nav.stand()
        return player.goLater('kickBallExecute')

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
    if player.brain.ball.vis.framesOff > 200:
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

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        if player.kick.h == 0:
            return player.goLater('positionForKick')
        player.brain.tracker.trackBallFixedPitch()
        player.brain.nav.orbitAngle(player.brain.ball.loc.dist, player.kick.h)

    if player.brain.nav.isStopped():
        player.inKickingState = False
        player.shouldOrbit = False
        player.kick.h = 0
        if player.kick == kicks.ORBIT_KICK_POSITION:
            return player.goLater('prepareForKick')
        else:
            return player.goLater('positionForKick')

    return player.stay()

def approachDangerousBall(player):
    """adjusts position to be farther away from the ball
    if the goalie is too close to the ball while in
    the goal box"""
    if player.firstFrame():
        player.stopWalking()

    #move away from the ball so it is no longer dangerous
    if player.brain.nav.isStopped():
        if player.brain.ball.loc.relY > 0:
            player.brain.nav.walk(0, -15, 0)
        else:
            player.brain.nav.walk(0, 15, 0)

    if not goalTran.dangerousBall(player) or transitions.shouldFindBall(player):
        return player.goLater('chase')

    return player.stay()
