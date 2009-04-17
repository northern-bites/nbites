
from . import PBConstants
# SubRoles for ready state

def pReadyChaser(team):
    kickOff = team.brain.gameController.myTeam.kickOff
    if kickOff:
        pos = PBConstants.READY_KICKOFF_NORMAL_CHASER[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_CHASER[:2]
    return [PBConstants.READY_CHASER, pos]

def pReadyOffender(team):
    kickOff = team.brain.gameController.myTeam.kickOff
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_OFFENDER_0[:2]
        else:
            pos = PBConstants.READY_KICKOFF_OFFENDER_1[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_OFFENDER[:2]
    return [PBConstants.READY_OFFENDER, pos]

def pReadyDefender(team):
    kickOff = team.brain.gameController.myTeam.kickOff
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_DEFENDER_0[:2]
        else:
            pos = PBConstants.READY_KICKOFF_DEFENDER_1[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_DEFENDER[:2]
    return [PBConstants.READY_DEFENDER, pos]

def pReadyStopper(team):
    kickOff = team.brain.gameController.myTeam.kickOff
    if kickOff:
        pos = PBConstants.READY_KICKOFF_STOPPER[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_RIGHT_POSITION[:2]
    return [PBConstants.READY_DEFENDER, pos]

def pReadyMiddie(team):
    kickOff = team.brain.gameController.myTeam.kickOff
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_MIDDIE_0[:2]
        else:
            pos = PBConstants.READY_KICKOFF_MIDDIE_1[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_MIDDIE[:2]
    return [PBConstants.READY_MIDDIE, pos]

def pReadyTwoChase(team):
    kickOff = team.brain.gameController.myTeam.kickOff
    if kickOff:
        pos = PBConstants.READY_KICKOFF_NORMAL_CHASER[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_LEFT_POSITION[:2]
    return [PBConstants.READY_CHASER, pos]

# Game Playing SubRoles
def pLeftWing(team):
    '''position left winger'''
    x = team.brain.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.LEFT_WING_MIN_X, 
                        PBConstants.LEFT_WING_MAX_X)
    y = team.brain.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.WING_MIN_Y, PBConstants.WING_MAX_Y)
    return [PBConstants.LEFT_WING, [x,y]]

def pRightWing(team):
    '''position right winger'''
    x = team.brain.clip(team.brain.ball.x + PBConstants.WING_X_OFFSET,
                        PBConstants.RIGHT_WING_MIN_X, 
                        PBConstants.RIGHT_WING_MAX_X)
    y = team.brain.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.WING_MIN_Y, PBConstants.WING_MAX_Y)
    return [PBConstants.RIGHT_WING, [x,y]]

def pLeftStriker(team):
    '''position left striker'''
    x = PBConstants.STRIKER_LEFT_X
    y = PBConstants.STRIKER_Y
    return [PBConstants.LEFT_STRIKER, [x,y]]

def pRightStriker(team):
    '''position right striker'''
    x = PBConstants.STRIKER_RIGHT_X
    y = PBConstants.STRIKER_Y
    return [PBConstants.RIGHT_STRIKER, [x,y]]

def pCenterStriker(team):
    '''position attacking midfield'''
    # Make our x equal with the ball, unless it is too far to the 
    x = team.brain.clip(team.brain.ball.x, PBConstants.STRIKER_LEFT_X,  
                        PBConstants.STRIKER_RIGHT_X)
    y = PBConstants.STRIKER_Y - 100. 
    return [PBConstants.CENTER_STRIKER, [x,y]]

def pRightSidelineO(team):
    '''position right sideline offender '''
    x = PBConstants.RIGHT_SIDELINE_O_X
    y = team.brain.clip(team.brain.ball.y - PBConstants.SIDELINE_O_Y_OFFSET, 
                        PBConstants.SIDELINE_O_MIN_Y, 
                        PBConstants.SIDELINE_O_MAX_Y)
    return [PBConstants.RIGHT_SIDELINE_O, [x,y]]

def pLeftSidelineO(team):
    '''position left sideline offender '''
    x = PBConstants.LEFT_SIDELINE_O_X
    y = team.brain.clip(team.brain.ball.y - PBConstants.SIDELINE_O_Y_OFFSET, 
                        PBConstants.SIDELINE_O_MIN_Y, 
                        PBConstants.SIDELINE_O_MAX_Y)
    return [PBConstants.LEFT_SIDELINE_O, [x,y]]

# Midfield subRoles
def pForwardMidfield(team):
    '''position forward midfield'''
    # Make our x equal with the ball, unless it is too far to the side
    x = team.brain.clip(team.brain.ball.x, 
                        PBConstants.FORWARD_MID_LEFT_LIMIT, 
                        PBConstants.FORWARD_MID_RIGHT_LIMIT)
    y = PBConstants.FORWARD_MIDFIELD_Y
    return [PBConstants.FORWARD_MIDFIELD, [x,y]]

def pCenterOMidfield(team):
    '''position center offensive middy'''
    # Make our x equal with the ball, unless it is too far to the side
    x = team.brain.clip(team.brain.ball.x, PBConstants.CENTER_MID_LEFT_LIMIT, 
                        PBConstants.CENTER_MID_RIGHT_LIMIT)
    y = PBConstants.OFFENSIVE_CENTER_MIDFIELD_Y
    return [PBConstants.CENTER_O_MIDFIELD, [x,y]]

def pDefensiveMidfield(team):
    '''position defensive midfield'''
    x = team.brain.clip(team.brain.ball.x, PBConstants.CENTER_MID_LEFT_LIMIT, 
                        PBConstants.CENTER_MID_RIGHT_LIMIT)
    y = PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y
    return [PBConstants.DEFENSIVE_MIDFIELD, [x,y]]

def pRightOMiddie(team):
    x = PBConstants.CENTER_MID_RIGHT_LIMIT
    y = team.brain.ball.y
    return [PBConstants.RIGHT_O_MIDDIE, [x,y]]

def pLeftOMiddie(team):
    x = PBConstants.CENTER_MID_LEFT_LIMIT
    y = team.brain.ball.y
    return [PBConstants.LEFT_O_MIDDIE, [x,y]]

def pRightDMiddie(team):
    x = PBConstants.CENTER_MID_RIGHT_LIMIT + 30.
    y = PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y - 50.
    return [PBConstants.RIGHT_D_MIDDIE, [x,y]]

def pLeftDMiddie(team):
    x = PBConstants.CENTER_MID_LEFT_LIMIT - 30.
    y = PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y - 50.
    return [PBConstants.LEFT_D_MIDDIE, [x,y]]

# Defender sub roles
def pStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    y = team.brain.clip(y, PBConstants.SWEEPER_Y,
                        PBConstants.STOPPER_MAX_Y)
    return [PBConstants.STOPPER, [x,y]]

def pDeepStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    y = team.brain.clip(y, PBConstants.SWEEPER_Y,PBConstants.DEEP_STOPPER_Y)
    return [PBConstants.DEEP_STOPPER, [x,y]]

def pSweeper(team):
    '''position sweeper'''
    x = PBConstants.SWEEPER_X
    y = PBConstants.SWEEPER_Y
    return [PBConstants.SWEEPER, [x,y]]

def pLeftDeepBack(team):
    '''position deep left back'''
    return [PBConstants.LEFT_DEEP_BACK]

def pRightDeepBack(team):
    '''position deep right back'''
    return [PBConstants.RIGHT_DEEP_BACK]

# Kickoff sub roles
def pKickoffSweeper(team):
    '''position kickoff sweeper'''
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_DEFENDER_0[:2]
    else:
        pos = PBConstants.KICKOFF_DEFENDER_1[:2]
    return [PBConstants.KICKOFF_SWEEPER, pos]

def pKickoffStriker(team):
    '''position kickoff striker'''
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_OFFENDER_0[:2]
    else:
        pos = PBConstants.KICKOFF_OFFENDER_1[:2]
    return [PBConstants.KICKOFF_STRIKER, pos]

def pKickoffMiddie(team):
    '''position kickoff middie'''
    if team.kickoffFormation == 0:
        pos = PBConstants.KICKOFF_MIDDIE_0[:2]
    else:
        pos = PBConstants.KICKOFF_MIDDIE_1[:2]
    return [PBConstants.KICKOFF_MIDDIE, pos]

def pKickoffPlaySweeper(team):
    '''position kickoff sweeper'''
    pos = PBConstants.KICKOFF_PLAY_DEFENDER[:2]
    return [PBConstants.KICKOFF_SWEEPER, pos]

def pKickoffPlayStriker(team):
    '''position kickoff striker'''
    pos = PBConstants.KICKOFF_PLAY_OFFENDER[:2]
    return [PBConstants.KICKOFF_STRIKER, pos]

def pKickoffPlayMiddie(team):
    '''position kickoff middie'''
    pos = PBConstants.KICKOFF_PLAY_MIDDIE[:2]
    return [PBConstants.KICKOFF_MIDDIE, pos]
