from man.motion import SweetMoves
from KickingConstants import DEFAULT_KICK_X_OFFSET

class Kick(object):
    """
    stores everything we need to know for a given kick
    """
    def __init__(self, _name, x=DEFAULT_KICK_X_OFFSET, y=0, h=0, move=None, dest = None):
        self.name = _name
        self.x_offset = x
        self.y_offset = y
        self.heading = h           # set manually.
        self.sweetMove = move
        self.dest = dest           # set manually.

    def getPosition(self):
        """returns all position info as a tuple"""
        return (self.x_offset, self.y_offset, self.heading)

    def __str__(self):
        return ("%s: \n x_offset: %g y_offset: %g heading: %g ==> dest: %s" %
                (self.name, self.x_offset, self.y_offset, self.heading, self.dest))

# Some standard kicks. x,y and move should not be modified unless you change
# the sweetMove.  Heading will be modified when the kick is constructed.
LEFT_SIDE_KICK =  Kick("L_Side", x = 12, y =  5, move=SweetMoves.DREW_LEFT_SIDE_KICK)
RIGHT_SIDE_KICK = Kick("R_Side", x = 12, y = -5, move=SweetMoves.DREW_RIGHT_SIDE_KICK)

RIGHT_DYNAMIC_STRAIGHT_KICK = Kick("R_D_Straight", y = -8, move=SweetMoves.DREW_KICK)
LEFT_DYNAMIC_STRAIGHT_KICK =  Kick("L_D_Straight", y =  8, move=SweetMoves.DREW_KICK)

# TODO ! make a null kick here.
# used when we haven't decided what kick to do yet
ORBIT_KICK_POSITION = Kick("NULL_KICK", x = DEFAULT_KICK_X_OFFSET + 5, y = -5)

LEFT_LONG_BACK_KICK =  Kick("L_LongBack", x = 12.5, y =  6.5, move=SweetMoves.LEFT_LONG_BACK_KICK)
RIGHT_LONG_BACK_KICK = Kick("R_LongBack", x = 12.5, y = -6.5, move=SweetMoves.RIGHT_LONG_BACK_KICK)

LEFT_SHORT_BACK_KICK =  Kick("L_ShortBack", x = 12.5, y =  6.5, move=SweetMoves.LEFT_SHORT_BACK_KICK)
RIGHT_SHORT_BACK_KICK = Kick("R_ShortBack", x = 12.5, y = -6.5, move=SweetMoves.RIGHT_SHORT_BACK_KICK)

SHORT_QUICK_LEFT_KICK =  Kick("L_ShortStraight", x = DEFAULT_KICK_X_OFFSET, y = 5,
                              move = SweetMoves.SHORT_QUICK_LEFT_KICK)
SHORT_QUICK_RIGHT_KICK =  Kick("R_ShortStraight", x = DEFAULT_KICK_X_OFFSET, y = -5,
                               move = SweetMoves.SHORT_QUICK_RIGHT_KICK)

#*****************#
# Range Constants #
#*****************#
"""
Add your own range constants if you have a new kick, or update them
if you change the kick. These are maximum ranges in centimeters.
"""

# TODO! make these accurate.
SIDE_KICK_RANGE = 150.

DYNAMIC_STRAIGHT_KICK_RANGE = 300.

LONG_BACK_KICK_RANGE = 500.

SHORT_BACK_KICK_RANGE = 200.

SHORT_QUICK_KICK_RANGE = 200.
