from . import PBConstants
from .. import NogginConstants
from ..util import MyMath
from math import (hypot, atan2)

# Game Playing SubRoles
def pChaser(team, workingPlay):
    """ never called. now handled entirely in player behavior """
    pass

def pLeftWing(team, workingPlay):
    """position left winger"""
    workingPlay.setSubRole(PBConstants.LEFT_WING)
    y = MyMath.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.LEFT_WING_MIN_Y,
                        PBConstants.LEFT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.WING_MIN_X, PBConstants.WING_MAX_X)
    h = team.brain.ball.heading

    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pRightWing(team, workingPlay):
    """position right winger"""
    workingPlay.setSubRole(PBConstants.RIGHT_WING)
    y = MyMath.clip(team.brain.ball.y + PBConstants.WING_Y_OFFSET,
                        PBConstants.RIGHT_WING_MIN_Y,
                        PBConstants.RIGHT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.WING_MIN_X, PBConstants.WING_MAX_X)
    h = team.brain.ball.heading

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pDubDOffender(team, workingPlay):
    """offender for when in dubD"""
    workingPlay.setSubRole(PBConstants.DUBD_OFFENDER)
    y = MyMath.clip(team.brain.ball.y,
                        PBConstants.LEFT_WING_MIN_Y,
                        PBConstants.RIGHT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x + 150, NogginConstants.GREEN_PAD_X,
                         NogginConstants.CENTER_FIELD_X)
    h = team.brain.ball.heading

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pDefensiveMiddie(team, workingPlay):
    workingPlay.setSubRole(PBConstants.DEFENSIVE_MIDDIE)

    if team.brain.ball.y < NogginConstants.MIDFIELD_Y:
        y = PBConstants.MIN_MIDDIE_Y
    else:
        y = PBConstants.MAX_MIDDIE_Y
    h = team.brain.ball.heading

    pos = (PBConstants.DEFENSIVE_MIDDIE_X, y, h)
    workingPlay.setPosition(pos)

def pOffensiveMiddie(team, workingPlay):
    workingPlay.setSubRole(PBConstants.OFFENSIVE_MIDDIE)
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_MIDDIE_Y,
                    PBConstants.MAX_MIDDIE_Y)
    h = team.brain.ball.heading

    pos = (PBConstants.OFFENSIVE_MIDDIE_POS_X, y, h)
    workingPlay.setPosition(pos)

# Defender sub roles
def pStopper(team, workingPlay):
    """position stopper"""
    workingPlay.setSubRole(PBConstants.STOPPER)
    x,y = getPointBetweenBallAndGoal(team.brain.ball,
                                     PBConstants.DEFENDER_BALL_DIST)
    if x < PBConstants.SWEEPER_X:
        x = PBConstants.SWEEPER_X

    elif x > PBConstants.STOPPER_MAX_X:
        pos_x = PBConstants.STOPPER_MAX_X
        delta_y = team.brain.ball.y - NogginConstants.MY_GOALBOX_MIDDLE_Y
        delta_x = team.brain.ball.x - NogginConstants.MY_GOALBOX_LEFT_X
        pos_y = (NogginConstants.MY_GOALBOX_MIDDLE_Y + delta_x / delta_y *
                 (PBConstants.STOPPER_MAX_X -
                  NogginConstants.MY_GOALBOX_LEFT_X))
    h = team.brain.ball.heading

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pBottomStopper(team, workingPlay):
    """position stopper"""
    workingPlay.setSubRole(PBConstants.BOTTOM_STOPPER)
    h = team.brain.ball.heading
    pos = (PBConstants.STOPPER_X,
           PBConstants.BOTTOM_STOPPER_Y, h)
    workingPlay.setPosition(pos)

def pTopStopper(team, workingPlay):
    """position stopper"""
    workingPlay.setSubRole(PBConstants.TOP_STOPPER)
    h = team.brain.ball.heading
    pos = (PBConstants.STOPPER_X,
           PBConstants.TOP_STOPPER_Y, h)
    workingPlay.setPosition(pos)

def pSweeper(team, workingPlay):
    """position sweeper"""
    workingPlay.setSubRole(PBConstants.SWEEPER)
    x = PBConstants.SWEEPER_X
    y = PBConstants.SWEEPER_Y
    h = team.brain.ball.heading

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pLeftDeepBack(team, workingPlay):
    """position deep left back"""
    workingPlay.setSubRole(PBConstants.LEFT_DEEP_BACK)
    h = team.brain.ball.heading
    pos = (PBConstants.DEEP_BACK_X, PBConstants.LEFT_DEEP_BACK_Y,h)
    workingPlay.setPosition(pos)

def pRightDeepBack(team, workingPlay):
    """position deep right back"""
    workingPlay.setSubRole(PBConstants.RIGHT_DEEP_BACK)
    h = team.brain.ball.heading
    pos = (PBConstants.DEEP_BACK_X, PBConstants.RIGHT_DEEP_BACK_Y,h)
    workingPlay.setPosition(pos)

#Goalie sub roles
def pGoalieNormal(team, workingPlay):
    """normal goalie position"""
    workingPlay.setSubRole(PBConstants.GOALIE_NORMAL)
    h = team.brain.ball.heading
    pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

    if PBConstants.USE_FANCY_GOALIE:
       pos = fancyGoaliePosition(team)

    workingPlay.setPosition(pos)

def pGoalieChaser(team, workingPlay):
    """goalie is being a chaser, presumably in/near goalbox not intended for
        pulling the goalie situations"""
    workingPlay.setSubRole(PBConstants.GOALIE_CHASER)
    h = team.brain.ball.heading
    pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

    if PBConstants.USE_FANCY_GOALIE:
       pos = fancyGoaliePosition(team)

    workingPlay.setPosition(pos)

# Kickoff sub roles
def pKickoffSweeper(team, workingPlay):
    """position kickoff sweeper"""
    workingPlay.setSubRole(PBConstants.KICKOFF_SWEEPER)
    x = PBConstants.KICKOFF_DEFENDER_X
    h = team.brain.my.headingTo(PBConstants.CENTER_FIELD)

    if team.kickoffFormation == 0:
        y = PBConstants.KICKOFF_DEFENDER_0_Y
    else:
        y = PBConstants.KICKOFF_DEFENDER_1_Y

    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pKickoffStriker(team, workingPlay):
    """position kickoff striker"""
    workingPlay.setSubRole(PBConstants.KICKOFF_STRIKER)
    x = PBConstants.KICKOFF_OFFENDER_X
    h = team.brain.my.headingTo(PBConstants.CENTER_FIELD)

    if team.kickoffFormation == 0:
        y = PBConstants.KICKOFF_OFFENDER_0_Y
    else:
        y = PBConstants.KICKOFF_OFFENDER_1_Y

    pos = (x,y,h)
    workingPlay.setPosition(pos)

# SubRoles for ready state
def pReadyChaser(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_CHASER)

    kickOff = team.brain.gameController.ownKickOff

    if kickOff:
        x = PBConstants.READY_KICKOFF_CHASER_X
        y = PBConstants.READY_KICKOFF_CHASER_Y

    else:
        x = PBConstants.READY_NON_KICKOFF_CHASER_X
        y = PBConstants.READY_NON_KICKOFF_CHASER_Y

    h = team.brain.my.headingTo(PBConstants.CENTER_FIELD)
    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pReadyOffender(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_OFFENDER)

    kickOff = team.brain.gameController.ownKickOff

    if kickOff:
        x = PBConstants.READY_KICKOFF_OFFENDER_X
        if team.kickoffFormation == 0:
            y = PBConstants.READY_KICKOFF_OFFENDER_0_Y
        else:
            y = PBConstants.READY_KICKOFF_OFFENDER_1_Y
    else:
        x = PBConstants.READY_NON_KICKOFF_OFFENDER_X
        y = PBConstants.READY_NON_KICKOFF_OFFENDER_Y

    h = team.brain.my.headingTo(PBConstants.CENTER_FIELD)
    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pReadyDefender(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_DEFENDER)

    kickOff = team.brain.gameController.ownKickOff

    if kickOff:
        x = PBConstants.READY_KICKOFF_DEFENDER_X
        if team.kickoffFormation == 0:
            y = PBConstants.READY_KICKOFF_DEFENDER_0_Y
        else:
            y = PBConstants.READY_KICKOFF_DEFENDER_1_Y
    else:
        x = PBConstants.READY_NON_KICKOFF_DEFENDER_X
        y = PBConstants.READY_NON_KICKOFF_DEFENDER_Y

    h = team.brain.my.headingTo(PBConstants.CENTER_FIELD)
    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pGoalieReady(team, workingPlay):
    """
    Go to our home position during ready
    """
    workingPlay.setSubRole(PBConstants.READY_GOALIE)
    position = (PBConstants.GOALIE_HOME_X,
                PBConstants.GOALIE_HOME_Y,
                team.brain.my.headingTo(PBConstants.CENTER_FIELD))
    workingPlay.setPosition(position)


def getPointBetweenBallAndGoal(ball,dist_from_ball):
    """returns defensive position between ball (x,y) and goal (x,y)
    at <dist_from_ball> centimeters away from ball"""
    delta_y = ball.y - NogginConstants.MY_GOALBOX_MIDDLE_Y
    delta_x = ball.x - NogginConstants.MY_GOALBOX_LEFT_X

    # don't divide by 0
    if delta_x == 0:
        delta_x = 0.1
    if delta_y == 0:
        delta_y = 0.1

    pos_x = ball.x - ( dist_from_ball/
                       hypot(delta_x,delta_y) )*delta_x
    pos_y = ball.y - ( dist_from_ball/
                       hypot(delta_x,delta_y) )*delta_y

    return pos_x,pos_y

def fancyGoaliePosition(team):
    """returns a goalie position using ellipse"""

    position = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y)

    # lets try maintaining home position until the ball is closer in
    # might help us stay localized better
    ball = team.brain.ball
    if ball.dist < PBConstants.ELLIPSE_POSITION_LIMIT:
        # Use an ellipse just above the goalline to determine x and y position
        # We get the angle from goal center to the ball to determine our X,Y
        theta = atan2( ball.y - PBConstants.LARGE_ELLIPSE_CENTER_Y,
                       ball.x - PBConstants.LARGE_ELLIPSE_CENTER_X)

        thetaDeg = PBConstants.RAD_TO_DEG * theta

        # Clip the angle so that the (x,y)-coordinate is not too close to the posts
        if PBConstants.ELLIPSE_ANGLE_MIN > MyMath.sub180Angle(thetaDeg):
            theta = PBConstants.ELLIPSE_ANGLE_MIN * PBConstants.DEG_TO_RAD
        elif PBConstants.ELLIPSE_ANGLE_MAX < MyMath.sub180Angle(thetaDeg):
            theta = PBConstants.ELLIPSE_ANGLE_MAX * PBConstants.DEG_TO_RAD

        # Determine X,Y of ellipse based on theta, set heading on the ball
        x, y = team.ellipse.getPositionFromTheta(theta)
        h = ball.heading
        position = (x,y,h)

    return position
