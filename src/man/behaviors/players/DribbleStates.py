"""
Here is the FSA for dribbling a ball.
"""
import DribbleTransitions as transitions
import DribbleConstants as constants
from ..navigator import Navigator
from ..kickDecider import kicks
from objects import RelRobotLocation, Location

### BASIC IDEA 
# We dribble by setting ourselves up for a dribble kick. The sweet spot 
# is in front of the ball, so setting ourselves up for a kick actually
# results in us running through the ball. (There is no actual dribble sweet
# move.) If vision sees a crowded area in front of us, we rotate around the 
# ball and dribble again when we see a clear path. We only dribble if these 
# conditions are true: 1. We are positioned according to positionedForDribble. 
# 2. We are facing our opponents' goal. 3. We can see the ball close to us.
# 4. There is time left according to timeLeft.

### TODO
# test goalie-in-net decision making
# test time-based decision making
# rotate towards goal when dribbling
# choose direction better, based on loc and heatmap?
# frame counter rotatation?

### DONE
# ballInGoalBox dribbling via goalie detection
# test DRIBBLE_ON_KICKOFF
# dribbleGoneBad needs work
# dribble for the score if close enough to goal
# get rid of 'dribble' state, reorganize FSA
# cross to cross dribbling
# time-left based decision making

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

    if not transitions.shouldDribble(player):
        return player.goLater('chase')
    elif transitions.centerLaneOpen(player):
        return player.goNow('executeDribble')
    else:
        return player.goNow('rotateToOpenSpace')

def executeDribble(player):
    """
    Move through the ball, so as to execute a dribble.
    """
    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    player.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                       ball.rel_y - kick_pos[1],
                                       0)

    if player.firstFrame():
        player.ballBeforeDribble = ball
        player.brain.nav.goTo(player.kickPose,
                              Navigator.PRECISELY,
                              Navigator.CAREFUL_SPEED,
                              False,
                              False)
    else:
        player.brain.nav.updateDest(player.kickPose)

    if transitions.ballLost(player):
        return player.goNow('lookForBall')
    elif not transitions.shouldDribble(player):
        return player.goLater('chase')
    elif not transitions.centerLaneOpen(player): # reorder CLO and DGB?
        return player.goNow('rotateToOpenSpace')
    elif transitions.dribbleGoneBad(player):
        return player.goNow('positionForDribble')

    return player.stay()

def rotateToOpenSpace(player):
    """
    Rotate around ball, so as to find an open lane to dribble thru
    """
    if player.firstFrame():
        if transitions.rotateLeft(player):
            player.setWalk(0, -.5, .15)
        else:
            player.setWalk(0, .5, -.15)

    if transitions.ballLost(player):
        return player.goNow('lookForBall')
    elif not transitions.shouldDribble(player):
        player.stand()
        return player.goLater('chase')
    elif transitions.centerLaneOpen(player):
        player.stand()
        return player.goLater('decideDribble')

    return player.stay()

def lookForBall(player):
    """
    Backup and look for ball. If fails, leave the FSA.
    """
    if player.firstFrame():
        lookForBall.counter = 0
        player.brain.tracker.repeatWidePan()
        backupLoc = RelRobotLocation(constants.BACKUP_WHEN_LOST,0,0)
        player.brain.nav.goTo(backupLoc,
                              Navigator.GENERAL_AREA,
                              Navigator.MEDIUM_SPEED,
                              False,
                              False)

    lookForBall.counter += 1
        
    if transitions.seesBall(player):
        player.brain.tracker.trackBall()
        return player.goNow('positionForDribble')
    elif lookForBall.counter == constants.LOOK_FOR_BALL_FC:
        return player.goLater('chase')

    return player.stay()

def positionForDribble(player):
    """
    We should position ourselves behind the ball for easy dribbling.
    """
    ball = player.brain.ball
    backed_off = constants.DRIBBLE_SETUP_POSITION
    player.kickPose = RelRobotLocation(ball.rel_x + backed_off,
                                       ball.rel_y,
                                       0)

    if player.firstFrame():
        player.brain.nav.goTo(player.kickPose,
                              Navigator.GENERAL_AREA,
                              Navigator.MEDIUM_SPEED,
                              False,
                              False)
    else:
        player.brain.nav.updateDest(player.kickPose)

    if transitions.ballLost(player):
        return player.goLater('lookForBall')
    elif transitions.navDone(player):
        return player.goLater('decideDribble')

    return player.stay()
