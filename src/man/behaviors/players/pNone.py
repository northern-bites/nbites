
from . import SoccerFSA
from . import NoneStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(NoneStates)
        self.setName('Player pNone')
