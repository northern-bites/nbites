from ..playbook import PBConstants
from .. import NogginConstants
import GoalieTransitions as helper
from ..util import MyMath

def goaliePosition(player):
    #consider using ball.x < fixed point- locDist could cause problems if
    #goalie is out of position. difference in accuracy?
    '''TODO-
    if helper.shouldMoveToSave():
        player.goNow('goaliePositionForSave') '''
    if helper.useClosePosition(player):
        return player.goNow('goaliePositionBallClose')
    return player.goNow('goaliePositionBallFar')

def goaliePositionForSave(player):
    pass

def goaliePositionBallClose(player):

    nav = player.brain.nav
    player.brain.tracker.trackBall()

    if helper.outOfPosition(player):
        nav.omniGoTo(player.brain.playbook.position)
    #elif nav.notAtHeading(NogginConstants.OPP_GOAL_HEADING):
    #    my = player.brain.my
    #    spinDir = MyMath.getSpinDir(my.h, NogginConstants.OPP_GOAL_HEADING)
    #    player.setSpeed(0,0,spinDir)
    #if not super far out of position. heading check?
    elif helper.useLeftStrafeClose(player):
        helper.strafeLeft(player)
    elif helper.useRightStrafeClose(player):
        helper.strafeRight(player)

    #switch out if we lose the ball for multiple frames
    if helper.useFarPosition(player):
        return player.goNow('goaliePositionBallFar')

    return player.stay()


def goaliePositionBallFar(player):

    nav = player.brain.nav
    player.brain.tracker.activeLoc()

    if helper.outOfPosition(player):
        nav.omniGoTo(player.brain.playbook.position)
    #elif nav.notAtHeading(NogginConstants.OPP_GOAL_HEADING):
    #    my = player.brain.my
    #    spinDir = MyMath.getSpinDir(my.h, NogginConstants.OPP_GOAL_HEADING)
    #    player.setSpeed(0,0,spinDir)
    elif helper.useLeftStrafeFar(player):
        helper.strafeLeft(player)
    elif helper.useRightStrafeFar(player):
        helper.strafeRight(player)

    #don't switch out if we don't see the ball
    if helper.useClosePosition(player):
        return player.goLater('goaliePositionBallClose')
    return player.stay()
