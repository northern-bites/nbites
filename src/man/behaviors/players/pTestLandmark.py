
from . import SoccerFSA
from . import TestLandmarkStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self, brain)
        self.addStates(TestLandmarkStates)
        self.setName('pTestLandmark')
