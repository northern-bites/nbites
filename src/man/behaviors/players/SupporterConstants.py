import noggin_constants as NogginConstants
from objects import Location, RobotLocation
import RoleConstants as role

CHASER_DISTANCE = 60

def getSupporterPosition(player, role):
    if role.isLeftDefender(role):
        return leftDefender(player)
    elif role.isRightDefender(role):
        return rightDefender(player)
    elif role.isChaser(role):
        return chaser(player)
    elif role.isCherryPicker(role):
        return cherryPicker(player)

def leftDefender(player):
    """
    Defenders position between ball and goal.
    """
    if player.brain.ball.y < NogginConstants.MIDFIELD_Y:
        return RobotLocation((player.brain.ball.x + 
                              NogginConstants.BLUE_GOALBOX_RIGHT_X) * .5, 
                             (player.brain.ball.y + 
                              NogginConstants.GOALBOX_WIDTH * .25 +
                              NogginConstants.BLUE_GOALBOX_BOTTOM_Y) * .5, 
                             player.brain.ball.bearing_deg + player.brain.loc.h)
    return RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
                         (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
                         player.brain.ball.bearing_deg + player.brain.loc.h)

def rightDefender(player):
    if player.brain.ball.y >=  NogginConstants.MIDFIELD_Y:
        return RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)
                                            * .5, (player.brain.ball.y +
                                                   NogginConstants.GOALBOX_WIDTH * .75 +
                                                   NogginConstants.BLUE_GOALBOX_BOTTOM_Y)
                                            * .5, player.brain.ball.bearing_deg +
                                            player.brain.loc.h)
    return RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
                          (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
                          player.brain.ball.bearing_deg + player.brain.loc.h)

# TODO rotation matrix
def chaser(player):
    if (player.brain.ball.x >= player.brain.loc.x and
        player.brain.ball.y >= player.brain.loc.y):
        return RobotLocation(player.brain.ball.x - CHASER_DISTANCE,
                             player.brain.ball.y - CHASER_DISTANCE,
                             player.brain.ball.bearing_deg + player.brain.loc.h)
    elif (player.brain.ball.x >= player.brain.loc.x and
        player.brain.ball.y < player.brain.loc.y):
        return RobotLocation(player.brain.ball.x - CHASER_DISTANCE,
                             player.brain.ball.y + CHASER_DISTANCE,
                             player.brain.ball.bearing_deg + player.brain.loc.h)
    elif (player.brain.ball.x < player.brain.loc.x and
        player.brain.ball.y >= player.brain.loc.y):
        return RobotLocation(player.brain.ball.x + CHASER_DISTANCE,
                             player.brain.ball.y - CHASER_DISTANCE,
                             player.brain.ball.bearing_deg + player.brain.loc.h)
    elif (player.brain.ball.x < player.brain.loc.x and
        player.brain.ball.y < player.brain.loc.y):
        return RobotLocation(player.brain.ball.x + CHASER_DISTANCE,
                             player.brain.ball.y + CHASER_DISTANCE,
                             player.brain.ball.bearing_deg + player.brain.loc.h)

def cherryPicker(player):
    return RobotLocation(player.brain.loc.x,
                         player.brain.loc.y,
                         player.brain.ball.bearing_deg + player.brain.loc.h)
