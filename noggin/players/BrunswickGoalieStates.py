from .. import NogginConstants as Constants
import man.motion.SweetMoves as SweetMoves
CENTER_SAVE_THRESH = 15.

def goaliePosition(player):
    if player.shouldSave():
        return player.goNow('goalieSave')
    #for now we don't want the goalie trying to move
    return player.goLater('goalieAtPosition')
    '''
    
    position = player.brain.playbook.position
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()
        #keep constant x,y change signs only
        player.brain.nav.setWalk(x,y,0)
    if player.brain.nav.destX != position[0] or \
            player.brain.nav.destY != position[1]:
        player.brain.nav.setWalk(x,y,0)

    # we're at the point, let's switch to another state
    if player.brain.nav.isStopped() and player.counter > 0:
        return player.goLater('goalieAtPosition')

    return player.stay()
    '''

def goalieAtPosition(player):
    """
    State for when we're at the position
    """
    if player.shouldSave():
        return player.goNow('goalieSave')
    if player.brain.playbook.subRole == "GOALIE_CHASER":
        #there is chase positioning in playbook, but if the goalie
        #needs to be chasing then ball is close enough that relative position
        #should be better
        pass
        #return player.goNow('chase')
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    return player.stay()

def goalieSave(player):
    ball = player.brain.ball
    # Figure out where the ball is going and when it will be there
    if ball.on:
      relX = ball.relX
      relY = ball.relY
    else:
      relX = ball.locRelX
      relY = ball.locRelY

    # Decide the type of save
    if relY > CENTER_SAVE_THRESH:
        print "Should be saving right"
        return player.goNow('saveLeft')
    elif relY < -CENTER_SAVE_THRESH:
        print "Should be saving left"
        return player.goNow('saveRight')
    else:
        print "Should be saving center"
        return player.goNow('saveCenter')

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
        player.executeMove(SweetMoves.SAVE_CENTER_DEBUG)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_CENTER_DEBUG):
        return player.goLater('holdCenterSave')
    return player.stay()

def holdRightSave(player):
    if player.shouldHoldSave():
        player.executeMove(Constants.SAVE_RIGHT_HOLD_DEBUG)
    else:
        return player.goLater('postSave')
    return player.stay()

def holdLeftSave(player):
    if player.shouldHoldSave():
        player.executeMove(Constants.SAVE_LEFT_HOLD_DEBUG)
    else:
        return player.goLater('postSave')
    return player.stay()

def holdCenterSave(player):
    if player.shouldHoldSave():
        player.executeMove(Constants.SAVE_CENTER_HOLD_DEBUG)
    else:
        return player.goLater('postSave')
    return player.stay()

def postSave(player):
    if player.firstFrame():
        player.standup()
        player.brain.tracker.trackBall()
        roleState = player.getRoleState(player.currentRole)
    return player.goNow(roleState)
