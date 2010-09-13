
from . import SoccerFSA
from . import GaitTestStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GaitTestStates)
        self.setName('pGaitUnitTest')
