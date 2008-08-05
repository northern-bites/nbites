
def locPans(player):
    ''' pans to the left '''
    
    motion = player.brain.motion

    #player.printf("sup")
    if motion.isHeadQueueEmpty():    
        motion.queueBadHeadPan()
    
        
        
    return player.stay()

def panLeftOnce(player):
    motion = player.brain.motion
    if player.firstFrame():
        motion.queuePanLeftOnce()

    if motion.isHeadQueueEmpty():    
        if player.lastDiffState == 'tracking':
            return player.goNow('tracking')

        return player.goLater('nothing')
    
    return player.stay()

def panRightOnce(player):
    motion = player.brain.motion
    if player.firstFrame():
        motion.queuePanRightOnce()

    if motion.isHeadQueueEmpty():
        if player.lastDiffState == 'tracking':
            return player.goNow('tracking')
        
        return player.goLater('nothing')
    
    return player.stay()
