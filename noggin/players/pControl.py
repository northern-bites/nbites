
from ..util import FSA
from . import KeyControlStates

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(KeyControlStates)
	self.currentState = 'nothing'
        self.setName('Player pControl')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
