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
    if player.penaltyKicking:
        player.brain.tracker.trackBall()
    elif ball.dist >= goalCon.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLocGoaliePos()
    elif( my.uncertX> goalCon.UNCERT_TOO_HIGH
          or my.uncertY > goalCon.UNCERT_TOO_HIGH) :
        player.brain.tracker.activeLocGoaliePos()
    else:
        player.brain.tracker.trackBall()

    return player.stay()


def kickOffPosition(player):
    """
    Do nothing until you know something happened.
    Meant for kickoff situations (For penalty kicks
    track ball)
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.resetGoalieLocalization()
        player.brain.nav.setDest(goalCon.MOVE_IN_KICKOFF, 0, 0, 1)
        if player.penaltyKicking:
            player.gainsOn()
            player.walkPose()

    # When the ball is far away we want to make sure we
    # are in position3
    if player.penaltyKicking :
        player.brain.tracker.trackBall()
    elif ball.dist >= goalCon.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLocGoaliePos()
    else:
        player.brain.tracker.trackBall()

    return player.stay()

