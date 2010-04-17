import man.motion.SweetMoves as SweetMoves
import GoalieConstants as constants
import GoalieTransitions as helper
from ..playbook import PBConstants
from .. import NogginConstants

DEBUG = False

def squat(player):
    if player.firstFrame():
        player.isChasing = False
        player.executeMove(SweetMoves.INITIAL_POS)
        player.squatting = True
        if DEBUG:
            player.executeMove(SweetMoves.SAVE_CENTER_DEBUG)
        else:
            player.executeMove(SweetMoves.GOALIE_SQUAT)
    if (player.stateTime >=
        SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT) +
        SweetMoves.getMoveTime(SweetMoves.INITIAL_POS)):
        return player.goLater('squatted')
    return player.stay()

def squatted(player):
    player.isChasing = False
    player.squatting = True
    brain = player.brain
    if brain.ball.locDist >= constants.ACTIVE_LOC_THRESH:
        brain.tracker.activeLoc()
    else:
        brain.tracker.trackBall()

    return player.stay()

def squatPosition(player):
    brain = player.brain
    position = brain.play.getPosition()
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.changeOmniGoToCounter = 0
        player.isChasing = False
        player.squatting = False

    if brain.ball.x >= constants.ACTIVE_LOC_THRESH:
        brain.tracker.activeLoc()
    else:
        brain.tracker.trackBall()

    useOmni = helper.useOmni(player)
    changedOmni = False

    ball = brain.ball
    bearing = None

    if (not player.atDestinationGoalie() or
        not player.atHeading()):
        if not useOmni:
            nav.goTo((PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y,\
                          NogginConstants.OPP_GOAL_HEADING))
        else:
            nav.omniGoTo((PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y,\
                              NogginConstants.OPP_GOAL_HEADING))
    else:
        player.stopWalking()
        return player.goLater("squat")

    return player.stay()

def chasePrepare(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.isChasing = False

        if player.squatting:
            if DEBUG:
                pass
            else:
                player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
        else:
            player.isChasing = True
            return player.goNow('chase')
    elif (player.stateTime >=
        SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT_STAND_UP)):
        player.isChasing = True
        player.squatting = False
        return player.goNow('chase')

    return player.stay()
