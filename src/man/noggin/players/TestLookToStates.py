
def gameInitial(player):
    player.gainsOn()
    return player.stay()

def gameReady(player):
    return player.goLater('standup')

def gameSet(player):
    return player.goLater('standup')

def gamePlaying(player):
    return player.goLater('standup')

def gamePenalized(player):
    return player.goLater('standup')

def standup(player):
    player.gainsOn()
    if player.firstFrame():
        player.standup()
    return player.goLater('lookRightBackNear')

# alternate testing path

def lookRightBackNear(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(-200,-200,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookRightBackFar')
    return player.stay()

def lookRightBackFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(-200,-2000,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookCenter')
    return player.stay()

def lookCenterRight(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(2000,-800,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookCenter')
    return player.stay()

def lookCenter(player):
    brain = player.brain
    brain.tracker.lookToPoint(5000,0,0)
    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('done')
    return player.stay()

def lookCenterLeft(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(2000,800,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeftBackFar')
    return player.stay()

def lookLeftBackFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(-200,600,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeftBackNear')
    return player.stay()

def lookLeftBackNear(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(-200,200,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('done')
    return player.stay()
        

# end alternate testing path

def lookRightClose(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(10,-100, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookRightFar')
    return player.stay()

def lookRightFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(10,-2000, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookStraightClose')
    return player.stay()

def lookStraightClose(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(200, 0, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookStraightFar')
    return player.stay()

def lookStraightFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(2000, 0, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeftClose')
    return player.stay()

def lookLeftClose(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(10, 100, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeftFar')
    return player.stay()

def lookLeftFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(10, 2000, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('done')
    return player.stay()


def done(player):
    if player.firstFrame():
        player.brain.tracker.setNeutralHead()
        player.walkPose()
        return player.stay()
    return player.stay()
