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
import noggin_constants as nogginConstants
import time
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
        player.brain.nav.chaseBallDeceleratingSpeed()
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

    return player.goNow('followPotentialField')

@superState('positionAndKickBall')
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
            player.setWalk(0, 0, 0)

        else:
            normalizer = Navigator.FAST_SPEED/(xComp**2 + yComp**2)**.5

            if ball.bearing_deg > constants.SHOULD_SPIN_TO_BALL_BEARING/2:
                hComp = copysign(Navigator.GRADUAL_SPEED, ball.bearing_deg)
            else:
                hComp = copysign(Navigator.GRADUAL_SPEED, ball.bearing_deg)

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
        player.brain.nav.destinationWalkTo(positionForKick.kickPose,
                                           Navigator.MEDIUM_SPEED)
        positionForKick.slowDown = False
    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        # slows down the walk when very close to the ball to stabalize motion kicking and to not walk over the ball
        if player.kick == kicks.M_LEFT_STRAIGHT or player.kick == kicks.M_RIGHT_STRAIGHT:
            if (not positionForKick.slowDown and 
                player.brain.ball.distance < constants.SLOW_DOWN_TO_BALL_DIST):
                positionForKick.slowDown = True
                player.brain.nav.destinationWalkTo(positionForKick.kickPose,
                                           Navigator.GRADUAL_SPEED)
            elif (positionForKick.slowDown and 
                player.brain.ball.distance >= constants.SLOW_DOWN_TO_BALL_DIST):
                positionForKick.slowDown = False
                player.brain.nav.destinationWalkTo(positionForKick.kickPose,
                                           Navigator.MEDIUM_SPEED)
            else:
                player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)
        else:
            player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)

    player.ballBeforeKick = player.brain.ball
    if transitions.ballInPosition(player, positionForKick.kickPose):
        if player.motionKick:
           return player.goNow('executeMotionKick')
        else:
            player.brain.nav.stand()
            return player.goNow('executeKick')

    return player.stay()

# TODO make hierarchical and put in its own states file
@superState('gameControllerResponder')
def prepareForPenaltyKick(player):
    """
    We're waiting here for a short time to psych out the opposing goalie,
    then turn very slightly if the flag is set.
    """
    if player.firstFrame():
        prepareForPenaltyKick.chase = False
        ball = player.brain.ball
        print "player.stateTime: ", player.stateTime
        #pseudo-random spin decision on which direction to kick
        now = time.time()
        if (int(now) % 2) == 0:
            player.penaltyKickRight = True
        else:
            player.penaltyKickRight = False

        print now
        print "Kicking Right? ", player.penaltyKickRight
        ball = player.brain.ball
        if player.penaltyKickRight:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y + 5, 0)
        else:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y - 5, 0)
        player.brain.nav.goTo(location, Navigator.PRECISELY, Navigator.MEDIUM_SPEED,
                              False, True, False, False)
    else:
        ball = player.brain.ball
        if player.penaltyKickRight:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y + 5, 0)
        else:
            location = RelRobotLocation(ball.rel_x - 10, ball.rel_y - 5, 0)
        player.brain.nav.updateDest(location)

    if prepareForPenaltyKick.chase:
        return player.stay()

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
        print "X: ", player.brain.ball.rel_x
        print "Y: ", player.brain.ball.rel_y
        player.brain.nav.stand()
        # prepareForPenaltyKick.chase = True
        return player.goNow('penaltyKickSpin')
    return player.stay()

@superState('gameControllerResponder')
def penaltyKickSpin(player):
    """
    Spin so that we change the heading of the kick
    """
    if player.firstFrame():
        penaltyKickSpin.speed = Navigator.SLOW_SPEED
        penaltyKickSpin.done = False
        penaltyKickSpin.threshCount = 0
        if player.penaltyKickRight:
            penaltyKickSpin.speed = penaltyKickSpin.speed * -1
        player.brain.nav.walk(0,0, penaltyKickSpin.speed)
        print "Spinning at speed: ", penaltyKickSpin.speed
    if penaltyKickSpin.done:
        print "X: ", player.brain.ball.rel_x
        print "Y: ", player.brain.ball.rel_y
        return player.stay()

    if player.penaltyKickRight:
        if player.brain.loc.h < -20:
            penaltyKickSpin.threshCount += 1
            if penaltyKickSpin.threshCount == 3:
                player.brain.nav.stand()
                print "stopped because right post: ", postBearing
                penaltyKickSpin.done = True
                #return player.stay()
                return player.goNow('positionForPenaltyKick')
        else:
            penaltyKickSpin.threshCount = 0
    else:
        if player.brain.loc.h > 20:
            penaltyKickSpin.threshCount += 1
            if penaltyKickSpin.threshCount == 3:
                player.brain.nav.stand()
                print "stopped because left post: ", postBearing
                penaltyKickSpin.done = True
                #return player.stay()
                return player.goNow('positionForPenaltyKick')
        else:
            penaltyKickSpin.threshCount = 0

    return player.stay()


@superState('gameControllerResponder')
def positionForPenaltyKick(player):
    """
    We're getting ready for a penalty kick
    """
    if player.firstFrame():
        positionForPenaltyKick.position = True
        positionForPenaltyKick.yes = False
        if player.brain.ball.rel_y > 0:
            player.kick = kicks.LEFT_SHORT_STRAIGHT_KICK
            print "Kicking with left"
        else:
            player.kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
            print "Kicking with right"

    if (transitions.shouldApproachBallAgain(player) or
        transitions.shouldRedecideKick(player)):
        print "Going Back to Chase"
        return player.goLater('approachBall')

    ball = player.brain.ball
    positionForPenaltyKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                                       ball.rel_y - player.kick.setupY,
                                                       0)
    #So we stand and wait for two seconds before actually positioning
    if player.stateTime < 2:
        return player.stay()
    elif positionForPenaltyKick.position:
        player.ballBeforeApproach = player.brain.ball
        player.brain.tracker.lookStraightThenTrack()
        positionForPenaltyKick.position = True
        player.brain.nav.goTo(positionForPenaltyKick.kickPose,
                              Navigator.PRECISELY,
                              Navigator.CAREFUL_SPEED,
                              False,
                              Navigator.ADAPTIVE)
        positionForPenaltyKick.position = False
    else:
        player.brain.nav.updateDest(positionForPenaltyKick.kickPose)

    if (transitions.ballInPosition(player, positionForPenaltyKick.kickPose) or
        player.brain.nav.isAtPosition()):
        positionForPenaltyKick.yes = True
        #return player.stay()
        player.brain.nav.stand()
        return player.goNow('executeKick')

    if positionForPenaltyKick.yes:
        print "ball relX: ", ball.rel_x
        print "ball relY: ", ball.rel_y

    return player.stay()
