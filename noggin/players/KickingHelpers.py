from .. import NogginConstants
import KickingConstants as constants
from ..util import MyMath

def inCenterOfField(player):
    return NogginConstants.FIELD_HEIGHT *2/3 > player.brain.my.y > \
        NogginConstants.FIELD_HEIGHT / 3

def inTopOfField(player):
    return NogginConstants.FIELD_HEIGHT*2/3 < player.brain.my.y

def inBottomOfField(player):
    return NogginConstants.FIELD_HEIGHT/3 > player.brain.my.y

def isFacingSideline(player):
    h = player.brain.my.h

    return (inTopOfField(player) and
            constants.FACING_SIDELINE_ANGLE > h >
            180.0 - constants.FACING_SIDELINE_ANGLE ) or \
            (inBottomOfField(player) and
             -constants.FACING_SIDELINE_ANGLE > h >
             -(180 - constants.FACING_SIDELINE_ANGLE) )

def getShotAimPoint(player):
    if player.brain.my.y < NogginConstants.MIDFIELD_Y:
        return constants.SHOT_FROM_LEFT_AIM_POINT
    else :
        return constants.SHOT_FROM_RIGHT_AIM_POINT

def getKickObjective(player):
    """
    Figure out what to do with the ball
    """
    kickDecider = player.kickDecider
    avgOppGoalDist = 0.0

    my = player.brain.my

    if my.x < NogginConstants.FIELD_WIDTH / 2:
        return constants.OBJECTIVE_CLEAR

    elif MyMath.dist(my.x, my.y,
                   NogginConstants.OPP_GOALBOX_RIGHT_X,
                   NogginConstants.OPP_GOALBOX_MIDDLE_Y ) > \
                   NogginConstants.FIELD_WIDTH / 3 :
                   return constants.OBJECTIVE_CENTER
    else :
        return constants.OBJECTIVE_SHOOT

