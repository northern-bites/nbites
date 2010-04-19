
from .. import NogginConstants
import GoalieTransitions as helper
import GoalieConstants as constants
from ..util import MyMath
from man.noggin.typeDefs.Location import RobotLocation

def goaliePosition(player):
    #consider using ball.x < fixed point- locDist could cause problems if
    #goalie is out of position. difference in accuracy?
    player.isChasing = False
    return player.goNow('goalieAwesomePosition')

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


    heading = None
    ball = brain.ball

    if ball.framesOff < 5:
        # TODO: this may be a problem if the ball is behind us
        heading = brain.my.h + ball.bearing
    else:
        heading = NogginConstants.OPP_GOAL_HEADING

    position = player.brain.play.getPosition()
    position = RobotLocation(position[0], position[1], heading)
    nav.positionPlaybook(position)

    if nav.isStopped():
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

def goalieAtPosition(player):
    brain = player.brain
    nav = player.brain.nav

    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    # Check that the position is correct
    ball = brain.ball
    heading = None

    if ball.framesOff < 5:
        # TODO: this may be a problem if the ball is behind us
        heading = brain.my.h + ball.bearing
    else:
        heading = NogginConstants.OPP_GOAL_HEADING

    position = player.brain.play.getPosition()
    position = RobotLocation(position[0], position[1], heading)

    if (abs(nav.dest.x - position.x) > constants.SHOULD_POSITION_DIFF or
        abs(nav.dest.y - position.y) >  constants.SHOULD_POSITION_DIFF or
        not player.atDestinationGoalie() or
        not player.atHeading()):
        return player.goNow("goalieAwesomePosition")
    return player.stay()
