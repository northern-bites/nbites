from .. import NogginConstants
import KickingConstants as constants

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
