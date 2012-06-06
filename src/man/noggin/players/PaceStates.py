from objects import RelRobotLocation

"""
States for the hacked defender to pace through.
"""

def pace(player):
    if player.firstFrame():
        player.pacingCounter = 0
        player.destinationArray = [#RelRobotLocation(7, 0, 0),
                            RelRobotLocation(0, -7, 0),
                            #RelRobotLocation(-7, 0, 0),
                            RelRobotLocation(0, 7, 0)]

    if player.brain.nav.isStopped():
        if player.pacingCounter > 3:
            player.pacingCounter = 0
        player.brain.nav.walkTo(player.destinationArray[player.pacingCounter])
        player.pacingCounter += 1

    return player.stay()
