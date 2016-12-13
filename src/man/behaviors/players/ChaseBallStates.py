"""
Here we house all of the state methods used for chasing the ball
"""
#
import SharedTransitions as shared
##
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import RoleConstants as roleConstants
import PlayOffBallTransitions as playOffTransitions
#
import PlayOffBallStates as playOffStates
##
from ..navigator import Navigator
#
from ..navigator import PID
from ..navigator import BrunswickSpeeds as speeds
import ClaimTransitions as claimTransitions
##
from ..kickDecider import KickDecider
from ..kickDecider import kicks
# from noggin_constants import MAX_SPEED, MIN_SPEED
# ^ this is uncommented in previous version
from ..util import *
from objects import RelRobotLocation, Location, RobotLocation
from math import fabs, degrees, radians, cos, sin, pi, copysign

@superState('gameControllerResponder')
@stay
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
#
@ifSwitchNow(claimTransitions.shouldCedeClaim, 'playOffBall')
##
def approachBall(player):
    if player.firstFrame():
        player.buffBoxFiltered = CountTransition(playOffTransitions.ballNotInBufferedBox,
                                                 0.8, 10)
#
        playOffStates.playerFourSearchBehavior.pointIndex = -1
        playOffStates.playerFiveSearchBehavior.pointIndex = -1
##
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
#    
    player.kick = player.decider.new2016KickStrategy()
##
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)

    ball = player.brain.ball

    # print "In walkToWayPoint"



# 
    if transitions.shouldDecelerate(player):
        # print "I should decelerate"
        speed = speeds.SPEED_SIX
    else:
        speed = speeds.SPEED_EIGHT
## Could fiddle with these speeds like in previous version with MIN_SPEED & MAX_SPEED
#  but would need to uncomment import at the top

    if fabs(relH) <= 50: #constants.MAX_BEARING_DIFF:
        wayPoint = RobotLocation(ball.x - constants.WAYPOINT_DIST*cos(radians(player.kick.setupH)),
                                 ball.y - constants.WAYPOINT_DIST*sin(radians(player.kick.setupH)),
                                 player.brain.loc.h)

        # print("Going to waypoint")

#
        player.brain.nav.goTo(wayPoint, Navigator.GENERAL_AREA, speed, True, fast = True)
## Navigator.CLOSE_ENOUGH in previous version
        if transitions.shouldSpinToKickHeading(player):
            return player.goNow('spinToKickHeading')

    else:
        player.brain.nav.chaseBall(speed, fast = True)

#
        if transitions.shouldPrepareForKick(player):
            return player.goLater('dribble')
## .goLater('positionAndKickBall') in previous version which makes way more sense!!
#  unsure why dribble here if we're preparing for a kick
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

#
    if fabs(relH) <= constants.FACING_KICK_ACCEPTABLE_BEARING:
        return player.goNow('dribble')
## .goNow('positionAndKickBall') in previous version which makes way more sense!!
#  unsure why dribble here if we're preparing for a kick

#
    if fabs(relH) <= constants.FACING_BALL_ACCEPTABLE_BEARING:
        speed = speeds.SPEED_FOUR
    elif fabs(relH) >= constants.MAX_BEARING_DIFF:
        speed = speeds.SPEED_EIGHT
    else:
        slope = (speeds.SPEED_EIGHT - speeds.SPEED_FOUR) / (constants.MAX_BEARING_DIFF - constants.FACING_BALL_ACCEPTABLE_BEARING)
        intercept = speeds.SPEED_EIGHT - slope*constants.MAX_BEARING_DIFF
        speed = slope*fabs(relH) + intercept
##

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
# 
            player.kick = player.decider.new2016KickStrategy()
##
            print("Decided kick: ", str(player.kick))
            return player.goNow('positionForKick')
        else:
            player.shouldKickOff = False
            # print("PREPAREFOREKICK THIS CASE")
#
            player.kick = player.decider.new2016KickStrategy()
##
        player.inKickingState = True
    
    elif player.finishedPlay:
        player.inKickOffPlay = False

    # only orbit is small orbit
    relH = player.decider.normalizeAngle(player.kick.setupH - player.brain.loc.h)
#
    if fabs(relH) < constants.SHOULD_ORBIT_BEARING and player.brain.loc.x < 800:
## didn't have second part of if in previous version
        return player.goNow('orbitBall')
#
    return player.goNow('dribble')
## Go now to particle field instead of dribble here


@superState('gameControllerResponder')
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
@ifSwitchNow(transitions.shouldFindBall, 'findBall')
def particleField(player):
    '''
    This state is based on electric field potential vector paths. The side of the ball that we'll
    kick has an attractive force and the opposite side is a repulsive force with a smaller magnitude
    '''
    if player.brain.nav.dodging:
        return player.stay()
	
    #Track ball in the first frame
    if player.firstFrame():
        player.brain.tracker.trackBall()
        particleField.xController.reset()
        particleField.hController.reset()
		
    ball = player.brain.ball
    heading = player.brain.loc.h
    relH = player.decider.normalizeAngle(player.kick.setupH - heading)
    #Establishes relative heading to ball
    
    if (transitions.shouldPositionForKick(player, ball, relH)): #If we should set up for a kick
        player.brain.nav.stand()
        destinationX = player.kick.destinationX
        destinationY = player.kick.destinationY
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
	player.kick.destinationX = destinationX
        player.kick.destinationY = destinationY
        return player.goNow('positionForKick')	
    else: #If we're not filling 
        attractorX = ball.rel_x - constants.ATTRACTOR_BALL_DIST*cos(radians(heading - player.kick.setupH))
	attractorY = ball.rel_y - constants.ATTRACTOR_BALL_DIST*sin(-radians(heading - player.kick.setupH))
	#Calculates the straight line distance to the attarctive point on the ball (hypotenuse)
	attractorDist = (attractorX**2 + attractorY**2)**0.5
	if attractorDist == 0
	    attractorDist = 0.000000001


	repulsorX = ball.rel_x - constants.REPULSOR_BALL_DIST*cos(radians(heading - player.kick.setupH))
        repulsorY = ball.rel_y - constants.REPULSOR_BALL_DIST*sin(-radians(heading - player.kick.setupH))
        #Calculates the straight line distance to the repulsive point on the ball (hypotenuse)
        repulsorDist = (repulsorX**2 + repulsorY**2)**0.5
        if repulsorDist == 0
            repulsorDist = 0.000000001

	#Define the actual particle field, an attractive one and a repulsive one
	xComp = constants.ATTRACTOR_REPULSOR_RATIO*attractorX/attractorDist**3 - repulsorX/repulsorDist**3	
	yComp = constants.ATTRACTOR_REPULSOR_RATIO*attractorY/attractorDist**3 - repulsorY/repulsorDist**3

	if xComp == 0 and yComp == 0: #If no field present
	    player.setWalk(0,0,copysign(speeds.SPEED_EIGHT, ball.bearing_deg))
	else: #if the particle field is detected
	    normalizer = speeds.SPEED_SEVEN/(xComp**2 + yComp**2)**0.5
	    #Speed is set based on relative distance from attractive point
	    
	    if fabs(ball.bearing_deg) < 2*constants.FACING_KICK_ACCEPTABLE_BEARING:
		hComp = 0
	    elif attractorDist < constants.CLOSE_TO_ATTRACTOR_DIST: #If we're closer than the acceptable distance to repulsive particle
		hComp = copysign(speeds.SPEED_SEVEN, ball.bearing_deg)
	    else: #If we're not close enough to the repulsive particle
		hComp = copySign(speeds.SPEED_FIVE, ball.bearing_deg)

	    player.setWalk(normalizer*xComp, normalizer*yComp, hComp)	 	

	#MONEY SPOT RIGHT HERE ^^ PID WILL COME INTO PLAY IN SET WALK
	    # Calculate corrections in x and h using PID controller 
    	    xError = attractorDist - constants.LINE_UP_X #calculate error based on distance from attractive point, NOT ball center
            hError = player.brain.ball.bearing
	    if xError > 0 or hError > 0: #These are not actual thresholds. We couldn't test so couldn't determine the actual thresholds
		player.goNow('particleFieldCorrection') 
	    
	# If close enough to ball, go orbit
        if player.brain.ball.distance < constants.LINE_UP_X:
            player.setWalk(0, 0, 0)
            return player.goLater('orbitBall')



	return player.stay()
	
def particleFieldCorrection(player):

    xError = attractorDist - constants.LINE_UP_X #calculate error based on distance from attractive point, NOT ball center
    hError = player.brain.ball.bearing 
    
    xSpeedCorrect = particleField.xController.correct(xError)
    hSpeedCorrect = particleField.hController.correct(hError)
	
    player.setWalk(xSpeedCorrect, 0, hSpeedCorrect)

    if xError == 0 and hError == 0: #If we've compensated for the error using the PID controller sufficiently
    	return player.goNow('particleField')
    return player.stay()	
	
	
	
# PID controllers used in Particle Field
particleField.xController = PID.PIDController(constants.LINE_UP_XP, constants.LINE_UP_XI, constants.LINE_UP_XD)
particleField.hController = PID.PIDController(constants.LINE_UP_HP, constants.LINE_UP_HI, constants.LINE_UP_HD)



###### ^^^^ look into the constants and changing them to patricle field and understanding them better



##########################################################################################
##########################################################################################
# This is where particle field function goes in
'''
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
'''

##################################################################################################
##################################################################################################

#####
#@superState('gameControllerResponder')
#@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
#@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
#@ifSwitchNow(transitions.shouldReturnHome, 'playOffBall')
#@ifSwitchNow(transitions.shouldFindBall, 'findBall')
##### Switch these to above function since with potential field orbit is now support fcn not the main one!

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
#
#	return player.goNow('positionForKick')
##
        player.setWalk(0, 0, 0)
##	^ why??

        # return player.goLater('positionForKick')
        if transitions.shouldNotDribble(player):
            return player.goNow('positionForKick')
        return player.goNow('dribble')

    if (transitions.orbitTooLong(player) or
        transitions.orbitBallTooFar(player)):
        return player.goLater('approachBall')

    # Orbit in correct direction at constant speed
    ySpeed = min(constants.ORBIT_Y_SPEED, fabs(orbitBall.yController.correct(relH)))
    if relH > 0: ySpeed = -ySpeed

    ySpeed = ySpeed * 1.5

    # Calculate corrections in x and h using PID controller 
    xError = player.brain.ball.distance - constants.ORBIT_X
    hError = player.brain.ball.bearing
    xSpeedCorrect = orbitBall.xController.correct(xError)
    hSpeedCorrect = orbitBall.hController.correct(hError)


    hSpeedCorrect = hSpeedCorrect * 1.3

    # Set walk vector
    player.setWalk(xSpeedCorrect, ySpeed, hSpeedCorrect)

    if constants.DEBUG_ORBIT:
        print "ORBIT DEBUG:"
        print "xError: ", xError
        print "hError: ", hError
        print "xSpeedCorrect: ", xSpeedCorrect
        print "hSpeedCorrect: ", hSpeedCorrect

    return player.stay()

# PID controllers used in orbitBall
orbitBall.xController = PID.PIDController(constants.ORBIT_XP, constants.ORBIT_XI, constants.ORBIT_XD)
orbitBall.yController = PID.PIDController(constants.ORBIT_YP, constants.ORBIT_YI, constants.ORBIT_YD)
orbitBall.hController = PID.PIDController(constants.ORBIT_HP, constants.ORBIT_HI, constants.ORBIT_HD)

### ^^Why are these outside of the orbit ball function?? ^^

@superState('positionAndKickBall')
@ifSwitchNow(transitions.shouldSpinToKickHeading, 'particleField') #was orbitBall
@ifSwitchLater(transitions.shouldApproachBallAgain, 'approachBall')
@ifSwitchNow(transitions.shouldSupport, 'positionAsSupporter')
@ifSwitchLater(transitions.shouldFindBall, 'findBall')
def dribble(player):
    if transitions.shouldNotDribble(player):
        if player.lastDiffState == 'particleField':  #was orbitBall
            return player.goNow('approachBall')
        return player.goNow('particleField')  #was orbitBall
    ball = player.brain.ball
    relH = player.decider.normalizeAngle(player.brain.loc.h)
    if ball.distance < constants.LINE_UP_X and not (relH > -constants.ORBIT_GOOD_BEARING and
        relH < constants.ORBIT_GOOD_BEARING):
        player.setWalk(0, 0, 0)
        return player.goLater('orbitBall')  
	#significantly smaller orbit now using particle field! if we're close enough to orbit and within acceptable bearing
	#Change to particleField potentially?
	
    if player.brain.ball.vis == True:
        if transitions.inGoalBox(player):
            player.brain.nav.walk(0.8, 0, 0)
        else:
            player.brain.nav.goTo(Location(ball.x, ball.y), Navigator.GENERAL_AREA, speeds.SPEED_SIX)
    else:
        player.brain.nav.walk(0.6, 0, 0)
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
        print("I think I should redecide my kick!!")
        return player.goLater('approachBall')

    ball = player.brain.ball
    positionForKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                                ball.rel_y - player.kick.setupY,
                                                0)

    #print("")
    #print("Ball is: ", ball.rel_x, ball.rel_y)
    #print("Ball dist:", ball.distance)
    #print("My kick setup is: ", player.kick.setupX, player.kick.setupY)
    #print("Ball.rel_x:", ball.rel_x, "rel_y:", ball.rel_y)

    if player.firstFrame():
        # print("IN position for kick, should kick off: ", player.shouldKickOff)
        player.brain.tracker.lookStraightThenTrack()

        # if player.kick == kicks.M_LEFT_SIDE or player.kick == kicks.M_RIGHT_SIDE:
        #     positionForKick.speed = Navigator.GRADUAL_SPEED
        # else:
        positionForKick.speed = speeds.SPEED_TWO
        # print("In position for kick! Setting walk speed")
        # print("My location:", str(positionForKick.kickPose))




#
        player.brain.nav.goTo(positionForKick.kickPose, Navigator.CLOSE_ENOUGH,
                              speeds.SPEED_FIVE, True, fast = True, useLoc = False)
## Way more parameters here than before (just .kickPose and .speed)






        # player.brain.nav.walkTo(positionForKick.kickPose, speeds.SPEED_THREE)

        # player.setWalk()

        # player.brain.nav.destinationWalkTo(positionForKick.kickPose, 
        #                                     positionForKick.speed)

    elif player.brain.ball.vis.on: # don't update if we don't see the ball
        # print "positionForKick -- we don't see it"
        print "We saw the ball so update destination"
	player.brain.nav.updateDest(positionForKick.kickPose)
        # print("MY kickpose:", str(positionForKick.kickPose))
        # player.brain.nav.updateDestinationWalkDest(positionForKick.kickPose)

    #else:
	#print "We failed both cases! ahhh oh no"
    # print "positionForKick"

    # print "positionForKick"

    player.ballBeforeKick = player.brain.ball
    # print("Kickpose: ", str(positionForKick.kickPose))
    print "got past ball before kick line"
    if (transitions.ballInPosition(player, positionForKick.kickPose)
    or player.brain.nav.currentState is 'atPosition'):
        # print("The ball is in position! CHASEBALL")
        # print player.kick
	print "Im planning on kicking"
        player.shouldKickOff = False
	return player.goNow('dontMove')
        #return player.goLater('executeSweetKick')
        # if player.motionKick or True:
        #    return player.goNow('executeMotionKick')
        # elif player.kick.bhKickType or True:
        #     player.brain.nav.stand()
        #     return player.goLater('executeBHKick')
        # else:
        #     player.brain.nav.stand()
        #     return player.goLater('executeSweetKick')

    print "we finished this round!! yay"
    return player.stay()

def dontMove(player):
    if player.firstFrame():
	print("Made it to the sweetspot!")
    	timeToCompletion = player.brain.gameController.timeSincePlaying
	player.brain.nav.stand()
	print ("timeToCompletion:")
	print (round(timeToCompletion,3))
    return player.stay()

'''
1) Figure out where we go to particle filter whether that's in dribble or from prepare for kick
2) Decide on setWalk from PID or particle field (or both...?)  
3) Fix one of Dan's issues (At least address it)
4) Run with no errors
5) At least try to test on the field
'''
