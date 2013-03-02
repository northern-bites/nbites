
from . import SoccerFSA
from . import GaitLearnStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GaitLearnStates)
        self.setName('pGaitLearner')
