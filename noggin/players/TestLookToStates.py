
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
    player.standup()
    return player.goLater('lookRightClose')

def lookRightClose(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(my.x, my.y - 50, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookRightFar')
    return player.stay()

def lookRightFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(my.x, my.y - 200, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookStraightClose')
    return player.stay()

def lookStraightClose(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(my.x + 10, my.y, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookStraightFar')
    return player.stay()

def lookStraightFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(my.x + 200, my.y, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeftClose')
    return player.stay()

def lookLeftClose(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(my.x, my.y + 10, 0)

    if player.counter == 50:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeftFar')
    return player.stay()

def lookLeftFar(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(my.x, my.y + 200, 0)

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
