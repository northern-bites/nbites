from . import SoccerFSA
from . import FallControllerStates
from . import RoleSwitchingStates
from . import GameControllerStates
from . import GoalieStates
from . import VisualGoalieStates
from . import GoalieTransitions
from . import PenaltyStates
from ..util import Transition

import noggin_constants as NogginConstants
from GoalieConstants import RIGHT, LEFT

from objects import Location, RelRobotLocation

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        ### ADD STATES AND NAME FSA ###
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(FallControllerStates)
        self.addStates(RoleSwitchingStates)
        self.addStates(GameControllerStates)
        self.addStates(GoalieStates)
        self.addStates(VisualGoalieStates)
        self.addStates(PenaltyStates)
        self.setName('pGoalie')
        self.currentState = 'fallController' # initial state

        ### THE STATE OF THE PLAYER ###
        self.role = 1
        self.squatTime = 0
        self.frameCounter = 0
        self.penaltyKicking = False
        self.aggressive = False
        self.returningFromPenalty = False
        self.side = RIGHT
        self.homeDirections = []
        self.brain.fallController.enabled = True
        self.roleSwitching = False

        ### ALL TRANSITIONS ARE DEFINED HERE ############
        ### Their conditions are in GoalieTransitions ###
        VisualGoalieStates.walkToGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.atGoalArea,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinAtGoal,

            Transition.CountTransition(GoalieTransitions.reachedMyDestination,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.INSTANT)
            : VisualGoalieStates.spinAtGoal

            }

        VisualGoalieStates.spinAtGoal.transitions = {
            Transition.CountTransition(GoalieTransitions.facingForward,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION + 5)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.shouldClearDangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.backUpForDangerousBall
            }

        VisualGoalieStates.waitToFaceField.transitions = {
            Transition.CountTransition(GoalieTransitions.notTurnedAround,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : PenaltyStates.afterPenalty
            }

        GoalieStates.watchWithCornerChecks.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldReposition,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.fixMyself,

            Transition.CountTransition(GoalieTransitions.shouldMoveForward,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : GoalieStates.moveForward,

            Transition.CountTransition(GoalieTransitions.shouldMoveBackwards,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.HIGH_PRECISION)
            : GoalieStates.moveBackwards,

            Transition.CountTransition(GoalieTransitions.goodPosition,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.watch,

            Transition.CountTransition(GoalieTransitions.shouldSquat,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveCenter,

            Transition.CountTransition(GoalieTransitions.facingSideways,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinAtGoal,

            Transition.CountTransition(GoalieTransitions.facingBackwards,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinAtGoal,

            # No dives
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
                                       Transition.OK_PRECISION + 5)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.shouldClearDangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.backUpForDangerousBall

            }

        GoalieStates.watch.transitions = {
            Transition.CountTransition(GoalieTransitions.shouldSquat,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.saveCenter,

            # No dives
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
                                       Transition.OK_PRECISION + 5)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.shouldClearDangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.backUpForDangerousBall
            }

        GoalieStates.moveForward.transitions = {
            Transition.CountTransition(GoalieTransitions.doneWalking,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION + 5)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.shouldClearDangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.backUpForDangerousBall
            }

        GoalieStates.moveBackwards.transitions = {
            Transition.CountTransition(GoalieTransitions.doneWalking,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION + 5)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.shouldClearDangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.backUpForDangerousBall
            }

        GoalieStates.fixMyself.transitions = {
            Transition.CountTransition(GoalieTransitions.doneWalking,
                                       Transition.ALL_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.shouldClearBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION + 5)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.shouldClearDangerousBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : VisualGoalieStates.backUpForDangerousBall
            }

        VisualGoalieStates.spinToFaceBall.transitions = {
            Transition.CountTransition(GoalieTransitions.facingBall,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.clearIt
            }

        VisualGoalieStates.backUpForDangerousBall.transitions = {
            Transition.CountTransition(GoalieTransitions.ballSafe,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : VisualGoalieStates.spinToFaceBall,

            Transition.CountTransition(GoalieTransitions.ballLostStopChasing,
                                       Transition.ALL_OF_THE_TIME,
                                       90)
            : GoalieStates.watchWithCornerChecks,

            Transition.CountTransition(GoalieTransitions.ballMovedStopChasing,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.watchWithCornerChecks
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

        GoalieStates.spinToWalkOffField.transitions = {
            Transition.CountTransition(GoalieTransitions.goodToBookIt,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.bookIt
            }

        GoalieStates.bookIt.transitions = {
            Transition.CountTransition(GoalieTransitions.safelyIllegal,
                                       Transition.SOME_OF_THE_TIME,
                                       Transition.OK_PRECISION)
            : GoalieStates.standStill
            }

        GoalieStates.waitForPenaltySave.transitions = {
            Transition.CountTransition(GoalieTransitions.saveNow,
                                       Transition.MOST_OF_THE_TIME,
                                       Transition.LOW_PRECISION)
            : GoalieStates.doDive,
            }
