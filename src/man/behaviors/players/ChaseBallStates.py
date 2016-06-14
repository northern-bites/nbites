"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import RoleConstants as roleConstants
import PlayOffBallTransitions as playOffTransitions
from ..navigator import Navigator
from ..navigator import PID
from ..kickDecider import KickDecider
from ..kickDecider import kicks
from noggin_constants import MAX_SPEED, MIN_SPEED
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

    if player.brain.nav.dodging:
        return player.stay()

    player.brain.tracker.trackBall()
    if player.shouldKickOff:
        if player.inKickOffPlay:
            return player.goNow('giveAndGo')
        else:
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

    if transitions.shouldDecelerate(player):
        speed = MIN_SPEED
    else:
        speed = MAX_SPEED

    if fabs(relH) <= constants.MAX_BEARING_DIFF:
        wayPoint = RobotLocation(ball.x - constants.WAYPOINT_DIST*cos(radians(player.kick.setupH)),
                                 ball.y - constants.WAYPOINT_DIST*sin(radians(player.kick.setupH)),
                                 player.brain.loc.h)

        player.brain.nav.goTo(wayPoint, Navigator.CLOSE_ENOUGH, speed, True, fast = True)

        if transitions.shouldSpinToKickHeading(player):
            return player.goNow('spinToKickHeading')

    else:
        player.brain.nav.chaseBall(speed, fast = True)

        if transitions.shouldPrepareForKick(player):
            return player.goLater('positionAndKickBall')

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

    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    if fabs(relH) <= constants.FACING_KICK_ACCEPTABLE_BEARING:
        return player.goNow('positionForKick')

    if fabs(relH) <= constants.FACING_BALL_ACCEPTABLE_BEARING:
        speed = Navigator.GRADUAL_SPEED
    elif fabs(relH) >= constants.MAX_BEARING_DIFF:
        speed = Navigator.FAST_SPEED
    else:
        slope = (Navigator.FAST_SPEED - Navigator.GRADUAL_SPEED) / (constants.MAX_BEARING_DIFF - constants.FACING_BALL_ACCEPTABLE_BEARING)
        intercept = Navigator.FAST_SPEED - slope*constants.MAX_BEARING_DIFF
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

    if not player.inKickOffPlay:
        if player.shouldKickOff or player.brain.gameController.timeSincePlaying < 10:
            # print "Overriding kick decider for kickoff!"
            player.shouldKickOff = False
            player.kick = player.decider.new2016KickStrategy()
        else:
            player.shouldKickOff = False
            # print("PREPAREFOREKICK THIS CASE")
            player.kick = player.decider.new2016KickStrategy()
        player.inKickingState = True
    
    elif player.finishedPlay:
        player.inKickOffPlay = False

#Josh's code
    player.motionKick = True
    return player.goNow('lineUp')
#dans potential field stuff
    # # only orbit is small orbit
    # relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)
    # if fabs(relH) < constants.SHOULD_ORBIT_BEARING:
    #     return player.goNow('orbitBall')
    # return player.goNow('followPotentialField')

# @superState('gameControllerResponder')
# @ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
# @ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
# @ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
# @ifSwitchNow(transitions.shouldFindBall, 'findBall')
# def followPotentialField(player):
#     """
#     This state is based on electric field potential vector paths. The ball is treated as an
#     attractive force where on the side that will be kicked. The opposite side is treated as 
#     a repulsive force of smaller magnitude.
#     """
#     if player.brain.nav.dodging:
#         return player.stay()

#     if player.firstFrame():
#         player.brain.tracker.trackBall()  

#     ball = player.brain.ball
#     heading = player.brain.loc.h
#     relH = player.decider.normalizeAngle(player.kick.setupH - heading)

#     if (transitions.shouldPositionForKick(player, ball, relH)):
#         player.brain.nav.stand()
#         destinationX = player.kick.destinationX
#         destinationY = player.kick.destinationY
#         player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
#         player.kick.destinationX = destinationX
#         player.kick.destinationY = destinationY
#         return player.goNow('positionForKick')

#     else:
#         attractorX = ball.rel_x - constants.ATTRACTOR_BALL_DIST*cos(radians(heading - player.kick.setupH))
#         attractorY = ball.rel_y - constants.ATTRACTOR_BALL_DIST*sin(-radians(heading - player.kick.setupH))
#         attractorDist = (attractorX**2 + attractorY**2)**.5
#         if attractorDist == 0:
#             attractorDist = .00000000001

#         repulsorX = ball.rel_x - constants.REPULSOR_BALL_DIST*cos(radians(heading - player.kick.setupH))
#         repulsorY = ball.rel_y - constants.REPULSOR_BALL_DIST*sin(-radians(heading - player.kick.setupH))
#         repulsorDist = (repulsorX**2 + repulsorY**2)**.5

#         if repulsorDist == 0:
#             repulsorDist = .00000000001

#         # super position of an attractive potential field and arepulsive one
#         xComp = constants.ATTRACTOR_REPULSOR_RATIO*attractorX/attractorDist**3 - repulsorX/repulsorDist**3
#         yComp = constants.ATTRACTOR_REPULSOR_RATIO*attractorY/attractorDist**3 - repulsorY/repulsorDist**3

#         if xComp == 0 and yComp == 0:
#             player.setWalk(0, 0, copysign(MAX_SPEED, ball.bearing_deg))

#         else:
#             normalizer = Navigator.FAST_SPEED/(xComp**2 + yComp**2)**.5

#             if fabs(ball.bearing_deg) < 2*constants.FACING_KICK_ACCEPTABLE_BEARING:
#                 hComp = 0
#             elif attractorDist < constants.CLOSE_TO_ATTRACTOR_DIST:
#                 hComp = copysign(Navigator.FAST_SPEED, ball.bearing_deg)
#             else:
#                 hComp = copysign(Navigator.MEDIUM_SPEED, ball.bearing_deg)
            
#             player.setWalk(normalizer*xComp, normalizer*yComp, hComp)

#     return player.stay()

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

    # Calculate corrections in x and h using PID controller 
    xError = player.brain.ball.distance - constants.LINE_UP_X
    hError = player.brain.ball.bearing
    xSpeedCorrect = lineUp.xController.correct(xError)
    hSpeedCorrect = lineUp.hController.correct(hError)

    # Set walk vector
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

@superState('gameControllerResponder')
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
def orbitBall(player):
    """
    State to orbit the ball. Uses two PID controllers!
    """
    if player.firstFrame():
        orbitBall.xController.reset()
        orbitBall.hController.reset()

    if player.brain.nav.dodging:
        return player.stay()

    # Calculate relative heading every frame
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    # Check if within the acceptable heading range
    if (relH > -constants.ORBIT_GOOD_BEARING and
        relH < constants.ORBIT_GOOD_BEARING):
        print "STOPPED! RelH: ", relH

        destinationX = player.kick.destinationX
        destinationY = player.kick.destinationY
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        player.kick.destinationX = destinationX
        player.kick.destinationY = destinationY

        player.setWalk(0, 0, 0)
        return player.goLater('positionForKick')

    if (transitions.orbitTooLong(player) or
        transitions.orbitBallTooFar(player)):
        return player.goLater('approachBall')

    # Orbit in correct direction at constant speed
    ySpeed = min(constants.ORBIT_Y_SPEED, fabs(orbitBall.yController.correct(relH)))
    if relH > 0: ySpeed = -ySpeed

    # Calculate corrections in x and h using PID controller 
    xError = player.brain.ball.distance - constants.ORBIT_X
    hError = player.brain.ball.bearing
    xSpeedCorrect = orbitBall.xController.correct(xError)
    hSpeedCorrect = orbitBall.hController.correct(hError)

    # Set walk vector
    player.setWalk(xSpeedCorrect, ySpeed, hSpeedCorrect)

    if constants.DEBUG_ORBIT:
        print "ORBIT DEBUG:"
        print xError
        print hError
        print xSpeedCorrect
        print hSpeedCorrect

    return player.stay()

# PID controllers used in orbitBall
orbitBall.xController = PID.PIDController(constants.ORBIT_XP, constants.ORBIT_XI, constants.ORBIT_XD)
orbitBall.yController = PID.PIDController(constants.ORBIT_YP, constants.ORBIT_YI, constants.ORBIT_YD)
orbitBall.hController = PID.PIDController(constants.ORBIT_HP, constants.ORBIT_HI, constants.ORBIT_HD)

@superState('positionAndKickBall')
def spinToBall(player):
    """
    spins to the ball until it is facing the ball 
    """
    if player.brain.nav.dodging:
        return player.stay()

    if player.firstFrame():
        player.brain.tracker.trackBall()

    theta = degrees(player.brain.ball.bearing)
    spinToBall.isFacingBall = fabs(theta) <= 2*constants.FACING_KICK_ACCEPTABLE_BEARING

    if spinToBall.isFacingBall:
        return player.goLater('approachBall')

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

        # if player.kick == kicks.M_LEFT_SIDE or player.kick == kicks.M_RIGHT_SIDE:
        #     positionForKick.speed = Navigator.GRADUAL_SPEED
        # else:
        positionForKick.speed = MIN_SPEED

        player.brain.nav.destinationWalkTo(positionForKick.kickPose, 
                                            positionForKick.speed)

    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)

    player.ballBeforeKick = player.brain.ball
    if transitions.ballInPosition(player, positionForKick.kickPose):
        print player.kick
        return player.goLater('executeSweetKick')
        if player.motionKick or True:
           return player.goNow('executeMotionKick')
        elif player.kick.bhKickType or True:
            player.brain.nav.stand()
            return player.goLater('executeBHKick')
        else:
            player.brain.nav.stand()
            return player.goLater('executeSweetKick')

    return player.stay()
