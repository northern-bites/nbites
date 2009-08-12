from . import PBConstants
from .. import NogginConstants
from ..util import MyMath


# Game Playing SubRoles
def pChaser(team, workingPlay):
    ''' never called. now handled entirely in rChaser '''
    pass

def pLeftWing(team, workingPlay):
    '''position left winger'''
    workingPlay.setSubRole(PBConstants.LEFT_WING)
    y = MyMath.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.LEFT_WING_MIN_Y,
                        PBConstants.LEFT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.WING_MIN_X, PBConstants.WING_MAX_X)
    pos = [x,y]
    workingPlay.setPosition(pos)

def pRightWing(team, workingPlay):
    '''position right winger'''
    workingPlay.setSubRole(PBConstants.RIGHT_WING)
    y = MyMath.clip(team.brain.ball.y + PBConstants.WING_Y_OFFSET,
                        PBConstants.RIGHT_WING_MIN_Y,
                        PBConstants.RIGHT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.WING_MIN_X, PBConstants.WING_MAX_X)
    pos = [x,y]
    workingPlay.setPosition(pos)

def pDubDOffender(team, workingPlay):
    '''offender for when in dubD'''
    workingPlay.setSubRole(PBConstants.DUBD_OFFENDER)
    y = MyMath.clip(team.brain.ball.y,
                        PBConstants.LEFT_WING_MIN_Y,
                        PBConstants.RIGHT_WING_MAX_Y)
    x = MyMath.clip(team.brain.ball.x + 150, NogginConstants.GREEN_PAD_X,
                         NogginConstants.CENTER_FIELD_X)
    pos = [x,y]
    workingPlay.setPosition(pos)

def pDefensiveMiddie(team, workingPlay):
    workingPlay.setSubRole(PBConstants.DEFENSIVE_MIDDIE)
    if team.brain.ball.y < NogginConstants.MIDFIELD_Y:
        y = PBConstants.MIN_MIDDIE_Y
    else:
        y = PBConstants.MAX_MIDDIE_Y
    pos = [PBConstants.DEFENSIVE_MIDDIE_X, y]
    workingPlay.setPosition(pos)

def pOffensiveMiddie(team, workingPlay):
    workingPlay.setSubRole(PBConstants.OFFENSIVE_MIDDIE)
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_MIDDIE_Y,
                    PBConstants.MAX_MIDDIE_Y)
    pos = [PBConstants.OFFENSIVE_MIDDIE_POS_X, y]
    workingPlay.setPosition(pos)

# Defender sub roles
def pStopper(team, workingPlay):
    '''position stopper'''
    workingPlay.setSubRole(PBConstants.STOPPER)
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    if x < PBConstants.SWEEPER_X:
        x = PBConstants.SWEEPER_X
    pos = [x,y]
    workingPlay.setPosition(pos)

def pBottomStopper(team, workingPlay):
    '''position stopper'''
    #HACK -courtesy of Tucker
    workingPlay.setSubRole(PBConstants.STOPPER)
    pos = [PBConstants.STOPPER_X,
           PBConstants.BOTTOM_STOPPER_Y]
    workingPlay.setPosition(pos)

def pTopStopper(team, workingPlay):
    '''position stopper'''
    #HACK -courtesy of Tucker
    workingPlay.setSubRole(PBConstants.STOPPER)
    pos = [PBConstants.STOPPER_X,
           PBConstants.TOP_STOPPER_Y]
    workingPlay.setPosition(pos)

def pDeepStopper(team, workingPlay):
    '''position stopper'''
    workingPlay.setSubRole(PBConstants.DEEP_STOPPER)
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    x = MyMath.clip(x, PBConstants.SWEEPER_X,PBConstants.DEEP_STOPPER_X)
    pos = [x,y]
    workingPlay.setPosition(pos)

def pSweeper(team, workingPlay):
    '''position sweeper'''
    workingPlay.setSubRole(PBConstants.SWEEPER)
    x = PBConstants.SWEEPER_X
    y = PBConstants.SWEEPER_Y
    pos = [x,y]
    workingPlay.setPosition(pos)

def pLeftDeepBack(team, workingPlay):
    '''position deep left back'''
    workingPlay.setSubRole(PBConstants.LEFT_DEEP_BACK)
    pos = [PBConstants.LEFT_DEEP_BACK_POS]
    workingPlay.setPosition(pos)

def pRightDeepBack(team, workingPlay):
    '''position deep right back'''
    workingPlay.setSubRole(PBConstants.RIGHT_DEEP_BACK)
    pos = [PBConstants.RIGHT_DEEP_BACK_POS]
    workingPlay.setPosition(pos)

#Goalie sub roles
def pGoalieNormal(team, workingPlay):
    '''normal goalie position'''
    workingPlay.setSubRole(PBConstants.GOALIE_NORMAL)
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
    pos = [x,y]
    workingPlay.setPosition(pos)
    #left for use by GoalieChaser
    return  position

def pGoalieChaser(team, workingPlay):
    '''goalie is being a chaser, presumably in/near goalbox not intended for
        pulling the goalie situations'''
    workingPlay.setSubRole(PBConstants.GOALIE_CHASER)
    position = pGoalieNormal(team, workingPlay)
    workingPlay.setPosition(position)

# Kickoff sub roles
def pKickoffSweeper(team, workingPlay):
    '''position kickoff sweeper'''
    workingPlay.setSubRole(PBConstants.KICKOFF_SWEEPER)
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_DEFENDER_0_POS
    else:
        pos = PBConstants.KICKOFF_DEFENDER_1_POS
    workingPlay.setPosition(pos)

def pKickoffStriker(team, workingPlay):
    '''position kickoff striker'''
    workingPlay.setSubRole(PBConstants.KICKOFF_STRIKER)
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_OFFENDER_0_POS
    else:
        pos = PBConstants.KICKOFF_OFFENDER_1_POS
    workingPlay.setPosition(pos)

def pKickoffPlaySweeper(team, workingPlay):
    '''position kickoff sweeper'''
    workingPlay.setSubRole(PBConstants.KICKOFF_SWEEPER)
    workingPlay.setPosition(PBConstants.KICKOFF_PLAY_DEFENDER_POS)

def pKickoffPlayStriker(team, workingPlay):
    '''position kickoff striker'''
    workingPlay.setSubRole(PBConstants.KICKOFF_STRIKER)
    workingPlay.setPosition(PBConstants.KICKOFF_PLAY_OFFENDER_POS)

# SubRoles for ready state
def pReadyChaser(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_CHASER)
    #TODO-have separate formations,roles(?),subroles
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        pos = PBConstants.READY_KICKOFF_NORMAL_CHASER_POS
    else:
        pos = PBConstants.READY_NON_KICKOFF_CHASER_POS
    workingPlay.setPosition(pos)

def pReadyOffender(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_OFFENDER)
    #TODO-have separate formations,roles(?),subroles
    kickOff = (team.brain.gameController.gc.kickOff== team.brain.my.teamColor)
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_OFFENDER_0_POS
        else:
            pos = PBConstants.READY_KICKOFF_OFFENDER_1_POS
    else:
        pos = PBConstants.READY_NON_KICKOFF_OFFENDER_POS
    workingPlay.setPosition(pos)

def pReadyDefender(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_DEFENDER)
    #TODO-have separate formations,roles(?),subroles
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_DEFENDER_0_POS
        else:
            pos = PBConstants.READY_KICKOFF_DEFENDER_1_POS
    else:
        pos = PBConstants.READY_NON_KICKOFF_DEFENDER_POS
    workingPlay.setPosition(pos)

def pGoalieReady(team, workingPlay):
    """
    Go to our home position during ready
    """
    #HACK- courtesy of Tucker
    workingPlay.setSubRole(PBConstants.GOALIE_NORMAL)
    position = [PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y]
    workingPlay.setPosition(position)
