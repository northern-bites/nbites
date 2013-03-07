
from . import SoccerFSA
from . import WalkTestStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(WalkTestStates)
        self.setName('pWalkUnitTest')
