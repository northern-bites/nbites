import noggin_constants as NogginConstants
from . import ChaseBallConstants as ChaseConstants
import man.motion.HeadMoves as HeadMoves
import man.noggin.util.MyMath as MyMath
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
    gcState = brain.gameController.currentState

    if player.firstFrame():
        nav.positionPlaybook()

        if gcState == 'gameReady':
            brain.tracker.locPans()
        else:
            brain.tracker.activeLoc()

    return player.stay()

def relocalize(player):
    if player.firstFrame():
        player.setWalk(constants.RELOC_X_SPEED, 0, 0)

    if player.brain.my.locScore != NogginConstants.locScore.BAD_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater(player.lastDiffState)

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.locPans()

    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
        direction = MyMath.sign(player.getWalk()[2])
        if direction == 0:
            direction = 1

        player.setWalk(0, 0, constants.RELOC_SPIN_SPEED * direction)

    return player.stay()

