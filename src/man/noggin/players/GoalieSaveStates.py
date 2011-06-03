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

#going to want it to get in a squat to prepare at somepoint in here

    brain = player.brain
    if player.firstFrame():
        player.isSaving = True
        player.isChasing = False
        brain.motion.stopHeadMoves()
        player.stopWalking()
        brain.tracker.trackBall()

    if helper.shouldSave(player):
        return player.goNow('goaliePickSave')

    return player.stay()

    #need to check if havent saved and need to move

    #Right now do not need to move side to side for saving
    #strafeDir = helper.strafeDirForSave(player)
    # if fabs(strafeDir) > 0:
        #player.setWalk(0, constants.STRAFE_SPEED * MyMath.sign(strafeDir), 0)
    # else:
        # player.stopWalking()

def goaliePickSave(player):
    player.brain.fallController.enableFallProtection(False)

    if helper.shouldSaveRight(player):
        return player.goNow('saveRight')
    elif helper.shouldSaveLeft(player):
        return player.goNow('saveLeft')
    elif helper.shouldSaveCenter(player):
        return player.goNow('saveCenter')

    return player.stay()

def saveRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SAVE_RIGHT_DEBUG)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_RIGHT_DEBUG):
        return player.goLater('holdRightSave')
    return player.stay()

def saveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SAVE_LEFT_DEBUG)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_LEFT_DEBUG):
        return player.goLater('holdLeftSave')
    return player.stay()

def saveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT):
        return player.goLater('holdCenterSave')
    return player.stay()

def testSaveRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_RIGHT)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('goalieSave')
    return player.stay()

def testSaveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('goalieSave')
    return player.stay()

def testSaveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('goalieSave')
    return player.stay()

#not sure how to decide this yet
def holdRightSave(player):
    #if helper.shouldHoldSave(player):
    #else:
        return player.goLater('postDiveSave')
    #return player.stay()

def holdLeftSave(player):
    #if helper.shouldHoldSave(player):
    #else:
        return player.goLater('postDiveSave')
    #return player.stay()

def holdCenterSave(player):
    #if helper.shouldHoldSave(player):
    #else:
        return player.goLater('postCenterSave')
    #return player.stay()

def postCenterSave(player):
    if player.brain.nav.isStopped():
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
        player.isSaving = False
        return player.stay()

    return player.stay()
def postDiveSave(player):
    if player.brain.nav.isStopped():
        player.brain.fallController.enableFallProtection(True)
        player.isSaving = False
        return player.stay()
