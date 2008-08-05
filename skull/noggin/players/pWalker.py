
from ..util import FSA
from . import WalkLearning

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(WalkLearning)
	self.currentState = 'nothing'
        self.setName('Player pWalker')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)

        #instance specific
        self.object = brain.bgrp
        self.speed = 30.
