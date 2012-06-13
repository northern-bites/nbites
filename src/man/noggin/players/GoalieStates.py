import time
from objects import RelRobotLocation
from ..navigator import Navigator as nav
from ..util import Transition
from goalie import GoalieSystem, RIGHT_SIDE_ANGLE, LEFT_SIDE_ANGLE
import VisualGoalieStates as VisualStates
import man.motion.SweetMoves as SweetMoves
from GoalieConstants import RIGHT, LEFT

def gameInitial(player):
    if player.firstFrame():
        player.brain.nav.stop()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False
        player.system = GoalieSystem()
        player.side = LEFT
        player.isSaving = False

    elif (player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown
          and not player.motion.isBodyActive()):
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    elif (not player.motion.isBodyActive()):
        player.gainsOff()

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.lookToAngle(RIGHT_SIDE_ANGLE)
        if player.lastDiffState == 'gameInitial':
            player.initialDelayCounter = 0

    # Wait until the sensors are calibrated before moving.
    if(not player.brain.motion.calibrated()):
        return player.stay()

    return player.goLater('walkToGoal')

def gameSet(player):
    if player.firstFrame():
        player.brain.logger.startLogging()
        player.brain.nav.stand()
        player.gainsOn()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBallFixedPitch()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated()):
        return player.stay()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated()):
        return player.stay()

    # check for legit penalty - don't forget!
    # and player.lastStateTime > 25.0
    if player.lastDiffState == 'gamePenalized':
        return player.goLater('decideSide')

    # something with falls is NOT working!
    if player.lastDiffState == 'fallen':
        return player.goLater('spinAtGoal')

    return player.goLater('watch')

def gamePenalized(player):
    if player.lastDiffState == '':
        # Just started up! Need to calibrate sensors
        player.gainsOn()
        player.brain.nav.stand()

    # Wait until the sensors are calibrated before moving.
    while (not player.brain.motion.calibrated()):
        return player.stay()

    if player.firstFrame():
        player.brain.logger.stopLogging()
        player.inKickingState = False
        player.stopWalking()
        player.penalizeHeads()

    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

##### EXTRA METHODS

def fallen(player):
    return player.stay()

def watch(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if (player.lastDiffState == 'kickBall' or
            player.lastDiffState == 'spinAtGoal'):
            player.brain.nav.stand()

    return Transition.getNextState(player, watch)

def kickBall(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        VisualStates.returnToGoal.kickPose = RelRobotLocation(player.brain.loc.lastOdoX,
                            player.brain.loc.lastOdoY,
                            player.brain.loc.lastOdoTheta)
        player.brain.tracker.trackBallFixedPitch()
        if player.brain.ball.loc.relY < 0:
            kick = SweetMoves.RIGHT_BIG_KICK
        else:
            kick = SweetMoves.LEFT_BIG_KICK

        player.executeMove(kick)

    if player.counter > 10 and player.brain.nav.isStopped():
            return player.goLater('didIKickIt')

    return player.stay()

def saveIt(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
        player.isSaving = False
    if (not player.motion.isBodyActive() and not player.isSaving):
        player.squatTime = time.time()
        player.isSaving = True
        return player.stay()
    if player.isSaving:
        stopTime = time.time()
        # This is to stand up before a penalty is called.
        if (stopTime - player.squatTime > 4):
            #player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
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
        return player.goLater('spinAtGoal')
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
