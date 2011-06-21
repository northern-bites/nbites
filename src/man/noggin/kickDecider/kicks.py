from man.motion import SweetMoves
from KickingConstants import DEFAULT_KICK_X_OFFSET

class Kick(object):
    """
    stores everything we need to know for a given kick
    """
    def __init__(self, x=DEFAULT_KICK_X_OFFSET, y=0, h=0, move=None):
        self.x_offset = x
        self.y_offset = y
        self.heading = h
        self.sweetMove = move

    def getPosition(self):
        """returns all position info as a tuple"""
        return (self.x_offset, self.y_offset, self.heading)

    def __str__(self):
        return ("x_offset: %g y_offset: %g heading: %g" %
                (self.x_offset, self.y_offset, self.heading))
# Some standard kicks. x,y and move should not be modified, h will be
LEFT_SIDE_KICK = Kick(x = 12, y = 5, move=SweetMoves.DREW_LEFT_SIDE_KICK)
RIGHT_SIDE_KICK = Kick(x = 12, y = -5, move=SweetMoves.DREW_RIGHT_SIDE_KICK)

RIGHT_DYNAMIC_STRAIGHT_KICK = Kick(y = -8, move=SweetMoves.DREW_KICK)
LEFT_DYNAMIC_STRAIGHT_KICK = Kick(y = 8, move=SweetMoves.DREW_KICK)

CENTER_KICK_POSITION = Kick(x = DEFAULT_KICK_X_OFFSET + 5, y = 0)

LEFT_LONG_BACK_KICK = Kick(x = 12.5, y = 6.5, move=SweetMoves.LEFT_LONG_BACK_KICK)
RIGHT_LONG_BACK_KICK = Kick(x = 12.5, y = -6.5, move=SweetMoves.RIGHT_LONG_BACK_KICK)

LEFT_SHORT_BACK_KICK = Kick(x = 12.5, y = 6.5, move=SweetMoves.LEFT_SHORT_BACK_KICK)
RIGHT_SHORT_BACK_KICK = Kick(x = 12.5, y = -6.5, move=SweetMoves.RIGHT_SHORT_BACK_KICK)

