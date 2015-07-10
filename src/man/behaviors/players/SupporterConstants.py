import noggin_constants as NogginConstants
from objects import Location, RobotLocation
import RoleConstants as role
from math import hypot

def getSupporterPosition(player, r):
    """
    Returns a position to stand at to support teammate who is chasing the ball.
    Used in positionAsSupporter in PlayOffBallStates.
    """
    if role.isLeftDefender(r):
        return leftDefender(player)
    elif role.isRightDefender(r):
        return rightDefender(player)
    elif role.isFirstChaser(r):
        return chaser(player)
    elif role.isStriker(r):
        return striker(player)
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
    if role.isDefender(player.roleOfClaimer):
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
def striker(player):
    return findStrikerHome(player.brain.ball, player.brain.ball.bearing_deg + player.brain.loc.h)

def inBounds(position):
    return (position.x >= NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X and 
            position.x <= NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X and
            position.y >= NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y and
            position.y <= NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y)

def distanceToPosition(player):
    def distToPositionHelper(position):
        return hypot(player.brain.loc.x - position.x, player.brain.loc.y - position.y)
    return distToPositionHelper

def findDefenderHome(left, ball, hh):
    bY = ball.y
    if ball.x < NogginConstants.MIDFIELD_X:
        if left:
            home = closePointOnSeg(role.evenDefenderBack.x, role.evenDefenderBack.y,
                                    role.evenDefenderForward.x, role.evenDefenderForward.y,
                                    ball.x, ball.y)
            return RobotLocation(home[0], home[1], hh)
        else:
            home = closePointOnSeg(role.oddDefenderBack.x, role.oddDefenderBack.y,
                                    role.oddDefenderForward.x, role.oddDefenderForward.y,
                                    ball.x, ball.y)
            return RobotLocation(home[0], home[1], hh)

    else:
        if left:
            if bY >= role.evenDefenderForward.y:
                return RobotLocation(role.evenDefenderForward.x,
                                    role.evenDefenderForward.y,
                                    hh)
            elif bY <= role.oddDefenderBack.y:
                return RobotLocation(role.evenDefenderBack.x,
                                    role.evenDefenderBack.y,
                                    hh)
            else:
                # ball is between the two defenders:
                # linear relationship between ball and where defender stands on their line
                
                xDist = role.evenDefenderForward.x - role.evenDefenderBack.x
                yDist = role.evenDefenderForward.y - role.evenDefenderBack.y

                t = ((bY - role.oddDefenderForward.y) / 
                    (role.evenDefenderForward.y - role.oddDefenderForward.y))

                hx = role.evenDefenderBack.x + t*xDist
                hy = role.evenDefenderBack.y + t*yDist

                return RobotLocation(hx, hy, hh)
        else:
            if bY <= role.oddDefenderForward.y:
                return RobotLocation(role.oddDefenderForward.x,
                                    role.oddDefenderForward.y,
                                    hh)
            elif bY >= role.evenDefenderBack.y:
                return RobotLocation(role.oddDefenderBack.x,
                                    role.oddDefenderBack.y,
                                    hh)
            else:
                # ball is between the two defenders:
                # linear relationship between ball and where defender stands on their line
                
                xDist = role.oddDefenderForward.x - role.oddDefenderBack.x
                yDist = role.oddDefenderForward.y - role.oddDefenderBack.y

                t = ((bY - role.evenDefenderForward.y) / 
                    (role.oddDefenderForward.y - role.evenDefenderForward.y))

                hx = role.oddDefenderBack.x + t*xDist
                hy = role.oddDefenderBack.y + t*yDist

                return RobotLocation(hx, hy, hh)

def findStrikerHome(ball, hh):
    # find closest point on triangle to the ball and flip it over
    # the y axis so we are positioned watching the ball from other side
    if ball.x < NogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X:
        if ball.y > NogginConstants.MIDFIELD_Y:
            home = closePointOnSeg(role.strikerTop.x,
                                    role.strikerTop.y,
                                    role.strikerForward.x,
                                    role.strikerForward.y,
                                    ball.x, ball.y)
            return RobotLocation(home[0], NogginConstants.FIELD_HEIGHT - home[1], hh)
        else:
            home = closePointOnSeg(role.strikerBottom.x,
                                    role.strikerBottom.y,
                                    role.strikerForward.x,
                                    role.strikerForward.y,
                                    ball.x, ball.y)
            return RobotLocation(home[0], NogginConstants.FIELD_HEIGHT - home[1], hh)
    else:
        home = closePointOnSeg(role.strikerBottom.x,
                                role.strikerBottom.y,
                                role.strikerTop.x,
                                role.strikerTop.y,
                                ball.x, ball.y)
        return RobotLocation(home[0], NogginConstants.FIELD_HEIGHT - home[1], hh)

# find the closest point on a line segment ((x1,y1),(x2,y2)) to a point (x3,y3)
def closePointOnSeg(x1, y1, x2, y2, x3, y3):
    dx = x2 - x1
    dy = y2 - y1
    d2 = dx*dx + dy*dy
    nx = ((x3-x1)*dx + (y3-y1)*dy) / d2
    nx = min(1, max(0, nx))
    return (dx*nx + x1, dy*nx + y1)