from objects import RelRobotLocation

"""
States for the hacked defender to pace through.
"""

def pace(player):
    if player.firstFrame():
        counter = 0
        destinationArray = [RelRobotLocation(0, 10, 0),
                            RelRobotLocation(-10, 0, 0),
                            RelRobotLocation(0, -10, 0),
                            RelRobotLocation(10, 0, 0)]

    if player.brain.nav.isStopped():
        player.brain.nav.walkTo(destinationArray[counter])
        counter += 1

    if counter == 4:
        counter = 0
