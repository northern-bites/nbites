from . import SoccerFSA
from . import GoalieStates
from . import VisualGoalieStates
from ..util import Transition
from . import GoalieTransitions
from . import PenaltyStates

import noggin_constants as NogginConstants
from GoalieConstants import RIGHT, LEFT

from objects import Location, RelRobotLocation

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoalieStates)
        self.addStates(VisualGoalieStates)
        self.addStates(PenaltyStates)
        self.setName('pGoalie')
        self.squatTime = 0
        self.frameCounter = 0
        self.penaltyKicking = False
        self.aggressive = False
        self.returningFromPenalty = False
        self.side = RIGHT
        self.homeDirections = []

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
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.ballMoreImportant,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.clearIt
            }

        VisualGoalieStates.waitToFaceField.transitions = {
            Transition.CountTransition(GoalieTransitions.notTurnedAround,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : PenaltyStates.afterPenalty
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

        GoalieStates.watchWithCornerChecks.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldReposition,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.fixMyself,

            Transition.CountTransition(GoalieTransitions.goodPosition,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.watch,

            Transition.CountTransition(GoalieTransitions.shouldSquat,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveCenter,

            # NOT diving for now... can't get back up!
            # Transition.CountTransition(GoalieTransitions.shouldDiveLeft,
            #                            Transition.SOME_OF_THE_TIME,
            #                            Transition.LOW_PRECISION)
            # : GoalieStates.saveLeft,

            # Transition.CountTransition(GoalieTransitions.shouldDiveRight,
            #                            Transition.SOME_OF_THE_TIME,
            #                            Transition.LOW_PRECISION)
            # : GoalieStates.saveRight,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.spinToFaceBall

            #### MORE POSITIONING STUFF FIX FIX FIX ###
            # Transition.CountTransition(GoalieTransitions.facingSideways,
            #                            Transition.MOST_OF_THE_TIME,
            #                            Transition.LOW_PRECISION)
            # : VisualGoalieStates.spinAtGoal
            }

        GoalieStates.watch.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldSquat,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveCenter,

            # NOT diving for now... can't get back up!
            # Transition.CountTransition(GoalieTransitions.shouldDiveLeft,
            #                            Transition.SOME_OF_THE_TIME,
            #                            Transition.LOW_PRECISION)
            # : GoalieStates.saveLeft,

            # Transition.CountTransition(GoalieTransitions.shouldDiveRight,
            #                            Transition.SOME_OF_THE_TIME,
            #                            Transition.LOW_PRECISION)
            # : GoalieStates.saveRight,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.spinToFaceBall

            #### MORE POSITIONING STUFF FIX FIX FIX ###
            # Transition.CountTransition(GoalieTransitions.facingSideways,
            #                            Transition.MOST_OF_THE_TIME,
            #                            Transition.LOW_PRECISION)
            # : VisualGoalieStates.spinAtGoal
            }

        GoalieStates.fixMyself.transitions = {
            Transition.CountTransition(GoalieTransitions.doneWalking,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.ballMoreImportant,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.spinToFaceBall
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

            Transition.CountTransition(GoalieTransitions.dangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.panic,

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

        VisualGoalieStates.panic.transitions = {
            Transition.CountTransition(GoalieTransitions.reachedMyDestination,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : VisualGoalieStates.attemptToNotScoreOnOurselves
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
            : GoalieStates.watchWithCornerChecks
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
