"""
The fall controller, implemented as a hierarchical FSA.
The top level in player FSA.
"""

from .. import SweetMoves
from SharedTransitions import sameStateForNSeconds
import noggin_constants as nogginConstants

@defaultState('gameInitial')
@ifSwitch(currentlyFalling, 'falling', False)
def notFalling(player):
    """
    Superstate for all the times we are not falling or fallen.
    """
    pass

@stay
@ifSwitch(fallenAndWaited, 'standingUp', False)
def falling(player):
    """
    Protects robot as he falls. Stops him and kills stiffness.
    """
    if player.firstFrame():
        self.brain.player.gainsOff()
        self.brain.tracker.stopHeadMoves()
        player.inKickingState = False

@stay
@ifSwitch(sameStateForNSeconds(SweetMoves.getMoveTime(move)), 
                               player.gameState, True)
def standingUp(player):
    """
    Attempts a standup. If fails, tries again!
    """
    if player.firstFrame():
        self.brain.interface.motionRequest.reset_providers = True
        self.brain.interface.motionRequest.timestamp = int(self.brain.time*1000)
        self.brain.player.gainsOn()
        self.brain.tracker.setNeutralHead()

        if (self.brain.interface.fallStatus.on_front):
            move = SweetMoves.STAND_UP_FRONT
        else:
            move = SweetMoves.STAND_UP_BACK

        self.brain.player.executeMove(move)
