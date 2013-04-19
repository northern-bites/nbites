#
# This contains the methods for testing different
# parts of goalie behaviors.  The tests change
# as necessary to test new parts of goalie behavior.
#

from .. import SweetMoves
import GoalieTransitions as goalTran

# ****************************
# Normal states to run through.
# ****************************

def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.executeMove(SweetMoves.INITIAL_POS)
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.executeMove(SweetMoves.INITIAL_POS)
        #player.brain.fallController.enableFallProtection(False)
        player.brain.tracker.trackBall()

    return player.stay()

def gameSet(player):
    player.brain.resetGoalieLocalization()
    return player.stay()

# To use a test put it in the goNow in this method
def gamePlaying(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.INITIAL_POS)
    return player.goNow('testSquatSave')

def gamePenalized(player):
    return player.stay()

# **************
# *** TESTS ****
# **************

def testSquatSave(player):
    ball=player.brain.ball
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    return player.stay()

def testHeat(player):
    ball = player.brain.ball
    if player.firstFrame():
        player.brain.tracker.trackBall()

    if player.counter % 10 == 0:
        print ball.vis.heat

    if ball.vis.heat >= 10:
        player.brain.speech.say("Save")

    return player.stay()

def testDive(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)

    return player.stay()

def testSaveDecision(player):
    ball = player.brain.ball

    if goalTran.shouldSave(player):
        return player.goNow('goalieSave')

    return player.stay()

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
