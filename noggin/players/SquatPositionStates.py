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
    if brain.ball.x >= constants.ACTIVE_LOC_THRESH:
        brain.tracker.activeLoc()
    else:
        brain.tracker.trackBall()
    #if my current position == getProperPosition
    #or wrap in moveSquattedPosition
    return player.stay()

def squatPosition(player):

    brain = player.brain
    position = brain.playbook.position
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.changeOmniGoToCounter = 0
        player.isChasing = False

    if brain.ball.x >= constants.ACTIVE_LOC_THRESH:
        brain.tracker.activeLoc()
    else:
        brain.tracker.trackBall()
    if player.counter % 5 == 0:
        player.brain.sensors.saveFrame()
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
            player.squatting = False

    elif (player.stateTime >=
        SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT_STAND_UP)):
        player.isChasing = True
        return player.goNow('chase')

    return player.stay()
