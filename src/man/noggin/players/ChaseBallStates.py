"""
Here we house all of the state methods used for chasing the ball
"""
from man.noggin.util import MyMath
from man.motion import SweetMoves
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
import KickingHelpers
import GoalieTransitions as goalTran
from ..playbook.PBConstants import GOALIE

def chase(player):
    """
    Method to determine which chase state should be used.
    We dump the robot into this state when we our switching from something else.
    """
    player.isChasing = True
    player.hasAlignedOnce = False

    if player.brain.play.isRole(GOALIE):
        if transitions.shouldScanFindBall(player):
            return player.goNow('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goNow('approachBall')
        elif transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        else:
            return player.goNow('scanFindBall')

    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldKick(player):
        return player.goNow('waitBeforeKick')
    else:
        return player.goNow('scanFindBall')

def chaseAfterKick(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)

        player.brain.tracker.trackBall()

        if player.chosenKick == SweetMoves.LEFT_FAR_KICK or \
                player.chosenKick == SweetMoves.RIGHT_FAR_KICK:
            return player.goLater('chase')

        if player.chosenKick == SweetMoves.LEFT_SIDE_KICK:
            turnDir = constants.TURN_RIGHT

        elif player.chosenKick == SweetMoves.RIGHT_SIDE_KICK:
            turnDir = constants.TURN_LEFT

        player.setWalk(0, 0, turnDir * constants.BALL_SPIN_SPEED)
        return player.stay()

    if player.brain.ball.framesOn > constants.BALL_ON_THRESH:
        return player.goLater('chase')
    elif player.counter > constants.CHASE_AFTER_KICK_FRAMES:
        return player.goLater('spinFindBall')
    return player.stay()


def approachBall(player):
    """
    Once we are alligned with the ball, approach it
    """
    if player.firstFrame():
        player.hasAlignedOnce = False

    if transitions.shouldActiveLoc(player):
        player.brain.tracker.activeLoc()
    else :
        player.brain.tracker.trackBall()

    if player.penaltyKicking and \
            player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')

    # Switch to other states if we should
    if player.brain.play.isRole(GOALIE):
        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif not player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
    else:
        if transitions.shouldDribble(player):
            return player.goNow('dribble')
        elif transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')

    return approachBallWalk(player)

def approachBallWalk(player):
    """
    Method that is used by both approach ball and dribble
    We use things as to when we should leave and how we should walk
    """

    if not player.brain.play.isRole(GOALIE):
        if not player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBallActiveLoc(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldAvoidObstacleDuringApproachBall(player):
            return player.goLater('avoidObstacle')

    # Determine our speed for approaching the ball
    ball = player.brain.ball
    if player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')

    player.brain.nav.chaseBall()

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    """
    if player.firstFrame():
        player.brain.CoA.setRobotSlowGait(player.brain.motion)

    player.inKickingState = True
    # Leave this state if necessary
    if transitions.shouldKick(player):
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goNow('waitBeforeKick')
    elif transitions.shouldScanFindBall(player):
        player.inKickingState = False
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('scanFindBall')
    elif transitions.shouldApproachFromPositionForKick(player):
        player.inKickingState = False
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('approachBall')

    player.brain.nav.kickPosition()

    return player.stay()

def dribble(player):
    """
    Keep running at the ball, but dribble
    """
    if player.firstFrame():
        player.brain.CoA.setRobotDribbleGait(player.brain.motion)

    # if we should stop dribbling, see what else we should do
    if transitions.shouldStopDribbling(player):

        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif transitions.shouldApproachBall(player):
            return player.goNow('approachBall')

    return approachBallWalk(player)

def waitBeforeKick(player):
    """
    Stop before we kick to make sure we want to kick
    """
    player.inKickingState = True
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.stopWalking()

    if not player.brain.nav.isStopped():
        return player.stay()

    if transitions.shouldKick(player):
        return player.goLater('getKickInfo')
    elif transitions.shouldApproachForKick(player):
        player.brain.tracker.trackBall()
        player.inKickingState = False
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        player.inKickingState = False
        player.brain.tracker.trackBall()
        return player.goLater('scanFindBall')
    elif transitions.shouldRepositionForKick(player):
        player.brain.tracker.trackBall()
        return player.goLater('positionForKick')

    # Just don't get stuck here!
    if player.counter > 50:
        return player.goNow('scanFindBall')
    return player.stay()

# WARNING: avoidObstacle could possibly go into our own box
def avoidObstacle(player):
    """
    If we detect something in front of us, dodge it
    """
    if player.firstFrame():
        player.doneAvoidingCounter = 0
        player.printf(player.brain.sonar)

        player.brain.CoA.setRobotGait(player.brain.motion)

        if (transitions.shouldAvoidObstacleLeft(player) and
            transitions.shouldAvoidObstacleRight(player)):
            # Backup
            player.printf("Avoid by backup");
            player.setWalk(constants.DODGE_BACK_SPEED, 0, 0)

        elif transitions.shouldAvoidObstacleLeft(player):
            # Dodge right
            player.printf("Avoid by right dodge");
            player.setWalk(0, constants.DODGE_RIGHT_SPEED, 0)

        elif transitions.shouldAvoidObstacleRight(player):
            # Dodge left
            player.printf("Avoid by left dodge");
            player.setWalk(0, constants.DODGE_LEFT_SPEED, 0)

    if not transitions.shouldAvoidObstacle(player):
        player.doneAvoidingCounter += 1
    else :
        player.doneAvoidingCounter -= 1
        player.doneAvoidingCounter = max(0, player.doneAvoidingCounter)

    if player.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        player.shouldAvoidObstacleRight = 0
        player.shouldAvoidObstacleLeft = 0
        player.stopWalking()
        return player.goLater(player.lastDiffState)

    return player.stay()

def steps(player):
    if player.brain.nav.isStopped():
        player.setSteps(3,3,0,5)
    elif player.brain.nav.currentState != "stepping":
        player.stopWalking()
    return player.stay()

# TODO
def approachDangerousBall(player):
    if player.firstFrame():
        player.stopWalking()
    #print "approach dangerous ball"
    #single steps towards ball and goal with spin
    player.setSteps(0, 0, 0, 0)

    if not goalTran.dangerousBall(player):
        return player.goLater('approachBall')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()

def orbitBeforeKick(player):
    """
    State for circling the ball when we're facing our goal.
    """
    brain = player.brain
    my = brain.my
    if player.firstFrame():
        player.orbitStartH = my.h
        brain.CoA.setRobotGait(brain.motion)
        brain.tracker.trackBall()

        shotPoint = KickingHelpers.getShotCloseAimPoint(player)
        bearingToGoal = my.getRelativeBearing(shotPoint)
        spinDir = -MyMath.sign(bearingToGoal)
        player.brain.nav.orbitAngle(spinDir * 90)
    if not player.brain.tracker.activeLocOn and \
            transitions.shouldScanFindBall(player):
        player.brain.CoA.setRobotGait(player.brain.motion)
        return player.goLater('scanFindBall')
    elif brain.ball.dist > constants.STOP_ORBIT_BALL_DIST:
        return player.goLater('chase')

    if player.brain.nav.isStopped() and not player.firstFrame():
        return player.goLater('positionForKick')
    return player.stay()
