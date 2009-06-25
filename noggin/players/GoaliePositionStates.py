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
    player.isChasing = False
    if player.brain.nav.notAtHeading(NogginConstants.OPP_GOAL_HEADING):
        return player.goLater('goalieSpinToPosition')
    elif helper.useClosePosition(player):
        return player.goNow('goaliePositionBallClose')
    return player.goNow('goaliePositionBallFar')

def goaliePositionForSave(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    strafeDir = helper.strafeDirForSave(player)
    if strafeDir == 'right':
        helper.strafeRight(player)
    elif strafeDir == 'left':
        helper.strafeLeft(player)
    else:
        player.stopWalking()

    return player.stay()

def goaliePositionBallClose(player):

    nav = player.brain.nav
    player.brain.tracker.trackBall()

    if player.firstFrame():
        player.lastStepTime = player.getTime()

    if not nav.atHeading(NogginConstants.OPP_GOAL_HEADING):
        return player.goLater('goalieSpinToPosition')
    elif helper.useLeftStrafeCloseSpeed(player):
        helper.strafeLeftSpeed(player)
    elif helper.useRightStrafeCloseSpeed(player):
        helper.strafeRightSpeed(player)
    else:
        player.stopWalking()
        player.lastStepTime = player.getTime()

    #switch out if we lose the ball for multiple frames
    if helper.useFarPosition(player):
        return player.goNow('goaliePositionBallFar')

    return player.stay()

def goaliePositionBallFar(player):

    nav = player.brain.nav
    player.brain.tracker.activeLoc()
    if player.firstFrame():
        player.lastStepTime = player.getTime()

    if helper.outOfPosition(player):
        player.goLater('goalieOutOfPosition')
    elif not nav.atHeading(NogginConstants.OPP_GOAL_HEADING):
        return player.goLater('goalieSpinToPosition')
    elif helper.useLeftStrafeFar(player):
        helper.strafeLeftSpeed(player)
    elif helper.useRightStrafeFar(player):
        helper.strafeRightSpeed(player)
    else:
        player.stopWalking()
        player.lastStepTime = player.getTime()

    #Don't switch out if we don't see the ball
    if helper.useClosePosition(player):
        return player.goLater('goaliePositionBallClose')
    return player.stay()

def goalieSpinToPosition(player):
    nav = player.brain.nav
    if helper.useFarPosition(player):
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()
    if not nav.atHeading(NogginConstants.OPP_GOAL_HEADING):
        spinDir = MyMath.getSpinDir(player.brain.my.h,
                                    NogginConstants.OPP_GOAL_HEADING)
        player.setSpeed(-1, 0, spinDir*10)
        return player.stay()
    else:
        return player.goLater('goaliePosition')

    return player.stay()

def goalieOutOfPosition(player):
    nav = player.brain.nav
    if helper.useFarPosition(player):
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    position = player.brain.playbook.position
    if player.firstFrame() or\
            nav.destX != position[0] or nav.destY != position[1]:
        nav.omniGoTo(position)

    if helper.useClosePosition(player):
        return player.goLater('goaliePositionBallClose')
    if nav.isStopped() and player.counter > 0:
        player.timeFromCenter = 0
        player.stepsOffCenter = 0
        return player.goLater('goaliePosition')

    return player.stay()
