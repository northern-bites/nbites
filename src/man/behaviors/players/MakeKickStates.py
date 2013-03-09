from .. import SweetMoves
from .. import StiffnessModes


def gamePlaying(player):
    if player.firstFrame():
        player.gainsOff()

    return player.goLater('printKick')


def gamePenalized(player):

    return player.goLater('printKick')


def printKick(player):
    angles = player.brain.sensors.angles
    if player.firstFrame():
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
