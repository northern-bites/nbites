
def positionLocalize(player):

    return player.stay()

def playbookPosition(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.activeLoc()

    position = player.brain.playbook.position
    if player.brain.nav.destX != position[0] or \
            player.brain.nav.destY != position[1]:
        player.brain.nav.goTo(position[0], position[1])
    player.printf("position = "+str(position[0])+" , "+str(position[1]) )
    return player.stay()

def positionOnBall(player):
    nextX, nextY,nextH = player.getBehindBallPosition()
    player.printf("position = "+str(nextX)+" , "+str(nextY) )
    if player.brain.nav.destX != nextX or \
            player.brain.nav.destY != nextY:
        player.brain.nav.goTo(nextX,nextY,nextH)

    if player.brain.nav.isStopped():
        player.goLater('chase')

    return player.stay()

