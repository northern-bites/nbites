"""
A behavior to test walking to line. 
States are defined as functions in LauraStates.
"""

from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import CommMonitorStates
from . import GameControllerStates
from . import LauraStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(CommMonitorStates)
        self.addStates(GameControllerStates)
        self.addStates(LauraStates)

        self.setName('pLaura')
        self.currentState = 'fallController' # initial state

        self.brain.fallController.enabled = True 
        self.roleSwitching = False
        self.role = brain.playerNumber
        self.commMode = -1
