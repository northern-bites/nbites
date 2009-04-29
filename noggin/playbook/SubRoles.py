
from . import PBConstants
from .. import NogginConstants
from ..util import MyMath
# SubRoles for ready state

def pReadyChaser(team):
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        pos = PBConstants.READY_KICKOFF_NORMAL_CHASER[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_CHASER[:2]
    return [PBConstants.READY_CHASER, pos]

def pReadyOffender(team):
    kickOff = (team.brain.gameController.gc.kickOff== team.brain.my.teamColor)
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_OFFENDER_0[:2]
        else:
            pos = PBConstants.READY_KICKOFF_OFFENDER_1[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_OFFENDER[:2]
    return [PBConstants.READY_OFFENDER, pos]

def pReadyDefender(team):
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        if team.kickoffFormation == 0:
            pos = PBConstants.READY_KICKOFF_DEFENDER_0[:2]
        else:
            pos = PBConstants.READY_KICKOFF_DEFENDER_1[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_DEFENDER[:2]
    return [PBConstants.READY_DEFENDER, pos]

def pReadyStopper(team):
    kickOff = (team.brain.gameController.gc.kickOff == team.brain.my.teamColor)
    if kickOff:
        pos = PBConstants.READY_KICKOFF_STOPPER[:2]
    else:
        pos = PBConstants.READY_NON_KICKOFF_DEFENDER[:2]
    return [PBConstants.READY_DEFENDER, pos]

# Game Playing SubRoles
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

# Defender sub roles
def pStopper(team):
    '''position stopper'''
    x,y = team.getPointBetweenBallAndGoal(PBConstants.DEFENDER_BALL_DIST)
    x = MyMath.clip(x, PBConstants.SWEEPER_X,
                        PBConstants.STOPPER_MAX_X)
    return [PBConstants.STOPPER, [x,y]]

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

def pKickoffPlaySweeper(team):
    '''position kickoff sweeper'''
    pos = PBConstants.KICKOFF_PLAY_DEFENDER[:2]
    return [PBConstants.KICKOFF_SWEEPER, pos]

def pKickoffPlayStriker(team):
    '''position kickoff striker'''
    pos = PBConstants.KICKOFF_PLAY_OFFENDER[:2]
    return [PBConstants.KICKOFF_STRIKER, pos]
