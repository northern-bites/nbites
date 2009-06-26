from ..playbook import PBConstants
from .. import NogginConstants
import PositionConstants as constants
import GoalieTransitions as helper
from ..util import MyMath

def goaliePosition(player):
    #consider using ball.x < fixed point- locDist could cause problems if
    #goalie is out of position. difference in accuracy?
    player.isChasing = False
    return player.goNow('goalieAwesomePosition')

    #if player.brain.nav.notAtHeading(NogginConstants.OPP_GOAL_HEADING):
    #    return player.goLater('goalieSpinToPosition')
    #if helper.useClosePosition(player):
    #    return player.goNow('goaliePositionBallClose')
    #return player.goNow('goaliePositionBallFar')

def goalieAwesomePosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    brain = player.brain
    position = brain.playbook.position
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.changeOmniGoToCounter = 0

    if brain.ball.locDist >= PBConstants.BALL_LOC_LIMIT:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    useOmni = (MyMath.dist(my.x, my.y, position[0], position[1]) <= 90.0)
    changedOmni = False

    if useOmni != nav.movingOmni:
        player.changeOmniGoToCounter += 1
    else :
        player.changeOmniGoToCounter = 0
    if player.changeOmniGoToCounter > constants.CHANGE_OMNI_THRESH:
        changedOmni = True

    if player.firstFrame() or \
            nav.destX != position[0] or \
            nav.destY != position[1] or \
            changedOmni:

        if not useOmni:
            nav.goTo((position[0], position[1], brain.ball.bearing))
        else:
            nav.omniGoTo((position[0], position[1], brain.ball.bearing))

    return player.stay()

def goaliePositionForSave(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    strafeDir = helper.strafeDirForSave(player)
    if strafeDir == -1:
        helper.strafeRightSpeed(player)
    elif strafeDir == 1:
        helper.strafeLeftSpeed(player)
    else:
        player.stopWalking()

    return player.stay()

def goaliePositionBallClose(player):

    nav = player.brain.nav
    player.brain.tracker.trackBall()

    #if not nav.atHeading(NogginConstants.OPP_GOAL_HEADING):
    #    return player.goLater('goalieSpinToPosition')
    if helper.useLeftStrafeCloseSpeed(player):
        helper.strafeLeftSpeed(player)
    elif helper.useRightStrafeCloseSpeed(player):
        helper.strafeRightSpeed(player)
    else:
        player.stopWalking()

    #switch out if we lose the ball for multiple frames
    if helper.useFarPosition(player):
        return player.goNow('goaliePositionBallFar')

    return player.stay()

def goaliePositionBallFar(player):

    nav = player.brain.nav
    player.brain.tracker.activeLoc()

    if helper.outOfPosition(player):
        player.goLater('goalieOutOfPosition')
    #elif not nav.atHeading(NogginConstants.OPP_GOAL_HEADING):
    #    return player.goLater('goalieSpinToPosition')
    elif helper.useLeftStrafeFarSpeed(player):
        helper.strafeLeftSpeed(player)
    elif helper.useRightStrafeFarSpeed(player):
        helper.strafeRightSpeed(player)
    else:
        player.stopWalking()

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
        player.setSpeed(0, 0, spinDir*10)
        return player.stay()
    else:
        player.stopWalking()
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
        player.framesFromCenter = 0
        player.stepOffCenter = 0
        return player.goLater('goaliePosition')

    return player.stay()
