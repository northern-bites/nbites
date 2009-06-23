
from . import SoccerFSA
from . import GoToStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoToStates)
        self.setName('pGoTo')
