
from . import SoccerFSA
from . import FallControllerStates
from . import GameControllerStates
from . import SnapshotStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(GameControllerStates)
        self.addStates(SnapshotStates)
        self.setName('pSnapshot')
        self.currentState = 'fallController' # initial state

        # Controls whether we check for a falling/fallen robot
        self.brain.fallController.enabled = True 
