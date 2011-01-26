
import GoalieTransitions as helper
import GoalieConstants as constants

def goaliePosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    brain = player.brain
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.isChasing = False
        nav.positionPlaybook()

    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()


    heading = None
    ball = brain.ball

    return player.stay()

def goaliePositionForSave(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    strafeDir = helper.strafeDirForSave(player)
    if fabs(strafeDir) > 0:
        player.setWalk(0, constants.STRAFE_SPEED * MyMath.sign(strafeDir), 0)
    else:
        player.stopWalking()

    return player.stay()
