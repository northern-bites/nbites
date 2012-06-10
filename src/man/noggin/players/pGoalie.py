from . import SoccerFSA
from . import GoalieStates

import noggin_constants as NogginConstants

from objects import Location

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoalieStates)
        self.setName('pGoalie')
        self.squatTime = 0
        self.frameCounter = 0
        self.penaltyKicking = False

    def run(self):
        gcState = self.brain.gameController.currentState

        if (gcState == 'gamePlaying'):
            # Make sure gamePlaying gets run
            if (self.brain.gameController.counter == 2):
                self.switchTo('position')

        SoccerFSA.SoccerFSA.run(self)
