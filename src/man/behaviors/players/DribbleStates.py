"""
Here is the FSA for dribbling a ball.
"""
import DribbleTransitions as transitions
import DribbleConstants as constants
from ..navigator import Navigator
from ..kickDecider import kicks
from objects import RelRobotLocation, Location
# from ..kickDecider import HackKickInformation as hackKick
# from math import fabs
# import noggin_constants as nogginConstants

### TODO
# 1. choose direction better, based on loc and heatmap
# 2. rotate a little longer post open lane found
def dribble(player):
    """
    Super State to determine what to do from various situations.
    """
    if transitions.ballLost(player):
        return player.goNow('lookForBall')
    elif (transitions.facingGoal(player) and transitions.middleThird(player) 
        and transitions.crowded(player) 
        and not transitions.ballGotFarAway(player)):
        return player.goNow('decideDribble')
    else:
        return player.goLater('chase')

def decideDribble(player):
    """
    Decide to dribble straight ahead or rotate to avoid other robots.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if transitions.ballToOurLeft(player):
            player.kick = kicks.LEFT_DRIBBLE
        else:
            player.kick = kicks.RIGHT_DRIBBLE

    if transitions.centerLaneOpen(player) and transitions.crowded(player):
        return player.goNow('executeDribble')
    elif transitions.crowded(player):
        return player.goNow('rotateToOpenSpace')
    else:
        return player.goLater('dribble')

def executeDribble(player):
    """
    Move through the ball, so as to execute a dribble.
    """
    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    player.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                       ball.rel_y - kick_pos[1],
                                       0)

    print "Executing dribble..."

    if player.firstFrame():
        # player.ballBeforeApproach = player.brain.ball
        # player.brain.tracker.lookStraightThenTrack()
        player.brain.nav.goTo(player.kickPose,
                              Navigator.CLOSE_ENOUGH,
                              Navigator.MEDIUM_SPEED,
                              False,
                              False)
    else:
        player.brain.nav.updateDest(player.kickPose)

    if (transitions.ballLost(player) or transitions.ballGotFarAway(player) or
        transitions.navDone(player)) or not transitions.crowded(player):
        return player.goLater('dribble')
    elif not transitions.centerLaneOpen(player):
        return player.goLater('rotateToOpenSpace')

    return player.stay()

def rotateToOpenSpace(player):
    """
    Rotate around ball, so as to find an open lane to dribble thru
    """
    if player.firstFrame():
        if transitions.leftLessCrowdedThanRight(player):
            print "Chose left"
            player.setWalk(0, -.5, .15)
        else:
            print "Chose right"
            player.setWalk(0, .5, -.15)

    print "Rotating..."

    if (transitions.ballLost(player) or transitions.ballGotFarAway(player) or
        transitions.centerLaneOpen(player)):
        player.stand()
        return player.goLater('dribble')

    return player.stay()

def lookForBall(player):
    """
    Backup and look for ball. If fails, leave the FSA.
    """
    return player.goLater('chase')
