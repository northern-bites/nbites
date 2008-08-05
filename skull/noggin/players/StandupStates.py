


def nothing(player):
    """
    Do nothing
    """
    if player.counter == 2:
        return player.goLater('fallOver')
    return player.stay()

def fallOver(player):
    """
    The Nao falls over
    """
    if player.counter==0:
        player.brain.motion.moveRightLegTo(50,-50,-300,1.)

    inertialAngles = player.checkPosition()
    if inertialAngles[1]<30 and inertialAngles[1]>-30:
        if player.uncertainityCounter<10:
            player.uncertainityCounter += 1
            print "uncertainity counter is ", player.uncertainityCounter
            return player.stay()
        else:
            player.uncertainityCounter = 0
            
            if  inertialAngles[0]>-170 and inertialAngles[0]<-10:

                return player.goNow('standUp')
                
            elif inertialAngles[0]<170 and inertialAngles[0]>10:

                return player.goNow('rollOver')            
                
                # Lying on right side
            elif inertialAngles[0]>170 or inertialAngles[0]<-170:  
                
                return player.goNow('rollFromRight')
                
                
                # Lying on left side
            elif inertialAngles[0]>-10 and inertialAngles[0]<10:
                
                return player.goNow('rollFromLeft')
    return player.stay()
                
def rollOver(player):
    player.brain.motion.queueRollOver()
    return player.goNow('standUp')

def standUp(player):
    player.brain.motion.queueStandUp()
    return player.goNow('stop')

def rollFromRight(player):
    player.brain.motion.queueRollFromRight()
    
    return player.goNow('standUp')
  


def rollFromLeft(player):
    player.brain.motion.queueRollFromLeft()
    
    return player.goNow('standUp')
