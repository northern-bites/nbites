def gameInitial(player):
    return player.stay()

def gameReady(player):
    return player.stay()

def gameSet(player):
    return player.stay()

def gamePlaying(player):
    printField(testKickDecider(player.brain,makeField()))
    return player.stay()

def makeField():
    field = [[' ' for x in xrange(X_SIZE)] for y in xrange(Y_SIZE)]

    for y in xrange(Y_SIZE):
        for x in xrange(X_SIZE):
            if x == 0 or y == 0 or x == X_SIZE or y == Y_SIZE:
                field[y][x] = '-'
            if (x == 0 or x == X_SIZE) and (y == NEAR_POST or y == FAR_POST):
                field[y][x] = 'G'

    return field
                
def printField(field):
    for y in field:
        print y

def testKickDecider(brain, field):
    decider = KickDecider2(brain,1,1,1,0,1)

    for ballY in xrange(Y_SIZE):
        for ballX in xrange(X_SIZE):
            for playerY in xrange(Y_SIZE):
                for playerX in xrange(X_SIZE):
                    decider.addTestData(playerX,playerY,ballX,ballY)
                    kick = decider.pBrunswick()

                    field[ballY][ballX] = 'B'
                    field[playerY][playerX] = 'P'
                    field[kick.setup.y][kick.setup.x] = 'S'
                    field[kick.dest.y][kick.dest.x] = 'D'

    return field
