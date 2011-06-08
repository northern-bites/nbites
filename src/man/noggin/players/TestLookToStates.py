
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
    return player.goLater('lookRightClose')

# alternate testing path

def lookRight2(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(1000,-500,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookRight1')
    return player.stay()

def lookRight1(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(1000,-250,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookCenterLong1')
    return player.stay()

def lookCenterLong1(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(1000,0,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookCenterShort')
    return player.stay()

def lookCenterShort(player):
    brain = player.brain
    brain.tracker.lookToPoint(200,0,0)
    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookCenterLong2')
    return player.stay()

def lookCenterLong2(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(1000,0,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeft1')
    return player.stay()

def lookLeft1(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(1000,250,0)

    if player.counter == 30:
        player.brain.tracker.stopHeadMoves()
        return player.goLater('lookLeft2')
    return player.stay()

def lookLeft2(player):
    brain = player.brain
    my = brain.my

    brain.tracker.lookToPoint(1000,500,0)

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
