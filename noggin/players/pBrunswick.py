
from . import SoccerFSA
from . import BrunswickStates
from . import ChaseBallStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(ChaseBallStates)
        self.addStates(BrunswickStates)
        self.setName('pBrunswick')
