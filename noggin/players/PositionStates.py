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
    position = brain.playbook.position
    nav = brain.nav
    my = brain.my

    useOmni = False #(MyMath.dist(my.x, my.y, position[0], position[1]) <= 50.0)

    if transitions.shouldRelocalize(player):
        return player.goLater('relocalize')

    if player.firstFrame() or \
            nav.destX != position[0] or \
            nav.destY != position[1] or \
            useOmni != nav.movingOmni:

        player.brain.tracker.locPans()
        #player.printf("I am going to " + str(position))
        if not useOmni:
            nav.goTo(position[0], position[1], NogginConstants.OPP_GOAL_HEADING)
        else:
            nav.omniGoTo(position[0], position[1], NogginConstants.OPP_GOAL_HEADING)
        #player.printf("position = "+str(position[0])+" , "+str(position[1]) )

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
        player.brain.tracker.trackBall()
    if nav.notAtHeading(nav.destH) or not nav.atDestinationCloser():
        return player.goLater('playbookPosition')

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
        player.brain.CoA.setRobotTurnGait(player.brain.motion)
        player.setSpeed(x=0,y=0,theta=turnRate)

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
        player.setSpeed(0,
                        0,
                        ChaseConstants.FIND_BALL_SPIN_SPEED)
        player.brain.tracker.trackBall()


    if transitions.shouldTurnToBall_fromAtBallPosition(player):
        return player.goLater('spinToBall')
    if transitions.atSpinBallDir(player):
        return player.goLater('atSpinBallPosition')

    return player.stay()

def relocalize(player):
    if player.firstFrame():
        player.stopWalking()
    if transitions.isWellLocalized(player):
        return player.goLater(player.lastDiffState)

    if not player.brain.motion.isHeadActive():
        player.brain.tracker.locPans()

    if player.counter > constants.RELOC_SPIN_FRAME_THRESH:
        player.setSpeed(0 , 0, constants.RELOC_SPIN_SPEED)
    return player.stay()
