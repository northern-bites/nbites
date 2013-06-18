import time
from objects import RelRobotLocation
from ..navigator import Navigator as nav
from ..util import Transition
import VisualGoalieStates as VisualStates
from .. import SweetMoves
from GoalieConstants import RIGHT, LEFT
import noggin_constants as Constants

SAVING = True

def gameInitial(player):
    if player.firstFrame():
        player.inKickingState = False
        player.gameState = player.currentState
        player.returningFromPenalty = False
        player.brain.fallController.enabled = False
        player.stand()
        player.zeroHeads()
        player.side = LEFT
        player.isSaving = False
        player.lastStiffStatus = True

    # If stiffnesses were JUST turned on, then stand up.
    if player.lastStiffStatus == False and player.brain.interface.stiffStatus.on:
        player.stand()
    # Remember last stiffness.
    player.lastStiffStatus = player.brain.interface.stiffStatus.on

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.gameState = player.currentState
        player.penaltyKicking = False
        player.stand()
        player.brain.tracker.lookToAngle(0)
        if player.lastDiffState != 'gameInitial':
            player.brain.nav.walkTo(RelRobotLocation(-80, 0, 0))

    # Wait until the sensors are calibrated before moving.
    if(not player.brain.motion.calibrated):
        return player.stay()

    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.gameState = player.currentState
        player.returningFromPenalty = False
        player.penaltyKicking = False
        player.stand()
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)

        # The ball will be right in front of us, for sure
        player.brain.tracker.lookToAngle(0)

    # The goalie always gets manually positioned, so reset loc to there.
    player.brain.resetGoalieLocalization()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.gameState = player.currentState
        player.penaltyKicking = False
        player.brain.nav.stand()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    if (player.lastDiffState == 'gamePenalized' and
        player.lastStateTime > 10):
        return player.goLater('waitToFaceField')

    if player.lastDiffState == 'fallen':
        return player.goLater('spinAtGoal')

    return player.goLater('watch')

def gamePenalized(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.gameState = player.currentState
        player.stopWalking()
        player.penalizeHeads()

    if player.lastDiffState == '':
        # Just started up! Need to calibrate sensors
        player.brain.nav.stand()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.gameState = player.currentState
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

##### EXTRA METHODS

def fallen(player):
    player.inKickingState = False
    return player.stay()

def watch(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stand()
        player.returningFromPenalty = False

    return Transition.getNextState(player, watch)

def kickBall(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        # save odometry if this was your first kick
        if player.lastDiffState == 'clearIt':
            VisualStates.returnToGoal.kickPose = \
                RelRobotLocation(player.brain.interface.odometry.x,
                                 player.brain.interface.odometry.y,
                                 player.brain.interface.odometry.h)
        #otherwise add to previously saved odo
        else:
            VisualStates.returnToGoal.kickPose.relX += \
                player.brain.interface.odometry.x
            VisualStates.returnToGoal.kickPose.relY += \
                player.brain.interface.odometry.y
            VisualStates.returnToGoal.kickPose.relH += \
                player.brain.interface.odometry.h

        player.brain.tracker.trackBall()

        player.executeMove(player.kick.sweetMove)

    if player.counter > 10 and player.brain.nav.isStopped():
            return player.goLater('didIKickIt')

    return player.stay()

def saveIt(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)
        if SAVING:
            player.executeMove(SweetMoves.GOALIE_SQUAT)
        else:
            player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
        player.isSaving = False
        #player.brain.fallController.enableFallProtection(False)
    if (not player.brain.motion.body_is_active and not player.isSaving):
        player.squatTime = time.time()
        player.isSaving = True
        return player.stay()
    if player.isSaving:
        stopTime = time.time()
        # This is to stand up before a penalty is called.
        if (stopTime - player.squatTime > 2):
            if SAVING:
                player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            return player.goLater('upUpUP')
    return player.stay()

def upUpUP(player):
    if player.firstFrame():
        #player.brain.fallController.enableFallProtection(True)
        player.upDelay = 0

    if player.brain.nav.isStanding():
        return player.stay()
    elif player.upDelay < 10:
        player.upDelay += 1
        return player.stay()
    else:
        return player.goLater('spinAtGoal')
    return player.stay()

############# PENALTY SHOOTOUT #############

def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.inKickingState = False
        player.gameState = player.currentState
        player.returningFromPenalty = False
        player.brain.fallController.enabled = False
        player.stand()
        player.brain.tracker.trackBall()
        player.side = LEFT
        player.isSaving = False
        player.penaltyKicking = True

    return player.stay()

def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.stand()
        player.brain.tracker.trackBall()

    return player.goLater('waitForPenaltySave')

def waitForPenaltySave(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stop()

    return Transition.getNextState(player, waitForPenaltySave)

def diveRight(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)

    return player.stay()

def diveLeft(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.executeMove(SweetMoves.GOALIE_DIVE_LEFT)

    return player.stay()

def squat(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.executeMove(SweetMoves.GOALIE_SQUAT)

    return player.stay()
