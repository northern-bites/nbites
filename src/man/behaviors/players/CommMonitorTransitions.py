def goodComm(player):
    fromBeginning, after = checkCommDropOffs(player)
    return fromBeginning + after == 0

def mediocreComm(player):
    fromBeginning, after = checkCommDropOffs(player)
    return fromBeginning + after == 0

def awfulComm(player):
    fromBeginning, after = checkCommDropOffs(player)
    return fromBeginning + after == 1

def checkCommDropOffs(player):
    offCommFromTheBeginning = 0
    droppedOffComm = 0
    for mate in player.brain.teamMembers:
        if mate.playerNumber == 1 or mate.playerNumber == player.brain.playerNumber:
            continue
        if mate.framesWithoutPacket == -1:
            offCommFromTheBeginning += 1
        elif mate.framesWithoutPacket > 20*30:
            droppedOffComm += 1
        if not mate.alive:
            off += 1
    return offCommFromTheBeginning, droppedOffComm