#
# States for positioning the goalie.
#
import GoalieConstants as goalCon

def goaliePosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    nav = player.brain.nav
    my = player.brain.my
    ball = player.brain.ball
    heading = None

    if player.firstFrame():
        nav.positionPlaybook()

    # When the ball is far away we want to make sure we
    # are in position
    if ball.dist >= goalCon.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLocGoaliePos()
    else:
        player.brain.tracker.trackBall()

    return player.stay()


def kickOffPosition(player):
    """
    Do nothing until you know something happened.
    Meant for kickoff situations
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.resetGoalieLocalization()

    # When the ball is far away we want to make sure we
    # are in position
    if ball.dist >= goalCon.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLocGoaliePos()
    else:
        player.brain.tracker.trackBall()

    return player.stay()

def goaliePenaltyKick(player):
    """
    Do nothing until you know something happened.
    Meant for penalty kick.  TRACK BALL
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.resetGoalieLocalization()
        player.brain.tracker.trackBall()

    return player.stay()
