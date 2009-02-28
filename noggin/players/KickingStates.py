#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants

def gameInitial(player):
    return player.goLater('standup')

def standup(player):
    if player.firstFrame():
        walkCommand = motion.WalkCommand(x=0,y=0,theta=0)
        player.motion.setNextWalkCommand(walkCommand)

    if player.counter == 1:
        return player.goLater('kickStraight')
    return player.stay()

def kickStraight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.KICK_STRAIGHT)

    if player.counter == 50:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()
