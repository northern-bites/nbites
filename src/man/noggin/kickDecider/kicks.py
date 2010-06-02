from man.motion import SweetMoves
from KickingConstants import *

class Kick:
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

# Some standard kicks. x,y and move should not be modified, h will be
LEFT_SIDE_KICK = Kick(y=2, move=SweetMoves.LEFT_SIDE_KICK)
RIGHT_SIDE_KICK = Kick(y=-2, move=SweetMoves.RIGHT_SIDE_KICK)

LEFT_SHORT_KICK = Kick(y=5, move=SweetMoves.SHORT_QUICK_LEFT_KICK)
RIGHT_SHORT_KICK = Kick(y=-5, move=SweetMoves.SHORT_QUICK_RIGHT_KICK)

LEFT_BIG_KICK = Kick(y=5, move=SweetMoves.LEFT_BIG_KICK)
RIGHT_BIG_KICK = Kick(y=-5, move=SweetMoves.RIGHT_BIG_KICK)

LEFT_FAR_KICK = Kick(y=5, move=SweetMoves.LEFT_FAR_KICK)
RIGHT_FAR_KICK = Kick(y=-5, move=SweetMoves.RIGHT_FAR_KICK)
