import man.motion.SweetMoves as SweetMoves
import GoalieConstants as constants
import GoalieTransitions as helper
from ..playbook import PBConstants
from .. import NogginConstants

def squat(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.INITIAL_POS)
        player.squatting = True
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    if (player.stateTime >=
        SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT) +
        SweetMoves.getMoveTime(SweetMoves.INITIAL_POS)):
        return player.goLater('squatted')
    return player.stay()

def squatted(player):
    brain = player.brain
    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        brain.tracker.activeLoc()
    else:
        brain.tracker.trackBall()

    return player.stay()

def squatPosition(player):

    brain = player.brain
    position = brain.playbook.position
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

    if (not nav.atDestinationGoalie() or
        not nav.atHeading()):
        if not useOmni:
            nav.goTo((PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y,\
                          NogginConstants.OPP_GOAL_HEADING))
        else:
            nav.omniGoTo((PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y,\
                              NogginConstants.OPP_GOAL_HEADING))
    else:
        return player.goLater("squat")

    return player.stay()

def chasePrepare(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.isChasing = False

        if player.squatting:
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)

    elif (player.stateTime >=
        SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT_STAND_UP)):
        player.isChasing = True
        player.squatting = False
        return player.goNow('chase')

    return player.stay()
