import os

from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import CommMonitorStates
from . import GameControllerStates
from . import CalibratePanStates
from . import CalibrateStates


class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(CommMonitorStates)
        self.addStates(GameControllerStates)
        self.addStates(CalibrateStates)
        self.addStates(CalibratePanStates)
        self.setName('pCalibrate')
        self.currentState = 'fallController' # initial state

        os.environ["LOG_THIS"] = 'false'
        self.brain.fallController.enabled = True 
        self.roleSwitching = False
        self.role = brain.playerNumber
        self.returningFromPenalty = False
        self.commMode = -1
