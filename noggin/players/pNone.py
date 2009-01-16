
import man.motion as motion

from ..util import FSA
from . import NoneStates

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(NoneStates)
	self.currentState = 'nothing'
        self.setName('Player pNone')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
