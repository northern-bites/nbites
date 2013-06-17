import DribbleConstants as constants
import noggin_constants as nogginConstants
import objects
from math import fabs

def crowded(player):
    """
    The vision heat map is showing a crowded area in front of me.
    """
    return ((player.brain.interface.visionObstacle.left_dist < constants.CROWDED_DIST 
            and not player.brain.interface.visionObstacle.block_left == 0)
            or (player.brain.interface.visionObstacle.mid_dist < constants.CROWDED_DIST
            and not player.brain.interface.visionObstacle.block_mid == 0)
            or (player.brain.interface.visionObstacle.right_dist < constants.CROWDED_DIST
            and not player.brain.interface.visionObstacle.block_right == 0))

def centerLaneOpen(player):
    """
    I have an open lane right in front of me.
    """
    return (player.brain.interface.visionObstacle.mid_dist > 
            constants.OPEN_LANE_DIST or 
            player.brain.interface.visionObstacle.block_mid == 0) 

def rotateLeft(player):
    return (player.brain.loc.y < (1./2.*nogginConstants.FIELD_HEIGHT))
    # return (player.brain.interface.visionObstacle.block_left == 0 or
    #         (player.brain.interface.visionObstacle.left_dist < 
    #         player.brain.interface.visionObstacle.right_dist and
    #         not player.brain.interface.visionObstacle.block_right == 0))

def middleThird(player):
    """
    We are in the middle third of the field.
    """
    return (player.brain.loc.x > (1./3.*nogginConstants.FIELD_WIDTH) 
            and player.brain.loc.x < (2./3.*nogginConstants.FIELD_WIDTH))

def facingGoal(player):
    """
    We are facing generally towards the goal we want to score on.
    """
    goalCenter = objects.Location(nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                  nogginConstants.CENTER_FIELD_Y)
    ballLocation = objects.Location(player.brain.ball.x, player.brain.ball.y)

    headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
    bearingForKick = headingBallToGoalCenter - player.brain.loc.h

    return (bearingForKick < constants.FACING_FORWARD_DEG and 
            bearingForKick > -constants.FACING_FORWARD_DEG)

def ballToOurLeft(player):
    """
    The ball is to our left.
    """
    goalCenter = objects.Location(nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                  nogginConstants.CENTER_FIELD_Y)
    ballLocation = objects.Location(player.brain.ball.x, player.brain.ball.y)

    headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
    bearingForKick = headingBallToGoalCenter - player.brain.loc.h

    return (bearingForKick > 0)

def ballLost(player):
    """
    We lost the ball.
    """
    return (player.brain.ball.vis.frames_off > constants.BALL_OFF_THRESH)

def ballGotFarAway(player):
    """
    The ball got far away somehow.
    """
    ball = player.brain.ball
    return ball.vis.on and ball.distance > constants.BALL_FAR_AWAY

def navDone(player):
    """
    Nav is done.
    """
    return player.brain.nav.isAtPosition()

def seesBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)
