#
# States for positioning the goalie.
#
import GoalieConstants as goalCon
import GoalieTransitions as goalTran

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
        player.brain.tracker.trackBall()

    if goalTran.goalieIsLost(player):
        return player.goLater('spinToField')

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
        player.brain.nav.setDest(goalCon.MOVE_IN_KICKOFF, 0, 0)
        if player.penaltyKicking:
            player.gainsOn()
            player.walkPose()

    # When the ball is far away we want to make sure we
    # are in position3
    player.brain.tracker.trackBall()
    return player.stay()



