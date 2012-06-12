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
            : GoalieStates.watch
            }

        VisualGoalieStates.decideSide.transitions = {
            Transition.CountTransition(GoalieTransitions.onLeftSideline,
                                       Transition.MOST_OF_THE_TIME,
                                       50)
            : VisualGoalieStates.walkToGoal,

            Transition.CountTransition(GoalieTransitions.onRightSideline,
                                       Transition.MOST_OF_THE_TIME,
                                       # magic number
                                       50)
            : VisualGoalieStates.walkToGoal
            }

        GoalieStates.watch.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldPerformSave,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveIt,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.MOST_OF_THE_TIME,
                                       # magic number
                                       50)
            : VisualGoalieStates.clearIt,

            Transition.CountTransition(GoalieTransitions.ballIsAtMyFeet,
                                       Transition.MOST_OF_THE_TIME,
                                       # magic number
                                       50)
            : GoalieStates.kickBall

            }

        VisualGoalieStates.spinToFaceBall.transitions = {
            Transition.CountTransition(GoalieTransitions.facingBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.clearIt
            }

        VisualGoalieStates.clearIt.transitions = {
            Transition.CountTransition(GoalieTransitions.reachedTheBall,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : GoalieStates.kickBall,

            Transition.CountTransition(GoalieTransitions.ballLostStopChasing,
                                       Transition.MOST_OF_THE_TIME,
                                       # magic number
                                       50)
            : VisualGoalieStates.returnToGoal,

            Transition.CountTransition(GoalieTransitions.ballMovedStopChasing,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.returnToGoal

            }

        VisualGoalieStates.returnToGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.doneWalking,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.watch
            }

    def run(self):
        gcState = self.brain.gameController.currentState

        # if (gcState == 'gamePlaying'):
        #     # Make sure gamePlaying gets run
        #     if (self.brain.gameController.counter == 2):
        #         self.switchTo('watch')

        SoccerFSA.SoccerFSA.run(self)
