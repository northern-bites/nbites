import DribbleConstants as constants
import noggin_constants as nogginConstants
import objects
from math import fabs, tan, radians

DRIBBLE_TOGGLE = False

def shouldDribble(player):
    """
    We should be in the dribble FSA.
    """
    return (DRIBBLE_TOGGLE and
            (facingGoal(player) or wouldScoreIfDribbledStraight(player)) 
            and timeLeft(player) and not onWingDownfield(player) and 
            not ballGotFarAway(player) and not ballLost(player) and 
            (inPosition(player) or shouldDribbleForGoal(player)))

def shouldDribbleForGoal(player):
    """
    We can dribble it into the goal. There is either no goalie in the net or
    there is almost no time left in the game.
    """
    return ((ballInGoalBox(player) and noGoalieInNet(player)) or
            lastSecondDribbleGoal(player))

def centerLaneOpen(player):
    """
    I have an open lane right in front of me. We don't worry about this if we
    are dribbling for a score. (visionObstacle.mid_dist won't give good data in
    this case.)
    """
    return (player.brain.interface.visionObstacle.mid_dist >
            constants.OPEN_LANE_DIST or
            player.brain.interface.visionObstacle.block_mid == 0 or
            shouldDribbleForGoal(player))

def noGoalieInNet(player):
    """
    We see no goalie between the crossbars.
    """
    vr = player.brain.interface.visionRobot
    return (not vr.red1.on and not vr.navy1.on)

def inPosition(player):
    """
    We are positioned well on the field for dribbling.
    """
    # return betweenCrosses(player)
    return middleThird(player)
    # return firstHalf(player)
    # return secondHalf(player)

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
    We are between our field cross and midfield.
    """
    field_len = nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X
    return (player.brain.loc.x > nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X and
            player.brain.loc.x < field_len / 2.)

def secondHalf(player):
    """
    We are between midfield and our opponent's field cross.
    """
    field_len = nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X
    return (player.brain.loc.x > field_len / 2. and
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
    We should rotate towards the left.
    """
    return player.brain.loc.h < 0
    # return (player.brain.loc.y < (1./2.*nogginConstants.FIELD_HEIGHT))
    # return (player.brain.interface.visionObstacle.block_left == 0 or
    #         (player.brain.interface.visionObstacle.left_dist <
    #         player.brain.interface.visionObstacle.right_dist and
    #         not player.brain.interface.visionObstacle.block_right == 0))

def dribbleGoneBad(player):
    """
    We have dribbled the ball too far to the (relative) left or right.
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

def seesBall(player):
    """
    We see the ball.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)

def ballInGoalBox(player):
    """
    The ball is in the goal box (between the posts actually), so we can
    dribble it in. We are also lined up well enough to dribble it in via going
    straight.
    """
    return (player.brain.ball.x > nogginConstants.FIELD_WHITE_WIDTH -
            nogginConstants.GOALBOX_DEPTH and
            player.brain.ball.y > nogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y and
            player.brain.ball.y < nogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y and
            wouldScoreIfDribbledStraight(player))

def centerField(player):
    return (player.brain.ball.y > 
            nogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y - 30 and
            player.brain.ball.y < 
            nogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y + 30)

def wouldScoreIfDribbledStraight(player):
    """
    If we were to just dribble straight from our current location, would we 
    score a goal?
    """
    tanOfHeading = tan(radians(90-player.brain.loc.h))
    yChange = (nogginConstants.FIELD_WHITE_WIDTH - player.brain.loc.x) / tanOfHeading
    yWhereWouldLeaveField = player.brain.loc.y - yChange
    
    return (yWhereWouldLeaveField > nogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y and
            yWhereWouldLeaveField < nogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y)

def navDone(player):
    """
    Nav is done.
    """
    return player.brain.nav.isAtPosition()

def onWingDownfield(player):
    """
    The ball is on the wing and downfield according to this transition. Also
    we not facing the goal and therefore leaving this position.
    """
    if player.brain.ball.y < nogginConstants.FIELD_WHITE_HEIGHT / 4.:
        return (player.brain.ball.x > 2./3.*nogginConstants.FIELD_WHITE_WIDTH and
                not player.brain.loc.h > constants.FACING_GOAL_ON_WING)
    return (player.brain.ball.y > 3. * nogginConstants.FIELD_WHITE_HEIGHT / 4. and
            player.brain.ball.x > 2./3.*nogginConstants.FIELD_WHITE_WIDTH and
            not player.brain.loc.h < -constants.FACING_GOAL_ON_WING)
    # if player.brain.ball.y < nogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y:
    #     return (player.brain.ball.y < nogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y and
    #             player.brain.ball.x > 2./3.*nogginConstants.FIELD_WHITE_WIDTH and
    #             not player.brain.loc.h > constants.FACING_GOAL_ON_WING)
    # return (player.brain.ball.y > nogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y and
    #         player.brain.ball.x > 2./3.*nogginConstants.FIELD_WHITE_WIDTH and
    #         not player.brain.loc.h < -constants.FACING_GOAL_ON_WING)

def timeLeft(player):
    """
    There is enough time left in the game to dribble. Or there is very little
    time left but the ball is close enough to the goal to dribble it in.
    """
    enough_time = constants.ENOUGH_TIME_FOR_NORMAL_BEHAVIOR
    return (player.brain.game.secs_remaining > enough_time or
            lastSecondDribbleGoal(player))

def lastSecondDribbleGoal(player):
    """
    There not enough time left for kicking in the goalbox to make sense.
    Dribble it in.
    """
    switch_to_dribble = constants.SWITCH_TO_DRIBBLE_IF_IN_GOALBOX
    return (player.brain.game.secs_remaining < switch_to_dribble and
            ballInGoalBox(player))
