from . import SoccerFSA
from . import GoalieStates
from . import VisualGoalieStates
from ..util import Transition
from . import GoalieTransitions

import noggin_constants as NogginConstants

from objects import Location

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoalieStates)
        self.addStates(VisualGoalieStates)
        self.setName('pGoalie')
        self.squatTime = 0
        self.frameCounter = 0
        self.penaltyKicking = False

        VisualGoalieStates.walkToGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.atGoalArea,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinAtGoal,

            Transition.CountTransition(GoalieTransitions.veryCloseToPost,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.spinAtGoal
            }

        VisualGoalieStates.spinAtGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.facingForward,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.standStill
            }

    def run(self):
        gcState = self.brain.gameController.currentState

        if (gcState == 'gamePlaying'):
            # Make sure gamePlaying gets run
            if (self.brain.gameController.counter == 2):
                self.switchTo('position')

        SoccerFSA.SoccerFSA.run(self)
