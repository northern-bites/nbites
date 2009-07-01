from . import SoccerFSA
from . import SquatGoalieStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(SquatGoalieStates)
        self.setName('pSquatGoalie')
