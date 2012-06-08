#
# These are the two positioning states for the goalie.
# They are very similar.  The goaliePosition state is
# the general positioning state which (when loc works)
# centers the goalie in the goal facing the ball.
# If the goalie is looking at the field edge and it is
# really close and straight in front of him. He will
# spin 180 to hopefully get him looking back at the field.
#
# The other positioning state is used straight out of kick
# off so that goalie does not try repositioning itself
# because it should already be in position. The only
# movement it does do it take a couple steps forward
# because manual position (2011) is too far back.
# This state will probably become unnecessary when loc
# improves.
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

    if player.firstFrame():
        player.brain.resetGoalieLocalization()
        player.brain.tracker.trackBall()
        player.brain.nav.walkTo(goalCon.MOVE_IN_KICKOFF, 0, 0)
        if player.penaltyKicking:
            player.gainsOn()
            player.stand()

    return player.stay()



