import noggin_constants as NogginConstants
from objects import Location, RobotLocation
import RoleConstants

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
                          NogginConstants.GOALBOX_WIDTH * .25 +
                          NogginConstants.BLUE_GOALBOX_BOTTOM_Y) * .5, 
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
                          NogginConstants.GOALBOX_WIDTH * .75 +
                          NogginConstants.BLUE_GOALBOX_BOTTOM_Y) * .5, 
                         player.brain.ball.bearing_deg + player.brain.loc.h)
    # return RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
    #                       (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
    #                       player.brain.ball.bearing_deg + player.brain.loc.h)

def chaser(player):
    """
    Chasers position off to one side of the ball about a meter away.
    """
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
    else:
        return RobotLocation(player.brain.ball.x + CHASER_DISTANCE,
                             player.brain.ball.y + CHASER_DISTANCE,
                             player.brain.ball.bearing_deg + player.brain.loc.h)

CHASER_DISTANCE = 60

def cherryPicker(player):
    """
    Cherry pickers stay where they are but look to the ball.
    """
    return RobotLocation(player.brain.loc.x,
                         player.brain.loc.y,
                         player.brain.ball.bearing_deg + player.brain.loc.h)
