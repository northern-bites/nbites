import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import ChaseBallConstants as constants
import KickingHelpers as helpers
from .. import NogginConstants
from ..util import MyMath

####### CHASING STUFF ##############

def shouldTurnToBall_FoundBall(player):
    """
    Should we turn to the ball heading after searching for the ball
    """
    return( player.brain.ball.framesOn > constants.BALL_ON_THRESH)

def shouldTurnToBall_ApproachBall(player):
    """
    Should turn to the ball, if we are currently approaching it
    """
    ball = player.brain.ball
    return (ball.on and
            abs(ball.bearing) > constants.BALL_APPROACH_BEARING_OFF_THRESH)

def shouldApproachBall(player):
    """
    Begin walking to the ball if it is close to straight in front of us
    """
    ball = player.brain.ball
    return ( ball.on and
             abs(ball.bearing) < constants.BALL_APPROACH_BEARING_THRESH )

def shouldApproachBallWithLoc(player):
    return player.brain.ball.on and \
        player.brain.my.locScore >= NogginConstants.OK_LOC and \
        constants.USE_LOC_CHASE and \
        player.brain.ball.locDist > 30

def shouldApproachFromPositionForKick(player):
    """
    Walk to the ball if its too far away
    """
    ball = player.brain.ball
    return shouldApproachBall(player) and \
        not shouldPositionForKick(player) and \
        ball.locDist > 50.0

def shouldTurnToBallFromPositionForKick(player):
    """
    Walk to the ball if its too far away
    """
    ball = player.brain.ball
    return (ball.on and
            abs(ball.bearing) > constants.BALL_APPROACH_BEARING_OFF_THRESH +
            constants.POSITION_FOR_KICK_BEARING_THRESH)

def shouldPositionForKick(player):
    """
    Should begin aligning on the ball for a kick when close
    """
    ball = player.brain.ball
    return ball.on and \
        constants.BALL_POS_KICK_LEFT_Y > ball.relY > \
        constants.BALL_POS_KICK_RIGHT_Y and \
        constants.BALL_POS_KICK_MAX_X > ball.relX > \
        constants.BALL_POS_KICK_MIN_X and \
        ball.bearing < constants.POSITION_FOR_KICK_BEARING_THRESH

def shouldPositionForKickFromApproachLoc(player):
    return shouldPositionForKick(player) and \
        player.atHeading()

def shouldRepositionForKick(player):
    """
    Stop waiting for kick and realign on the ball instead
    """
    ball = player.brain.ball
    return False

def shouldApproachForKick(player):
    """
    While positioning for kick, we need to walk forward
    """
    ball = player.brain.ball
    return (ball.on and
            ball.relX > constants.BALL_KICK_LEFT_X_FAR ) # 5 is a BUFFER

def shouldKick(player):
    """
    Ball is in the correct foot position to kick
    """
    ball = player.brain.ball
    return ball.on and \
        constants.BALL_KICK_LEFT_Y_L > ball.locRelY > \
        constants.BALL_KICK_RIGHT_Y_R and \
        constants.BALL_KICK_LEFT_X_CLOSE < ball.locRelX < \
        constants.BALL_KICK_LEFT_X_FAR


def shouldDribble(player):
    """
    Ball is in between us and the opp goal, let's dribble for a while
    """
    my = player.brain.my
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return  (constants.USE_DRIBBLE and
             not player.penaltyKicking and
             0 < player.brain.ball.relX < constants.SHOULD_DRIBBLE_X and
             0 < abs(player.brain.ball.relY) < constants.SHOULD_DRIBBLE_Y and
             abs(goalBearing) < constants.SHOULD_DRIBBLE_BEARING and
             not inOppGoalbox(player) and
             player.brain.my.x > (
            NogginConstants.FIELD_WHITE_WIDTH / 3.0 +
            NogginConstants.GREEN_PAD_X) )

def shouldStopDribbling(player):
    """
    While dribbling we should stop
    """
    my = player.brain.my
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return (player.penaltyKicking or
            inOppGoalbox(player) or
            player.brain.ball.relX > constants.STOP_DRIBBLE_X or
            abs(player.brain.ball.relY) > constants.STOP_DRIBBLE_Y or
            abs(goalBearing) > constants.STOP_DRIBBLE_BEARING or
            player.brain.my.x < ( NogginConstants.FIELD_WHITE_WIDTH / 3.0 +
                                  NogginConstants.GREEN_PAD_X))


def inOppGoalbox(player):
    return (NogginConstants.OPP_GOALBOX_LEFT_X < player.brain.my.x and
            NogginConstants.OPP_GOALBOX_BOTTOM_Y < player.brain.my.y <
            NogginConstants.OPP_GOALBOX_TOP_Y)

######### BALL IN BOX ###############

def shouldChaseAroundBox(player):
    ball = player.brain.ball
    my = player.brain.my
    intersect = MyMath.linesIntersect

    return ( intersect( my.x, my.y, ball.x, ball.y, # BOTTOM_GOALBOX_LINE
                    NogginConstants.MY_GOALBOX_LEFT_X,
                    NogginConstants.MY_GOALBOX_BOTTOM_Y,
                    NogginConstants.MY_GOALBOX_RIGHT_X,
                    NogginConstants.MY_GOALBOX_BOTTOM_Y) or
         intersect( my.x, my.y, ball.x, ball.y, # LEFT_GOALBOX_LINE
                    NogginConstants.MY_GOALBOX_RIGHT_X,
                    NogginConstants.MY_GOALBOX_TOP_Y,
                    NogginConstants.MY_GOALBOX_RIGHT_X,
                    NogginConstants.MY_GOALBOX_BOTTOM_Y) or
         intersect( my.x, my.y, ball.x, ball.y, # BOTTOM_GOALBOX_LINE
                    NogginConstants.MY_GOALBOX_LEFT_X,
                    NogginConstants.MY_GOALBOX_TOP_Y,
                    NogginConstants.MY_GOALBOX_RIGHT_X,
                    NogginConstants.MY_GOALBOX_TOP_Y) )

def shouldNotGoInBox(player):
    return (False and player.ballInMyGoalBox() and \
                player.brain.ball.locDist < constants.IGNORE_BALL_IN_BOX_DIST)

####### AVOIDANCE STUFF ##############

def shouldAvoidObstacleLeft(player):
    """
    Need to avoid an obstacle on our left side
    """
    sonar = player.brain.sonar
    if  (sonar.LLdist != sonar.UNKNOWN_VALUE and
         sonar.LLdist < constants.AVOID_OBSTACLE_FRONT_DIST) or \
         (sonar.LRdist != sonar.UNKNOWN_VALUE and
          sonar.LRdist < constants.AVOID_OBSTACLE_SIDE_DIST):
        player.shouldAvoidObstacleLeftCounter += 1
    else :
        player.shouldAvoidObstacleLeftCounter = 0


    if player.shouldAvoidObstacleLeftCounter > \
            constants.AVOID_OBSTACLE_FRAMES_THRESH:
        return True
    return False

def shouldAvoidObstacleRight(player):
    """
    Need to avoid an obstacle on our right side
    """
    sonar = player.brain.sonar
    if (sonar.RRdist != sonar.UNKNOWN_VALUE and
         sonar.RRdist < constants.AVOID_OBSTACLE_SIDE_DIST) or \
         (sonar.RLdist != sonar.UNKNOWN_VALUE and
          sonar.RLdist < constants.AVOID_OBSTACLE_FRONT_DIST):
         player.shouldAvoidObstacleRightCounter += 1
    else :
        player.shouldAvoidObstacleRightCounter = 0

    if player.shouldAvoidObstacleRightCounter > \
            constants.AVOID_OBSTACLE_FRAMES_THRESH:
        return True
    return False

def shouldAvoidObstacle(player):
    """
    Should avoid an obstacle
    """
    return ((shouldAvoidObstacleLeft(player) or
             shouldAvoidObstacleRight(player)) and
            not player.penaltyKicking)

def shouldAvoidObstacleDuringApproachBall(player):
    return shouldAvoidObstacle(player) and \
        (player.brain.ball.locDist >
         constants.SHOULD_AVOID_OBSTACLE_APPROACH_DIST)

####### FIND BALL STUFF ##############

def shouldScanFindBall(player):
    """
    We lost the ball, scan to find it
    """
    return (player.brain.ball.framesOff > constants.BALL_OFF_THRESH)

def shouldScanFindBallActiveLoc(player):
    """
    We lost the ball, scan to find it
    """
    return not (player.brain.tracker.activePanUp or
                player.brain.tracker.activePanOut) and \
        (player.brain.ball.framesOff > constants.BALL_OFF_ACTIVE_LOC_THRESH)

def shouldSpinFindBall(player):
    """
    Should spin if we already tried searching
    """
    return (player.stateTime >= SweetMoves.getMoveTime(HeadMoves.HIGH_SCAN_BALL))

def shouldntStopChasing(player):
    """
    Dont switch out of chaser in certain circumstances
    """
    return player.inKickingState

def shouldWalkToBallLocPos(player):
    return player.counter > constants.WALK_TO_BALL_LOC_POS_FRAMES and \
        player.brain.ball.framesOff > constants.BALL_OFF_THRESH

def shouldActiveLoc(player):
    if player.brain.ball.on:
        return (player.brain.ball.locDist > constants.APPROACH_ACTIVE_LOC_DIST
                and abs(player.brain.ball.locBearing) <
                constants.APPROACH_ACTIVE_LOC_BEARING)

    else:
        return player.brain.ball.locDist > constants.APPROACH_ACTIVE_LOC_DIST

def shouldStopPenaltyKickDribbling(player):
    """
    While dribbling we should stop
    """
    my = player.brain.my
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return (inPenaltyKickStrikezone(player) or
            player.brain.ball.relX > constants.STOP_DRIBBLE_X or
            abs(player.brain.ball.relY) > constants.STOP_DRIBBLE_Y or
            abs(goalBearing) > constants.STOP_DRIBBLE_BEARING or
            player.counter > constants.STOP_PENALTY_DRIBBLE_COUNT)

def inPenaltyKickStrikezone(player):
    return (NogginConstants.OPP_GOALBOX_LEFT_X + 75. < player.brain.my.x)

def shouldNotApproachWithLocAnymore(player):
    return (player.brain.my.locScoreFramesBad > constants.APPROACH_NO_LOC_THRESH
            and
            player.brain.ball.locDist > constants.APPROACH_NO_MORE_LOC_DIST )
