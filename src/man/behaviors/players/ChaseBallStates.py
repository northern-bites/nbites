"""
Here we house all of the state methods used for chasing the ball
"""
import SharedTransitions as shared
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import RoleConstants as roleConstants
import PlayOffBallTransitions as playOffTransitions
import PlayOffBallStates as playOffStates
from ..navigator import Navigator
from ..navigator import PID
from ..navigator import BrunswickSpeeds as speeds
from ..kickDecider import KickDecider
from ..kickDecider import kicks
# from noggin_constants import MAX_SPEED, MIN_SPEED
from ..util import *
from objects import RelRobotLocation, Location, RobotLocation
from math import fabs, degrees, radians, cos, sin, pi, copysign

@superState('gameControllerResponder')
@stay
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
def approachBall(player):
    if player.firstFrame():
        player.buffBoxFiltered = CountTransition(playOffTransitions.ballNotInBufferedBox,
                                                 0.8, 10)
        playOffStates.playerFourSearchBehavior.pointIndex = -1
        playOffStates.playerFiveSearchBehavior.pointIndex = -1

    if player.brain.nav.dodging:
        return player.stay()

    player.brain.tracker.trackBall()
    if player.shouldKickOff:
        if player.inKickOffPlay:
            print "giveAndGo"
            return player.goNow('giveAndGo')
        else:
            print "positionAndKickBall"
            return player.goNow('positionAndKickBall')

    elif player.penaltyKicking:
        return player.goNow('prepareForPenaltyKick')
    else:
        return player.goNow('lineUpKick')

    return player.goLater('lineUpKick')

@defaultState('walkToWayPoint')
@superState('gameControllerResponder')
@stay
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
def lineUpKick(player):
    """ super state to walk around the ball and line up to kick"""
    pass

@superState('lineUpKick')
def walkToWayPoint(player):
    if player.brain.nav.dodging:
        return player.stay()

    if player.firstFrame():
        player.decider = KickDecider.KickDecider(player.brain)
        player.brain.tracker.trackBall()
    
    player.kick = player.decider.new2016KickStrategy()
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    ball = player.brain.ball

    # print "In walkToWayPoint"

    if transitions.shouldDecelerate(player):
        # print "I should decelerate"
        speed = speeds.SPEED_SIX
    else:
        speed = speeds.SPEED_EIGHT

    if fabs(relH) <= constants.MAX_BEARING_DIFF:
        wayPoint = RobotLocation(ball.x - constants.WAYPOINT_DIST*cos(radians(player.kick.setupH)),
                                 ball.y - constants.WAYPOINT_DIST*sin(radians(player.kick.setupH)),
                                 player.brain.loc.h)

        # print("Going to waypoint")
        player.brain.nav.goTo(wayPoint, Navigator.GENERAL_AREA, speed, True, fast = True)

        if transitions.shouldSpinToKickHeading(player):
            return player.goNow('spinToKickHeading')

    else:
        player.brain.nav.chaseBall(speed, fast = True)

        if transitions.shouldPrepareForKick(player):
            return player.goLater('dribble')

    return player.stay()

@superState('lineUpKick')
def spinToKickHeading(player):
    """
    spins to the ball until it is facing the ball 
    """
    if player.firstFrame():
        
        player.brain.tracker.trackBall()

    if player.brain.nav.dodging:
        return player.stay()

    # print "spinToKickHeading"

    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    if fabs(relH) <= constants.FACING_KICK_ACCEPTABLE_BEARING:
        return player.goNow('dribble')

    if fabs(relH) <= constants.FACING_BALL_ACCEPTABLE_BEARING:
        speed = speeds.SPEED_FOUR
    elif fabs(relH) >= constants.MAX_BEARING_DIFF:
        speed = speeds.SPEED_EIGHT
    else:
        slope = (speeds.SPEED_EIGHT - speeds.SPEED_FOUR) / (constants.MAX_BEARING_DIFF - constants.FACING_BALL_ACCEPTABLE_BEARING)
        intercept = speeds.SPEED_EIGHT - slope*constants.MAX_BEARING_DIFF
        speed = slope*fabs(relH) + intercept

    # spins the appropriate direction
    player.brain.nav.walk(0., 0., copysign(speed, relH))

    return player.stay()

@defaultState('prepareForKick')
@superState('gameControllerResponder')
@ifSwitchLater(transitions.shouldSpinToBall, 'spinToBall')
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
@ifSwitchLater(transitions.shouldFindBall, 'findBall')
def positionAndKickBall(player):
    """
    Superstate used to position for kick and kick the ball when close enough.
    """
    pass

@superState('positionAndKickBall')
def prepareForKick(player):
    if player.firstFrame():
        player.decider = KickDecider.KickDecider(player.brain)
        player.brain.nav.stand()
    # print "prepareForKick"

    if not player.inKickOffPlay:
        if player.shouldKickOff or player.brain.gameController.timeSincePlaying < 10:
            print "DIAGONAL Overriding kick decider for kickoff!"
            player.shouldKickOff = False
            player.kick = player.decider.new2016KickStrategy()
            print("Decided kick: ", str(player.kick))
            return player.goNow('positionForKick')
        else:
            player.shouldKickOff = False
            # print("PREPAREFOREKICK THIS CASE")
            player.kick = player.decider.new2016KickStrategy()
        player.inKickingState = True
    
    elif player.finishedPlay:
        player.inKickOffPlay = False

    # only orbit is small orbit
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)
    if fabs(relH) < constants.SHOULD_ORBIT_BEARING and player.brain.loc.x < 800:
        return player.goNow('orbitBall')
    return player.goNow('dribble')

@superState('positionAndKickBall')
def lineUp(player):
    """
    State to line up for orbit. Uses two PID controllers!
    """
    if player.firstFrame():
        lineUp.xController.reset()
        lineUp.hController.reset()

    if player.brain.nav.dodging:
        return player.stay()

    # print "lining up to orbit"

    # Calculate corrections in x and h using PID controller 
    xError = player.brain.ball.distance - constants.LINE_UP_X
    hError = player.brain.ball.bearing
    xSpeedCorrect = lineUp.xController.correct(xError)
    hSpeedCorrect = lineUp.hController.correct(hError)

    # Set walk vector
    # print "xSpeedCorrect: " + str(xSpeedCorrect)
    player.setWalk(xSpeedCorrect, 0, hSpeedCorrect)

    # If close enough to ball, go orbit
    if player.brain.ball.distance < constants.LINE_UP_X:
        player.setWalk(0, 0, 0)
        return player.goLater('orbitBall')

    return player.stay()

# PID controllers used in lineUp
lineUp.xController = PID.PIDController(constants.LINE_UP_XP, constants.LINE_UP_XI, constants.LINE_UP_XD)
lineUp.hController = PID.PIDController(constants.LINE_UP_HP, constants.LINE_UP_HI, constants.LINE_UP_HD)

# TODO PID controller for spin to ball
# TODO PID controller for positionForKick

@superState('positionAndKickBall')
def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.brain.nav.dodging:
        return player.stay()

    # Calculate relative heading every frame
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    # Are we within the acceptable heading range?
    if (relH > -constants.ORBIT_GOOD_BEARING and
        relH < constants.ORBIT_GOOD_BEARING):
        #player.stopWalking()
        destinationX = player.kick.destinationX
        destinationY = player.kick.destinationY
        # print("     ORBIT I'm in orbit and deciding my chooseAlignedKickFromKick")
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        # print("     ORBIT chosen kick:", str(player.kick))
        player.kick.destinationX = destinationX
        player.kick.destinationY = destinationY

        if transitions.shouldNotDribble(player):
            return player.goNow('positionForKick')
        return player.goNow('dribble')
        

    if (transitions.orbitTooLong(player) or
        transitions.orbitBallTooFar(player)):
        #player.stopWalking()
        return player.goLater('approachBall')

        
    xSpeed = 0.0
    ySpeed = 0.0
    hSpeed = 0.0

    # Set our walk. Nav will make sure that we don't set duplicate speeds.
    if relH < 0:
        if relH < -20:
            xSpeed = 0.0
            ySpeed = 1.0
            hSpeed = -0.45
            # player.setWalk(0, 0.7, -0.25)
        else:
            xSpeed = 0.0
            ySpeed = 0.6
            hSpeed = -0.25
            # player.setWalk(0, 0.5, -0.15)
    elif relH > 0:
        if relH > 20:
            xSpeed = 0.0
            ySpeed = -1.0
            hSpeed = 0.45
            # player.setWalk(0, -0.7, 0.25)
        else:
            xSpeed = 0.0
            ySpeed = -0.6
            hSpeed = 0.25
            # player.setWalk(0, -0.5, 0.15)

    # X correction
    if (constants.ORBIT_BALL_DISTANCE + constants.ORBIT_DISTANCE_FAR <
        player.brain.ball.distance): # Too far away
        xSpeed = orbitBall.X_SPEED
        # player.brain.nav.setXSpeed(orbitBall.X_SPEED)
    elif (constants.ORBIT_BALL_DISTANCE - constants.ORBIT_DISTANCE_CLOSE >
          player.brain.ball.distance): # Too close
        xSpeed = orbitBall.X_BACKUP_SPEED
        # player.brain.nav.setXSpeed(-orbitBall.X_BACKUP_SPEED)
    elif (constants.ORBIT_BALL_DISTANCE + constants.ORBIT_DISTANCE_GOOD >
          player.brain.ball.distance and constants.ORBIT_BALL_DISTANCE -
          constants.ORBIT_DISTANCE_GOOD < player.brain.ball.distance):
        xSpeed = 0.0
        # player.brain.nav.setXSpeed(0)

    # H correction
    if relH < 0: # Orbiting clockwise
        if player.brain.ball.rel_y > 2:
            hSpeed = -0.2
            # player.brain.nav.setHSpeed(0)
        elif player.brain.ball.rel_y < -2:
            if relH < -20:
                hSpeed = -0.35
                # player.brain.nav.setHSpeed(-0.35)
            else:
                hSpeed = -0.2
                # player.brain.nav.setHSpeed(-0.2)
        else:
            if relH < -20:
                hSpeed = -0.45
                # player.brain.nav.setHSpeed(-0.25)
            else:
                hSpeed = -0.2
                # player.brain.nav.setHSpeed(-0.15)
    else: # Orbiting counter-clockwise
        if player.brain.ball.rel_y > 2:
            if relH > 20:
                hSpeed = 0.35
                # player.brain.nav.setHSpeed(0.35)
            else:
                hSpeed = 0.2
                # player.brain.nav.setHSpeed(0.2)
        elif player.brain.ball.rel_y < -2:
            hSpeed = 0.2
            # player.brain.nav.setHSpeed(0)
        else:
            if relH > 20:
                hSpeed = 0.45
                # player.brain.nav.setHSpeed(0.25)
            else:

                hSpeed = 0.2
                # player.brain.nav.setHSpeed(0.15)

    player.setWalk(xSpeed, ySpeed, hSpeed)
    # print("     IN ORBIT: x:", xSpeed, " y:", ySpeed, " h:", hSpeed)
    # DEBUGGING PRINT OUTS
    if constants.DEBUG_ORBIT and player.counter%10 == 0:
        print "desiredHeading is:  | ", player.kick.setupH
        print "player heading:     | ", player.brain.loc.h
        print "orbit heading:      | ", relH
        print "walk is:            |  (",player.brain.nav.getXSpeed(),",",player.brain.nav.getYSpeed(),",",player.brain.nav.getHSpeed(),")"
        print "==============================="
        
    return player.stay()

orbitBall.X_SPEED = .35
orbitBall.X_BACKUP_SPEED = .2

@superState('positionAndKickBall')
@ifSwitchNow(transitions.shouldSpinToKickHeading, 'orbitBall')
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
@ifSwitchLater(transitions.shouldFindBall, 'findBall')
def dribble(player):
    if transitions.shouldNotDribble(player):
        print "It's no longer dribble time"
        return player.goNow('orbitBall')
    print "Dribble time"
    ball = player.brain.ball
    relH = player.decider.normalizeAngle(player.brain.loc.h)
    if ball.distance < constants.LINE_UP_X and not (relH > -constants.ORBIT_GOOD_BEARING and
        relH < constants.ORBIT_GOOD_BEARING):
        player.setWalk(0, 0, 0)
        return player.goLater('orbitBall')

    if player.brain.ball.vis == True:
        player.brain.nav.goTo(Location(ball.x, ball.y), Navigator.GENERAL_AREA, speeds.SPEED_TWO)
    else:
        player.brain.nav.walk(10, 0, 0)
    return player.stay()

@superState('positionAndKickBall')
def spinToBall(player):
    """
    spins to the ball until it is facing the ball 
    """
    if player.brain.nav.dodging:
        return player.stay()

    if player.firstFrame():
        player.brain.tracker.trackBall()

    # print "spinToBall"

    theta = degrees(player.brain.ball.bearing)
    spinToBall.isFacingBall = fabs(theta) <= 2*constants.FACING_KICK_ACCEPTABLE_BEARING

    if spinToBall.isFacingBall:
        return player.goLater('dribble')

    # spins the appropriate direction
    if theta < 0:
        player.brain.nav.walk(0., 0., -1*constants.FIND_BALL_SPIN_SPEED)
    else:
        player.brain.nav.walk(0., 0., constants.FIND_BALL_SPIN_SPEED)

    return player.stay()


@superState('positionAndKickBall')
def positionForKick(player):
    """
    Get the ball in the sweet spot
    """
    if transitions.shouldRedecideKick(player):
        print("I think I should redecide my kick!!")
        return player.goLater('approachBall')

    ball = player.brain.ball
    positionForKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                                ball.rel_y - player.kick.setupY,
                                                0)

    # print("")
    # print("Ball is: ", ball.rel_x, ball.rel_y)
    # print("Ball dist:", ball.distance)
    # print("My kick setup is: ", player.kick.setupX, player.kick.setupY)

    # print("Ball.rel_x:", ball.rel_x, "rel_y:", ball.rel_y)

    if player.firstFrame():
        # print("IN position for kick, should kick off: ", player.shouldKickOff)
        player.brain.tracker.lookStraightThenTrack()

        # if player.kick == kicks.M_LEFT_SIDE or player.kick == kicks.M_RIGHT_SIDE:
        #     positionForKick.speed = Navigator.GRADUAL_SPEED
        # else:
        positionForKick.speed = speeds.SPEED_TWO
        # print("In position for kick! Setting walk speed")
        # print("My location:", str(positionForKick.kickPose))
        player.brain.nav.goTo(positionForKick.kickPose, Navigator.CLOSE_ENOUGH,
                              speeds.SPEED_FIVE, True, fast = True, useLoc = False)
        # player.brain.nav.walkTo(positionForKick.kickPose, speeds.SPEED_THREE)

        # player.setWalk()

        # player.brain.nav.destinationWalkTo(positionForKick.kickPose, 
        #                                     positionForKick.speed)

    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        # print "positionForKick -- we don't see it"
        player.brain.nav.updateDest(positionForKick.kickPose)
        # print("MY kickpose:", str(positionForKick.kickPose))
        # player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)

    # print "positionForKick"

    # print "positionForKick"

    player.ballBeforeKick = player.brain.ball
    # print("Kickpose: ", str(positionForKick.kickPose))

    if (transitions.ballInPosition(player, positionForKick.kickPose)
    or player.brain.nav.currentState is 'atPosition'):
        # print("The ball is in position! CHASEBALL")
        # print player.kick
        player.shouldKickOff = False
        return player.goLater('executeSweetKick')
        # if player.motionKick or True:
        #    return player.goNow('executeMotionKick')
        # elif player.kick.bhKickType or True:
        #     player.brain.nav.stand()
        #     return player.goLater('executeBHKick')
        # else:
        #     player.brain.nav.stand()
        #     return player.goLater('executeSweetKick')

    return player.stay()
