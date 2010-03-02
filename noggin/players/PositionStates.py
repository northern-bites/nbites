from .. import NogginConstants
from . import ChaseBallConstants as ChaseConstants
import man.noggin.util.MyMath as MyMath
from man.noggin.typeDefs.Location import RobotLocation, Location
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
        if gcState == 'gameReady':
            brain.tracker.locPans()
        else:
            brain.tracker.activeLoc()

    # determine final goal heading
    if ball.on:
        destHeading = my.h + ball.bearing
    elif ball.framesOff < 30:
        destHeading = my.h + ball.locBearing
    else:
        destHeading = NogginConstants.OPP_GOAL_HEADING

    # turn playbook value into location for navigator
    position = player.brain.play.getPosition()
    position = RobotLocation(position[0], position[1], destHeading)

    if gcState == 'gameReady':
        nav.positionReady(position)
    else:
        nav.positionPlaybook(position)

    if brain.my.locScore == NogginConstants.BAD_LOC:
        player.shouldRelocalizeCounter += 1

        # only need to check after we increment counter
        if player.shouldRelocalizeCounter >=\
               constants.SHOULD_RELOC_FRAME_THRESH:
            return player.goLater('relocalize')

    else:
        player.shouldRelocalizeCounter = 0

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
    position = player.brain.play.getPosition()
    position = Location(position[0], position[1])
    if player.firstFrame():
        player.stopWalking()
        player.notAtPositionCounter = 0

    # buffer switching back to playbook position
    if transitions.shouldReposition(nav.dest, position):
        player.notAtPositionCounter += 1

        if player.notAtPositionCounter >=\
               constants.NOT_AT_POSITION_FRAMES_THRESH:
            return player.goLater('playbookPosition')
    else:
        player.notAtPositionCounter = 0

    return player.stay()

def spinToBall(player):
    """
    State to spin to turn to the ball
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    ball = player.brain.ball

    turnRate = MyMath.clip(ball.locBearing*ChaseConstants.BALL_SPIN_GAIN,
                           -ChaseConstants.BALL_SPIN_SPEED,
                           ChaseConstants.BALL_SPIN_SPEED)

    if transitions.atSpinBallDir(player):
        return player.goLater('atSpinBallPosition')

    elif transitions.shouldSpinFindBallPosition(player):
        return player.goLater('spinFindBallPosition')

    elif player.currentSpinDir != MyMath.sign(turnRate):
        player.stopWalking()
        player.currentSpinDir = MyMath.sign(turnRate)
    elif player.stoppedWalk and ball.on and player.brain.nav.isStopped():
        player.setWalk(x=0,y=0,theta=turnRate)

    return player.stay()

def atSpinBallPosition(player):
    """
    Spun to the ball heading, spin again
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.activeLoc()

    if transitions.shouldTurnToBall_fromAtBallPosition(player):
        return player.goLater('spinToBall')
    elif transitions.shouldSpinFindBallPosition(player):
        return player.goLater('spinFindBallPosition')

    return player.stay()

def spinFindBallPosition(player):
    """
    Spin to find the ball if it is not being seen.
    """
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    if player.firstFrame() and player.stoppedWalk:
        player.setWalk(0, 0, ChaseConstants.FIND_BALL_SPIN_SPEED)
        player.brain.tracker.trackBall()


    if transitions.shouldTurnToBall_fromAtBallPosition(player):
        return player.goLater('spinToBall')
    if transitions.atSpinBallDir(player):
        return player.goLater('atSpinBallPosition')

    return player.stay()

def relocalize(player):
    if player.firstFrame():
        pass #player.stopWalking()
    if player.brain.my.locScore == NogginConstants.GOOD_LOC or \
            player.brain.my.locScore == NogginConstants.OK_LOC:
        return player.goLater(player.lastDiffState)

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.locPans()

    direction = MyMath.sign(player.getWalk()[2])
    if direction == 0:
        direction = 1

    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
        player.setWalk(0 , 0, constants.RELOC_SPIN_SPEED * direction)
    return player.stay()
