import ChaseBallTransitions as transitions
from ..navigator import Navigator
from ..kickDecider import kicks
from ..util import *
from objects import RelRobotLocation
from random import randrange

@superState('gameControllerResponder')
def prepareForPenaltyKick(player):
    """
    We're waiting here for a short time to psych out the opposing goalie,
    then turn very slightly if the flag is set.
    """
    ball = player.brain.ball
    if player.firstFrame():
        print "player.stateTime: ", player.stateTime

        #We decide whether to kick left or right once!
        #pseudo-random spin decision on which direction to kick
        side = randrange(0, 1000)
        print (int(side) % 2)
        if (int(side) % 2) == 0:
            player.penaltyKickRight = True
        else:
            player.penaltyKickRight = False
        
        print "Kicking Right? ", player.penaltyKickRight
        
        #update the ball parameter loc with our calculations
        if player.penaltyKickRight:
            location = RelRobotLocation(ball.rel_x - 20, ball.rel_y + 10, 0)
        else:
            location = RelRobotLocation(ball.rel_x - 20, ball.rel_y - 10, 0)
        
        player.brain.nav.destinationWalkTo(location, Navigator.MEDIUM_SPEED)
    else:
        if player.penaltyKickRight:
            location = RelRobotLocation(ball.rel_x - 20, ball.rel_y + 10, 0)
        else:
            location = RelRobotLocation(ball.rel_x - 20, ball.rel_y - 10, 0)
        player.brain.nav.updateDestinationWalkDest(location)

    if (location.relX**2 + location.relY**2)**.5 < 15:
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
        #variable Assignment
        penaltyKickSpin.threshCount = 0

        penaltyKickSpin.speed = Navigator.MEDIUM_SPEED
        if player.penaltyKickRight:
            penaltyKickSpin.speed *= -1

        player.brain.nav.walk(0,0, penaltyKickSpin.speed)
        print "Spinning at speed: ", penaltyKickSpin.speed

    # To be used when we can input vision info 
    #visionLines = player.brain.visionLines
    #print "Accessing visionLines data and printing it: "
    #if penaltyKickSpin.numberOfLineValues:
    #    for i in range (0, visionLines.line_size()):
    #        print "VisionLine number: " + str(i)
    #        print "VisionLine id: " + str(visionLines.line(i).id)
    #        print "r value: "   + str(visionLines.line(i).inner.r  )
    #        print "t value: "   + str(visionLines.line(i).inner.t  )
    #        print "ep0 value: " + str(visionLines.line(i).inner.ep0)
    #        print "ep1 value: " + str(visionLines.line(i).inner.ep1)
    #    penaltyKickSpin.numberOfLineValues = False

    if player.penaltyKickRight:
        if player.brain.loc.h < -20: #<= (pi/2 - visionLines[0].inner.t):
            penaltyKickSpin.threshCount += 1
            if penaltyKickSpin.threshCount == 4:
                player.brain.nav.stand()
                print "Stopped because facing right" 
                player.kick = kicks.LEFT_STRAIGHT_KICK
                player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
                return player.goNow('positionForPenaltyKick')
        else:
            penaltyKickSpin.threshCount = 0
    else:
        if player.brain.loc.h > 20: #>= (pi/2 - visionLines[0].inner.t):
            penaltyKickSpin.threshCount += 1
            if penaltyKickSpin.threshCount == 4:
                player.brain.nav.stand()
                print "Stopped because facing left" 
                player.kick = kicks.LEFT_STRAIGHT_KICK
                player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
                return player.goNow('positionForPenaltyKick')
        else:
            penaltyKickSpin.threshCount = 0

    return player.stay()


@superState('gameControllerResponder')
def positionForPenaltyKick(player):
    """
    We're getting ready for a penalty kick
    """
    ball = player.brain.ball
    positionForPenaltyKick.kickPose = RelRobotLocation(ball.rel_x - player.kick.setupX,
                                                       ball.rel_y - player.kick.setupY,
                                                       0)

    if player.firstFrame():
        positionForPenaltyKick.position = True
        player.brain.nav.destinationWalkTo(positionForPenaltyKick.kickPose,
                                           Navigator.GRADUAL_SPEED)
    else:
        #print "Updating our destination to be in the kickpose"
        player.brain.nav.updateDest(positionForPenaltyKick.kickPose)

    if transitions.ballInPosition(player, positionForPenaltyKick.kickPose):
        player.brain.nav.stand()
        return player.goNow('executeKick')

    return player.stay()
