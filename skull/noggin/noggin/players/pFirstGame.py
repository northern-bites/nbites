

import util.FSA as FSA
import FirstGameStates

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(FirstGameStates)
	self.currentState = 'gameInitial'
        self.setName('Player pFirstGame')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
