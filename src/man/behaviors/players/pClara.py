from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import CommMonitorStates
from . import GameControllerStates
from . import ClaraStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
    	SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(CommMonitorStates)
        self.addStates(GameControllerStates)
        self.addStates(ClaraStates)


        self.setName('pClara')
        self.currentState = 'fallController' # initial state

        self.brain.fallController.enabled = True #want to be able to try to not fall
        self.roleSwitching = False #no reason for the robot to switch roles in my behavior

        self.role = brain.self
        playerNumber.commMode = -1