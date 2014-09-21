import noggin_constants as NogginConstants
from objects import Location, RobotLocation
import RoleConstants
from math import hypot

def getSupporterPosition(player, role):
    """
    Returns a position to stand at to support teammate who is chasing the ball.
    Used in positionAsSupporter in PlayOffBallStates.
    """
    if RoleConstants.isLeftDefender(role):
        return leftDefender(player)
    elif RoleConstants.isRightDefender(role):
        return rightDefender(player)
    elif RoleConstants.isChaser(role):
        return chaser(player)
    else: # cherry picker
        return cherryPicker(player)

def leftDefender(player):
    """
    Defenders position between ball and goal.
    """
    # if player.brain.ball.y < NogginConstants.MIDFIELD_Y:
    return RobotLocation((player.brain.ball.x + 
                          NogginConstants.BLUE_GOALBOX_RIGHT_X) * .5, 
                         (player.brain.ball.y +
                          (NogginConstants.GOALBOX_WIDTH * .75 +
                          NogginConstants.BLUE_GOALBOX_BOTTOM_Y)) * .5, 
                         player.brain.ball.bearing_deg + player.brain.loc.h)
    # return RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
    #                      (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
    #                      player.brain.ball.bearing_deg + player.brain.loc.h)

def rightDefender(player):
    """
    Defenders position between ball and goal.
    """
    # if player.brain.ball.y >=  NogginConstants.MIDFIELD_Y:
    return RobotLocation((player.brain.ball.x + 
                          NogginConstants.BLUE_GOALBOX_RIGHT_X) * .5, 
                         (player.brain.ball.y + 
                          (NogginConstants.GOALBOX_WIDTH * .25 +
                          NogginConstants.BLUE_GOALBOX_BOTTOM_Y)) * .5, 
                         player.brain.ball.bearing_deg + player.brain.loc.h)
    # return RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
    #                       (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
    #                       player.brain.ball.bearing_deg + player.brain.loc.h)

def chaser(player):
    """
    Chasers position off to one side of the ball about a meter away if a chaser
    or cherry picker is calling them off. Chasers position further away up field if
    a defender is calling them off.
    """
    if RoleConstants.isDefender(player.roleOfClaimer):
        if player.brain.ball.y >= player.brain.loc.y:
            return RobotLocation(player.brain.ball.x + 200,
                                 player.brain.ball.y - CHASER_DISTANCE,
                                 player.brain.ball.bearing_deg + player.brain.loc.h)
        return RobotLocation(player.brain.ball.x + 200,
                             player.brain.ball.y + CHASER_DISTANCE,
                             player.brain.ball.bearing_deg + player.brain.loc.h)
    else:
        southEast = RobotLocation(player.brain.ball.x - CHASER_DISTANCE,
                                  player.brain.ball.y - CHASER_DISTANCE,
                                  player.brain.ball.bearing_deg + player.brain.loc.h)
        southWest = RobotLocation(player.brain.ball.x - CHASER_DISTANCE,
                                 player.brain.ball.y + CHASER_DISTANCE,
                                 player.brain.ball.bearing_deg + player.brain.loc.h)
        northEast = RobotLocation(player.brain.ball.x + CHASER_DISTANCE,
                                 player.brain.ball.y - CHASER_DISTANCE,
                                 player.brain.ball.bearing_deg + player.brain.loc.h)
        northWest = RobotLocation(player.brain.ball.x + CHASER_DISTANCE,
                                 player.brain.ball.y + CHASER_DISTANCE,
                                 player.brain.ball.bearing_deg + player.brain.loc.h)

        supportPostitions = [southEast,southWest,northEast,northWest]
        positionsFilteredByInBounds = [position for position in supportPostitions if inBounds(position)]
        if len(positionsFilteredByInBounds) > 0:
            return min(positionsFilteredByInBounds, key=distanceToPosition(player))
        
        # print "no in bounds position"
        return southEast

CHASER_DISTANCE = 60

def cherryPicker(player):
    """
    Cherry pickers stay where they are but look to the ball.
    """
    return RobotLocation(player.brain.loc.x,
                         player.brain.loc.y,
                         player.brain.ball.bearing_deg + player.brain.loc.h)

def inBounds(position):
    return (position.x >= NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X and 
            position.x <= NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X and
            position.y >= NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y and
            position.y <= NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y)

def distanceToPosition(player):
    def distToPositionHelper(position):
        return hypot(player.brain.loc.x - position.x, player.brain.loc.y - position.y)
    return distToPositionHelper