from . import SoccerFSA
from . import CommMonitorStates
from . import FallControllerStates
from . import GameControllerStates
from . import RoleSwitchingStates
from . import WalkToLineStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(CommMonitorStates)
        self.addStates(GameControllerStates)
        self.addStates(WalkToLineStates)

        self.setName('pWalkToLine')
        self.currentState = 'fallController'

        self.brain.fallController.enabled = True
        self.roleSwitching = False
        self.role = brain.playerNumber
        self.commMode = -1