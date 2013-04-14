
from . import SoccerFSA
from . import GoToStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoToStates)
        self.setName('pGoTo')

        self.brain.speech.volue = 0.99

    def run(self):

        if self.currentState != self.lastState:
            self.brain.speech.say(self.currentState)
        SoccerFSA.SoccerFSA.run(self)
