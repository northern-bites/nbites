from .. import NogginConstants
import KickingConstants as constants
from ..util import MyMath
from math import cos, sin

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

def getShotCloseAimPoint(player):
    return (NogginConstants.FIELD_WIDTH,
            NogginConstants.MIDFIELD_Y)

def getShotFarAimPoint(player):
    if player.brain.my.y < NogginConstants.MIDFIELD_Y:
        return constants.SHOOT_AT_LEFT_AIM_POINT
    else :
        return constants.SHOOT_AT_RIGHT_AIM_POINT

def getKickObjective(player):
    """
    Figure out what to do with the ball
    """
    kickDecider = player.kickDecider
    avgOppGoalDist = 0.0

    my = player.brain.my

    if not player.hasKickedOffKick:
        return constants.OBJECTIVE_KICKOFF

    if my.x < NogginConstants.FIELD_WIDTH / 2:
        return constants.OBJECTIVE_CLEAR

    elif MyMath.dist(my.x, my.y,
                   NogginConstants.OPP_GOALBOX_RIGHT_X,
                   NogginConstants.OPP_GOALBOX_MIDDLE_Y ) > \
                   NogginConstants.FIELD_WIDTH / 3 :
                   return constants.OBJECTIVE_CENTER
    elif my.x > NogginConstants.FIELD_WIDTH * 3/4 and \
            NogginConstants.FIELD_HEIGHT/4. < my.y < \
            NogginConstants.FIELD_HEIGHT * 3./4.:
        return constants.OBJECTIVE_SHOOT_CLOSE
    else :
        return constants.OBJECTIVE_SHOOT_FAR

def getNextOrbitPos(player):
    relX = constants.ORBIT_OFFSET_DIST * cos(constants.ORBIT_STEP_ANGLE)
    relY = constants.ORBIT_OFFSET_DIST * sin(constants.ORBIT_STEP_ANGLE)
    relTheta = constants.ORBIT_STEP_ANGLE

    return relX, relY, relTheta
