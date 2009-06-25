import man.motion.SweetMoves as SweetMoves
from .. import NogginConstants as FC

GOTO_POINTS = [(FC.CENTER_FIELD_X, FC.CENTER_FIELD_Y, FC.OPP_GOAL_HEADING),
               (FC.OPP_GOALBOX_LEFT_X, FC.FIELD_HEIGHT*0.75, FC.MY_GOAL_HEADING),
               (FC.OPP_GOALBOX_LEFT_X, FC.FIELD_HEIGHT*0.25, FC.MY_GOAL_HEADING),
               (FC.MY_GOALBOX_RIGHT_X, FC.FIELD_HEIGHT*0.25, FC.OPP_GOAL_HEADING),
               (FC.MY_GOALBOX_RIGHT_X, FC.FIELD_HEIGHT*0.75, FC.OPP_GOAL_HEADING),
               (FC.CENTER_FIELD_X, FC.CENTER_FIELD_Y, FC.OPP_GOAL_HEADING)]

def gameReady(player):
    if player.firstFrame():
        player.brain.tracker.scanBall()
    return player.stay()

def gamePlaying(player):
    player.brain.resetGoalieLocalization()
    player.goToPoint = GOTO_POINTS[0]
    player.goToCounter = 0
    return player.goNow('goToPoint')

def goToPoint(player):
    if player.firstFrame():
        player.brain.tracker.locPans()
        player.brain.nav.goTo(GOTO_POINTS[player.goToCounter])
    if player.brain.nav.isStopped() and not player.firstFrame():
        return player.goLater('atPoint')

    return player.stay()

def atPoint(player):
    if player.firstFrame():
        player.goToCounter += 1
        player.executeMove(SweetMoves.SAVE_CENTER_DEBUG)
    elif player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_CENTER_DEBUG):
        if player.goToCounter >= len(GOTO_POINTS):
            return player.goLater('atFinalPoint')
        else:
            return player.goLater('goToPoint')
    return player.stay()

def atFinalPoint(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()

    return player.stay()
