"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import DribbleTransitions as dr_trans
from ..navigator import Navigator
from ..kickDecider import HackKickInformation as hackKick
from ..kickDecider import kicks
from objects import RelRobotLocation, Location
from math import fabs
import noggin_constants as nogginConstants

DRIBBLE_ON_KICKOFF = False

def chase(player):
    """
    Super State to determine what to do from various situations
    """

    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    else:
        return player.goNow('approachBall')

def kickoff(player):
    """
    Have the robot kickoff if it needs to kick it.
    Otherwise wait 10 seconds or wait for the ball to move.
    """
    if player.shouldKickOff:
        return player.goNow('chase')

    if player.firstFrame():
        kickoff.ballRelX = player.brain.ball.rel_x
        kickoff.ballRelY = player.brain.ball.rel_y

    if (player.brain.gameController.timeSincePlaying > 10 or
        fabs(player.brain.ball.rel_x - kickoff.ballRelX) > constants.KICKOFF_BALL_MOVE_THRESH or
        fabs(player.brain.ball.rel_y - kickoff.ballRelY) > constants.KICKOFF_BALL_MOVE_THRESH):
        return player.goNow('chase')

    return player.stay()

kickoff.ballRelX = "the relX position of the ball when we started"
kickoff.ballRelY = "the relY position of the ball when we started"

#def spinToBall(player):
    #if player.firstFrame():
        #player.brain.tracker.trackBall()
        #player.brain.nav.stand()

    #if transitions.shouldFindBall(player):
        #return player.goLater('chase')

    #if transitions.shouldStopSpinningToBall(player):
        #return player.goNow('approachBall')
    #else:
        #spinDir = player.brain.loc.spinDirToPoint(Location(player.brain.ball.x,
                                                           #player.brain.ball.y))

        #if fabs(player.brain.ball.bearing_deg) > constants.CHANGE_SPEED_THRESH:
            #speed = Navigator.GRADUAL_SPEED
        #else:
            #speed = Navigator.SLOW_SPEED
        #player.setWalk(0,0,spinDir*speed)
        #return player.stay()

def approachBall(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.shouldKickOff:
            player.brain.nav.chaseBall(Navigator.QUICK_SPEED, fast = True)
        else:
            player.brain.nav.chaseBall(fast = True)

    if (transitions.shouldFindBall(player)):
        return player.goLater('chase')

    if (transitions.shouldPrepareForKick(player) or
        player.brain.nav.isAtPosition()):
        player.inKickingState = True

        if player.shouldKickOff:
            if player.brain.ball.rel_y > 0:
                player.kick = kicks.LEFT_STRAIGHT_KICK
            else:
                player.kick = kicks.RIGHT_STRAIGHT_KICK
            return player.goNow('positionForKick')
        else:
            return player.goNow('prepareForKick')

    else:
        return player.stay()

def prepareForKick(player):
    if player.firstFrame():
        prepareForKick.hackKick = hackKick.KickInformation(player.brain)
        player.orbitDistance = player.brain.ball.distance
        player.brain.tracker.performKickPan(prepareForKick.hackKick.shouldKickPanRight())
        player.brain.nav.stand()
        return player.stay()

    prepareForKick.hackKick.collectData()

    if player.brain.ball.distance > 40:
        # Ball has moved away. Go get it!
        player.inKickingState = False
        return player.goLater('chase')
 
    # If loc is good, stop pan ASAP and do the kick
    # Loc is currently never accurate enough @summer 2012
    #  Might have to do it anyway if comm is always down.

    # If hackKickInfo has enough information already, prematurely end pan and kick.
    if player.brain.tracker.isStopped() or \
            prepareForKick.hackKick.hasEnoughInformation():
        prepareForKick.hackKick.calculateDataAverages()
        if hackKick.DEBUG_KICK_DECISION:
            print str(prepareForKick.hackKick)
        player.kick = prepareForKick.hackKick.shoot()
        if hackKick.DEBUG_KICK_DECISION:
            print str(player.kick)
        return player.goNow('orbitBall')

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        orbitBall.counter = 0
        orbitBall.increasedHeadingCount = 0
        if hackKick.DEBUG_KICK_DECISION:
            print "Orbiting at angle: ",player.kick.h

        if player.kick.h == 0:
            return player.goNow('positionForKick')

        # Reset from pre-kick pan to straight, then track the ball.
        player.brain.tracker.lookStraightThenTrack()
        orbitBall.desiredHeading = player.kick.h

        if player.kick.h < 0:
            #set y vel at 50% speed
            print "Turn to left, move right"
            player.brain.nav.walk(0, -.5, .15)
            orbitBall.orbitClockwise = False
        
        if player.kick.h > 0:
            #set y vel at 50% speed in opposite direction
            print "Turn to right, move left"
            player.brain.nav.walk(0, .5, -.15)
            orbitBall.orbitClockwise = True

    elif player.brain.nav.isStopped():
        player.shouldOrbit = False
        player.kick.h = 0
        if player.kick == kicks.ORBIT_KICK_POSITION:
            return player.goNow('prepareForKick')
        else:
            player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
            return player.goNow('positionForKick')

    #all of this is basically the same as in shoot() in hackKickInformation
    goalCenter = Location(nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                  nogginConstants.CENTER_FIELD_Y)
    ballLocation = Location(player.brain.ball.x, player.brain.ball.y)
    headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
    bearingForKick = headingBallToGoalCenter - player.brain.loc.h

    #the kick was chosen before we came into orbitBall()
    if player.kick == kicks.LEFT_STRAIGHT_KICK or player.kick == kicks.RIGHT_STRAIGHT_KICK:
        orbitBall.desiredHeading = 0 - bearingForKick
    elif player.kick == kicks.RIGHT_SIDE_KICK:
        orbitBall.desiredHeading = 70 - bearingForKick
    elif player.kick == kicks.LEFT_SIDE_KICK:
        orbitBall.desiredHeading = -70 -bearingForKick
    elif player.kick == kicks.LEFT_LONG_BACK_KICK or player.kick == kicks.RIGHT_LONG_BACK_KICK:
        if bearingForKick < -125:
            orbitBall.desiredHeading = -180 - bearingForKick
        else:
            orbitBall.desiredHeading = 180 - bearingForKick

    # #debugging
    # if orbitBall.counter%20 == 0:
    #     print "desiredHeading is:  | ", orbitBall.desiredHeading
    #     print "ball to goal center:| ", headingBallToGoalCenter
    #     print "player heading:     | ", player.brain.loc.h
    #     print "bearing for kick:   | ", bearingForKick
    #     print "walk is:            |  (",player.brain.nav.getXSpeed(),",",player.brain.nav.getYSpeed(),",",player.brain.nav.getHSpeed(),")" 
    #     print "=====================++++++++++"

    #our in-house heading checker is of the opinion that we're pointed in the right direction
    if orbitBall.desiredHeading > -5 and orbitBall.desiredHeading < 5:
        player.brain.nav.walk(0, 0, 0)
        print "Done orbiting, going to positionForKick"
        player.shouldOrbit = False
        player.kick.h = 0
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        return player.goNow('positionForKick')

    if player.stateTime > 8:
        print "In state orbitBall for too long, switching to chase"
        player.shouldOrbit = False
        player.brain.nav.walk(0, 0, 0)
        return player.goLater('chase')

    #These next three if statements might need some fine tuning
    #ATM that doesn't appear to be the case
    if player.orbitDistance < player.brain.ball.distance - 7:
        #We're too far away
        player.brain.nav.setXSpeed(.15)
        
    if player.orbitDistance > player.brain.ball.distance + 1:
        #We're too close
        player.brain.nav.setXSpeed(-.15)

    if player.orbitDistance > player.brain.ball.distance -1 and player.orbitDistance < player.brain.ball.distance +1:
        #print "We're at a good distance"
        player.brain.nav.setXSpeed(0)

    if (transitions.shouldFindBallKick(player) or
        transitions.shouldCancelOrbit(player)):
        player.inKickingState = False
        return player.goLater('chase')

    if orbitBall.orbitClockwise:
        if player.brain.ball.rel_y > 2:
            player.brain.nav.setHSpeed(0)
            #print "turn clockwise SLOWER"
        elif player.brain.ball.rel_y < 2:
            player.brain.nav.setHSpeed(-.2)
            #print "turn clockwise FASTER"
        else:
            player.brain.nav.setHSpeed(-.15)
            #print "turn clockwise NORMAL"
    else:
        if player.brain.ball.rel_y > 2:
            player.brain.nav.setHSpeed(.2)
            #print "turn counterclockwise FASTER"
        elif player.brain.ball.rel_y < 2:
            player.brain.nav.setHSpeed(0)
            #print "turn counterclockwise SLOWER"
        else:
            player.brain.nav.setHSpeed(.15)
            #print "turn clockwise NORMAL"

    #Keeps track of the number of frames in orbitBall
    orbitBall.counter = orbitBall.counter + 1
    return player.stay()

def positionForKick(player):
    """
    Get the ball in the sweet spot
    """
    if (transitions.shouldApproachBallAgain(player) or
        transitions.shouldRedecideKick(player)):
        player.inKickingState = False
        return player.goLater('chase')

    if not player.shouldKickOff or DRIBBLE_ON_KICKOFF:
        if dr_trans.shouldDribble(player):
            return player.goNow('decideDribble')

    ball = player.brain.ball
    kick_pos = player.kick.getPosition()
    positionForKick.kickPose = RelRobotLocation(ball.rel_x - kick_pos[0],
                                                ball.rel_y - kick_pos[1],
                                                0)

    #only enque the new goTo destination once
    if player.firstFrame():
        player.ballBeforeApproach = player.brain.ball
        # Safer when coming from orbit in 1 frame. Still works otherwise, too.
        player.brain.tracker.lookStraightThenTrack()
        #TODO: try getting rid of ADAPTIVE here, if ball estimates are good,
        #we don't need to lower the speed/shuffle to the ball
        player.brain.nav.goTo(positionForKick.kickPose,
                              Navigator.PRECISELY,
                              Navigator.GRADUAL_SPEED,
                              False,
                              Navigator.ADAPTIVE)
    else:
        player.brain.nav.updateDest(positionForKick.kickPose)

    if transitions.shouldFindBallKick(player) and player.counter > 15:
        player.inKickingState = False
        return player.goLater('chase')

    if (transitions.ballInPosition(player, positionForKick.kickPose) or
        player.brain.nav.isAtPosition()):
        print "DEBUG_SUITE: In 'positionForKick', either ballInPosition or nav.isAtPosition. Switching to 'kickBallExecute'."
        player.brain.nav.stand()
        return player.goNow('kickBallExecute')

    return player.stay()

# Currently not used as of 6/7/13.
# TODO: implement this again?
def lookAround(player):
    """
    Nav is stopped. We want to look around to get better loc.
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.stopHeadMoves() # HACK so that tracker goes back to stopped.
        player.brain.tracker.repeatBasicPan()

    # Make sure we leave this state...
    if player.brain.ball.vis.frames_off > 200:
        return player.goLater('chase')

    if player.brain.tracker.isStopped() and player.counter > 2:
            player.brain.tracker.trackBall()
            player.brain.kickDecider.decideKick()
            if transitions.shouldOrbit(player) and not player.penaltyKicking:
                print "Don't have a kick, orbitting"
                return player.goNow('orbitBall')
            else:
                return player.goLater('chase')

    return player.stay()
