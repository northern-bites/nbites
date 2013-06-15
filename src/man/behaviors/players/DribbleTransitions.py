import DribbleConstants as constants
import noggin_constants as nogginConstants
import objects
from math import fabs

def crowded(player):
    """
    The vision heat map is showing a crowded area in front of me.
    """
    print "Crowded?"
    print ((player.brain.interface.visionObstacle.left_dist < constants.CROWDED_DIST 
            and not player.brain.interface.visionObstacle.left_dist == 0)
            or (player.brain.interface.visionObstacle.mid_dist < constants.CROWDED_DIST
            and not player.brain.interface.visionObstacle.mid_dist == 0)
            or (player.brain.interface.visionObstacle.right_dist < constants.CROWDED_DIST
            and not player.brain.interface.visionObstacle.right_dist == 0))
    return ((player.brain.interface.visionObstacle.left_dist < constants.CROWDED_DIST 
            and not player.brain.interface.visionObstacle.left_dist == 0)
            or (player.brain.interface.visionObstacle.mid_dist < constants.CROWDED_DIST
            and not player.brain.interface.visionObstacle.mid_dist == 0)
            or (player.brain.interface.visionObstacle.right_dist < constants.CROWDED_DIST
            and not player.brain.interface.visionObstacle.right_dist == 0))

def centerLaneOpen(player):
    """
    I have an open lane right in front of me.
    """
    print "Center lane?"
    print player.brain.interface.visionObstacle.mid_dist
    print (player.brain.interface.visionObstacle.mid_dist > 
            constants.OPEN_LANE_DIST or 
            player.brain.interface.visionObstacle.mid_dist == 0) 
    return (player.brain.interface.visionObstacle.mid_dist > 
            constants.OPEN_LANE_DIST or 
            player.brain.interface.visionObstacle.mid_dist == 0) 

def middleThird(player):
    """
    We are in the middle third of the field.
    """
    print "Middle third?"
    print (player.brain.loc.x > (1./3.*nogginConstants.FIELD_WIDTH) 
            and player.brain.loc.x < (2./3.*nogginConstants.FIELD_WIDTH))
    # return (player.brain.loc.x > (1./3.*nogginConstants.FIELD_WIDTH) 
    #         and player.brain.loc.x < (2./3.*nogginConstants.FIELD_WIDTH))
    return True

def facingGoal(player):
    """
    We are facing generally towards the goal we want to score on.
    """
    goalCenter = objects.Location(nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                  nogginConstants.CENTER_FIELD_Y)
    ballLocation = objects.Location(player.brain.ball.x, player.brain.ball.y)

    headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
    bearingForKick = headingBallToGoalCenter - player.brain.loc.h

    print "Facing goal?"
    print bearingForKick
    # return (bearingForKick < constants.FACING_FORWARD_DEG and 
    #         bearingForKick > -constants.FACING_FORWARD_DEG)
    return True

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

def seesBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)

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

# def ballMoved(player):
#     """
#     Ball has moved away from where it was seen last.
#     """
#     ball = player.brain.ball
#     ballBefore = player.ballBeforeApproach
#     return (fabs(ball.x - ballBefore.x) > constants.BALL_MOVED_THR or
#             fabs(ball.y - ballBefore.y) > constants.BALL_MOVED_THR)
# 
# def ballClose(player):
#     """
#     We're close to the ball.
#     """
#     ball = player.brain.ball
#     return (ball.vis.frames_on > constants.BALL_ON_THRES and
#             ball.distance < constants.BALL_CLOSE_DISTANCE)
# 
# def shouldSpinToBall(player):
#     """
#     We're not facing the ball well enough yet.
#     """
#     ball = player.brain.ball
#     return (ball.vis.on and
#             fabs(ball.rel_y) > constants.SHOULD_SPIN_TO_BALL_Y and
#             not (ball.distance > constants.SHOULD_SPIN_TO_BALL_DIST and
#                  fabs(ball.bearing_deg) < constants.SHOULD_SPIN_TO_BALL_BEAR))
# 
# def shouldStopSpinning(player):
#     """
#     We're done spinning.
#     """
#     ball = player.brain.ball
#     return (ball.vis.on and
#             fabs(ball.rel_y) < constants.STOP_SPINNING_TO_BALL_Y)
# 
#
# def itsBeenTooLong(player):
#     """
#     We've been in state too long.
#     """
#     return player.counter > 200
# 
