
import GoalieTransitions as goalTran
from ..import NogginConstants as nogCon
import GoalieConstants as goalCon
import ChaseBallStates as chaseBall
from man.noggin.typeDefs.Location import RobotLocation

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

    if ball.dist >= goalCon.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLocGoaliePos()
    else:
        player.brain.tracker.trackBall()
 
    return player.stay()

