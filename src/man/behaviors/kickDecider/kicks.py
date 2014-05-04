from .. import SweetMoves
from objects import Location, RobotLocation
import math
from KickingConstants import DEFAULT_KICK_X_OFFSET

class Kick(object):
    """
    stores everything we need to know for a given kick
    """
    def __init__(self, _name, x=DEFAULT_KICK_X_OFFSET, y=0, h=0,
                 move=None, maxDist=300):
        self.name = _name
        self.sweetMove = move
        self.maxDist = maxDist           # upper limit of the range of the kick

        self.setupX = x
        self.setupY = y
        self.setupH = h

        self.destinationX = 0          # set by kickDecider
        self.destinationY = 0          # set by kickDecider

    def getPosition(self):
        return (self.setupX, self.setupY, self.setupH)

    def isBackKick(self):
        return (self is LEFT_LONG_BACK_KICK or
                self is RIGHT_LONG_BACK_KICK or
                self is LEFT_SHORT_BACK_KICK or
                self is RIGHT_SHORT_BACK_KICK)

    def isStraightKick(self):
        return (self is LEFT_STRAIGHT_KICK or
                self is RIGHT_STRAIGHT_KICK or
                self is LEFT_SHORT_STRAIGHT_KICK or
                self is RIGHT_SHORT_STRAIGHT_KICK)

    def __str__(self):
        return self.name

    def __eq__(self, other):
        return self.name == other.name

    def __ne__(self, other):
        return not self == other


# Some standard kicks. x,y and move should not be modified unless you change
# the sweetMove. Here heading indicates where one should setup to kick in a
# particular direction, but it will be modified later on to indicate where the
# robot needs to orbit to.
LEFT_SIDE_KICK =  Kick("L_Side", x = 17.5, y =  2,
                       h = 90,
                       move=SweetMoves.GOOGZ_LEFT_SIDE_KICK)
RIGHT_SIDE_KICK = Kick("R_Side", x = 17.5, y = -2,
                       h = -90,
                       move=SweetMoves.GOOGZ_RIGHT_SIDE_KICK)
# Not used 04-19-13
LEFT_SHORT_SIDE_KICK =  Kick("L_Short_Side", x = 15.5, y = -.5,
                             h = 90,
                             move=SweetMoves.LEFT_SHORT_SIDE_KICK)
RIGHT_SHORT_SIDE_KICK = Kick("R_Short_Side", x = 15.5, y = .5,
                             h = -90,
                             move=SweetMoves.RIGHT_SHORT_SIDE_KICK)

# Not used 05-02-14
LEFT_STRAIGHT_KICK =  Kick("L_Straight", x = 16.5, y = 5.0,
                           move=SweetMoves.LEFT_STRAIGHT_KICK)
RIGHT_STRAIGHT_KICK = Kick("R_Straight", x = 16.5, y = -5.0,
                           move=SweetMoves.RIGHT_STRAIGHT_KICK)

LEFT_SHORT_STRAIGHT_KICK =  Kick("L_Short_Straight", x = 18.2, y =  4.4, 
                                 move=SweetMoves.LEFT_SHORT_STRAIGHT_KICK)
RIGHT_SHORT_STRAIGHT_KICK = Kick("R_Short_Straight", x = 18.2, y = -4.4,
                                 move=SweetMoves.RIGHT_SHORT_STRAIGHT_KICK)

LEFT_BIG_KICK =  Kick("L_Big_Straight", x = 15.5, y =  4.5,
                      move=SweetMoves.LEFT_BIG_KICK)
RIGHT_BIG_KICK = Kick("R_Big_Straight", x = 15.5, y = -4.5,
                      move=SweetMoves.RIGHT_BIG_KICK)

# used when we haven't decided what kick to do yet
ORBIT_KICK_POSITION = Kick("NULL_KICK", x = 12, y = 0)

LEFT_LONG_BACK_KICK =  Kick("L_Long_Back", x = 15.7, y =  4.1,
                            move=SweetMoves.LEFT_LONG_BACK_KICK)
RIGHT_LONG_BACK_KICK = Kick("R_Long_Back", x = 15.7, y = -4.1,
                            move=SweetMoves.RIGHT_LONG_BACK_KICK)

# Not used 06-09-13
LEFT_SHORT_BACK_KICK =  Kick("L_Short_Back", x = 16.8, y =  4,
                             move=SweetMoves.LEFT_SHORT_BACK_KICK)
RIGHT_SHORT_BACK_KICK = Kick("R_Short_Back", x = 16.8, y = -4,
                             move=SweetMoves.RIGHT_SHORT_BACK_KICK)

# Dribble kick just tells player to run thru the ball, no actual sweet move
LEFT_DRIBBLE =  Kick("L_Dribble", x = 0, y = 3.0)
RIGHT_DRIBBLE =  Kick("R_Dribble", x = 0, y = -3.0)

# Motion kicks
M_LEFT_SIDE =  Kick("M_Left_Side", x = 11, y = 1.5)
M_RIGHT_SIDE =  Kick("M_Right_Side", x = 11, y = -1.5)

M_LEFT_STRAIGHT =  Kick("M_Left_Straight", x = 15, y = 5.0)
M_RIGHT_STRAIGHT =  Kick("M_Right_Straight", x = 15, y = -5.0)

def chooseAlignedKickFromKick(player, kick):
    ballRelY = player.brain.ball.stat_rel_y
    if (kick == LEFT_STRAIGHT_KICK or
        kick == RIGHT_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_STRAIGHT_KICK
        else:
            return RIGHT_STRAIGHT_KICK
    elif (kick == M_LEFT_STRAIGHT or
        kick == M_RIGHT_STRAIGHT):
        if ballRelY > 0:
            return M_LEFT_STRAIGHT
        else:
            return M_RIGHT_STRAIGHT
    elif (kick == LEFT_DRIBBLE or
        kick == RIGHT_DRIBBLE):
        if ballRelY > 0:
            return LEFT_DRIBBLE
        else:
            return RIGHT_DRIBBLE
    elif (kick == LEFT_SHORT_STRAIGHT_KICK or
          kick == RIGHT_SHORT_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_SHORT_STRAIGHT_KICK
        else:
            return RIGHT_SHORT_STRAIGHT_KICK
    elif (kick == LEFT_BIG_KICK or
          kick == RIGHT_BIG_KICK):
        if ballRelY > 0:
            return LEFT_BIG_KICK
        else:
            return RIGHT_BIG_KICK
    elif (kick == LEFT_LONG_BACK_KICK or
          kick == RIGHT_LONG_BACK_KICK):
        if ballRelY > 0:
            return LEFT_LONG_BACK_KICK
        else:
            return RIGHT_LONG_BACK_KICK
    elif (kick == LEFT_SHORT_BACK_KICK or
          kick == RIGHT_SHORT_BACK_KICK):
        if ballRelY > 0:
            return LEFT_SHORT_BACK_KICK
        else:
            return RIGHT_SHORT_BACK_KICK
    elif "Side" in kick.name:
        return kick
    elif (kick == ORBIT_KICK_POSITION):
        return kick
    else:
        raise NameError('Invalid kick passed to chooseAlignedKickFromKick')
