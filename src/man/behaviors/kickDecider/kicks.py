from .. import SweetMoves
from objects import Location, RobotLocation
import math
import PMotion_proto

class Kick(object):
    """
    Represents a kick. Includes sweet move (if there is one), sweet spot, 
    global heading representing what direction we mean to kick the ball in, 
    the intended target of the kick, and some indication of the range of a
    kick.
    """
    def __init__(self, _name, x=16, y=0, h=0,
                 dist=100, move=None, bhType=None):
        self.name = _name
        self.sweetMove = move
        self.bhKickType = bhType

        # TODO use destination objects
        self.setupX = x                  # sweet spot
        self.setupY = y                  # sweet spot
        self.setupH = h                  # global heading for kick allignment

        self.destinationX = 0            # where the kick is meant to go
        self.destinationY = 0            # where the kick is meant to go

        self.distance = dist             # range of the kick

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
        return self.name #+ " " + self.sweetMove + " " + self.bhKickType

    def __eq__(self, other):
        return self.name == other.name

    def __ne__(self, other):
        return not self == other

# Some standard kicks. x, y, and move should not be modified unless you change
# the sweetMove. Here heading indicates where one should setup to kick in a
# particular direction, but it will be modified later on to indicate where the
# robot needs to orbit to. Dist indicates how far the kick goes on average. The
# x deals with the distance in front of the robot while the y deals with the 
# distance to the side of the robot

# TODO there is no reason to specify a left and right kick with a flipped
#      sign in y component of sweetspot. The second kick should be procedurally
#      generated from the first. 

# Sweet move-based kicks
LEFT_SHORT_STRAIGHT_KICK =  Kick("L_Short_Straight", 
                                 x = 15, y =  3.6,
                                 dist = 300,
                                 move=SweetMoves.LEFT_SHORT_STRAIGHT_KICK)
RIGHT_SHORT_STRAIGHT_KICK = Kick("R_Short_Straight", 
                                 x = 15, y = -3.6,
                                 dist = 300,
                                 move=SweetMoves.RIGHT_SHORT_STRAIGHT_KICK)

LEFT_BIG_KICK =  Kick("L_Big_Straight", 
                      x = 14.8, y =  4.5,
                      dist = 500,
                      move=SweetMoves.LEFT_BIG_KICK)
RIGHT_BIG_KICK = Kick("R_Big_Straight", 
                      x = 14.8, y = -4.5,
                      dist = 500,
                      move=SweetMoves.RIGHT_BIG_KICK)

ZELLVARRO_LEFT_KICK = Kick("Zellvarro_Left_Kick",
                      x = 15.5, y = 3.95, 
                      dist = 300,
                      move=SweetMoves.ZELLVARRO_LEFT_KICK)

ZELLVARRO_RIGHT_KICK = Kick("Zellvarro_Left_Kick",
                      x = 15.5, y = -3.95,
                      dist = 300,
                      move=SweetMoves.ZELLVARRO_RIGHT_KICK)

LEFT_SIDE_KICK = Kick("Left_Side_Kick",
                      x = 15.5, y = 3.95, 
                      dist = 300,
                      move=SweetMoves.LEFT_SIDE_KICK)
RIGHT_SIDE_KICK = Kick("Right_Side_Kick",
                      x = 15.5, y = -3.95, 
                      dist = 300,
                      move=SweetMoves.RIGHT_SIDE_KICK)

LEFT_STRAIGHT_KICK =  Kick("L_Straight", x = 14.5, y = 4.0,
                           move=SweetMoves.LEFT_STRAIGHT_KICK)
RIGHT_STRAIGHT_KICK = Kick("R_Straight", x = 14.5, y = -4.0,
                           move=SweetMoves.RIGHT_STRAIGHT_KICK)

LEFT_SHORT_BACK_KICK =  Kick("L_Short_Back", x = 16.8, y =  4,
                             move=SweetMoves.LEFT_SHORT_BACK_KICK)
RIGHT_SHORT_BACK_KICK = Kick("R_Short_Back", x = 16.8, y = -4,
                             move=SweetMoves.RIGHT_SHORT_BACK_KICK)

LEFT_KICK = LEFT_SHORT_STRAIGHT_KICK
RIGHT_KICK = RIGHT_SHORT_STRAIGHT_KICK

# BH kick engine kicks
BH_LEFT_FORWARD_KICK =  Kick("BH_L_FORWARD", x = 13.6, y =  3.2, dist = 475, bhType = PMotion_proto.messages.Kick.kickForwardLeft)
BH_RIGHT_FORWARD_KICK =  Kick("BH_R_FORWARD", x = 13.6, y =  -3.2, dist = 475, bhType = PMotion_proto.messages.Kick.kickForwardRight)

# Motion kicks
M_LEFT_STRAIGHT =  Kick("M_Left_Straight", x = 13.3, y = 3., dist = 130)
M_RIGHT_STRAIGHT =  Kick("M_Right_Straight", x = 13.3, y = -3., dist = 130)

M_LEFT_CHIP_SHOT =  Kick("M_Left_Chip_Shot", x = 12.3, y = -.3, h = 60, dist = 130)
M_RIGHT_CHIP_SHOT =  Kick("M_Right_Chip_Shot", x = 12.3, y = .3, h = -60, dist = 130)

M_LEFT_SIDE = Kick("M_Left_Side", x = 14.5, y = -2.76, h = 80, dist = 110)
M_RIGHT_SIDE = Kick("M_Right_Side", x = 14.5, y = 2.76, h = -80, dist = 110)

# TODO automate/generalize
def chooseAlignedKickFromKick(player, kick):
    ballRelY = player.brain.ball.stat_rel_y
    if (kick == LEFT_STRAIGHT_KICK or
        kick == RIGHT_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_STRAIGHT_KICK
        else:
            return RIGHT_STRAIGHT_KICK
    if (kick == BH_LEFT_FORWARD_KICK or
        kick == BH_RIGHT_FORWARD_KICK):
        if ballRelY > 0:
            return BH_LEFT_FORWARD_KICK
        else:
            return BH_RIGHT_FORWARD_KICK
    elif (kick == M_LEFT_STRAIGHT or
        kick == M_RIGHT_STRAIGHT):
        if ballRelY > 0:
            return M_LEFT_STRAIGHT
        else:
            return M_RIGHT_STRAIGHT
    elif (kick == LEFT_SHORT_STRAIGHT_KICK or
          kick == RIGHT_SHORT_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_SHORT_STRAIGHT_KICK
        else:
            return RIGHT_SHORT_STRAIGHT_KICK
    elif (kick == ZELLVARRO_LEFT_KICK or
          kick == ZELLVARRO_RIGHT_KICK):
        if ballRelY > 0:
            return ZELLVARRO_LEFT_KICK
        else:
            return ZELLVARRO_RIGHT_KICK
    elif (kick == LEFT_BIG_KICK or
          kick == RIGHT_BIG_KICK):
        if ballRelY > 0:
            return LEFT_BIG_KICK
        else:
            return RIGHT_BIG_KICK
    elif (kick == LEFT_SHORT_BACK_KICK or
          kick == RIGHT_SHORT_BACK_KICK):
        if ballRelY > 0:
            return LEFT_SHORT_BACK_KICK
        else:
            return RIGHT_SHORT_BACK_KICK
    elif ("Side" in kick.name) or ("Chip" in kick.name):
        return kick
    elif (kick == ORBIT_KICK_POSITION):
        return kick
    else:
        raise NameError('Invalid kick passed to chooseAlignedKickFromKick')
