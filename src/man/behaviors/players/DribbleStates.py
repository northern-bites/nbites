"""
Here is the FSA for dribbling a ball.
"""
import DribbleTransitions as transitions
import DribbleConstants as constants
from ..navigator import Navigator
from ..kickDecider import kicks
from ..util import *
from objects import RelRobotLocation, Location

### BASIC IDEA
# We dribble by setting ourselves up for a dribble kick. The sweet spot
# is in front of the ball, so setting ourselves up for a kick actually
# results in us running through the ball. (There is no actual dribble sweet
# move.) If vision sees a crowded area in front of us, we rotate around the 
# ball and dribble again to space. We only dribble if shoulDribble returns 
# true, see DribbleTransitions.py for more info.

# TODO hierarchy
@superState('gameControllerResponder')
def decideDribble(player):
    """
    Decide to dribble straight ahead or rotate to avoid other robots.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()

    if not transitions.shouldDribble(player):
        player.inKickingState = False
        return player.goLater('approachBall')
    elif transitions.centerLaneOpen(player):
        return player.goNow('executeDribble')
    else:
        return player.goNow('rotateToOpenSpace')

@superState('gameControllerResponder')
def executeDribble(player):
    """
    Move through the ball, so as to execute a dribble.
    """
    if transitions.ballToOurLeft(player):
        player.kick = kicks.LEFT_DRIBBLE
    else:
        player.kick = kicks.RIGHT_DRIBBLE

    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    player.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                       ball.rel_y - kick_pos[1],
                                       0)

    if player.firstFrame():
        player.aboutToRotate = False
        player.ballBeforeDribble = ball
        if player.corner_dribble:
            player.brain.nav.goTo(player.kickPose,
                                  Navigator.PRECISELY,
                                  Navigator.CAREFUL_SPEED,
                                  False,
                                  False)
        else:
            player.brain.nav.goTo(player.kickPose,
                                  Navigator.PRECISELY,
                                  Navigator.GRADUAL_SPEED,
                                  False,
                                  False)
    else:
        player.brain.nav.updateDest(player.kickPose)

    if player.corner_dribble:
        if transitions.ballLost(player) or transitions.ballGotFarAway(player):
            player.corner_dribble = False
            return player.goLater('approachBall')
        elif transitions.dribbleGoneBad(player):
            return player.goNow('positionForDribble')
        elif transitions.centerField(player):
            player.corner_dribble = False
            return player.goLater('approachBall')
    else:
        if transitions.ballLost(player):
            return player.goNow('lookForBall')
        elif not transitions.shouldDribble(player):
            player.inKickingState = False
            return player.goLater('approachBall')
        elif not transitions.centerLaneOpen(player):
            player.aboutToRotate = True # we will go from position to rotate
            return player.goNow('positionForDribble')
        elif transitions.dribbleGoneBad(player):
            return player.goNow('positionForDribble')

    return player.stay()

@superState('gameControllerResponder')
def rotateToOpenSpace(player):
    """
    Rotate around ball, so as to find an open lane to dribble thru
    """
    if player.firstFrame():
        rotateToOpenSpace.counter = 0
        if transitions.rotateLeft(player):
            player.setWalk(0, -.7, .25)
        else:
            player.setWalk(0, .7, -.25)

    if transitions.ballLost(player):
        return player.goNow('lookForBall')
    elif not transitions.shouldDribble(player):
        player.inKickingState = False
        player.stand()
        return player.goLater('approachBall')
    elif rotateToOpenSpace.counter == constants.ROTATE_FC:
        player.stand()
        return player.goLater('decideDribble')
    elif transitions.centerLaneOpen(player):
        rotateToOpenSpace.counter += 1
        return player.stay() # so counter is not reset, see below

    rotateToOpenSpace.counter = 0
    return player.stay()

@superState('gameControllerResponder')
def lookForBall(player):
    """
    Backup and look for ball. If fails, leave the FSA.
    """
    if player.firstFrame():
        lookForBall.setDest = False
        player.brain.tracker.repeatWidePan()
        player.stand()

    if transitions.seesBall(player):
        player.brain.tracker.trackBall()
        return player.goNow('positionForDribble')
    elif player.brain.nav.isStopped():
        if not lookForBall.setDest:
            backupLoc = RelRobotLocation(constants.BACKUP_WHEN_LOST,0,0)
            player.brain.nav.walkTo(backupLoc)
            lookForBall.setDest = True
        else:
            player.inKickingState = False
            return player.goLater('approachBall')

    return player.stay()

@superState('gameControllerResponder')
def positionForDribble(player):
    """
    We should position ourselves behind the ball for easy dribbling.
    """
    if transitions.ballToOurLeft(player):
        player.kick = kicks.LEFT_DRIBBLE
    else:
        player.kick = kicks.RIGHT_DRIBBLE

    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    player.kickPose = RelRobotLocation(ball.rel_x + constants.SETUP_POSITION,
                                       ball.rel_y - kick_pos[1],
                                       0)

    if player.firstFrame():
        player.brain.nav.goTo(player.kickPose,
                              Navigator.GENERAL_AREA,
                              Navigator.MEDIUM_SPEED,
                              False,
                              False)
    else:
        player.brain.nav.updateDest(player.kickPose)

    if player.corner_dribble:
        if transitions.ballLost(player) or transitions.ballGotFarAway(player):
            player.corner_dribble = False
            return player.goLater('approachBall')
        elif transitions.navDone(player):
            return player.goLater('executeDribble')
    else:
        if transitions.ballLost(player):
            return player.goLater('lookForBall')
        elif not transitions.shouldDribble(player):
            player.inKickingState = False
            player.stand()
            return player.goLater('approachBall')
        elif player.aboutToRotate and transitions.navDone(player):
            player.aboutToRotate = False
            return player.goLater('rotateToOpenSpace')
        elif transitions.navDone(player):
            return player.goLater('decideDribble')

    return player.stay()
