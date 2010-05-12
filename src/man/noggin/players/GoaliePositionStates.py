
import GoalieTransitions as helper
import GoalieConstants as constants

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
        nav.positionPlaybook()

    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()


    heading = None
    ball = brain.ball

    if nav.isStopped() and player.counter > 0:
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

    position = player.brain.play.getPosition()

    if (abs(nav.dest.x - position.x) > constants.SHOULD_POSITION_DIFF or
        abs(nav.dest.y - position.y) >  constants.SHOULD_POSITION_DIFF or
        not player.atDestinationGoalie() or
        not player.atHeading()):
        return player.goNow("goalieAwesomePosition")
    return player.stay()
