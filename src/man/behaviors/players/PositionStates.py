import noggin_constants as NogginConstants
from   ..util import MyMath
import PositionConstants as constants
import PositionTransitions

def positionLocalize(player):
    """
    Localize better in order to position
    """
    return player.stay()

def playbookPosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    brain = player.brain
    nav = brain.nav

    if player.firstFrame():
        nav.positionPlaybook()

    if nav.isAtPosition():
        brain.tracker.trackBallFixedPitch()
    else:
        brain.tracker.repeatWidePanFixedPitch()

    #TODO: I think the transition is broken right now!
    #if PositionTransitions.leavingTheField(player):
    #    return player.goLater('spinToField')

    return player.stay()

def spinToField(player):

    fieldEdge = player.brain.interface.visualField.visual_field_edge

    if player.firstFrame():
        if fieldEdge.distance_l > fieldEdge.distance_r:
            player.brain.nav.walkTo(0,0,constants.SPIN_AROUND_LEFT)
            player.brain.tracker.spinPanFixedPitch()
        else:
            player.brain.nav.walkTo(0,0,constants.SPIN_AROUND_RIGHT)
            player.brain.tracker.spinPanFixedPitch()

    elif player.brain.nav.isAtPosition():
        return player.goLater('playbookPosition')
    return player.stay()


def relocalize(player):
    if player.firstFrame():
        player.setWalk(constants.RELOC_X_SPEED, 0, 0)

    if player.brain.my.locScore is not NogginConstants.locScore.BAD_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater(player.lastDiffState)

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.head_is_active:
        player.brain.tracker.repeatWidePanFixedPitch()

#    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
#        direction = MyMath.sign(player.getWalk()[2])
#        if direction == 0:
#            direction = 1
#@todo: we just spin left to relocalize since getWalk was deprecated
# maybe we can make this smarter?
        player.setWalk(0, 0, constants.RELOC_SPIN_SPEED)

    return player.stay()
