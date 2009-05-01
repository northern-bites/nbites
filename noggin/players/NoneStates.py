
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants


def gamePlaying(player):
    ''' Overwritting the gameInital State'''
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
        player.standup()

    if player.counter < 100:
        player.brain.sensors.saveFrame()

    return player.stay()
