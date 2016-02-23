import noggin_constants as NogginConstants
from objects import Location, RobotLocation
import RoleConstants as role
from math import hypot, atan2, degrees

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
    if role.isStriker(player.roleOfClaimer):
        return striker(player)

    if role.isDefender(player.roleOfClaimer):
        if player.brain.ball.y >= player.brain.loc.y:
            return RobotLocation(player.brain.ball.x + 250,
                                 player.brain.ball.y - CHASER_DISTANCE,
                                 player.brain.ball.bearing_deg + player.brain.loc.h)
        return RobotLocation(player.brain.ball.x + 250,
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
        positionsFilteredByInBounds = [position for position in supportPostitions if (inBounds(position) and notBlockingGoal(position))]
        if len(positionsFilteredByInBounds) > 0:
            return min(positionsFilteredByInBounds, key=distanceToPosition(player))
        
        # print "no in bounds position"
        return southEast

CHASER_DISTANCE = 100

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

def notBlockingGoal(position):
    return position.x < NogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X

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
    if not hasattr(findStrikerHome, 'upperHalf'):
        findStrikerHome.upperHalf = (ball.y - NogginConstants.CENTER_FIELD_Y) >= 0 

    # the buffer zone is twice this because its this distance on each side of midfield
    # the buffer keeps us from oscillating sides of the field when the ball is near the center line
    oscBuff = 50
    if findStrikerHome.upperHalf and (ball.y - NogginConstants.CENTER_FIELD_Y) < -1*oscBuff:
        findStrikerHome.upperHalf = False
    elif not findStrikerHome.upperHalf and (ball.y - NogginConstants.CENTER_FIELD_Y) > oscBuff:
        findStrikerHome.upperHalf = True

    goalCenter = Location(NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X, NogginConstants.MIDFIELD_Y)
    ballToGoal = Location(goalCenter.x - ball.x, goalCenter.y - ball.y) # vector

    # avoid divide by zeros
    if ballToGoal == Location(0, 0):
        ballToGoal = Location (1, 0)

    # the point at which we draw our normal vector from
    percentageToPivot = 0.8
    pivotPoint = Location(ball.x + ballToGoal.x*0.7, ball.y + ballToGoal.y*0.7)

    # two possible normal vectors. If ball.y is greater than midfield.y choose (dy, -dx)
    # else choose (-dy, dx)
    if findStrikerHome.upperHalf:
        normalVect = Location(ballToGoal.y, -1*ballToGoal.x)
    else:
        normalVect = Location(-1*ballToGoal.y, ballToGoal.x)

    # normalize the vector and make its magnitude to the desired value
    normalVectLength = 100
    normalizeMag = normalVectLength/normalVect.distTo(Location(0,0))
    normalVect.x *= normalizeMag
    normalVect.y *= normalizeMag

    strikerHome = RobotLocation(pivotPoint.x + normalVect.x , pivotPoint.y + normalVect.y, hh)

    # if for some reason you get placed off the field project back onto the field
    if strikerHome.x > NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X - 20:
        strikerHome.x = NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X - 20

    return strikerHome

# find the closest point on a line segment ((x1,y1),(x2,y2)) to a point (x3,y3)
def calculateHomePosition(player):
    """
    Calculate home position.
    """
    if player.brain.ball.vis.frames_off < 10:
        ball = player.brain.ball
        bearing = ball.bearing_deg
    elif player.brain.sharedBall.ball_on:
        ball = player.brain.sharedBall
        bearing = degrees(atan2(ball.y - player.brain.loc.y,
                          ball.x - player.brain.loc.x)) - player.brain.loc.h
    else:
        ball = None

    if ball != None and not (role.isDefender(player.role) and NogginConstants.FIXED_D_HOME):
        if role.isLeftDefender(player.role):
            home = findDefenderHome(True, ball, bearing + player.brain.loc.h)
        elif role.isRightDefender(player.role):
            home = findDefenderHome(False, ball, bearing + player.brain.loc.h)
        elif role.isStriker(player.role):
            home = findStrikerHome(ball, bearing + player.brain.loc.h)
        else:
            home = player.homePosition
    else:
        home = player.homePosition

    return home

def closePointOnSeg(x1, y1, x2, y2, x3, y3):
    dx = x2 - x1
    dy = y2 - y1
    d2 = dx*dx + dy*dy
    nx = ((x3-x1)*dx + (y3-y1)*dy) / d2
    nx = min(1, max(0, nx))
    return (dx*nx + x1, dy*nx + y1)