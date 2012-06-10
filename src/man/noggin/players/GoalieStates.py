import time
from objects import RelRobotLocation, RelLocation
from GoalieConstants import INITIAL_ANGLE
from math import sin, cos, radians
from collections import deque
from vision import certainty
from man.motion.HeadMoves import FIXED_PITCH_LEFT_SIDE_PAN
from ..navigator import Navigator as nav
from ..util import Transition
import noggin_constants as nogginConstants
import goalie

import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.brain.nav.stop()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False
        player.system = goalie.GoalieSystem()

    elif (player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown
          and not player.motion.isBodyActive()):
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.lookToAngle(goalie.INITIAL_ANGLE)
        print "Looking to: " + str(goalie.INITIAL_ANGLE)
        if player.lastDiffState == 'gameInitial':
            player.initialDelayCounter = 0

    #HACK! TODO: this delay is to make sure the sensors get calibrated before
    #we start walking; find a way to query motion to see whether the sensors are
    #calibrated or not before starting
    if player.initialDelayCounter < 230:
        player.initialDelayCounter += 1
        return player.stay()

    return player.goLater('walkToGoal')

def gameSet(player):
    if player.firstFrame():
        player.brain.logger.startLogging()
        player.brain.nav.stand()
        player.gainsOn()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBallFixedPitch()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.trackBallFixedPitch()

    #if player.lastDiffState == 'gamePenalized':
        # Need to at least *try* to get back into goal.

    return player.goLater('position')

def gamePenalized(player):
    if player.firstFrame():
        player.brain.logger.stopLogging()
        player.inKickingState = False
        player.stopWalking
        player.executeMove(SweetMoves.SIT_POS)
        player.penalizeHeads()

    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.stopWalking()
        player.zeroHeads()
        player.GAME_FINISHED_satDown = False
        return player.stay()

    if (player.brain.nav.isStopped() and not player.GAME_FINISHED_satDown
        and not player.motion.isBodyActive()):
        player.GAME_FINISHED_satDown = True
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if not player.motion.isBodyActive() and player.GAME_FINISHED_satDown:
        player.gainsOff()
    return player.stay()

##### Non-main game methods

def updatePostObservations(player):
    """
    Updates the underlying C++ data structures.
    """
    if player.brain.vision.ygrp.on and player.brain.vision.ygrp.certainty != certainty.NOT_SURE:
        print "RIGHT: Saw right post."
        player.system.pushRightPostObservation(player.brain.vision.ygrp.dist,
                                               player.brain.vision.ygrp.bearing)
        print "  Pushed " + str(player.brain.vision.ygrp.bearing) + " " + str(player.brain.vision.ygrp.dist)
    if player.brain.vision.yglp.on:
        print "LEFT: Saw left post."
        player.system.pushLeftPostObservation(player.brain.vision.yglp.dist,
                                              player.brain.vision.yglp.bearing)
        print "  Pushed " + str(player.brain.vision.yglp.bearing) + " " + str(player.brain.vision.yglp.dist)

def walkToGoal(player):
    print "========================================"
    if player.firstFrame():
        player.brain.tracker.repeatHeadMove(FIXED_PITCH_LEFT_SIDE_PAN)
        player.brain.nav.goTo(player.system.home,
                              nav.CLOSE_ENOUGH, nav.FAST_SPEED)

    updatePostObservations(player)

    player.system.home.relY = player.system.centerGoalRelY()
    player.system.home.relX = player.system.centerGoalRelX()

    # if player.system.home.relY < 120:
    #     player.system.home.relY = player.system.leftPostRelY()
    #     player.system.home.relX = player.system.leftPostRelX() - 70.0
    #     player.brain.tracker.lookToAngle(player.system.leftPostBearing())

    print "BEARINGS " + str(player.system.centerGoalBearing())
    print "  LEFT " + str(player.system.leftPostBearing())
    print "  RIGHT " + str(player.system.rightPostBearing())

    print "DISTANCES " + str(player.system.centerGoalDistance())
    print "  LEFT " + str(player.system.leftPostDistance())
    print "  RIGHT " + str(player.system.rightPostDistance())

    print "TO GET TO"
    print "  LEFT " + str(player.system.leftPostRelX()) + " " + str(player.system.leftPostRelY())
    print "  RIGHT " + str(player.system.rightPostRelX()) + " " + str(player.system.rightPostRelY())

    print "Going to " + str(player.system.home.relX) + " " + str(player.system.home.relY)

    return player.stay()
    #return Transition.getNextState(player, walkToGoal)

#walkToGoal.transitions = { turnForward: Transition.CountTransition(atGoal) }

def fallen(player):
    return player.stay()

def position(player):
    # step forward - NOPE, hacked out US open 2012
    if player.firstFrame():
        """player.brain.nav.walkTo(RelRobotLocation(15,0,0),
                                #player.brain.nav.CLOSE_ENOUGH,
                                (3,3,10),
                                #player.brain.nav.SLOW_SPEED)
                                0.2)

    # Just in case walkTo fails, eventually stop anyway
    if player.brain.nav.isStopped() or player.counter > 300:
        player.brain.nav.stop()
        return player.goNow('watch')"""

    return player.goLater('watch')

def watch(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if player.lastDiffState == 'kickBall':
            player.brain.nav.stand()

    #if player.brain.ball.dist < 100:
    #    player.executeMove(SweetMoves.GOALIE_SQUAT)
    #    return player.goLater('saveIt')

    return player.stay()

def kickBall(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if player.brain.ball.loc.relY < 0:
            kick = SweetMoves.RIGHT_BIG_KICK
        else:
            kick = SweetMoves.LEFT_BIG_KICK

        player.executeMove(kick)

    if player.counter > 10 and player.brain.nav.isStopped():
        return player.goLater('watch')

    return player.stay()

def saveIt(player):
    if player.firstFrame():
        player.isSaving = False
    if (not player.motion.isBodyActive() and not player.isSaving):
        player.squatTime = time.time()
        player.isSaving = True
        return player.stay()
    if player.isSaving:
        stopTime = time.time()
        # This is to stand up before a penalty is called.
        if (stopTime - player.squatTime > 4):
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            return player.goLater('upUpUP')
    return player.stay()

def upUpUP(player):
    if player.firstFrame():
        player.upDelay = 0

    if player.motion.isBodyActive():
        return player.stay()
    elif player.upDelay < 60:
        player.upDelay += 1
        return player.stay()
    else:
        return player.goLater('watch')
    return player.stay()

############# PENALTY SHOOTOUT #############


def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.gainsOn()
        player.stopWalking()
        player.stand()
        player.brain.loc.resetBall()

        player.brain.tracker.trackBallFixedPitch()
        player.initialDelayCounter = 0

    if player.initialDelayCounter < 230:
        player.initialDelayCounter += 1
        return player.stay()

    return player.stay()

def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.stand()

    return player.goLater('position')
    #return player.goLater('watch')
