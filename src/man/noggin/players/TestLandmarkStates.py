# Test class for landmark tracking localization system

import man.motion.SweetMoves as SweetMoves
from .. import NogginConstants
from man.motion import MotionConstants

def gameInitial(player):
    player.gainsOn()
    player.executeMove(SweetMoves.INITIAL_POS)

    # we don't want to auto-stand after falling
    player.brain.fallController.executeStandup = False
    player.brain.fallController.enabled = False

    #debugging
    print player.brain.sensors.motionAngles[MotionConstants.HeadPitch],player.brain.sensors.motionAngles[MotionConstants.HeadYaw]

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
    return player.stay()

def gameSet(player):
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.RIGHT_SIDE_KICK)
        player.executeMove(SweetMoves.RIGHT_SIDE_KICK)

    if player.stateTime > SweetMoves.getMoveTime(SweetMoves.RIGHT_SIDE_KICK):
        player.brain.tracker.afterKickScan(0)
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
    return player.stay()
