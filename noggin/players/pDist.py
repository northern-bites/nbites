from . import SoccerFSA
from . import DistStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(DistStates)
        self.setName('Player pDist')
