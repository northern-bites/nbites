from .. import NogginConstants
from ..playbook.PBConstants import GOALIE
import ChaseBallStates
import ChaseBallTransitions as transitions
PENALTY_RELOCALIZE_FRAMES = 100

def penaltyKick(player):
    player.penaltyKicking = True
    player.penaltyMadeFirstKick = True
    player.penaltyMadeSecondKick = False
    return player.goLater('chase')

def penaltyKickRelocalize(player):
    my = player.brain.my
    if player.firstFrame():
        player.brain.tracker.locPans()
    if my.locScore == NogginConstants.BAD_LOC and \
            player.counter < PENALTY_RELOCALIZE_FRAMES:
        return player.stay()
    return player.goLater('scanFindBall')

def penaltyGoalie(player):
    player.penaltyKicking = True
    player.penaltyMadeFirstKick = True
    player.penaltyMadeSecondKick = False

    roleState = player.getRoleState(GOALIE)
    return player.goLater(roleState)

def penaltyBallInOppGoalbox(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.activeLoc()
    if not player.brain.ball.inOppGoalBox():
        return player.goLater('chase')
    return player.stay()

def penaltyKickShortDribble(player):
    if player.firstFrame():
        player.penaltyMadeFirstKick = True
    if transitions.shouldStopPenaltyKickDribbling(player):

        if transitions.shouldKick(player):
            return player.goLater('chase')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldChaseBall(player):
            return player.goLater('chase')

    return ChaseBallStates.approachBallWalk(player)
