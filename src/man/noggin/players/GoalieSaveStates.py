# These are the states for goalie saves.
#They should be able to dive right and left
#and save center however right now they only
#allow you to saveCenter which is the goalie
#squat.
import man.motion.SweetMoves as SweetMoves
import GoalieTransitions as helper

CENTER_SAVE_THRESH = 15

TESTING = False

def goalieSave(player):

    brain = player.brain
    ball = brain.ball

    if player.firstFrame():
        brain.tracker.stopHeadMoves()
        player.stopWalking()
        brain.tracker.trackBall()
        player.isSaving = True


    if helper.shouldSave(player):
        brain.fallController.enableFallProtection(False)
        print ball.endY
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
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('doneSaving')
    return player.stay()

def testSaveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('doneSaving')
    return player.stay()

def testSaveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('doneSaving')
    return player.stay()

# HOLD SAVE

def holdRightSave(player):
    if helper.shouldHoldSave(player):
        return player.stay()
    else:
        player.executeMove(SweetMoves.GOALIE_ROLL_OUT_RIGHT)
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
    if player.counter == 5:
        player.executeMove(SweetMoves.GOALIE_ROLL_OUT_RIGHT)
        return player.goLater('postDiveSave')

    return player.stay()

def rollOutLeft(player):
    if player.counter == 5:
        player.executeMove(SweetMoves.GOALIE_ROLL_OUT_LEFT)
        return player.goLater('postDiveSave')

    return player.stay()

def postCenterSave(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)

    if player.counter == 25:
        return player.goLater('doneSaving')

    return player.stay()

def postDiveSave(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.STAND_UP_BACK)

    if player.counter == 50:
        return player.goLater('doneSaving')

    return player.stay()

def doneSaving(player):
    if player.firstFrame():
        player.brain.fallController.enableFallProtection(True)
        player.isSaving = False

    return player.stay()
