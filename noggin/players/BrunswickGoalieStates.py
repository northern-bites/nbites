from .. import NogginConstants as Constants
import man.motion.SweetMoves as SweetMoves

#arbitrary, currently same as AIBO
CENTER_SAVE_THRESH = 15.
BODY_SAVE_OFFSET_DIST_Y = 15
BALL_SAVE_LIMIT_TIME = 1.35
MOVE_TO_SAVE_DIST_THRESH = 200.


def goaliePosition(player):
    if player.shouldSave():
        return player.goNow('goalieSave')

    position = player.brain.playbook.position
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.activeLoc()
        player.printf("I am going to " + str(player.brain.playbook.position))
        player.brain.nav.goTo(position[0], position[1], Constants.OPP_GOAL_HEADING)

    if player.brain.nav.destX != position[0] or \
            player.brain.nav.destY != position[1]:
        player.brain.nav.goTo(position[0], position[1], Constants.OPP_GOAL_HEADING)
        #player.printf("position = "+str(position[0])+" , "+str(position[1]) )

    # we're at the point, let's switch to another state
    if player.brain.nav.isStopped() and player.counter > 0:
        return player.goLater('atPosition')

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
    if relX > CENTER_SAVE_THRESH:
        print "Should be saving right"
        return player.goNow('saveRight')
    elif relX < -CENTER_SAVE_THRESH:
        print "Should be saving left"
        return player.goNow('saveLeft')
    else:
        print "Should be saving center"
        return player.goNow('saveCenter')

def saveRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SAVE_RIGHT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_RIGHT):
        if player.shouldHoldSave():
            return player.goLater('holdSaveRight')
        else:
            return player.goLater('postSave')
    return player.stay()

def saveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SAVE_LEFT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_LEFT):
        if player.shouldHoldSave():
            return player.goLater('holdSaveLeft')
        else:
            return player.goLater('postSave')
    return player.stay()

def saveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SAVE_CENTER)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SAVE_CENTER):
        if player.shouldHoldSave():
            return player.goLater('holdSaveCenter')
        else:
            return player.goLater('postSave')
    return player.stay()

def postSave(player):
    if player.firstFrame():
        player.standup()
        player.brain.tracker.switchTo('locPans')
        roleState = player.getRoleState(player.currentRole)
    return player.goNow(roleState)


def shouldSave(self):
    ball = self.brain.ball

    if ball.on:
        relX = ball.relX
        relY = ball.relY
    else:
        relX = ball.locRelX
        relY = ball.locRelY

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX
    relVelY = ball.relVelY
    if relVelY < 0.0:
      timeUntilSave = (relY - BODY_SAVE_OFFSET_DIST_Y) / -relVelY
      anticipatedX = (relX + relVelX * (timeUntilSave - BALL_SAVE_LIMIT_TIME))
    else:
      timeUntilSave = -1
      anticipatedX = ball.x

    # No Time, Save now
    if (0 <= timeUntilSave < BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 5. and relVelY < 0.
        and relY < MOVE_TO_SAVE_DIST_THRESH):
        return True;
    return False;

def shouldHoldSave(self):
    ball = self.brain.ball

    if ball.on:
        relY = ball.relY
        print "hold save sees ball"
    else:
        relY = ball.locRelY
        print "hold save doesn't see ball"
    relVelY = ball.relVelY
    if relVelY < 0.0:
        timeUntilSave = (relY - BODY_SAVE_OFFSET_DIST_Y) / -relVelY
    else:
        timeUntilSave = -1
    print "holdsave time= ", timeUntilSave
    if 0 <= timeUntilSave < BALL_SAVE_LIMIT_TIME and relVelY < 0 and\
        relY < MOVE_TO_SAVE_DIST_THRESH:
        return True
    return False
    