from .. import NogginConstants
import KickingConstants as constants
from ..typeDefs.Location import Location

def getShotCloseAimPoint(player):
    return Location(NogginConstants.FIELD_WIDTH,
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

    elif my.dist( Location(NogginConstants.OPP_GOALBOX_RIGHT_X,
                           NogginConstants.OPP_GOALBOX_MIDDLE_Y )) > \
                           NogginConstants.FIELD_WIDTH / 3 :
        return constants.OBJECTIVE_CENTER

    elif my.x > NogginConstants.FIELD_WIDTH * 3/4 and \
            NogginConstants.FIELD_HEIGHT/4. < my.y < \
            NogginConstants.FIELD_HEIGHT * 3./4.:
        return constants.OBJECTIVE_SHOOT_CLOSE

    else :
        return constants.OBJECTIVE_SHOOT_FAR

