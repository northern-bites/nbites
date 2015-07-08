"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import RoleConstants as roleConstants
import PlayOffBallTransitions as playOffTransitions
from ..navigator import Navigator
from ..kickDecider import KickDecider
from ..kickDecider import kicks
from ..util import *
from objects import RelRobotLocation, Location, RobotLocation
from math import fabs, degrees, cos, sin, pi, radians, copysign

@superState('gameControllerResponder')
@stay
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
def approachBall(player):
    if player.firstFrame():
        player.buffBoxFiltered = CountTransition(playOffTransitions.ballNotInBufferedBox,
                                                 0.8, 10)
        player.brain.tracker.trackBall()
        if player.shouldKickOff:
            if player.inKickOffPlay:
                return player.goNow('giveAndGo')
            else:
                return player.goNow('positionAndKickBall')

        elif player.penaltyKicking:
            return player.goNow('prepareForPenaltyKick')
        else:
            player.brain.nav.chaseBall(Navigator.FAST_SPEED, fast = True)

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
        return player.goNow('positionAndKickBall')
    
    elif transitions.shouldDecelerate(player):
        player.brain.nav.chaseBall(Navigator.BRISK_SPEED, fast = True)
    else:
        player.brain.nav.chaseBall(Navigator.FAST_SPEED, fast = True)


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

    if not player.inKickOffPlay:
        if player.shouldKickOff or player.brain.gameController.timeSincePlaying < 10:
            print "Overriding kick decider for kickoff!"
            player.shouldKickOff = False
            player.kick = player.decider.kicksBeforeBallIsFree()
        else:
            if roleConstants.isDefender(player.role):
                player.kick = player.decider.defender()
            else:
                player.kick = player.decider.attacker()
        player.inKickingState = True

    elif player.finishedPlay:
        player.inKickOffPlay = False

    #  TODO just to be safe since we are only motionkicking now
    player.motionKick = True
    # only orbit is small orbit
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)
    if fabs(relH) < constants.SHOULD_ORBIT_BEARING:
        return player.goNow('orbitBall')
    return player.goNow('followPotentialField')

@superState('gameControllerResponder')
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
def followPotentialField(player):
    """
    This state is based on electric field potential vector paths. The ball is treated as an
    attractive force where on the side that will be kicked. The opposite side is treated as 
    a repulsive force of smaller magnitude.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()  

    ball = player.brain.ball
    heading = player.brain.loc.h
    relH = player.decider.normalizeAngle(player.kick.setupH - heading)

    if (transitions.shouldPositionForKick(player, ball, relH)):
        player.brain.nav.stand()
        destinationX = player.kick.destinationX
        destinationY = player.kick.destinationY
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        player.kick.destinationX = destinationX
        player.kick.destinationY = destinationY
        return player.goNow('positionForKick')

    else:
        attractorX = ball.rel_x - constants.ATTRACTOR_BALL_DIST*cos(radians(heading - player.kick.setupH))
        attractorY = ball.rel_y - constants.ATTRACTOR_BALL_DIST*sin(-radians(heading - player.kick.setupH))
        attractorDist = (attractorX**2 + attractorY**2)**.5
        if attractorDist == 0:
            attractorDist = .00000000001

        repulsorX = ball.rel_x - constants.REPULSOR_BALL_DIST*cos(radians(heading - player.kick.setupH))
        repulsorY = ball.rel_y - constants.REPULSOR_BALL_DIST*sin(-radians(heading - player.kick.setupH))
        repulsorDist = (repulsorX**2 + repulsorY**2)**.5

        if repulsorDist == 0:
            repulsorDist = .00000000001

        # super position of an attractive potential field and arepulsive one
        xComp = constants.ATTRACTOR_REPULSOR_RATIO*attractorX/attractorDist**3 - repulsorX/repulsorDist**3
        yComp = constants.ATTRACTOR_REPULSOR_RATIO*attractorY/attractorDist**3 - repulsorY/repulsorDist**3

        if xComp == 0 and yComp == 0:
            player.setWalk(0, 0, copysign(Navigator.FAST_SPEED, ball.bearing_deg))

        else:
            normalizer = Navigator.FAST_SPEED/(xComp**2 + yComp**2)**.5

            if fabs(ball.bearing_deg) < constants.FACING_BALL_ACCEPTABLE_BEARING:
                hComp = 0
            elif attractorDist < constants.CLOSE_TO_ATTRACTOR_DIST:
                hComp = copysign(Navigator.FAST_SPEED, ball.bearing_deg)
            else:
                hComp = copysign(Navigator.MEDIUM_SPEED, ball.bearing_deg)
            
            player.setWalk(normalizer*xComp, normalizer*yComp, hComp)

    return player.stay()

@superState('positionAndKickBall')
def orbitBall(player):
    """
    State to orbit the ball
    """
    # Calculate relative heading every frame
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    # Are we within the acceptable heading range?
    if (relH > -constants.ORBIT_GOOD_BEARING and
        relH < constants.ORBIT_GOOD_BEARING):
        print "STOPPED! Because relH is: ", relH
        #player.stopWalking()
        destinationX = player.kick.destinationX
        destinationY = player.kick.destinationY
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        player.kick.destinationX = destinationX
        player.kick.destinationY = destinationY
        return player.goNow('positionForKick')

    if (transitions.orbitTooLong(player) or
        transitions.orbitBallTooFar(player)):
        #player.stopWalking()
        return player.goLater('approachBall')

    # Set our walk. Nav will make sure that we don't set duplicate speeds.
    if relH < 0:
        if relH < -20:
            player.setWalk(0, 0.7, -0.25)
        else:
            player.setWalk(0, 0.5, -0.15)
    elif relH > 0:
        if relH > 20:
            player.setWalk(0, -0.7, 0.25)
        else:
            player.setWalk(0, -0.5, 0.15)

    # DEBUGGING PRINT OUTS
    if constants.DEBUG_ORBIT and player.counter%20 == 0:
        print "desiredHeading is:  | ", player.kick.setupH
        print "player heading:     | ", player.brain.loc.h
        print "orbit heading:      | ", relH
        print "walk is:            |  (",player.brain.nav.getXSpeed(),",",player.brain.nav.getYSpeed(),",",player.brain.nav.getHSpeed(),")"
        print "==============================="

    # X correction
    if (constants.ORBIT_BALL_DISTANCE + constants.ORBIT_DISTANCE_FAR <
        player.brain.ball.distance): # Too far away
        player.brain.nav.setXSpeed(.15)
    elif (constants.ORBIT_BALL_DISTANCE - constants.ORBIT_DISTANCE_CLOSE >
          player.brain.ball.distance): # Too close
        player.brain.nav.setXSpeed(-.15)
    elif (constants.ORBIT_BALL_DISTANCE + constants.ORBIT_DISTANCE_GOOD >
          player.brain.ball.distance and constants.ORBIT_BALL_DISTANCE -
          constants.ORBIT_DISTANCE_GOOD < player.brain.ball.distance):
        player.brain.nav.setXSpeed(0)

    # H correction
    if relH < 0: # Orbiting clockwise
        if player.brain.ball.rel_y > 2:
            player.brain.nav.setHSpeed(0)
        elif player.brain.ball.rel_y < -2:
            if relH < -20:
                player.brain.nav.setHSpeed(-0.35)
            else:
                player.brain.nav.setHSpeed(-0.2)
        else:
            if relH < -20:
                player.brain.nav.setHSpeed(-0.25)
            else:
                player.brain.nav.setHSpeed(-0.15)
    else: # Orbiting counter-clockwise
        if player.brain.ball.rel_y > 2:
            if relH > 20:
                player.brain.nav.setHSpeed(0.35)
            else:
                player.brain.nav.setHSpeed(0.2)
        elif player.brain.ball.rel_y < -2:
            player.brain.nav.setHSpeed(0)
        else:
            if relH > 20:
                player.brain.nav.setHSpeed(0.25)
            else:
                player.brain.nav.setHSpeed(0.15)

    return player.stay()

@superState('positionAndKickBall')
def spinToBall(player):
    """
    spins to the ball until it is facing the ball 
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        print "spinning to ball"

    theta = degrees(player.brain.ball.bearing)
    spinToBall.isFacingBall = fabs(theta) <= constants.FACING_BALL_ACCEPTABLE_BEARING

    if spinToBall.isFacingBall:
        print "facing ball"
        return player.goLater('positionAndKickBall')

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
        return player.goLater('approachBall')

    ball = player.brain.ball
    positionForKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                                ball.rel_y - player.kick.setupY,
                                                0)

    if player.firstFrame():
        player.brain.tracker.lookStraightThenTrack()

        if player.kick == kicks.M_LEFT_SIDE or player.kick == kicks.M_RIGHT_SIDE:
            positionForKick.speed = Navigator.SLOW_SPEED
        else:
            positionForKick.speed = Navigator.MEDIUM_SPEED

        player.brain.nav.destinationWalkTo(positionForKick.kickPose, 
                                            positionForKick.speed)

    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)

    player.ballBeforeKick = player.brain.ball
    if transitions.ballInPosition(player, positionForKick.kickPose):
        if player.motionKick:
           return player.goNow('executeMotionKick')
        else:
            player.brain.nav.stand()
            return player.goNow('executeKick')

    return player.stay()