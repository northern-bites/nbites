# new file for goalie decisions
# allows goalie to make decisions without having to
# go into each state the goalie could possibly end up
#in and tell it how to do things

from ..playbook import PBConstants as PBCon
from .. import NogginConstants as NogCon
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran
import GoalieTransitions as goalTran

def goalieStateChoice(player):
    ball = player.brain.ball

    # for simplicity to start off with we just want the goalie to chase
    # the ball as long as it is close enough and it isnt dangerous

    if player.isChasing:
        if goalTran.shouldStopChase(player):
            return 'goaliePosition'
        else:
            return player.currentState

    if player.isPositioning:
        if goalTran.outOfPosition(player):
            return player.currentState
        elif goalTran.shouldChase(player):
            return 'goalieChase'

    return player.currentState

