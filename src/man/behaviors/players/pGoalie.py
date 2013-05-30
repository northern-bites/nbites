from . import SoccerFSA
from . import GoalieStates
from . import VisualGoalieStates
from ..util import Transition
from . import GoalieTransitions

import noggin_constants as NogginConstants
from GoalieConstants import RIGHT, LEFT

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
        self.aggressive = False

        # All transitions are defined here. Their conditions are in
        # GoalieTransitions
        VisualGoalieStates.walkToGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.atGoalArea,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinAtGoal
            }

        VisualGoalieStates.spinAtGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.facingForward,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.findGoalboxCorner,

            Transition.CountTransition(GoalieTransitions.ballMoreImportant,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.clearIt
            }

        VisualGoalieStates.waitToFaceField.transitions = {
            Transition.CountTransition(GoalieTransitions.notTurnedAround,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : VisualGoalieStates.decideLeftSide
            }

        VisualGoalieStates.decideLeftSide.transitions = {
            Transition.CountTransition(GoalieTransitions.onThisSideline,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : VisualGoalieStates.walkToGoal,

            Transition.CountTransition(GoalieTransitions.unsure,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.decideRightSide
            }

        VisualGoalieStates.decideRightSide.transitions = {
            Transition.CountTransition(GoalieTransitions.onThisSideline,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.walkToGoal,

            Transition.CountTransition(GoalieTransitions.unsure,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.decideLeftSide
            }

        GoalieStates.watch.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldPerformSave,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveIt,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.clearIt,

            Transition.CountTransition(GoalieTransitions.facingSideways,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinAtGoal
            }

        VisualGoalieStates.shouldISaveIt.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldPerformSave,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveIt,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.clearIt,

            Transition.CountTransition(GoalieTransitions.noSave,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : GoalieStates.watch
            }

        VisualGoalieStates.spinToFaceBall.transitions = {
            Transition.CountTransition(GoalieTransitions.facingBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.clearIt
            }

        VisualGoalieStates.clearIt.transitions = {
            Transition.CountTransition(GoalieTransitions.reachedMyDestination,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : GoalieStates.kickBall,

            Transition.CountTransition(GoalieTransitions.ballLostStopChasing,
                                       Transition.ALL_OF_THE_TIME,
                                       90)
            : VisualGoalieStates.returnToGoal,

            Transition.CountTransition(GoalieTransitions.ballMovedStopChasing,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.returnToGoal,

            Transition.CountTransition(GoalieTransitions.walkedTooFar,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.returnToGoal
            }

        VisualGoalieStates.repositionAfterWhiff.transitions = {
            Transition.CountTransition(GoalieTransitions.reachedMyDestination,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : GoalieStates.kickBall
            }

        VisualGoalieStates.returnToGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.doneWalking,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.findGoalboxCorner
            }

        VisualGoalieStates.didIKickIt.transitions = {
            Transition.CountTransition(GoalieTransitions.whiffed,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.repositionAfterWhiff,

            Transition.CountTransition(GoalieTransitions.successfulKick,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : VisualGoalieStates.returnToGoal
            }

        VisualGoalieStates.findGoalboxCorner.transitions = {
            Transition.CountTransition(GoalieTransitions.foundACorner,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.centerAtGoalBasedOnCorners,

            Transition.CountTransition(GoalieTransitions.noCorner,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : GoalieStates.watch,

            Transition.CountTransition(GoalieTransitions.ballMoreImportant,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.clearIt
            }

        VisualGoalieStates.centerAtGoalBasedOnCorners.transitions = {
            Transition.CountTransition(GoalieTransitions.lostCorner,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.watch,

            Transition.CountTransition(GoalieTransitions.reachedMyDestination,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.watch
            }

        GoalieStates.waitForPenaltySave.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldDiveRight,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.diveRight,

            Transition.CountTransition(GoalieTransitions.shouldDiveLeft,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.diveLeft,

            Transition.CountTransition(GoalieTransitions.shouldSquat,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.squat

            }

    def run(self):
        gcState = self.brain.gameController.currentState
        SoccerFSA.SoccerFSA.run(self)
