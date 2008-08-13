#import sensors.inst as sensors
import math

import man.motion
from man.motion import MotionConstants
from man.motion import SweetMoves

from .. import NogginConstants
from . import SuzhouConstants as PlayerConstants

def initial(player):
    return player.stay()


def spinFindBall(player):

    if player.shouldChase():
        return player.goNow('chase')

    if player.firstFrame():
        if player.brain.ball.lastVisionBearing < 0:
            # spin right
            player.brain.nav.spinRight()
            player.brain.motion.enqueue(SweetMoves.SPIN_RIGHT_REFIND_HEADS)

        else:
            # spin left
            player.brain.nav.spinLeft()
            player.brain.motion.enqueue(SweetMoves.SPIN_LEFT_REFIND_HEADS)

    return player.stay()
### Chaser States  ###

def chase(player):
    ''' Get to the CHOPPA!!! (or the ball)'''

    # should i look for the ball?
    if player.shouldRefind():
        return player.goNow('spinFindBall')

    elif player.shouldApproachBall():
        return player.goNow('approachBall')

    elif player.shouldDefend():
        return player.goNow('defend')

    if player.firstFrame():
        pass

    if player.brain.ball.on:
        # Track the ball
        player.brain.tracker.switchTo('tracking', player.brain.ball)
        destX,destY,destH = player.getShootingPos(PlayerConstants.CHASE_BALL_OFFSET)
        player.brain.nav.goTo(destX,destY,destH)
 
    else:
        player.brain.motion.enqueue(SweetMoves.SLOW_HEAD_SCAN)

    return player.stay()

def approachBall(player):
    ''' Get within kicking distance '''
    if not player.shouldApproachBall():
        if player.shouldChase():
            return player.goNow('chase')
        else:
            return player.goNow('spinFindBall')
    

    player.brain.nav.goTo(player.brain.ball.x,
                          player.brain.ball.y)

def confirmBall(player):
    ''' Bends over ball to check if it's still there '''
    pass

def kick(player):
    ''' kick da ball'''
    pass



def defend(player):
    pass

