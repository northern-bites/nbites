
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
        player.walkPose()
        return player.stay()
    return player.goLater('lookState0')

# alternate testing path

def lookState0(player):
    brain = player.brain
    my = brain.my

    brain.tracker.printAngles()

    return player.stay()

def lookState1(player):
    brain = player.brain
    my = brain.my

    if player.counter > 100:
        return player.goLater('lookState2')
    elif player.counter > 50:
        brain.tracker.lookToAngles(.5,-.4)
        return player.stay()
    else:
        brain.tracker.lookToAngles(.5,.4)

    return player.stay()

def lookState2(player):
    brain = player.brain
    my = brain.my

    if player.counter > 100:
        return player.goLater('lookState3')
    elif player.counter > 50:
        brain.tracker.lookToAngles(.3,-1)
        return player.stay()
    else:
        brain.tracker.lookToAngles(.3,1)

    return player.stay()

def lookState3(player):
    brain = player.brain
    my = brain.my

    brain.tracker.printAngles()

    if player.counter > 140:
        return player.goLater('lookState5')
    elif player.counter > 70:
        brain.tracker.lookToAngles(.2,-2)
        return player.stay()
    else:
        brain.tracker.lookToAngles(.2,2)

    return player.stay()

def lookState5(player):
    brain = player.brain
    my = brain.my

    if player.counter > 140:
        return player.goLater('lookState6')
    elif player.counter > 70:
        brain.tracker.lookToAngles(-.4,-2)
        return player.stay()
    else:
        brain.tracker.lookToAngles(-.4,2)

    return player.stay()

def lookState6(player):
    brain = player.brain
    my = brain.my

    if player.counter > 100:
        return player.goLater('lookState7')
    elif player.counter > 50:
        brain.tracker.lookToAngles(-.5,-.7)
        return player.stay()
    else:
        brain.tracker.lookToAngles(-.5,.7)

    return player.stay()

def lookState7(player):
    brain = player.brain
    my = brain.my

    if player.counter > 100:
        return player.goLater('lookState8')
    elif player.counter > 50:
        brain.tracker.lookToAngles(-.6,-.3)
        return player.stay()
    else:
        brain.tracker.lookToAngles(-.6,.3)

    return player.stay()

def lookState8(player):
    brain = player.brain
    my = brain.my

    if player.counter > 100:
        return player.goLater('lookState9')
    elif player.counter > 50:
        brain.tracker.lookToAngles(-.7,-.2)
        return player.stay()
    else:
        brain.tracker.lookToAngles(-.7,.2)

    return player.stay()

def lookState9(player):
    brain = player.brain
    my = brain.my

    if player.counter > 100:
        return player.goLater('done')
    elif player.counter > 50:
        brain.tracker.lookToAngles(-.75,-.2)
        return player.stay()
    else:
        brain.tracker.lookToAngles(-.75,.2)

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
