#
# This file maintains the core functionality of a soccer player, and is included
# in the SoccerFSA by default
# When the robot is in initial, it sits down
# When the robot is in ready, it stands up and locPans
# When the robot is in set, it fixates on the ball
# When the robot is in playing, it does the same as in set
# When the robot is in finish, it does the same as in initial
#

import man.motion as motion
import man.motion.SweetMoves as SweetMoves

def gamePenalized(player):
    if player.firstFrame():
        player.stopWalking()
        player.setHeads(0.,0.)
        player.motion.stopBodyMoves()
        player.brain.tracker.stopHeadMoves()
    return player.stay()

def gameInitial(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame():
        player.gainsOn()
        player.motion.stopBodyMoves()
        player.setHeads(0.,0.)
        player.brain.tracker.stopHeadMoves()
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()

def gameReady(player):
    """
    Stand up, and pan for localization
    """
    if player.firstFrame():
        #TODO: this is not the best way to stand up
        dummyWalk = motion.WalkCommand(x=0,y=0,theta=0)
        player.motion.setNextWalkCommand(dummyWalk)
    player.brain.tracker.switchTo('locPans')
    return player.stay()

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.brain.ball.on:
        player.brain.tracker.trackBall()
    elif player.brain.ball.framesOff > 3:
        player.brain.tracker.switchTo('scanBall')
    return player.stay()

def gamePlaying(player):
    """
    This method must be overriden by intersted SoccerPlayers
    """
    if player.firstFrame():
        player.motion.stopBodyMoves()
        player.brain.tracker.stopHeadMoves()
    return player.stay()

###
# Standup states
###
def fallen(player):
    """
    Stops the player when the robot has fallen
    """
    player.brain.nav.switchTo('stopped')
    return player.stay()

def gameFinished(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame():
        player.stopWalking() # Ensure navigator stops correctly
        player.setHeads(0.,0.)
        player.brain.tracker.stopHeadMoves()
        player.GAME_FINISHED_satDown = False
        return player.stay()

    # Sit down once we've finished walking
    if player.brain.nav.isStopped() and not player.GAME_FINISHED_satDown:
        player.GAME_FINISHED_satDown = True
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if not player.motion.isBodyActive() and  player.GAME_FINISHED_satDown:
        player.gainsOff()
    return player.stay()
