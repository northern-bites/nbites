from .. import SweetMoves
from KickingConstants import DEFAULT_KICK_X_OFFSET

class Kick(object):
    """
    stores everything we need to know for a given kick
    """
    def __init__(self, _name, x=DEFAULT_KICK_X_OFFSET, y=0, h=0,
                 move=None, dest = None):
        self.name = _name
        self.x_offset = x
        self.y_offset = y
        self.heading = h           # set manually.
        self.sweetMove = move
        self.dest = dest           # set manually.

    def getPosition(self):
        """returns all position info as a tuple"""
        return (self.x_offset, self.y_offset, self.heading)

    def isBackKick(self):
        return (self is LEFT_LONG_BACK_KICK or
                self is RIGHT_LONG_BACK_KICK or
                self is LEFT_SHORT_BACK_KICK or
                self is RIGHT_SHORT_BACK_KICK)

    def __str__(self):
        #return ("%s: \n x_offset: %g y_offset: %g heading: %g ==> dest: %s" %
         #       (self.name, self.x_offset, self.y_offset, self.heading, self.dest))
        return self.name

    def __eq__(self, other):
        return self.name == other.name

    def __ne__(self, other):
        return not self == other

# Some standard kicks. x,y and move should not be modified unless you change
# the sweetMove.  Heading will be modified when the kick is constructed.

#The sweet spot needs testing once the approach ball is fixed.
#From extensve testing in pKicker and with printlines of the balls
#relX and relY, this appears accurate TODO TODO TODO

#new- sweet spots done
LEFT_SIDE_KICK =  Kick("L_Side", x = 15.5, y =  -.5,
                       move=SweetMoves.LEFT_SIDE_KICK)
RIGHT_SIDE_KICK = Kick("R_Side", x = 15.5, y = .5,
                       move=SweetMoves.RIGHT_SIDE_KICK)
#new- sweet spots sorta done
LEFT_SHORT_SIDE_KICK =  Kick("L_Short_Side", x = 15.5, y = -.5,
                             move=SweetMoves.LEFT_SHORT_SIDE_KICK)
RIGHT_SHORT_SIDE_KICK = Kick("R_Short_Side", x = 15.5, y = .5,
                             move=SweetMoves.RIGHT_SHORT_SIDE_KICK)
#new- sweet spots done
LEFT_STRAIGHT_KICK =  Kick("L_Straight", x = 16.5, y = 5.0,
                           move=SweetMoves.LEFT_STRAIGHT_KICK)
RIGHT_STRAIGHT_KICK = Kick("R_Straight", x = 16.5, y = -5.0,
                           move=SweetMoves.RIGHT_STRAIGHT_KICK)

#new- sweet spots done
LEFT_SHORT_STRAIGHT_KICK =  Kick("L_Short_Straight", x = 16.5, y =  5.3,
                                 move=SweetMoves.LEFT_SHORT_STRAIGHT_KICK)
RIGHT_SHORT_STRAIGHT_KICK = Kick("R_Short_Straight", x = 16.5, y = -5.3,
                                 move=SweetMoves.RIGHT_SHORT_STRAIGHT_KICK)
#new
LEFT_QUICK_STRAIGHT_KICK =  Kick("L_Quick_Straight", x = 15, y = 5.5,
                                 move=SweetMoves.LEFT_QUICK_STRAIGHT_KICK)
RIGHT_QUICK_STRAIGHT_KICK = Kick("R_Quick_Straight", x = 15, y = -5.5,
                                 move=SweetMoves.RIGHT_QUICK_STRAIGHT_KICK)

#usopen12 hack
LEFT_BIG_KICK =  Kick("L_Big_Straight", x = 11, y =  3,
                      move=SweetMoves.LEFT_BIG_KICK)
RIGHT_BIG_KICK = Kick("R_Big_Straight", x = 11, y = -3,
                      move=SweetMoves.RIGHT_BIG_KICK)

# used when we haven't decided what kick to do yet
ORBIT_KICK_POSITION = Kick("NULL_KICK", x = 12, y = 0)

LEFT_LONG_BACK_KICK =  Kick("L_Long_Back", x = 16.5, y =  4,
                            move=SweetMoves.LEFT_LONG_BACK_KICK)
RIGHT_LONG_BACK_KICK = Kick("R_Long_Back", x = 16.5, y = -4,
                            move=SweetMoves.RIGHT_LONG_BACK_KICK)

LEFT_SHORT_BACK_KICK =  Kick("L_Short_Back", x = 16.5, y =  4,
                             move=SweetMoves.LEFT_SHORT_BACK_KICK)
RIGHT_SHORT_BACK_KICK = Kick("R_Short_Back", x = 16.5, y = -4,
                             move=SweetMoves.RIGHT_SHORT_BACK_KICK)

def chooseAlignedKickFromKick(player, kick):
    ballRelY = player.brain.ball.rel_y
    if (kick == LEFT_STRAIGHT_KICK or
        kick == RIGHT_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_STRAIGHT_KICK
        else:
            return RIGHT_STRAIGHT_KICK
    elif (kick == LEFT_SHORT_STRAIGHT_KICK or
          kick == RIGHT_SHORT_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_SHORT_STRAIGHT_KICK
        else:
            return RIGHT_SHORT_STRAIGHT_KICK
    elif (kick == LEFT_QUICK_STRAIGHT_KICK or
          kick == RIGHT_QUICK_STRAIGHT_KICK):
        if ballRelY > 0:
            return LEFT_QUICK_STRAIGHT_KICK
        else:
            return RIGHT_QUICK_STRAIGHT_KICK
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
