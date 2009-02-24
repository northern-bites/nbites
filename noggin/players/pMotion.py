
import man.motion as motion

from ..util import FSA
from . import MotionStates
from . import GameControllerStates

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(GameControllerStates)
        self.addStates(MotionStates)
        self.currentState = 'nothing'
        self.setName('pMotion')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)
