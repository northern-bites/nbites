import time
from objects import RelRobotLocation, RobotLocation
from ..navigator import Navigator as nav
from ..util import *
import VisualGoalieStates as VisualStates
from .. import SweetMoves
from ..headTracker import HeadMoves
import GoalieConstants as constants
import math
import noggin_constants as nogginConstants

#TestingChange
SAVING = True
DIVING = True
savedebug = False

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.inKickingState = False
        player.returningFromPenalty = False
        player.brain.fallController.enabled = False
        player.stand()
        player.zeroHeads()
        player.isSaving = False
        player.lastStiffStatus = True
        player.justKicked = False

    # If stiffnesses were JUST turned on, then stand up.
    if player.lastStiffStatus == False and player.brain.interface.stiffStatus.on:
        player.stand()
    # Remember last stiffness.
    player.lastStiffStatus = player.brain.interface.stiffStatus.on

    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.penaltyKicking = False
        player.stand()
        player.brain.tracker.lookToAngle(0)

    # Wait until the sensors are calibrated before moving.
    if(not player.brain.motion.calibrated):
        return player.stay()

    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.returningFromPenalty = False
        player.penaltyKicking = False
        player.stand()
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
        player.inPosition = constants.CENTER_POSITION

        watchWithLineChecks.correctFacing = False
        watchWithLineChecks.numFixes = 0
        watchWithLineChecks.numTurns = 0
        watchWithLineChecks.looking = False

        # The ball will be right in front of us, for sure
        player.brain.tracker.lookToAngle(0)

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    player.brain.resetGoalieLocalization()

    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        if player.brain.penalizedHack:
            player.brain.resetGoalieLocalization()
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.penaltyKicking = False
        player.brain.nav.stand()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    # TODO penalty handling
    if player.penalized:
        player.penalized = False
        return player.goLater('afterPenalty')

    if player.lastDiffState == 'afterPenalty':
        return player.goLater('walkToGoal')

    if player.lastDiffState == 'fallen':
        if (fallen.lastState == 'clearBall'
        and player.brain.ball.vis.on
        and math.fabs(player.brain.ball.bearing_deg) < 25.0
        and player.brain.ball.distance < 90):
            print "I was already going to clear it!"
            return player.goLater('clearBall')
        elif (fallen.lastState == 'returnUsingLoc'
        or fallen.lastState == 'didIKickIt'
        or fallen.lastState == 'kickBall'
        or fallen.lastState == 'repositionAfterWhiff'):
            print("I'm likely away frm the goalbox")
            return player.goLater('returnUsingLoc')
        elif player.justDived:
            player.justDived = False
            return player.goLater('spinToRecover')
        else:
            return player.goLater('checkSafePlacement')

    #TODO before game/scrimmage change this to watch;
    #TESTINGCHANGE
    # return player.goLater('watchWithLineChecks')
    return player.goLater('watch')
    # return player.goLater('checkSafePlacement')

@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.stopWalking()
        player.penalizeHeads()
        player.penalized = True

    # TODO is this actually possible?
    if player.lastDiffState == '':
        # Just started up! Need to calibrate sensors
        player.brain.nav.stand()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    return player.stay()

@superState('gameControllerResponder')
def gameFinished(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.stopWalking()
        player.zeroHeads()
        if nogginConstants.V5_ROBOT:
            player.executeMove(SweetMoves.SIT_POS_V5)
        else:
            player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

##### EXTRA METHODS

@superState('gameControllerResponder')
def fallen(player):
    fallen.lastState = player.lastDiffState
    player.inKickingState = False
    return player.stay()

fallen.lastState = 'watch'

@superState('gameControllerResponder')
def standStill(player):
    if player.firstFrame():
        player.brain.nav.stop()

    return player.stay()

@superState('gameControllerResponder')
def watchWithLineChecks(player):
    if player.firstFrame():
        player.goodRightCornerObservation = False
        player.goodLeftCornerObservation = False
        watchWithLineChecks.counter = 0
        print ("My num turns:", watchWithLineChecks.numTurns)
        print ("My num fix:", watchWithLineChecks.numFixes)
        watchWithLineChecks.lines[:] = []
        player.homeDirections = []
        watchWithLineChecks.hasPanned = False

        if (player.lastDiffState is not 'lineCheckReposition'
        and player.lastDiffState is not 'moveBackwards'):
            print "My facing is not necessarily correct! I'm checking"
            watchWithLineChecks.correctFacing = False
            watchWithLineChecks.numFixes = 0
            watchWithLineChecks.numTurns = 0
            watchWithLineChecks.looking = False

        elif watchWithLineChecks.numTurns > 0:
            print "I think I have corrected my facing now..."
            watchWithLineChecks.correctFacing = True

        player.brain.tracker.trackBall()
        player.brain.nav.stand()
        player.returningFromPenalty = False

    if (player.brain.ball.vis.frames_on > constants.BALL_ON_SAFE_THRESH
        and player.brain.ball.distance > constants.BALL_SAFE_DISTANCE_THRESH
        and not watchWithLineChecks.looking
        and not watchWithLineChecks.hasPanned):
        watchWithLineChecks.looking = True
        player.brain.tracker.performBasicPan()
        watchWithLineChecks.hasPanned = True

    if player.brain.tracker.isStopped():
        watchWithLineChecks.looking = False
        player.brain.tracker.trackBall()

    if watchWithLineChecks.counter > 150 or watchWithLineChecks.numFixes > 6:
        print "Counter was over 400, going to watch!"
        return player.goLater('watch')

    # Bc we won't be looking at landmarks if ball is on
    if not player.brain.ball.vis.on:
        watchWithLineChecks.counter += 1
    return Transition.getNextState(player, watchWithLineChecks)

watchWithLineChecks.lines = []
watchWithLineChecks.wentToClearIt = False

@superState('gameControllerResponder')
def lineCheckReposition(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        dest = average(player.homeDirections)
        print "My home directions: "
        print dest
        if dest.relX == 0.0 and dest.relY == 0.0:
            print "I think this was a turn, I'm increasing my num turns!"
            watchWithLineChecks.numTurns += 1
        else:
            print "This was a reposition, I think"
            watchWithLineChecks.numFixes += 1
        player.brain.nav.walkTo(dest, speed = nav.QUICK_SPEED)

    if player.counter > 300:
        return player.goLater('watchWithLineChecks')

    return Transition.getNextState(player, lineCheckReposition)

@superState('gameControllerResponder')
def goToGoalbox(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)
        player.brain.nav.walkTo(goToGoalbox.angleDest)


goToGoalbox.angleDest = RelRobotLocation(0,0,0)
goToGoalbox.dest = RelRobotLocation(0,0,0)

@superState('gameControllerResponder')
def returnUsingLoc(player):
    if player.firstFrame():
        dest = constants.HOME_POSITION
        player.brain.nav.goTo(dest,
                            speed = nav.QUICK_SPEED)
        print("I'm trying to return using loc!")
        player.brain.tracker.trackBall()
        returnUsingLoc.panning = False

    if (player.counter % 90 == 0):
        print("Switching headtracker")
        if not returnUsingLoc.panning:
            player.brain.tracker.repeatBasicPan()
        else:
            player.brain.tracker.trackBall

    if player.counter > 600:
        print "This is taking a suspiciously long time"
        return player.goLater('checkSafePlacement')

    return Transition.getNextState(player, returnUsingLoc)

@superState('gameControllerResponder')
def shiftPosition(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.inPosition = shiftPosition.dest
        print("H:", shiftPosition.dest.h)

        player.brain.nav.goTo(shiftPosition.dest,
                            speed = nav.QUICK_SPEED,
                            fast = False)

    if player.counter > 300:
        print "Took too long"
        return player.goLater('faceBall')
    # shiftPosition.dest.relH = player.brain.ball.bearing_deg
    # dest = player.brain.nav.destination

    return Transition.getNextState(player, shiftPosition)

shiftPosition.destb = constants.HOME_POSITION

@superState('gameControllerResponder')
def faceBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        print("ball at ", player.brain.ball.bearing_deg)
        facingDest = RelRobotLocation(0.0, 0.0, 0.0)

        facingDest.relH = player.brain.ball.bearing_deg
        if player.inPosition == constants.FAR_LEFT_POSITION:
            facingDest.relH += 10.0
        player.brain.nav.walkTo(facingDest, speed = nav.FAST_SPEED)

    if player.counter > 300 or player.brain.ball.vis.frames_off > 10:
        return player.goLater('watch')


    return Transition.getNextState(player, faceBall)

@superState('gameControllerResponder')
def watch(player):
    if player.firstFrame():
        player.brain.fallController.enabled = True
        player.brain.tracker.trackBall()
        # player.brain.tracker.repeatBasicPan()
        player.brain.nav.stand()
        player.returningFromPenalty = False
        if (player.lastState is not 'shiftPosition'
            and player.lastState is not 'faceBall'):
            player.inPosition = constants.HOME_POSITION
        print ("I'm moving to watch! I think I'm in the right position")
        # player.brain.tracker.lookToAngle(0)


#TestingChange
    if player.counter % 2 == 0 and savedebug:
        print("Horizon dist == ", player.brain.vision.horizon_dist)

        ball = player.brain.ball
        nball = player.brain.naiveBall

        print("Ball bearing:", ball.bearing_deg)
        print("Ball x:", ball.x)
        print("Ball y:", ball.y)
        print("Ball dist:", ball.distance)
        print("Ball dist:", ball.distance)
        print("ball.vis.frames_on", ball.vis.frames_on)
        print("nb xvel:", nball.x_vel)
    #     print("nb altxvel:", nball.alt_x_vel)
        print("ball mov vel:", ball.mov_vel_x)
    #     print("ball mov speed:", ball.mov_speed)
    #     print("stationary: ", nball.stationary)
    #     print("yintercept", nball.yintercept)
    # #     print("1", nball.x_v_1)
    # #     print("2", nball.x_v_2)
    # #     print("3", nball.x_v_3)
        print"- - -  -- - -- --- ---    - --"

    # return player.stay()
    return Transition.getNextState(player, watch)

def average(locations):
    x = 0.0
    y = 0.0
    h = 0.0

    for item in locations:
        x += item.relX
        y += item.relY
        h += item.relH

    if len(locations) == 0:
        return RelRobotLocation(0.0, 0.0, 0.0)

    return RelRobotLocation(x/len(locations),
                            y/len(locations),
                            h/len(locations))

def correct(destination):
    if math.fabs(destination.relX) < constants.STOP_NAV_THRESH:
        destination.relX = 0.0
    if math.fabs(destination.relY) < constants.STOP_NAV_THRESH:
        destination.relY = 0.0
    if math.fabs(destination.relH) < constants.STOP_NAV_THRESH:
        destination.relH = 0.0

    destination.relX = destination.relX / constants.OVERZEALOUS_ODO
    destination.relY = destination.relY / constants.OVERZEALOUS_ODO
    destination.relH = destination.relH / constants.OVERZEALOUS_ODO

    return destination

@superState('gameControllerResponder')
def moveBackwards(player):
    if player.firstFrame():
        watchWithLineChecks.numFixes += 1
        player.brain.tracker.lookToAngle(0)
        # player.brain.tracker.trackBall
        player.brain.nav.walkTo(RelRobotLocation(-100.0, 0, 0))

#TestingChange
    if player.counter > 250:
        print("Walking backwards too long... switch to a different state!")
        return player.goLater('spinToRecover')

    return Transition.getNextState(player, moveBackwards)

@superState('gameControllerResponder')
def spinToRecover(player):
    if player.firstFrame():
        player.brain.resetGoalieLocalization()
        player.setWalk(0,0,15.0)

    if player.counter > 275:
        print("Too long... switch to a different state!")
        return player.goLater('returnUsingLoc')

    return Transition.getNextState(player, spinToRecover)

@superState('gameControllerResponder')
def kickBall(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stop()

    if player.counter is 20 and player.brain.ball.vis.on:
        player.executeMove(player.kick.sweetMove)

    if player.brain.ball.vis.frames_off > 10.0:
        print("I lost the ball! I'm returning to goal")
        return player.goLater('returnUsingLoc')

    if player.counter > 30 and player.brain.nav.isStopped():
        return player.goLater('didIKickIt')

    return player.stay()

@superState('gameControllerResponder')
def saveCenter(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.brain.tracker.lookToAngle(0)
        if SAVING:
            player.executeMove(SweetMoves.GOALIE_SQUAT)
        # else:
        #     player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
        #TESTINGCHANGE
  

    if player.counter > 40:
        if SAVING:
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            return player.goLater('upUpUP')
        else:
            return player.goLater('watch')

    return player.stay()


@superState('gameControllerResponder')
def upUpUP(player):
    if player.firstFrame():
        player.brain.fallController.enabled = True
        player.upDelay = 0

    if player.brain.nav.isStopped():
        #TODO testing change, put this back!!!
        #TESTINGCHANGE
        return player.goLater('watch')
    return player.stay()

@superState('gameControllerResponder')
def saveRight(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.brain.tracker.lookToAngle(0)
        if SAVING and DIVING:
            player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)
            player.brain.tracker.performHeadMove(HeadMoves.OFF_HEADS)
        # else:
        #     player.executeMove(SweetMoves.GOALIE_TEST_DIVE_RIGHT)

    if player.counter > 80:
        if SAVING and DIVING:
            # player.executeMove(SweetMoves.GOALIE_ROLL_OUT_RIGHT)
            return player.goLater('rollOut')
        else:
            return player.goLater('watch')

    return player.stay()

@superState('gameControllerResponder')
def saveLeft(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.brain.tracker.lookToAngle(0)
        if SAVING and DIVING:
            player.executeMove(SweetMoves.GOALIE_DIVE_LEFT)
            player.brain.tracker.performHeadMove(HeadMoves.OFF_HEADS)
        # else:
        #     player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)

    if player.counter > 80:
        if SAVING and DIVING:
            player.executeMove(SweetMoves.GOALIE_ROLL_OUT_LEFT)
            return player.goLater('rollOut')
            #TESTINGCHANGE
        else:
            return player.goLater('watch')

    return player.stay()

@superState('gameControllerResponder')
def rollOut(player):
    if player.brain.nav.isStopped():
        player.brain.fallController.enabled = True
        player.justDived = True
        return player.goLater('fallen')

    return player.stay()

# ############# PENALTY SHOOTOUT #############

@superState('gameControllerResponder')
def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.inKickingState = False
        player.returningFromPenalty = False
        player.brain.fallController.enabled = False
        player.stand()
        player.brain.tracker.trackBall()
        player.side = constants.LEFT
        player.isSaving = False
        player.penaltyKicking = True

    return player.stay()

@superState('gameControllerResponder')
def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.inKickingState = False
        player.returningFromPenalty = False
        player.brain.fallController.enabled = False
        player.stand()
        player.zeroHeads()
        player.isSaving = False
        player.lastStiffStatus = True

    return player.goLater('waitForPenaltySave')

@superState('gameControllerResponder')
def waitForPenaltySave(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stop()

    return Transition.getNextState(player, waitForPenaltySave)

@superState('gameControllerResponder')
def doDive(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.brain.tracker.performHeadMove(HeadMoves.OFF_HEADS)
        if doDive.side == constants.RIGHT:
            player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)
        elif doDive.side == constants.LEFT:
            player.executeMove(SweetMoves.GOALIE_DIVE_LEFT)
        else:
            player.executeMove(SweetMoves.GOALIE_SQUAT)
    return player.stay()

@superState('gameControllerResponder')
def squat(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        player.executeMove(SweetMoves.GOALIE_SQUAT)

    return player.stay()
