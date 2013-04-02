# This is the set of states that the goalie enters
# when it is saving.  The states include testing states
# which keep the goalie from diving.  To have the goalie
# dive you need to turn the TESTING boolean to false.
#
# The states work by entering a super state that makes
# the decision about how to save.  From there the goalie
# enters one of three states (center, left, right). After
# saving the goalie enters a hold state.  They all do the
# same thing but we have different states so that the goalie
# knows what state it just came from so it knows how to get up.
# If the goalie dove it will then do the roll out of the dive
# and get up otherwise it will just get up.
#
# We disable fall protection at the beginning of saving
# because otherwise the goalie wont save.  We also have
# an isSaving boolean which keeps the goalie from switching
# roles while it is in the process of saving.  Both isSaving
# and fall protection are dealt with in the doneSaving state.

from .. import SweetMoves
import GoalieTransitions as helper
import GoalieConstants as goalCon

# Goalie will only dive when false *****
TESTING = False

def goalieSave(player):

    brain = player.brain
    ball = brain.ball

    if player.firstFrame():
        player.stopWalking()
        brain.tracker.trackBallFixedPitch()
        player.isSaving = True

    if helper.shouldSave(player):
        brain.tracker.stopHeadMoves()
        brain.fallController.enableFallProtection(False)
        if TESTING:
            print "Saving because"
            print  "Ball.relVelX is" + str(ball.loc.relVelX)
            print  "And Ball.heat is" + str(ball.vis.heat)
            if helper.shouldSaveRight(player):
                return player.goNow('testSaveRight')
            elif helper.shouldSaveLeft(player):
                return player.goNow('testSaveLeft')
            else:
                return player.goNow('testSaveCenter')
        else:
            if helper.shouldSaveRight(player):
                return player.goNow('saveRight')
            if helper.shouldSaveLeft(player):
                return player.goNow('saveLeft')
            else:
                return player.goNow('saveCenter')

    return player.stay()

# Strafe was created a while ago for the goalie
# I dont think it works entirely plus with the
# goalie dive we can pretty much cover the entire
# goal from the center without having to strafe.
# I left it here incase it was wanted in the future.
def saveStrafe(player):
    strafeDir = helper.strafeDirForSave(player)
    if fabs(strafeDir) > 0:
        player.setWalk(0, constants.STRAFE_SPEED * MyMath.sign(strafeDir), 0)
    else:
        player.stopWalking()


# REAL SAVES - states make goalie dive or squat

def saveRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_DIVE_RIGHT):
        return player.goLater('holdRightSave')
    return player.stay()

def saveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_DIVE_LEFT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_DIVE_LEFT):
        return player.goLater('holdLeftSave')
    return player.stay()

def saveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT):
        return player.goLater('holdCenterSave')
    return player.stay()

# TEST SAVES (JUST MOVE ARMS)

def testSaveRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_RIGHT)
    if (player.counter > goalCon.TEST_SAVE_WAIT and
        not helper.shouldHoldSave(player)):
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('doneSaving')
    return player.stay()

def testSaveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)
    if(player.counter > goalCon.TEST_SAVE_WAIT and
        not helper.shouldHoldSave(player)):
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('doneSaving')
    return player.stay()

def testSaveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
    if (player.counter > goalCon.TEST_SAVE_WAIT and
        not helper.shouldHoldSave(player)):
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('doneSaving')
    return player.stay()

# HOLD SAVE
# The goalie is allowed 5 seconds to hold the save
# so these states keep the goalie in position for
# five seconds (2011 rules)

def holdRightSave(player):
    if helper.shouldHoldSave(player):
        return player.stay()
    else:
        return player.goLater('rollOutRight')

    return player.stay()

def holdLeftSave(player):
    if helper.shouldHoldSave(player):
        return player.stay()
    else:
        return player.goLater('rollOutLeft')

    return player.stay()

def holdCenterSave(player):
    if helper.shouldHoldSave(player):
        return player.stay()
    else:
        return player.goLater('postCenterSave')

    return player.stay()

# POST SAVE

def rollOutRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_ROLL_OUT_RIGHT)
        return player.goLater('postDiveSave')

    return player.stay()

def rollOutLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_ROLL_OUT_LEFT)
        return player.goLater('postDiveSave')

    return player.stay()

# TODO: Get rid of the random numbers which decide stand up
# They have been tested and work but are not robust.

def postCenterSave(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)

    if player.counter == goalCon.SQUAT_WAIT:
        return player.goLater('doneSaving')

    return player.stay()

def postDiveSave(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.STAND_UP_BACK)

    if player.counter == goalCon.DIVE_WAIT:
        return player.goLater('doneSaving')

    return player.stay()

# Will remain here until role is changed
def doneSaving(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(True)
        player.isSaving = False

    return player.stay()
