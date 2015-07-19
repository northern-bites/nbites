def goodComm(player):
    off = checkCommDropOffs(player)
    return off == 0

def mediocreComm(player):
    off = checkCommDropOffs(player)
    return off == 1

def awfulComm(player):
    off = checkCommDropOffs(player)
    return off >= 2

def checkCommDropOffs(player):
    off = 0

    for mate in player.brain.teamMembers:
        if mate.playerNumber == 1 or mate.playerNumber == player.brain.playerNumber:
            continue
        if not mate.alive:
            off += 1
    
    return off
