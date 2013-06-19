import DribbleConstants as constants
import noggin_constants as nogginConstants
import objects
from math import fabs

def shouldDribble(player):
    """
    We should be in the dribble FSA.
    """
    return (facingGoal(player) and positionedForDribble(player) and timeLeft(player)
            and not ballGotFarAway(player) and not ballLost(player))

# def crowded(player):
#     """
#     The vision heat map is showing a crowded area in front of me.
#     """
#     return ((player.brain.interface.visionObstacle.left_dist < constants.CROWDED_DIST
#             and not player.brain.interface.visionObstacle.block_left == 0)
#             or (player.brain.interface.visionObstacle.mid_dist < constants.CROWDED_DIST
#             and not player.brain.interface.visionObstacle.block_mid == 0)
#             or (player.brain.interface.visionObstacle.right_dist < constants.CROWDED_DIST
#             and not player.brain.interface.visionObstacle.block_right == 0))

def centerLaneOpen(player):
    """
    I have an open lane right in front of me.
    """
    return (player.brain.interface.visionObstacle.mid_dist >
            constants.OPEN_LANE_DIST or
            player.brain.interface.visionObstacle.block_mid == 0)

def positionedForDribble(player):
    """
    We are either between the two field crosses or in the opponents' goal box.
    """
    return betweenCrosses(player) or ballInGoalBox(player)

def betweenCrosses(player):
    """
    We are between the two field crosses.
    """
    return (player.brain.loc.x > nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X and
            player.brain.loc.x < nogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X)

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

def rotateLeft(player):
    """
    The goal is to the left of us, so we should rotate that way away from traffic.
    """
    return (player.brain.loc.y < (1./2.*nogginConstants.FIELD_HEIGHT))
    # return (player.brain.interface.visionObstacle.block_left == 0 or
    #         (player.brain.interface.visionObstacle.left_dist <
    #         player.brain.interface.visionObstacle.right_dist and
    #         not player.brain.interface.visionObstacle.block_right == 0))

def dribbleGoneBad(player):
    """
    We have dribbled the ball too far to the left or right.
    """
    return abs(player.brain.ball.rel_y) > constants.BALL_TOO_FAR_TO_SIDE

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

# def ballMoved(player):
#     """
#     Ball has moved away from where it was seen last
#     """
#     ball = player.brain.ball
#     ballBefore = player.ballBeforeDribble
#     return (abs(ball.x - ballBefore.x) > constants.BALL_MOVED_THR or
#             abs(ball.y - ballBefore.y) > constants.BALL_MOVED_THR)

def seesBall(player):
    """
    We see the ball.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)

def ballInGoalBox(player):
    """
    The ball is in the goal box (between the posts actually), so we can
    dribble it in.
    """
    return False
    # return (player.brain.ball.x > nogginConstants.FIELD_WHITE_WIDTH -
    #         nogginConstants.GOALBOX_DEPTH and
    #         player.brain.ball.y > nogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y and
    #         player.brain.ball.y < nogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y)

def navDone(player):
    """
    Nav is done.
    """
    return player.brain.nav.isAtPosition()

def timeLeft(player):
    """
    There is enough time left in the game to dribble. Or there is very little
    time left but the ball is close enough to the goal to dribble it in.
    """
    return (player.brain.game.secs_remaining > 25 or
            (player.brain.game.secs_remaining < 10 and
             ballInGoalBox(player)))
