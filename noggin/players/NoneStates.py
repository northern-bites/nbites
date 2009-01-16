
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants

def nothing(player):
    """
    Do nothing
    """

    if player.counter == 10:
        return player.goLater('done')
    return player.stay()





def done(player):
    ''' Do nothing'''
    return player.stay()

