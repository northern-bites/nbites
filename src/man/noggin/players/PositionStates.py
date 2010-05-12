from .. import NogginConstants
from . import ChaseBallConstants as ChaseConstants
import man.noggin.util.MyMath as MyMath
import PositionTransitions as transitions
import PositionConstants as constants

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
    my = brain.my
    ball = brain.ball
    gcState = brain.gameController.currentState

    if player.firstFrame():
        nav.positionPlaybook()

        if gcState == 'gameReady':
            brain.tracker.locPans()
        else:
            brain.tracker.activeLoc()

    if transitions.shouldAvoidObstacle(player):
        return player.goNow('avoidObstacle')

    # we're at the point, let's switch to another state
    if nav.isStopped() and player.counter > 0:
        return player.goLater('atPosition')

    return player.stay()

def atPosition(player):
    """
    State for when we're at the position
    """
    nav = player.brain.nav

    if player.firstFrame():
        player.stopWalking()
        player.notAtPositionCounter = 0

    # buffer switching back to playbook position
    if transitions.shouldReposition(player, nav.dest, player.brain.my):
        player.notAtPositionCounter += 1

        if player.notAtPositionCounter >=\
               constants.NOT_AT_POSITION_FRAMES_THRESH:
            return player.goLater('playbookPosition')
    else:
        player.notAtPositionCounter = 0

    return player.stay()

def relocalize(player):
    if player.firstFrame():
        pass #player.stopWalking()

    if player.brain.my.locScore == NogginConstants.GOOD_LOC or \
            player.brain.my.locScore == NogginConstants.OK_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 15:
            player.shouldRelocalizeCounter = 0
            return player.goLater(player.lastDiffState)

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.locPans()

    direction = MyMath.sign(player.getWalk()[2])
    if direction == 0:
        direction = 1

    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
        player.setWalk(0 , 0, constants.RELOC_SPIN_SPEED * direction)

    return player.stay()
