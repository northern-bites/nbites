from . import PBConstants
from .. import NogginConstants
from ..util import MyMath
from ..typeDefs.Location import Location

#### Goalie sub roles ####

def pGoalieKickOff(team, workingPlay):
    """ keeps the goalie from moving on kickoff"""
    workingPlay.setSubRole(PBConstants.GOALIE_KICKOFF)
    h = team.brain.ball.heading
    pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

    if PBConstants.USE_FANCY_GOALIE:
        pos = team.fancyGoaliePosition()

    workingPlay.setPosition(pos)

def pGoalieCenter(team, workingPlay):
    """normal goalie position in the center of the goal"""
    workingPlay.setSubRole(PBConstants.GOALIE_CENTER)
    dest = Location(PBConstants.GOALIE_HOME_X,
                             PBConstants.GOALIE_HOME_Y)
    h = dest.headingTo(team.brain.ball)
    pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

    if PBConstants.USE_FANCY_GOALIE:
       pos = team.fancyGoaliePosition()

    workingPlay.setPosition(pos)

def pGoalieSave(team, workingPlay):
    """ goalie saving """
    workingPlay.setSubRole(PBConstants.GOALIE_SAVE)
    dest = Location(PBConstants.GOALIE_HOME_X,
                             PBConstants.GOALIE_HOME_Y)
    h = dest.headingTo(team.brain.ball)
    pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

    if PBConstants.USE_FANCY_GOALIE:
        pos = team.fancyGoaliePosition()

    workingPlay.setPosition(pos)

def pGoalieChaser(team, workingPlay):
    """goalie is being a chaser, presumably in/near goalbox not intended for
        pulling the goalie situations"""
    workingPlay.setSubRole(PBConstants.GOALIE_CHASER)
    dest = Location(PBConstants.GOALIE_HOME_X,
                             PBConstants.GOALIE_HOME_Y)
    h = dest.headingTo(team.brain.ball)
    pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

    if PBConstants.USE_FANCY_GOALIE:
       pos = team.fancyGoaliePosition()

    workingPlay.setPosition(pos)

#### Chaser sub roles ####

def pChaser(team, workingPlay):
    """ never called. now handled entirely in player behavior """
    pass


#### Defender sub roles ####

def pSweeper(team, workingPlay):
    """position sweeper"""
    workingPlay.setSubRole(PBConstants.SWEEPER)
    x = PBConstants.SWEEPER_X
    y = PBConstants.SWEEPER_Y
    y += PBConstants.SWEEPER_Y_OFFSET * \
        MyMath.sign(team.brain.ball.y-NogginConstants.CENTER_FIELD_Y)
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pRightDeepBack(team, workingPlay):
    """position deep left back"""
    workingPlay.setSubRole(PBConstants.RIGHT_DEEP_BACK)
    dest = Location(PBConstants.DEEP_BACK_X,
                             PBConstants.RIGHT_DEEP_BACK_Y)
    h = dest.headingTo(team.brain.ball)
    pos = (PBConstants.DEEP_BACK_X, PBConstants.RIGHT_DEEP_BACK_Y,h)
    workingPlay.setPosition(pos)

def pLeftDeepBack(team, workingPlay):
    """position deep right back"""
    workingPlay.setSubRole(PBConstants.LEFT_DEEP_BACK)
    dest = Location(PBConstants.DEEP_BACK_X,
                             PBConstants.LEFT_DEEP_BACK_Y)
    h = dest.headingTo(team.brain.ball)
    pos = (PBConstants.DEEP_BACK_X, PBConstants.LEFT_DEEP_BACK_Y,h)
    workingPlay.setPosition(pos)

def pCenterBack(team, workingPlay):
    """position center back"""
    workingPlay.setSubRole(PBConstants.CENTER_BACK)
    x,y = team.getPointBetweenBallAndGoal(team.brain.ball,
                                          PBConstants.DEFENDER_BALL_DIST)
    x = MyMath.clip(x,
                    PBConstants.SWEEPER_X,
                    PBConstants.STOPPER_X)
    y = MyMath.clip(y,
                    PBConstants.MIN_CENTER_BACK_Y,
                    PBConstants.MAX_CENTER_BACK_Y)
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pStopper(team, workingPlay):
    """position stopper"""
    workingPlay.setSubRole(PBConstants.STOPPER)
    x = PBConstants.STOPPER_X
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_STOPPER_Y,
                    PBConstants.MAX_STOPPER_Y)
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)


#### Offender sub roles ####

def pPicker(team, workingPlay):
    """position picker"""
    workingPlay.setSubRole(PBConstants.PICKER)
    x = PBConstants.PICKER_X
    y = PBConstants.PICKER_Y
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pLeftWing(team, workingPlay):
    """position left winger"""
    workingPlay.setSubRole(PBConstants.LEFT_WING)
    midpoint = (PBConstants.WING_MAX_X - PBConstants.WING_MIN_X)*.5
    scale = (PBConstants.WING_MAX_X - midpoint)/(PBConstants.PICKER_X_THRESH -
                                                 midpoint)
    x = -1*scale*(team.brain.ball.x - midpoint)
    y = PBConstants.LEFT_WING_Y
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pRightWing(team, workingPlay):
    """position right winger"""
    workingPlay.setSubRole(PBConstants.RIGHT_WING)
    midpoint = (PBConstants.WING_MAX_X - PBConstants.WING_MIN_X)*.5
    scale = (PBConstants.WING_MAX_X - midpoint)/(PBConstants.PICKER_X_THRESH -
                                                 midpoint)
    x = -1*scale*(team.brain.ball.x - midpoint)
    y = PBConstants.RIGHT_WING_Y
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pStriker(team, workingPlay):
    """position striker"""
    workingPlay.setSubRole(PBConstants.STRIKER)
    x = PBConstants.STRIKER_X
    if team.brain.ball.y < NogginConstants.CENTER_FIELD_Y:
        y = PBConstants.LEFT_STRIKER_Y
    else:
        y = PBConstants.RIGHT_STRIKER_Y
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)

def pForward(team, workingPlay):
    """position forward"""
    workingPlay.setSubRole(PBConstants.FORWARD)
    x = PBConstants.FORWARD_X
    if team.brain.ball.y < NogginConstants.CENTER_FIELD_Y:
        y = PBConstants.LEFT_FORWARD_Y
    else:
        y = PBConstants.RIGHT_FORWARD_Y
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)

#### Middie sub roles ####

def pDefensiveMiddie(team, workingPlay):
    workingPlay.setSubRole(PBConstants.DEFENSIVE_MIDDIE)
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_MIDDIE_Y,
                    PBConstants.MAX_MIDDIE_Y)
    dest = Location(PBConstants.DEFENSIVE_MIDDIE_X, y)
    h = dest.headingTo(team.brain.ball)

    pos = (PBConstants.DEFENSIVE_MIDDIE_X, y, h)
    workingPlay.setPosition(pos)

def pOffensiveMiddie(team, workingPlay):
    workingPlay.setSubRole(PBConstants.OFFENSIVE_MIDDIE)
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_MIDDIE_Y,
                    PBConstants.MAX_MIDDIE_Y)
    dest = Location(PBConstants.OFFENSIVE_MIDDIE_X, y)
    h = dest.headingTo(team.brain.ball)

    pos = (PBConstants.OFFENSIVE_MIDDIE_X, y, h)
    workingPlay.setPosition(pos)

def pDubDMiddie(team, workingPlay):
    """middie for when in dubD"""
    workingPlay.setSubRole(PBConstants.DUB_D_MIDDIE)
    y = MyMath.clip(team.brain.ball.y,
                    PBConstants.MIN_MIDDIE_Y,
                    PBConstants.MAX_MIDDIE_Y)
    x = PBConstants.OFFENSIVE_MIDDIE_X
    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x, y, h)
    workingPlay.setPosition(pos)

##### Kickoff sub roles ####

def pKickoffSweeper(team, workingPlay):
    """position kickoff sweeper"""
    workingPlay.setSubRole(PBConstants.KICKOFF_SWEEPER)
    x = PBConstants.KICKOFF_DEFENDER_X
    if team.kickoffFormation == 0:
        y = PBConstants.KICKOFF_DEFENDER_0_Y
    else:
        y = PBConstants.KICKOFF_DEFENDER_1_Y

    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pKickoffStriker(team, workingPlay):
    """position kickoff striker"""
    workingPlay.setSubRole(PBConstants.KICKOFF_STRIKER)
    x = PBConstants.KICKOFF_OFFENDER_X
    if team.kickoffFormation == 0:
        y = PBConstants.KICKOFF_OFFENDER_0_Y
    else:
        y = PBConstants.KICKOFF_OFFENDER_1_Y

    dest = Location(x, y)
    h = dest.headingTo(team.brain.ball)

    pos = (x,y,h)
    workingPlay.setPosition(pos)

#### SubRoles for ready state ####
def pReadyChaser(team, workingPlay):
    workingPlay.setSubRole(PBConstants.READY_CHASER)
    kickOff = team.brain.gameController.ownKickOff
    if kickOff:
        x = PBConstants.READY_KICKOFF_CHASER_X
        y = PBConstants.READY_KICKOFF_CHASER_Y
    else:
        x = PBConstants.READY_NON_KICKOFF_CHASER_X
        if team.kickoffFormation == 0:
            y = PBConstants.READY_NON_KICKOFF_CHASER_0_Y
        else:
            y = PBConstants.READY_NON_KICKOFF_CHASER_1_Y
    dest = Location(x, y)
    h = dest.headingTo(PBConstants.CENTER_FIELD)

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
        if team.kickoffFormation == 0:
            y = PBConstants.READY_NON_KICKOFF_OFFENDER_0_Y
        else:
            y = PBConstants.READY_NON_KICKOFF_OFFENDER_1_Y
    dest = Location(x, y)
    h = dest.headingTo(PBConstants.CENTER_FIELD)

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
        if team.kickoffFormation == 0:
            y = PBConstants.READY_NON_KICKOFF_DEFENDER_0_Y
        else:
            y = PBConstants.READY_NON_KICKOFF_DEFENDER_1_Y
    dest = Location(x, y)
    h = dest.headingTo(PBConstants.CENTER_FIELD)

    pos = (x,y,h)
    workingPlay.setPosition(pos)

def pReadyGoalie(team, workingPlay):
    """Go to our home position during ready"""
    workingPlay.setSubRole(PBConstants.READY_GOALIE)
    dest = Location(PBConstants.GOALIE_HOME_X,
                             PBConstants.GOALIE_HOME_Y)
    position = (PBConstants.GOALIE_HOME_X,
                PBConstants.GOALIE_HOME_Y,
                dest.headingTo(PBConstants.CENTER_FIELD))
    workingPlay.setPosition(position)
