import noggin_constants as nogginConstants
from math import fabs

# Many of these functions are higher order functions, meaning that they return
# transitions (a kind of function) that takes player as an argument!
# e.g. to check if the ball has been seen for 4 frames do the following:
#      ballOnForNFrames(4)(player) because ballOnForNFrames is NOT a transition
#      it RETURNS a transition

### THE BALL
def ballOnForNFrames(atleastThisManyFrames):
    """
    We have seen the ball for N frames.
    """
    def transition(player):
        ball = player.brain.ball
        return (ball.vis.frames_on > atleastThisManyFrames)
    return transition

def ballOffForNFrames(atleastThisManyFrames):
    """
    We have not seen the ball for N frames.
    """
    def transition(player):
        ball = player.brain.ball
        return (ball.vis.frames_off > atleastThisManyFrames)
    return transition

def ballForNFramesAndMClose(atleastThisManyFrames, atleastThisClose):
    """
    We have seen the ball for N frames and it is M centimeters away or closer.

    NOTE: This is not a CountTransition, see util/Transitions.py if you want
    a rolling fiter of predicate checks.
    """
    def transition(player):
        ball = player.brain.ball
        return (ballOnForNFrames(atleastThisManyFrames)(player) and
                ball.distance < atleastThisClose)
    return transition

def ballForNFramesAndMFar(atleastThisManyFrames, atleastThisFar):
    """
    We have seen the ball for N frames and it is M centimeters away or further.

    NOTE: This is not a CountTransition, see util/Transitions.py if you want
    a rolling fiter of predicate checks.
    """
    def transition(player):
        ball = player.brain.ball
        return (ballOnForNFrames(atleastThisManyFrames)(player) and
                ball.distance > atleastThisFar)
    return transition

def ballMovedNCMsFromWhereSeenLast(ballMovedTolerance):
    """
    The ball is atleast N cms from where it was seen when ballBeforeCheck was set.

    @requires player.ballBeforeCheck to be set to a tuple of filtered ball
              relative coordinates for comparision,
    e.g. player.ballBeforeCheck = (player.brain.ball.rel_x, player.brain.ball.rel_y)
    """
    def transition(player):
        ball = player.brain.ball
        ballBefore = player.ballBeforeCheck
        return (fabs(ball.rel_x - ballBefore[0]) > ballMovedTolerance or
                fabs(ball.rel_y - ballBefore[1]) > ballMovedTolerance)
    return transition

### STATE-TIME/FRAME-COUNT
def sameStateForNFrames(numFrames):
    """
    We have been in the same state for N frames.

    NOTE: ~30 frames = 1 second, probably better to use sameStateForNSeconds
    """
    def transition(player):
        return (player.counter > numFrames)
    return transition

def sameStateForNSeconds(numSeconds):
    """
    We have been in the same state for N seconds.
    """
    def transition(player):
        return (player.stateTime > numSeconds)
    return transition

# NOTE could be tested more
def noTimeLeft(seconds):
    """
    There are not more than N seconds left in the game.
    """
    def transition(player):
        return (player.brain.game.secs_remaining < seconds)
    return transition

### FIELD POSITION
def atRobotLocation(robotLocation, precision):
    """
    Checks against loc to see if you are at the specified location within
    the specified precision. I created this transition because goTo didn't
    seem to be stopping correctly. -DZ
    """
    def transition(player):
        return (player.brain.loc.x > robotLocation.getX() - precision[0] and
                player.brain.loc.x < robotLocation.getX() + precision[0] and
                player.brain.loc.y > robotLocation.getY() - precision[1] and
                player.brain.loc.y < robotLocation.getY() + precision[1] and
                player.brain.loc.h > robotLocation.getH() - precision[2] and
                player.brain.loc.h < robotLocation.getH() + precision[2])
    return transition

def betweenCrosses(player):
    """
    We are between the two field crosses.
    """
    return (player.brain.loc.x > nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X and
            player.brain.loc.x < nogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X)

def middleThird(player):
    """
    We are in the middle third of the field.
    """
    field_len = nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X
    return (player.brain.loc.x > field_len / 3. and
            player.brain.loc.x < 2. * field_len / 3.)

def firstHalf(player):
    """
    We are on our half.
    """
    return ourHalf(player.brain.loc)

def secondHalf(player):
    """
    We are on the opponent's half.
    """
    return not ourHalf(player.brain.loc)

def navAtPosition(player):
    """
    Does nav think that it's at position?
    """
    return player.brain.nav.isAtPosition()

### HELPER FUNCTIONS
def ourHalf(location):
    """
    We are in our half.
    """
    return location.x < nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X / 2.

def leftSide(location):
    """
    We are on the left side of the field.
    """
    return location.y < nogginConstants.CENTER_FIELD_Y

def lowerLeft(location):
    """
    We are in the lower left quarter of the field.
    """
    return ourHalf(location) and leftSide(location)

def lowerRight(location):
    """
    We are in the lower right quarter of the field.
    """
    return ourHalf(location) and not leftSide(location)

def upperLeft(location):
    """
    We are in the upper left quarter of the field.
    """
    return not ourHalf(location) and leftSide(location)

def upperRight(location):
    """
    We are in the upper right quarter of the field.
    """
    return not ourHalf(location) and not leftSide(location)

