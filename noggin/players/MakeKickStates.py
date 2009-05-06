from man.motion import SweetMoves
from man.motion import StiffnessModes

def gameInitial(player):
    return player.goLater('standup')

def standup(player):
    if player.firstFrame():
        player.executeStiffness(StiffnessModes.MAKE_KICK_STIFFNESSES)
        player.executeMove(SweetMoves.STAND_FOR_KICK_LEFT)

    if player.counter == 30:
        return player.goLater('printKick')
    return player.stay()


def printKick(player):
    angles = player.brain.sensors.angles
    if not player.counter%70:
        print "((%.2f,%.2f,%.2f,%.2f),"%(angles[2],angles[3],angles[4],angles[5])
        print "(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f),"%(angles[6],angles[7],angles[8], \
                                                angles[9],angles[10], \
                                                angles[11])
        print "(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f),"%(angles[12],angles[13],angles[14], \
                                                angles[15],angles[16], \
                                                angles[17])
        print "(%.2f,%.2f,%.2f,%.2f), ),"%(angles[18],angles[19],angles[20],angles[21])
        print " "
        print " "

    return player.stay()
