
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants


GOTO_BALL_REFIND_THRESH = 25
RE_TURN_THRESH = 10

BALL_RESCAN_THRESH = 3
BALL_RESPIN_THRESH = 60
def nothing(player):
    """
    Do nothing
    """

    #player.brain.motion.setDefaultPosition()


    if player.counter == 10:
        return player.goLater('done')
    return player.stay()

def track(player):

    player.brain.tracker.switchTo('tracking', player.brain.ball)
    print "bearing",player.brain.bearing
    return player.goLater('done')

def spinFindBall(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()
        player.brain.tracker.switchTo('nothing')
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

    player.brain.nav.stopWalking()
    player.brain.motion.stopBodyMoves()

    player.brain.tracker.switchTo('tracking',player.brain.ball)
    
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
            math.fabs(player.brain.ball.bearing) < 7.0):
            print  "AHAHAA! Found the ball"
            return player.goNow('approachBallFar')

        else:
            print  "Need to align to the ball"
            if not player.brain.motion.isWalkActive():
                player.setWalkTurnConfig()
                turn = motion.WalkTurn(player.brain.ball.bearing,30)
                player.brain.motion.setNextWalkCommand(turn)


    elif BALL_RESCAN_THRESH < player.brain.ball.framesOff < BALL_RESPIN_THRESH:
        print "Missing ball for 3< frames, scanning"
        scan1 = motion.HeadJointCommand(3.0, [ 65.0, 25.0], 0)
        scan2 = motion.HeadJointCommand(6.0, [-65.0, 25.0], 0)
        scan3 = motion.HeadJointCommand(3.0, [ 0.0, 25.0], 0)

        headScan = motion.HeadScanCommand([scan1, scan2, scan3], True)
        player.brain.motion.enqueue(headScan)

#start a pan
    elif player.brain.ball.framesOff >= BALL_RESPIN_THRESH:
        #spin
        return player.goLater("spinFindBall")

    return player.stay()

def approachBallFar(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('tracking',player.brain.ball)

    if (player.brain.ball.on):
        if player.brain.ball.bearing <7.:
            print "ball dist =",player.brain.ball.dist," bearing = ",player.brain.ball.bearing
            if player.brain.ball.dist < 50 and \
                    player.brain.ball.lastVisionDist < 50:
                return player.goLater('circleOnBallRight')
            else:
            #walk
                player.setWalkStraightConfig()
                player.brain.nav.walkStraight()
                return player.stay()
        else:
            return player.goNow('alignOnBall')

    elif (player.brain.ball.framesOff >= 15):
        print "can't see the ball anymore"
        return player.goLater("spinFindBall")

    return player.stay()


### Circle on ball
def circleOnBallRight(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.switchTo('nothing')
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
        player.setWalkSidewaysConfig()
        rightStep = motion.WalkSideways(-10.,30)
        player.brain.motion.setNextWalkCommand(rightStep)
        player.brain.tracker.switchTo('tracking',player.brain.ball)
        

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
        player.setWalkTurnConfig()
        turn = motion.WalkTurn(60.,30)
        player.brain.motion.setNextWalkCommand(turn)

    elif (not player.brain.motion.isWalkActive() and
          player.counter>10):
        return player.goNow('circleOnBallSideStepEnd')

    return player.stay()

def circleOnBallSideStepEnd(player):
    if player.firstFrame():
        player.setWalkSidewaysConfig()
        walk = motion.WalkSideways(-200.,30)
        player.brain.motion.setNextWalkCommand(walk)
        #player.sideStepBearing = player.brain.ball.bearing

    elif (not player.brain.motion.isWalkActive() and
          player.counter>10):
        return player.goLater('circleOnBallRight')

    if (math.fabs(player.brain.ball.bearing)<7 and 
        player.brain.ball.on):
        print "stopping cause bearing is really good"
        player.brain.motion.stopBodyMoves()
        return player.goLater('circleOnBallRight')


#    elif player.inlineWithGoal():
 #      player.brain.motion.stopBodyMoves()
  #     return player.goNow('approachBallClose') #approach ball close



    return player.stay()

def checkOnBallAlignment(player):
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.stopBodyMoves()
        player.brain.tracker.switch('nothing')

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
        player.brain.motion.stopBodyMoves()
        player.brain.tracker.switchTo('tracking',player.brain.ball)

    elif player.counter == 2:

        ballDist = 50.0
        if player.brain.ball.on:
            ballDist = player.brain.ball.dist

        print "Walking ",ballDist, " to ball"
        player.setWalkStraightConfig()
        walk = motion.WalkStraight(ballDist + 50.0,30)

        player.brain.motion.setNextWalkCommand(walk)

    if player.brain.ball.on:
        print "see ball - %g %g" % (player.brain.ball.dist, player.brain.ball.bearing)
        pass

    if not player.brain.ball.framesOff == 2:
        player.executeMove(SweetMoves.NEUT_HEADS)
        print "neut heads"

    if player.brain.ball.on and (player.brain.ball.dist > 70 or
                                            math.fabs(player.brain.ball.bearing) > 30 ):
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()
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

def bend(player):
    if player.firstFrame():
        player.brain.motion.setSupportMode(3)
        player.brain.motion.postGotoTorsoOrientation(30.,0.,2.,1)

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
    return player.stay()


def scan (player):
    if player.firstFrame():
        #do scan
        scan1 = motion.HeadJointCommand(3.0, [ 65.0, 25.0], 0)
        scan2 = motion.HeadJointCommand(6.0, [-65.0, 25.0], 0)
        scan3 = motion.HeadJointCommand(3.0, [ 0.0, 25.0], 0)

        headScan = motion.HeadScanCommand([scan1, scan2, scan3], True)
        player.brain.motion.enqueue(headScan)

    return player.stay()
###
# Game Controller States
###

def gamePlaying(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('nothing')
        player.brain.nav.stopWalking()
    return player.goNow("spinFindBall")
    
def gamePenalized(player):

    # GO TO PENALIZED POS
    if player.firstFrame():
        player.brain.tracker.switchTo('nothing')
        player.brain.motion.stopHeadMoves()
        player.brain.motion.stopBodyMoves()
        player.executeMove(SweetMoves.PENALIZED_POS)
        player.brain.nav.stopWalking()
        player.brain.tracker.switchTo('nothing')
    # kill joint stiffness
    return player.stay()

def gameInitial(player):
    # GO TO INITIAL POS
    if player.firstFrame():
        player.brain.tracker.switchTo('nothing')
        player.brain.nav.stopWalking()
        player.brain.motion.stopBodyMoves()
        player.brain.motion.stopHeadMoves()

    
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
        player.brain.tracker.switchTo('nothing')

    # SCAN HEADS, STAY STANDING
    if (player.firstFrame() or
        player.brain.ball.framesOff>10):
        player.brain.tracker.switchTo('nothing')
        scan1 = motion.HeadJointCommand(3., [ 65.0, 30.0], 0)
        scan2 =  motion.HeadJointCommand(2., [65.,10.], 0)
        scan3 = motion.HeadJointCommand(5.0, [-65.,10.],0)
        scan4 = motion.HeadJointCommand(2., [-65.0, 30.0], 0)
        scan5 = motion.HeadJointCommand(3., [ 0.0, 30.0], 0)

        headScan = motion.HeadScanCommand([scan5, scan1, scan2, scan3,scan4], True)
        player.brain.motion.enqueue(headScan)

    if player.brain.ball.on:
        player.brain.tracker.switchTo('tracking',
                                      player.brain.ball
                                      )
    return player.stay()

def gameReady(player):

    # SPIN, LOCALIZE, FIND POSITION ON FIELD
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.switchTo('nothing')
        player.brain.tracker.switchTo('nothing')
        player.brain.motion.stopHeadMoves()
        player.executeMove(SweetMoves.READY_POS)
    return player.stay()




def gameFinished(player):

    # SIT DOWN
    if player.firstFrame():
        player.brain.nav.stopWalking()
        player.brain.tracker.switchTo('nothing')
        player.brain.motion.stopBodyMoves()
        # kill joint stiffness
        player.brain.motion.stopHeadMoves()

    if player.counter == 2:
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()

