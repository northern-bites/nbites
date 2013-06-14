import noggin_constants as NogginConstants
from   ..util import MyMath
import PositionConstants as constants
import PositionTransitions
from math import fabs

def playbookPosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    brain = player.brain
    nav = brain.nav

    if player.firstFrame():
        nav.positionPlaybook()

    if nav.isAtPosition():
        brain.tracker.trackBall()
    else:
        brain.tracker.repeatBasicPan()

    #TODO: I think the transition is broken right now!
    #if PositionTransitions.leavingTheField(player):
    #    return player.goLater('spinToField')

    return player.stay()

def spinToField(player):

    fieldEdge = player.brain.interface.visualField.visual_field_edge

    if player.firstFrame():
        if fieldEdge.distance_l > fieldEdge.distance_r:
            player.brain.nav.walkTo(0,0,constants.SPIN_AROUND_LEFT)
            player.brain.tracker.spinPan()
        else:
            player.brain.nav.walkTo(0,0,constants.SPIN_AROUND_RIGHT)
            player.brain.tracker.spinPan()

    elif player.brain.nav.isAtPosition():
        return player.goLater('playbookPosition')
    return player.stay()
