
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
        player.isPositioning = True
        player.isChasing = False
        player.isSaving = False
        nav.goTo(RobotLocation(nogCon.LANDMARK_LEFT_POST_X + 20,
                               ((nogCon.LANDMARK_LEFT_POST_Y + 
                                 nogCon.LANDMARK_RIGHT_POST_Y)/2)))
        #nav.positionPlaybook()

    if goalTran.inBox(player):
        player.stopWalking()

    if ball.dist >= goalCon.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall() 

   # if player.brain.nav.isStopped():
        #if goalTran.shouldPositionLeft(player):
            #player.goNow('goaliePositionLeft')

        #elif goalTran.shouldPositionRight(player):
            #player.goNow('goaliePositionRight')

    return player.stay()

def goaliePositionRight(player):
#move to the right position.
    if player.firstFrame():
        nav.goTo(RobotLocation(nogCon.LANDMARK_LEFT_POST_X + 20,
                               nogCon.LANDMARK_LEFT_POST_Y- 10, 0))

    elif player.shouldPositionCenter(player):
        player.goNow('goaliePosition')

    #for now dont try to get across the whole goal in one go
    elif player.shouldPositionLeft(player):
        player.goNow('goaliePosition')


def goaliePositionLeft(player):
#move to the left position.
    if player.firstFrame():
        nav.goTo(RobotLocation(nogCon.LANDMARK_RIGHT_POST_X + 20,
                               nogCon.LANDMARK_RIGHT_POST_Y + 10, 0))

    elif player.shouldPositionCenter(player):
        player.goNow('goaliePosition')

    #for now dont try to get across the whole goal in one go
    elif player.shouldPositionRight(player):
        player.goNow('goaliePosition')

