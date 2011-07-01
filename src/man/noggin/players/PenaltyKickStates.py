import noggin_constants as NogginConstants
from ..playbook.PBConstants import GOALIE
import ChaseBallTransitions as transitions
PENALTY_RELOCALIZE_FRAMES = 100

def penaltyKick(player):
    """
    Set penalty kicking variables. Acts as a layer above 'chase'
    """
    player.penaltyKicking = True
    player.penaltyMadeFirstKick = True
    player.penaltyMadeSecondKick = False
    return player.goNow('chase')

def penaltyKickRelocalize(player):
    """
    Since you will be facing the goal, do loc pans if you are lost
    """
    my = player.brain.my
    if player.firstFrame():
        player.brain.tracker.locPans()
    if my.locScore == NogginConstants.BAD_LOC and \
            player.counter < PENALTY_RELOCALIZE_FRAMES:
        return player.stay()
    return player.goLater('findBall')

def penaltyBallInOppGoalbox(player):
    """
    We can't do anything if the ball is in the opponent's goalbox
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.activeLoc()
    if not player.brain.ball.inOppGoalBox():
        return player.goLater('chase')
    return player.stay()

def penaltyKickShortDribble(player):
    """
    Acts as a layer above dribble for penalty shots.
    """
    if player.firstFrame():
        player.penaltyMadeFirstKick = True
    if transitions.shouldStopPenaltyKickDribbling(player):
        if transitions.shouldClaimBall(player):
            return player.goLater('chase')
        elif transitions.shouldPositionForKick(player):
            return player.goLater('positionForKick')
        elif transitions.shouldChaseBall(player):
            return player.goLater('chase')

    return player.goLater('dribble')
