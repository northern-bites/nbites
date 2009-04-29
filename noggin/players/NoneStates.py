
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants


def gameInitial(player):
    ''' Overwritting the gameInital State'''
    if player.firstFrame():
        player.brain.leds.off("FaceLeds")
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.brain.leds.on("FaceLeds")
    return player.stay()

def nothing(player):
    """
    Do nothing
    """

    if player.counter == 10:
        return player.goLater('done')
    return player.stay()

def done (player):
    ''' Do nothing'''
    return player.stay()

