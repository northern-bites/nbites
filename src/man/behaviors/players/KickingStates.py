"""
Here we house all of the state methods used for kicking the ball
"""

from . import ChaseBallTransitions as transitions
from . import ChaseBallConstants as constants
from ..util import *
from ..kickDecider import kicks
from ..navigator import Navigator as nav
from objects import Location, RelRobotLocation

# TODO refactor, super state?

@superState('gameControllerResponder')
@ifSwitchLater(transitions.ballMoved, 'approachBall') # TODO this doesn't work
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
def executeMotionKick(player):
    """
    Do a motion kick.
    """
    ball = player.brain.ball
    executeMotionKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                                  ball.rel_y - player.kick.setupY,
                                                  0)

    if player.firstFrame():
        player.brain.nav.destinationWalkTo(executeMotionKick.kickPose,
                                           nav.CAREFUL_SPEED,
                                           player.kick)
    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        player.brain.nav.updateDestinationWalkDest(executeMotionKick.kickPose)
    elif player.kickedOut and not player.brain.ball.vis.on:
        player.kickedOut = False
        return player.goNow('spinSearch')

    # TODO not ideal at all!
    if player.counter > 40:
        player.inKickingState = False
        return player.goNow('afterKick')

    return player.stay()

@superState('gameControllerResponder')
@ifSwitchLater(transitions.ballMoved, 'approachBall') # TODO this doesn't work
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
def executeSweetKick(player):
    """
    Kick the ball using sweet move. But don't do it. They suck!
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        executeSweetKick.sweetMove = player.kick.sweetMove
        return player.stay()

    # TODO consider lowering this
    if player.counter == 30:
        player.executeMove(executeSweetKick.sweetMove)
        return player.stay()

    # TODO not ideal at all!
    if player.counter > 40 and player.brain.nav.isStopped():
        return player.goNow('afterKick')

    return player.stay()

executeSweetKick.sweetMove = None

@superState('gameControllerResponder')
@ifSwitchLater(transitions.ballMoved, 'approachBall') # TODO this doesn't work
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
def executeBHKick(player):
    """
    Kick the ball using BH kick engine.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        return player.stay()

    if player.counter > 30:
        player.brain.nav.callKickEngine(player.kick.bhKickType)

    if player.counter > 130:
        return player.goNow('afterKick')

    return player.stay()

@superState('gameControllerResponder')
def afterKick(player):
    """
    State to follow up after a kick.
    """
    if player.firstFrame():
        # player.stand()        # stand up right, ready to walk
        player.brain.tracker.afterKickScan(player.kick.name)
        return player.stay()

    if player.penaltyKicking:
        return player.stay()

    elif player.passBack:
        return player.goNow('passToFieldCross')

    elif transitions.shouldKickAgain(player):
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        if player.motionKick:
            player.motionKick = False
            return player.goNow('spinToBall')
        else:        
            return player.goNow('positionForKick')
    
    elif player.kickedOut:
        player.kickedOut = False
        return player.goNow('spinSearch')

    elif transitions.shouldChaseBall(player):
        return player.goLater('approachBall')
    elif player.stateTime > 2:
        destinationOfKick = Location(player.kick.destinationX,
                                     player.kick.destinationY)
        player.brain.nav.goTo(destinationOfKick, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = True, pb = False)

    if player.stateTime > 4:
        return player.goLater('approachBall')

    return player.stay()

@superState('gameControllerResponder')
def spinAfterBackKick(player):
    """
    State to spin to the ball after we kick it behind us.
    """
    # TODO This is essentially spinFindBall... maybe we go straight to that.

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goLater('approachBall')

    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        kick = player.brain.kickDecider.getKick()
        if (kick is kicks.LEFT_LONG_BACK_KICK or
            kick is kicks.LEFT_SHORT_BACK_KICK):
            player.setWalk(0, 0, constants.FIND_BALL_SPIN_SPEED)
        else:
            player.setWalk(0, 0, -1*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.spinPan()

    return player.stay()
