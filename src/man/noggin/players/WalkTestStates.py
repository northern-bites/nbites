"""
Instructions: This file specifies a list of motion trajectories and how
long to follow them for. When the list has been used up, the robot will
sit down.

To use this as a true unit test, turn on MOTION_DEBUG in cmake,
and save the /tmp/joints_log.xls file somewhere safe.  Then you can
compare this file with the new one to see if they are different or not.
(Maybe using another program (like R) to do this comparison?
"""

from ..WebotsConfig import WEBOTS_ACTIVE
import man.motion.SweetMoves as SweetMoves

#Types
DEST = 2
STEP = 1 #currently broken in Nav.
WALK = 0

# all walk vector values must be in the range [-1,1]
UNIT_TEST1 = ((WALK, (1, 0, 0), 100),
              (WALK, (0, .5, 0), 100),
              (WALK, (.5, -.5, .75), 75),
              (WALK, (-.4, .5, .5), 50),
              (WALK, (.2, .2, .2), 50),
              )

STRAIGHT_ONLY = ((WALK, (1.0, 0, 0), 150),
                 (WALK, (0, 1.0, 0), 150),
                 )

CARDINAL_DEST_TEST = ((DEST, (15, 0, 0), 150),
                      (DEST, (-15, 0, 0), 150),
                      (DEST, (0, 10, 0), 150),
                      (DEST, (0, -10, 0), 150),
                      (DEST, (0, 0, 90), 150),
                      (DEST, (0, 0, -90), 150),
                      )

MIXED_DEST_TEST = ((DEST, (15, 15, 0), 200),
                   (DEST, (15, -15, 0), 200),
                   (DEST, (0, -15, -90), 200),
                   (DEST, (50, 15, -90), 300),
                   (DEST, (-30, -15, -90), 300),
                   )

def gamePlaying(player):
    """
    This method must be overriden by interested SoccerPlayers
    """
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.stopHeadMoves()

        player.testCounter = 0
        player.unitTest = MIXED_DEST_TEST
    return player.goLater('walkTest')

def walkTest(player):
    """
    This method processes the list of commands until there are
    none left
    """
    stability = player.brain.stability
    stability.updateStability() # collect stability variance data

    if player.firstFrame():
        if player.testCounter >= len(player.unitTest):
            player.printf('Stability heuristic for this gait: {0}'.format(stability.getStabilityHeuristic()))
            return player.goLater('sitdown')
        currentCommand  = player.unitTest[player.testCounter]
        player.testCounter += 1
        player.testFrames = currentCommand[2]
        currentVector = currentCommand[1]
        if currentCommand[0] == WALK:
            player.setWalk(currentVector[0],
                           currentVector[1],
                           currentVector[2],)
        elif currentCommand[0] == STEP:
            player.setSteps(currentVector[0],
                            currentVector[1],
                            currentVector[2],
                            currentVector[3],)
        elif currentCommand[0] == DEST:
            player.setDestination(currentVector[0],
                                  currentVector[1],
                                  currentVector[2],)
        else:
            player.printf("WARNING! Unrecognized command"
                          " type in WalkUnitTest")
    if player.counter == player.testFrames:
        return player.goNow('switchDirections')

    return player.stay()

def switchDirections(player):
    return player.goLater('walkTest')

def sitdown(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()

    return player.stay()


if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"
