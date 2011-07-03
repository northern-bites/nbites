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
        player.excuteMove(SweetMoves.INITIAL_POS)
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.excuteMove(SweetMoves.INITIAL_POS)
        #player.brain.fallController.enableFallProtection(False)
        player.brain.tracker.trackBall()
        #player.brain.tracker.activeLoc()

    return player.stay()

def gameSet(player):
    player.brain.resetGoalieLocalization()
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.INITIAL_POS)
    return player.goNow('testDX')

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

def testHeat(player):
    ball = player.brain.ball
    if player.firstFrame():
        player.brain.tracker.trackBall()

    if player.counter % 10 == 0:
        print ball.heat

    if ball.heat >= 10:
        player.brain.speech.say("Save")

    return player.stay()

def testDive(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)

    return player.stay()

def testDX(player):
    ball=player.brain.ball

    if player.firstFrame():
        player.brain.tracker.trackBall()

    print "dx"
    print ball.loc.dx
    print "relVelX"
    print ball.loc.relVelX
    print "heat"
    print ball.heat

    return player.stay()

def testSaveDecision(player):
    ball = player.brain.ball

    if goalTran.shouldSave(player):
        return player.goNow('goalieSave')

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
