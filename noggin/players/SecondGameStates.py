
import math

import man.motion as motion
from man.motion import SweetMoves
from man.motion import MotionConstants


GOTO_BALL_REFIND_THRESH = 25
RE_TURN_THRESH = 10

BALL_RESCAN_THRESH = 3
BALL_RESPIN_THRESH = 60

USE_CIRCLING = False

def nothing(player):
    """
    Do nothing
    """
    
    #player.brain.motion.setDefaultPosition()


    if player.counter == 10:
        return player.goLater('done')
    return player.stay()

def track(player):

    player.brain.tracker.trackBall()
    print "bearing",player.brain.bearing
    return player.goLater('done')

def spinFindBall(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()
        player.brain.tracker.stopHead()
    elif not player.brain.ball.on and player.counter == 2:
    # Spin and adjust head to look for ball
        player.setWalkTurnConfig()
        player.brain.nav.spinLeft()
        heads = motion.HeadJointCommand(1.0,[60.,25.],0)
        player.brain.motion.enqueue(heads)

    elif player.brain.ball.on:
        return player.goNow('sawBall')
    return player.stay()

def sawBall(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()
        player.brain.tracker.trackBall()

    if not player.brain.motion.isWalkActive():
        return player.goNow('alignOnBall')

    print "waiting for previous walk to stop"
    return player.stay()

def alignOnBall(player):
    if player.firstFrame():
        if player.brain.motion.isWalkActive():
            player.brain.nav.stopWalking()
            player.brain.motion.stopBodyMoves()
            return player.stay()

    if player.brain.ball.on:
        print "ball bearing", player.brain.ball.bearing
        player.brain.tracker.switchTo('tracking', player.brain.ball)
        #later, we want to align on ball here

        if (not player.brain.motion.isWalkActive() and
            math.fabs(player.brain.ball.bearing) < 10.0):
            print  "AHAHAA! Found the ball"
            return player.goNow('approachBallFar')

        else:
            print  "Need to align to the ball"
            if not player.brain.motion.isWalkActive():
                print "aligning to ball"
                player.brain.nav.turnTo(player.brain.ball.bearing)

    elif BALL_RESCAN_THRESH < player.brain.ball.framesOff < BALL_RESPIN_THRESH:
        print "Missing ball for 3< frames, scanning"
        # scan for the ball
        player.brain.tracker.switchTo('scan')


    elif player.brain.ball.framesOff >= BALL_RESPIN_THRESH:
        #spin
        return player.goLater("spinFindBall")

    return player.stay()

def approachBallFar(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()
        return player.stay()

    if (player.brain.ball.on):
        if math.fabs(player.brain.ball.bearing) <7.:
            print "ball dist =",player.brain.ball.dist," bearing = ",player.brain.ball.bearing
            if player.brain.ball.dist < 50 and \
                    player.brain.ball.lastVisionDist < 50:
                return player.goLater('checkOwnGoal')
            else:
            #walk
                player.brain.nav.walkStraight()
                return player.stay()
        else:
            return player.goLater('alignOnBall')

    elif (player.brain.ball.framesOff >= 15):
        print "can't see the ball anymore"
        return player.goLater("spinFindBall")

    return player.stay()

def checkOwnGoal(player):
    if player.firstFrame():
        lookUp = motion.HeadJointCommand(0.5,[0.,0.],1)
        player.brain.motion.enqueue(lookUp)
        seenMyGoal = 0

    if (player.counter<7 and
        seenMyGoal<2):
        if (player.brain.myGoalCrossbar or
            player.brain.myGoalLeftPost or
            player.brain.myGoalRightPost):
            seenMyGoal+=1

    elif seenMyGoal>=2:
        return player.goLater('noOwnGoal')

    if USE_CIRCLING:
        return player.goLater('circleOnBallRight')
    else:
        return player.goLater('approachBallClose')

def noOwnGoal(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.trackBall()

    if (player.brain.ball.framesOff>20 or
        player.brain.ball.dist>70):
        return player.goLater('spinFindBall')
    print "DON'T KICK AN OWN GOAL"
    return player.stay()



### Circle on ball
def circleOnBallRight(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.stopHead()
        player.brain.motion.stopHeadMoves()
        player.seenGoal = 0

    if player.counter == 2:
        lookUp = motion.HeadJointCommand(2.0,[0.,0.],1)
        player.brain.motion.enqueue(lookUp)

    if 22< player.counter<=30:
        if (player.brain.oppGoalLeftPost.on or
            player.brain.oppGoalRightPost.on or
            player.brain.oppGoalCrossbar.on):
            print "i see opp goal"
            player.seenGoal += 1

        if (player.brain.myGoalLeftPost.on or
            player.brain.myGoalRightPost.on):
            print "i see my goal"
            player.seenGoal -= 3

    if player.counter==30:
        player.brain.motion.stopHeadMoves()
        lookDown = motion.HeadJointCommand(2.0,[0.,40.],1)
        player.brain.motion.enqueue(lookDown)

    if player.counter == 40:
        if player.seenGoal>=2:
            return player.goLater('approachBallClose')
        else:
            return player.goNow('circleOnBallSideStep')

    return player.stay()

def circleOnBallSideStep(player):
    if player.firstFrame():
        player.brain.nav.walkSidewaysTo(-10.)
        player.brain.tracker.trackBall()
        

    # step right
    elif player.shouldFindBall():
        return player.goLater('spinFindBall')

    elif player.brain.ball.dist > 70:
        return player.goLater('alignOnBall')


    elif (not player.brain.motion.isWalkActive() and
          player.counter>10):
        print "walk is not active, going to circle ball turn"
        return player.goLater('circleOnBallTurn')

    return player.stay()


def circleOnBallTurn(player):
    if player.shouldFindBall():
        return player.goLater('spinFindBall')

    elif player.brain.ball.dist > 70:
        return player.goLater('alignOnBall')

    #if player.inlineWithGoal():
     #   return player.goLater('approachBallClose') #approach ball close

    # turn left
    if player.firstFrame():
        player.brain.nav.turnTo(60.)

    elif (not player.brain.motion.isWalkActive() and
          player.counter>10):
        return player.goNow('circleOnBallSideStepEnd')

    return player.stay()

def circleOnBallSideStepEnd(player):
    if player.firstFrame():
        player.brain.nav.walkSidewaysTo(-200.)

    elif (not player.brain.motion.isWalkActive() and
          player.counter>10):
        return player.goLater('circleOnBallRight')

    if (math.fabs(player.brain.ball.bearing)<7 and 
        player.brain.ball.on):
        print "stopping cause bearing is really good"
        return player.goLater('circleOnBallRight')

    return player.stay()

def checkOnBallAlignment(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.stopHead()

        headYaw = player.brain.sensors.visionAngles[0]
        headPos = motion.HeadJointCommand(2.0,[0.,25.],1)
        player.brain.motion.enqueue(headPos)

    # Either - go to kick ball - or go back where you came from
    # Back where you came from, check if you come from here, if so
    # in that state, move on to the next in succession of circle states

    # Check which state you came from, go to next one according to
    # the results of your test - approachBallClose
    # go back to cirlcing - in circle state, check if we come from
    #
    return player.goNow(player.lastDiffState)

def approachBallClose(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.trackBall()

    elif player.counter == 2:

        ballDist = 50.0
        if player.brain.ball.on:
            ballDist = player.brain.ball.dist

        print "Walking ",ballDist + 50.0, " to ball"
        player.brain.nav.walkStraightTo(ballDist+20.)

    if player.brain.ball.on:
        print "see ball - %g %g" % (player.brain.ball.dist, player.brain.ball.bearing)
        pass

    if not player.brain.ball.framesOff == 2:
        player.executeMove(SweetMoves.NEUT_HEADS)
        print "neut heads"

    if player.brain.ball.on and (player.brain.ball.dist > 70 or
                                 math.fabs(player.brain.ball.bearing) > 30 ):
        player.brain.nav.stopWalking()
 
        print "switching to alignOnBall - %g %g" % (player.brain.ball.dist, player.brain.ball.bearing)
        return player.goLater('alignOnBall')

    if player.counter > 10 and not player.brain.motion.isWalkActive():
        player.switchTo("spinFindBall")

    """
    if player.brain.ball.framesOff > 100:#hack - delete later
        #need to go to 'walkThroughBall'/kick
        print "would walk through ball"
        return player.goLater('done')
        """
    return player.stay()


def done(player):
    """
    print "Ball width", player.brain.ball.width
    print "MY X,Y,H (%g,%g,%g)" % (player.brain.my.x,
                                   player.brain.my.y,
                                   player.brain.my.h)
    """
    player.brain.motion.stopBodyMoves()
    player.brain.nav.stopWalking()
    player.brain.tracker.stopHead()
    return player.stay()

""" Odocal
def walk(player):
    if player.firstFrame():
        player.setWalkTurnConfig()
        walk = motion.WalkStraight(200.0,30)
        player.brain.motion.setNextWalkCommand(walk)
    return player.stay()

def spin(player):
    if player.firstFrame():
        player.setWalkTurnConfig()
        turn = motion.WalkTurn(720.0,30)
        player.brain.motion.setNextWalkCommand(turn)
    return player.stay()
"""
def scan (player):
    if player.firstFrame():
        #do scan
        player.brain.tracker.switchTo('scan')

    return player.stay()
###
# Game Controller States
###

def gamePlaying(player):
    if player.firstFrame():
        player.brain.tracker.stopHead()
        player.brain.nav.stopWalking()
    return player.goNow("spinFindBall")
    #return player.goLater('spin')

def gamePenalized(player):

    # GO TO PENALIZED POS
    if player.firstFrame():
        player.brain.tracker.stopHead()
        player.brain.motion.stopBodyMoves()
        player.executeMove(SweetMoves.PENALIZED_POS)
        player.brain.nav.stopWalking()

    # kill joint stiffness
    return player.stay()

def gameInitial(player):
    # GO TO INITIAL POS
    if player.firstFrame():
        player.brain.tracker.stopHead()
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()


    
    if player.counter == 2:
        player.executeMove(SweetMoves.INITIAL_POS)
        player.executeMove(SweetMoves.NEUT_HEADS)
    """
    if (player.brain.oppGoalLeftPost.on or
        player.brain.oppGoalRightPost.on or
        player.brain.oppGoalCrossbar.on):
        print "i see opp goal"
        

    if (player.brain.myGoalLeftPost.on or
        player.brain.myGoalRightPost.on):
        print "i see my goal"
    """    


    #print "Ball angleX, angleY %g, %g" % (player.brain.ball.angleX,
    #                                      player.brain.ball.angleY)
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.motion.stopHeadMoves()
        player.brain.motion.stopBodyMoves()
        player.brain.tracker.stopHead()

    # SCAN HEADS, STAY STANDING
    if (player.firstFrame() or
        player.brain.ball.framesOff>10):
        player.brain.tracker.switchTo('scan')

    if player.brain.ball.on:
        player.brain.tracker.trackBall()


    return player.stay()

def gameReady(player):

    # SPIN, LOCALIZE, FIND POSITION ON FIELD
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.stopHead()
        player.executeMove(SweetMoves.READY_POS)
    return player.stay()
    #return player.goLater('walk')



def gameFinished(player):

    # SIT DOWN
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.stopHead()
        player.brain.motion.stopBodyMoves()
        # kill joint stiffness
        player.brain.motion.stopHeadMoves()

    if player.counter == 2:
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()

