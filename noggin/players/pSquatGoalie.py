from . import SoccerFSA
from . import SquatGameStates
from . import SquatPositionStates
from . import GoalieTransitions
from . import ChaseBallStates
from . import FindBallStates
from . import KickingStates
from ..playbook import PBConstants

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(SquatGameStates)
        self.addStates(SquatPositionStates)
        self.addStates(ChaseBallStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.setName('pSquatGoalie')

        self.squatting = False
        self.isChasing = False
        self.penaltyKicking = False
        self.inKickingState = False
        self.hasKickedOffKick = True

        self.shouldChaseCounter = 0
        self.ballRelX = 0
        self.ballRelY = 0

    def run(self):
        gcState = self.brain.gameController.currentState
        if gcState == 'gamePlaying' or\
                (gcState == 'penaltyShotsGamePlaying'
                 and self.brain.playbook.role == PBConstants.GOALIE):
                state = GoalieTransitions.goalieRunChecks(self)
                if state == 'goaliePosition':
                    state = 'squatPosition'
                elif state == 'chase':
                    state = 'chasePrepare'
                if not state == self.currentState:
                    self.switchTo(state)
        SoccerFSA.SoccerFSA.run(self)
