from .. import NogginConstants
from ..playbook.PBConstants import GOALIE

PENALTY_RELOCALIZE_FRAMES = 100

def penaltyKick(player):
    player.penaltyKicking = True
    player.penaltyMadeFirstKick = False
    player.penaltyMadeSecondKick = False
    return player.goNow('approachBallWithLoc')

def penaltyKickRelocalize(player):
    my = player.brain.my
    if player.firstFrame():
        player.brain.tracker.locPans()
    if my.locScore == NogginConstants.BAD_LOC and \
            player.counter < PENALTY_RELOCALIZE_FRAMES:
        return player.stay()
    return player.goNow('scanFindBall')

def penaltyGoalie(player):
    player.penaltyKicking = True
    player.penaltyMadeFirstKick = False
    player.penaltyMadeSecondKick = False

    roleState = player.getRoleState(GOALIE)
    return player.goNow(roleState)


