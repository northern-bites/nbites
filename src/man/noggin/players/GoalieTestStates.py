#
#Tests for the different areas of goalie actions.
#

import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants
import GoalieTransitions as goalTran
import GoalieConstants as goalCon


def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.fallController.disable()
        player.brain.tracker.trackBall()
        #player.brain.tracker.activeLoc()
        player.standup()

    return player.stay()

def gameSet(player):
    player.brain.resetGoalieLocalization()
    return player.stay()

def gamePlaying(player):
    return player.goNow('testChase')

def gamePenalized(player):
    angles = player.brain.sensors.angles
    if player.firstFrame():
        print "((%.2f,%.2f,%.2f,%.2f),"%(angles[2],angles[3],angles[4],angles[5])
        print "(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f),"%(angles[6],angles[7],angles[8], \
                                                      angles[9],angles[10], \
                                                angles[11])
        print "(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f),"%(angles[12],angles[13],angles[14], \
                                                angles[15],angles[16], \
                                                angles[17])
        print "(%.2f,%.2f,%.2f,%.2f), ),"%(angles[18],angles[19],angles[20],angles[21])
        print " "
        print " "

    return player.stay()

def testDive(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)

    return player.stay()

def testDX(player):
    ball=player.brain.ball
    if ball.framesOn > 0:
        print "relX"
        print ball.relX
        print "relY"
        print ball.relY
        print "dx"
        print ball.dx
        print "dy"
        print ball.dy

    return player.stay()

def testSaveDecision(player):
    ball = player.brain.ball
 
    if ball.framesOn > 0 :
        if(ball.dx > 11 or ball.dx < -11):
            print "moving"
            if ball.relY > goalCon.CENTER_SAVE_THRESH:
                print "save right + 1"
                player.counterRightSave +=1
                if player.counterRightSave > 3:
                    print "save right"
                    player.counterRightSave = 0
                    player.counterLeftSave = 0
                    player.counterCenterSave = 0
            elif ball.relY < -goalCon.CENTER_SAVE_THRESH:
                print "save left + 1"
                player.counterLeftSave +=1
                if player.counterLeftSave > 3:
                    print "save left"
                    player.counterRightSave = 0
                    player.counterLeftSave = 0
                    player.counterCenterSave = 0
            elif( ball.relY > -goalCon.CENTER_SAVE_THRESH and
                  ball.relY < goalCon.CENTER_SAVE_THRESH):
                print "save center + 1"
                player.counterCenterSave +=1
                if player.counterCenterSave > 3:
                    print "save center"
                    player.counterRightSave = 0
                    player.counterLeftSave = 0
                    player.counterCenterSave = 0

       # if ball.relX < 85:
            #print "save now"
       # else:
           # print "save later"
    
    return player.stay()


# not using below here for now
def testDangerousBall(player):
    if player.counter % 100 == 0:
        if goalTran.dangerousBall(player):
            print "dangerous"
        else:
            print "not dangerous"

    return player.stay()

def testInBox(player):
    if player.counter % 100 == 0:
        if goalTran.goalieInBox(player):
            print "in"
        else:
            print "out"

    return player.stay()



def testShouldPositionRight(player):
    if player.counter % 100 == 0:
        if goalTran.shouldPositionRight(player):
            print "position right"
        elif goalTran.shouldPositionLeft(player):
            print "position left"    
        elif goalTran.shouldPositionCenter(player):
            print "postion center"

    return player.stay()

def testChase(player):
    player.isSaving = False
    player.penaltyKicking = False

    if goalTran.shouldChase(player):
        print "chase"
    else:
        print "staying"

    return player.stay()

def testStopChase(player):
    player.isChasing = True

    if goalTran.shouldStopChase(player):
        print "stop"
    else:
        print "not stopping"

    return player.stay()
