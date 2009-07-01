from . import PBConstants
from .. import NogginConstants
from ..util import MyMath


# Game Playing SubRoles
def pChaser(team):
    my = team.brain.my
    pos = [my.x, my.y]
    return [PBConstants.CHASE_NORMAL, pos]

def pLeftWing(team):
    '''position left winger'''
    y = MyMath.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.LEFT_WING_MIN_Y,
                        PBConstants.LEFT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.WING_MIN_X, PBConstants.WING_MAX_X)
    return [PBConstants.LEFT_WING, [x,y]]

def pRightWing(team):
    '''position right winger'''
    y = MyMath.clip(team.brain.ball.y + PBConstants.WING_Y_OFFSET,
                        PBConstants.RIGHT_WING_MIN_Y,
                        PBConstants.RIGHT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.WING_MIN_X, PBConstants.WING_MAX_X)
    return [PBConstants.RIGHT_WING, [x,y]]

def pDubDOffender(team):
    '''offender for when in dubD'''
    y = MyMath.clip(team.brain.ball.y,
                        PBConstants.LEFT_WING_MIN_Y,
                        PBConstants.RIGHT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x + 150, NogginConstants.GREEN_PAD_X,
                         NogginConstants.CENTER_FIELD_X)
    return [PBConstants.DUBD_OFFENDER, [x,y]]

def pDefensiveMiddie(team):
    if team.brain.ball.y < NogginConstants.MIDFIELD_Y:
        y = PBConstants.MIN_MIDDIE_Y
    else:
        y = PBConstants.MAX_MIDDIE_Y
    pos = [PBConstants.DEFENSIVE_MIDDIE_X, y]
    return [PBConstants.DEFENSIVE_MIDDIE, pos]

def pOffensiveMiddie(team):
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_MIDDIE_Y,
                    PBConstants.MAX_MIDDIE_Y)
    pos = [PBConstants.OFFENSIVE_MIDDIE_POS_X, y]
    return [PBConstants.OFFENSIVE_MIDDIE, pos]

# Defender sub roles
def pStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    if x < PBConstants.SWEEPER_X:
        x = PBConstants.SWEEPER_X
    return [PBConstants.STOPPER, [x,y]]

def pBottomStopper(team):
    '''position stopper'''
    return [PBConstants.STOPPER, [PBConstants.STOPPER_X,
                                  PBConstants.BOTTOM_STOPPER_Y]]

def pTopStopper(team):
    '''position stopper'''
    return [PBConstants.STOPPER, [PBConstants.STOPPER_X,
                                  PBConstants.TOP_STOPPER_Y]]

def pDeepStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    x = MyMath.clip(x, PBConstants.SWEEPER_X,PBConstants.DEEP_STOPPER_X)
    return [PBConstants.DEEP_STOPPER, [x,y]]

def pSweeper(team):
    '''position sweeper'''
    x = PBConstants.SWEEPER_X
    y = PBConstants.SWEEPER_Y
    return [PBConstants.SWEEPER, [x,y]]

def pLeftDeepBack(team):
    '''position deep left back'''
    return [PBConstants.LEFT_DEEP_BACK,[PBConstants.LEFT_DEEP_BACK_POS]]

def pRightDeepBack(team):
    '''position deep right back'''
    return [PBConstants.RIGHT_DEEP_BACK,[PBConstants.RIGHT_DEEP_BACK_POS]]

#Goalie sub roles
def pGoalieNormal(team):
    '''normal goalie position'''
    ball = team.brain.ball
    position = [PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y]

    if PBConstants.USE_FANCY_GOALIE:
        # lets try maintaining home position until the ball is closer in
        # might help us stay localized better
        if 0 < ball.locDist < PBConstants.ELLIPSE_POSITION_LIMIT:
            # Use an ellipse just above the goalline to determine x and y position
            # We get the angle from goal center to the ball to determine our X,Y
            theta = MyMath.safe_atan2( ball.y - PBConstants.LARGE_ELLIPSE_CENTER_Y,
                                       ball.x - PBConstants.LARGE_ELLIPSE_CENTER_X)

            thetaDeg = PBConstants.RAD_TO_DEG * theta

            # Clip the angle so that the (x,y)-coordinate is not too close to the posts
            if PBConstants.ELLIPSE_ANGLE_MIN > MyMath.sub180Angle(thetaDeg):
                theta = PBConstants.ELLIPSE_ANGLE_MIN * PBConstants.DEG_TO_RAD
            elif PBConstants.ELLIPSE_ANGLE_MAX < MyMath.sub180Angle(thetaDeg):
                theta = PBConstants.ELLIPSE_ANGLE_MAX * PBConstants.DEG_TO_RAD

            # Determine X,Y of ellipse based on theta, set heading on the ball
            position = team.ellipse.getPositionFromTheta(theta)

    return [PBConstants.GOALIE_NORMAL, position]

def pGoalieChaser(team):
    '''goalie is being a chaser, presumably in/near goalbox not intended for
        pulling the goalie situations'''

    ball = team.brain.ball
    goal = pGoalieNormal(team)
    position = [goal[1][0], goal[1][1]]
    return [PBConstants.GOALIE_CHASER, position]

# Kickoff sub roles
def pKickoffSweeper(team):
    '''position kickoff sweeper'''
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_DEFENDER_0
    else:
        pos = PBConstants.KICKOFF_DEFENDER_1
    return [PBConstants.KICKOFF_SWEEPER, pos]

def pKickoffStriker(team):
    '''position kickoff striker'''
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_OFFENDER_0
    else:
        pos = PBConstants.KICKOFF_OFFENDER_1
    return [PBConstants.KICKOFF_STRIKER, pos]

def pKickoffPlaySweeper(team):
    '''position kickoff sweeper'''
    pos = PBConstants.KICKOFF_PLAY_DEFENDER
    return [PBConstants.KICKOFF_SWEEPER, pos]

def pKickoffPlayStriker(team):
    '''position kickoff striker'''
    pos = PBConstants.KICKOFF_PLAY_OFFENDER
    return [PBConstants.KICKOFF_STRIKER, pos]

# SubRoles for ready state
def pReadyChaser(team):
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        pos = PBConstants.READY_KICKOFF_NORMAL_CHASER
    else:
        pos = PBConstants.READY_NON_KICKOFF_CHASER
    return [PBConstants.READY_CHASER, pos]

def pReadyOffender(team):
    kickOff = (team.brain.gameController.gc.kickOff== team.brain.my.teamColor)
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_OFFENDER_0
        else:
            pos = PBConstants.READY_KICKOFF_OFFENDER_1
    else:
        pos = PBConstants.READY_NON_KICKOFF_OFFENDER
    return [PBConstants.READY_OFFENDER, pos]

def pReadyDefender(team):
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_DEFENDER_0
        else:
            pos = PBConstants.READY_KICKOFF_DEFENDER_1
    else:
        pos = PBConstants.READY_NON_KICKOFF_DEFENDER
    return [PBConstants.READY_DEFENDER, pos]
