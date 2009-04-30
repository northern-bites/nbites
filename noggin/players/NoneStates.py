
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants


def gamePlaying(player):
    ''' Overwritting the gameInital State'''
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
    return player.stay()

