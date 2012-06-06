import time
from objects import RelRobotLocation
from ..navigator import Navigator
from ..kickDecider import kicks
from man.motion import SweetMoves

import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants

def gameInitial(player):
    if player.firstFrame():
        player.brain.nav.stop()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False

    elif (player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown
          and not player.motion.isBodyActive()):
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()

    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBall()

    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.stopWalking
        player.executeMove(SweetMoves.SIT_POS)
        player.penalizeHeads()

    return player.stay()

def fallen(player):
    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.trackBall()

    # Came from penalized? Fell recently? Become a chaser!
    if player.lastDiffState == 'gamePenalized':
        print "become a chaser!" #/* ** */ ADD CHASER CODE

    return player.goLater('gaurd')

def gaurd(player):
    # Stop any nav motion and track the ball
    if player.firstFrame():
        player.brain.nav.stop()
        player.brain.tracker.trackBall()

    # First, sanity check on visible goal distance
    yglpVis = player.brain.yglp.vis
    ygrpVis = player.brain.ygrp.vis
    if (yglpVis.on and yglpVis.dist < 200) or \
            (ygrpVis.on and ygrpVis.dist < 200):
        # Goals are close: become a full fledged chaser!
        print "become a chaser!" #/* ** */ ADD CHASER CODE

    # Is the ball nearby? Omni approach it.
    if player.brain.ball.vis.framesOn > 4 and \
            player.brain.ball.vis.dist < 150:
        return player.goLater('omniApproach')

    # No luck. Sit tight.
    return player.stay()

def omniApproach(player):
    # If close to ball, switch to omniPosition
    if player.brain.ball.vis.dist < 50:
        return player.goLater('omniPositionForKick')

    # If lost the ball, stop and chill.
    if transitions.shouldFindBall(player):
        return player.goLater('gaurd')

    # Approach with all speed!
    player.brain.nav.chaseBall()
    return player.stay()

def omniPositionForKick(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.inKickingState = False

        # decide kick in util method
        # only enque once (for obstacle avoidance)
        player.brain.nav.goTo(getKickPosition(player),
                              Navigator.CLOSE_ENOUGH,
                              Navigator.FULL_SPEED,
                              Navigator.ADAPTIVE)
    else:
        # update kicking position
        player.brain.nav.updateDest(getKickPosition(player))

    # Did we lose the ball?
    if transitions.shouldFindBall(player):
        return player.goLater('gaurd')

    # At position?
    position = getKickPosition(player)
    if (0 < position.relX < constants.BALL_X_OFFSET and
            position.relY < constants.BALL_Y_OFFSET and
            position.relH < constants.GOOD_ENOUGH_H):
        print "kicking!" # KICK IT!!!$@%&!!
        player.brain.nav.stop()
        player.executeMove(SweetMoves.LEFT_BIG_KICK) #check this @!
    else:
        print "orbiting!" # ORBIT

    return player.stay()

def getKickPosition(player):
    kick = kicks.SHORT_QUICK_LEFT_KICK #check this @!
    ballLoc = player.brain.ball.loc
    (kick_x, kick_y, kick_heading) = kick.getPosition()
    dest = RelRobotLocation(ballLoc.relX - kick_x - 3,
                            ballLoc.relY - kick_y,
                            0)
    return dest
