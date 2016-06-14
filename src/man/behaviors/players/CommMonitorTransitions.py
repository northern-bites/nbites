def goodComm(player):
    off = checkCommDropOffs(player)
    return off == 0

def mediocreComm(player):
    off = checkCommDropOffs(player)
    return off == 1

def awfulComm(player):
    return True
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

    # ------------- src/man/behaviors/players/CommMonitorTransitions.py -------------
    # index 68f5071..9ef7d5d 100644
    # @@ -1,26 +1,22 @@
    #  def goodComm(player):
    # -    fromBeginning, after = checkCommDropOffs(player)
    # -    return fromBeginning + after == 0
    # +    off = checkCommDropOffs(player)
    # +    return off == 0
     
    #  def mediocreComm(player):
    # -    fromBeginning, after = checkCommDropOffs(player)
    # -    return fromBeginning + after == 1
    # +    off = checkCommDropOffs(player)
    # +    return off == 1
     
    #  def awfulComm(player):
    # -    fromBeginning, after = checkCommDropOffs(player)
    # -    return fromBeginning + after >= 2
    # +    off = checkCommDropOffs(player)
    # +    return off >= 2
     
    #  def checkCommDropOffs(player):
    # -    offCommFromTheBeginning = 0
    # -    droppedOffComm = 0
    # +    off = 0
     
    #      for mate in player.brain.teamMembers:
    #          if mate.playerNumber == 1 or mate.playerNumber == player.brain.playerNumber:
    #              continue
    # -
    # -        if mate.framesWithoutPacket == -1:
    # -            offCommFromTheBeginning += 1
    # -        elif mate.framesWithoutPacket > 20*30:
    # -            droppedOffComm += 1
    # +        if not mate.alive:
    # +            off += 1
         
    # -    return offCommFromTheBeginning, droppedOffComm
    # +    return off