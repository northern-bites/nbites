
from . import PBConstants
# SubRoles for ready state

def pReadyChaser(team):
    kickOff = team.brain.game.myTeam.kickOff
    if kickOff:
        team.moveMyCharge(PBConstants.READY_KICKOFF_NORMAL_CHASER)
    else:
        team.moveMyCharge(PBConstants.READY_NON_KICKOFF_CHASER)
    team.removeBallRepulsor()
    return [PBConstants.READY_CHASER]

def pReadyOffender(team):
    kickOff = team.brain.game.myTeam.kickOff
    if kickOff:
        if team.kickoffFormation == 0:
            team.moveMyCharge(PBConstants.READY_KICKOFF_OFFENDER_0)
        else:
            team.moveMyCharge(PBConstants.READY_KICKOFF_OFFENDER_1)
    else:
        team.moveMyCharge(PBConstants.READY_NON_KICKOFF_OFFENDER)
    team.removeBallRepulsor()
    return [PBConstants.READY_OFFENDER]

def pReadyDefender(team):
    kickOff = team.brain.game.myTeam.kickOff
    if kickOff:
        if team.kickoffFormation == 0:
            team.moveMyCharge(PBConstants.READY_KICKOFF_DEFENDER_0)
        else:
            team.moveMyCharge(PBConstants.READY_KICKOFF_DEFENDER_1)
    else:
        team.moveMyCharge(PBConstants.READY_NON_KICKOFF_DEFENDER)
    team.removeBallRepulsor()
    return [PBConstants.READY_DEFENDER]

def pReadyStopper(team):
    kickOff = team.brain.game.myTeam.kickOff
    if kickOff:
        team.moveMyCharge(PBConstants.READY_KICKOFF_STOPPER)
    else:
        team.moveMyCharge(PBConstants.READY_NON_KICKOFF_RIGHT_POSITION)
    team.removeBallRepulsor()
    return [PBConstants.READY_DEFENDER]

def pReadyMiddie(team):
    kickOff = team.brain.game.myTeam.kickOff
    if kickOff:
        if team.kickoffFormation == 0:
            team.moveMyCharge(PBConstants.READY_KICKOFF_MIDDIE_0)
        else:
            team.moveMyCharge(PBConstants.READY_KICKOFF_MIDDIE_1)
    else:
        team.moveMyCharge(PBConstants.READY_NON_KICKOFF_MIDDIE)
    team.removeBallRepulsor()
    return [PBConstants.READY_MIDDIE]

def pReadyTwoChase(team):
    kickOff = team.brain.game.myTeam.kickOff
    if kickOff:
        team.moveMyCharge(PBConstants.READY_KICKOFF_NORMAL_CHASER)
    else:
        team.moveMyCharge(PBConstants.READY_NON_KICKOFF_LEFT_POSITION)
    team.removeBallRepulsor()
    return [PBConstants.READY_CHASER]

# Game Playing SubRoles
def pLeftWing(team):
    '''position left winger'''
    x = team.brain.clip(team.brain.ball.x - PBConstants.WING_X_OFFSET,
                        PBConstants.LEFT_WING_MIN_X, 
                        PBConstants.LEFT_WING_MAX_X)
    y = team.brain.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.WING_MIN_Y, PBConstants.WING_MAX_Y)
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.LEFT_WING]

def pRightWing(team):
    '''position right winger'''
    x = team.brain.clip(team.brain.ball.x + PBConstants.WING_X_OFFSET,
                        PBConstants.RIGHT_WING_MIN_X, 
                        PBConstants.RIGHT_WING_MAX_X)
    y = team.brain.clip(team.brain.ball.y - PBConstants.WING_Y_OFFSET,
                        PBConstants.WING_MIN_Y, PBConstants.WING_MAX_Y)
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.RIGHT_WING]

def pLeftStriker(team):
    '''position left striker'''
    x = PBConstants.STRIKER_LEFT_X
    y = PBConstants.STRIKER_Y
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.LEFT_STRIKER]

def pRightStriker(team):
    '''position right striker'''
    x = PBConstants.STRIKER_RIGHT_X
    y = PBConstants.STRIKER_Y
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.RIGHT_STRIKER]

def pCenterStriker(team):
    '''position attacking midfield'''
    # Make our x equal with the ball, unless it is too far to the 
    x = team.brain.clip(team.brain.ball.x, PBConstants.STRIKER_LEFT_X,  
                        PBConstants.STRIKER_RIGHT_X)

    y = PBConstants.STRIKER_Y - 100. 
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.CENTER_STRIKER]

def pRightSidelineO(team):
    '''position right sideline offender '''
    x = PBConstants.RIGHT_SIDELINE_O_X
    y = team.brain.clip(team.brain.ball.y - PBConstants.SIDELINE_O_Y_OFFSET, 
                        PBConstants.SIDELINE_O_MIN_Y, 
                        PBConstants.SIDELINE_O_MAX_Y)
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.RIGHT_SIDELINE_O]

def pLeftSidelineO(team):
    '''position left sideline offender '''
    x = PBConstants.LEFT_SIDELINE_O_X
    y = team.brain.clip(team.brain.ball.y - PBConstants.SIDELINE_O_Y_OFFSET, 
                        PBConstants.SIDELINE_O_MIN_Y, 
                        PBConstants.SIDELINE_O_MAX_Y)
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.LEFT_SIDELINE_O]

# Midfield subRoles
def pForwardMidfield(team):
    '''position forward midfield'''
    # Make our x equal with the ball, unless it is too far to the side
    x = team.brain.clip(team.brain.ball.x, 
                        PBConstants.FORWARD_MID_LEFT_LIMIT, 
                        PBConstants.FORWARD_MID_RIGHT_LIMIT)
    y = PBConstants.FORWARD_MIDFIELD_Y
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.FORWARD_MIDFIELD]

def pCenterOMidfield(team):
    '''position center offensive middy'''
    # Make our x equal with the ball, unless it is too far to the side
    x = team.brain.clip(team.brain.ball.x, PBConstants.CENTER_MID_LEFT_LIMIT, 
                        PBConstants.CENTER_MID_RIGHT_LIMIT)
    y = PBConstants.OFFENSIVE_CENTER_MIDFIELD_Y
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.CENTER_O_MIDFIELD]

def pDefensiveMidfield(team):
    '''position defensive midfield'''
    x = team.brain.clip(team.brain.ball.x, PBConstants.CENTER_MID_LEFT_LIMIT, 
                        PBConstants.CENTER_MID_RIGHT_LIMIT)
    y = PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.DEFENSIVE_MIDFIELD]

def pRightOMiddie(team):
    x = PBConstants.CENTER_MID_RIGHT_LIMIT
    y = team.brain.ball.y
    team.moveMyCharge((x,y))
    return [PBConstants.RIGHT_O_MIDDIE]

def pLeftOMiddie(team):
    x = PBConstants.CENTER_MID_LEFT_LIMIT
    y = team.brain.ball.y
    team.moveMyCharge((x,y))
    return [PBConstants.LEFT_O_MIDDIE]

def pRightDMiddie(team):
    x = PBConstants.CENTER_MID_RIGHT_LIMIT + 30.
    y = PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y - 50.
    team.moveMyCharge((x,y))
    return [PBConstants.RIGHT_D_MIDDIE]

def pLeftDMiddie(team):
    x = PBConstants.CENTER_MID_LEFT_LIMIT - 30.
    y = PBConstants.DEFENSIVE_CENTER_MIDFIELD_Y - 50.
    team.moveMyCharge((x,y))
    return [PBConstants.LEFT_D_MIDDIE]

# Defender sub roles
def pStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    y = team.brain.clip(y, PBConstants.SWEEPER_Y,
                        PBConstants.STOPPER_MAX_Y)
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.STOPPER]

def pDeepStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    y = team.brain.clip(y, PBConstants.SWEEPER_Y,PBConstants.DEEP_STOPPER_Y)
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.DEEP_STOPPER]

def pSweeper(team):
    '''position sweeper'''
    x = PBConstants.SWEEPER_X
    y = PBConstants.SWEEPER_Y
    team.moveMyCharge((x,y))
    team.moveBallRepulsor()
    return [PBConstants.SWEEPER]

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
        team.moveMyCharge(PBConstants.KICKOFF_DEFENDER_0)
    else:
        team.moveMyCharge(PBConstants.KICKOFF_DEFENDER_1)
    team.moveBallRepulsor()
    return [PBConstants.KICKOFF_SWEEPER]

def pKickoffStriker(team):
    '''position kickoff striker'''
    if team.kickoffFormation == 0:
        team.moveMyCharge(PBConstants.KICKOFF_OFFENDER_0)
    else:
        team.moveMyCharge(PBConstants.KICKOFF_OFFENDER_1)
    team.moveBallRepulsor()
    return [PBConstants.KICKOFF_STRIKER]

def pKickoffMiddie(team):
    '''position kickoff middie'''
    if team.kickoffFormation == 0:
        team.moveMyCharge(PBConstants.KICKOFF_MIDDIE_0)
    else:
        team.moveMyCharge(PBConstants.KICKOFF_MIDDIE_1)
    team.moveBallRepulsor()
    return [PBConstants.KICKOFF_MIDDIE]

def pKickoffPlaySweeper(team):
    '''position kickoff sweeper'''
    team.moveMyCharge(PBConstants.KICKOFF_PLAY_DEFENDER)
    team.moveBallRepulsor()
    return [PBConstants.KICKOFF_SWEEPER]

def pKickoffPlayStriker(team):
    '''position kickoff striker'''
    team.moveMyCharge(PBConstants.KICKOFF_PLAY_OFFENDER)
    team.moveBallRepulsor()
    return [PBConstants.KICKOFF_STRIKER]

def pKickoffPlayMiddie(team):
    '''position kickoff middie'''
    team.moveMyCharge(PBConstants.KICKOFF_PLAY_MIDDIE)
    team.moveBallRepulsor()
    return [PBConstants.KICKOFF_MIDDIE]
