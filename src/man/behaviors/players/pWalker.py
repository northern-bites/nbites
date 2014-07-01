"""
A behavior to test walking. States are defined as functions in WalkingStates.
"""

from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import GameControllerStates
from . import WalkingStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(GameControllerStates)
        self.addStates(WalkingStates)
        self.setName('pWalker')
        self.currentState = 'fallController' # initial state

        self.brain.fallController.enabled = True 
        self.roleSwitching = False
        self.role = brain.playerNumber
