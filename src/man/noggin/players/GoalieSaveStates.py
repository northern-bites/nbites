# These are the states for goalie saves.
# They should be able to dive right and left
# and save center.

import man.motion.SweetMoves as SweetMoves
import GoalieTransitions as helper
import GoalieConstants as goalCon

TESTING = True

def goalieSave(player):

    brain = player.brain
    ball = brain.ball

    if player.firstFrame():
        player.stopWalking()
        brain.tracker.trackBall()
        player.isSaving = True

    if helper.shouldSave(player):
        print "Saving because"
        print  "Ball.relVelX is" + str(ball.loc.relVelX)
        print  "And Ball.heat is" + str(ball.heat)
        brain.tracker.stopHeadMoves()
        brain.fallController.enableFallProtection(False)
        if TESTING:
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
    #add check for strafe in future

    return player.stay()

#moves left or right
#NEEDS WORK
#NOT USED RIGHT NOW
def saveStrafe(player):
    strafeDir = helper.strafeDirForSave(player)
    if fabs(strafeDir) > 0:
        player.setWalk(0, constants.STRAFE_SPEED * MyMath.sign(strafeDir), 0)
    else:
        player.stopWalking()


# REAL SAVES

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

def doneSaving(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(True)
        player.isSaving = False

    return player.stay()
