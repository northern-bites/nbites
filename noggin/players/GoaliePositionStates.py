
from .. import NogginConstants
import GoalieTransitions as helper
import GoalieConstants as constants
from ..util import MyMath
from man.noggin.typeDefs.Location import RobotLocation

def goaliePosition(player):
    #consider using ball.x < fixed point- locDist could cause problems if
    #goalie is out of position. difference in accuracy?
    player.isChasing = False
    player.squatting = False
    return player.goNow('squatPosition')

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
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.changeOmniGoToCounter = 0

    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    useOmni = helper.useOmni(player)
    changedOmni = False

    ball = brain.ball
    bearing = None
    if ball.on:
        bearing = ball.bearing
    elif ball.framesOff < 3:
        bearing = ball.locBearing
    else:
        bearing = NogginConstants.OPP_GOAL_HEADING

    position = RobotLocation(brain.play.getPosition(), my.h + bearing)

    if (not player.atDestinationGoalie() or
        not player.atHeading()):
        if not useOmni:
            nav.goTo(position)
        else:
            nav.omniGoTo(position)
    else:
        return player.goLater("goalieAtPosition")
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

    if not player.atHeading(NogginConstants.OPP_GOAL_HEADING):
        spinDir = player.brain.my.getSpinDir(NogginConstants.OPP_GOAL_HEADING)
        player.setWalk(0, 0, spinDir*10)
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

    position = RobotLocation(player.brain.play.getPosition())
    if player.firstFrame() or\
            nav.dest.x != position.x or nav.dest.y != position.y:
        nav.omniGoTo(position)

    if helper.useClosePosition(player):
        return player.goLater('goaliePositionBallClose')
    if nav.isStopped() and player.counter > 0:
        player.framesFromCenter = 0
        player.stepOffCenter = 0
        return player.goLater('goaliePosition')

    return player.stay()

def goalieAtPosition(player):
    brain = player.brain
    nav = player.brain.nav
    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    # Check that the position is correct
    position = RobotLocation(player.brain.play.getPosition())

    if (abs(nav.destX - position[0]) > constants.SHOULD_POSITION_DIFF or
        abs(nav.destY - position[1]) >  constants.SHOULD_POSITION_DIFF or
        not player.atDestinationGoalie() or
        not player.atHeading()):
        return player.goNow("goalieAwesomePosition")
    return player.stay()
