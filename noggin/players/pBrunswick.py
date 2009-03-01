
from . import SoccerFSA
from . import BrunswickStates
from . import FindBallStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FindBallStates)
        self.addStates(BrunswickStates)
        self.setName('pBrunswick')
