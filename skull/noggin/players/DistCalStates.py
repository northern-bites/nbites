"""
This file is used for determining distance and variance calibration.

Future improvements: close the file after each iteration to force the buffer to write to file. There may be a more direct way of doing this, but this ensures that if the robot crashes halfway through or runs out of batteries that you still get your data.  

See the pause state for commenting out which sections you want recorded.

Also, be sure to update the speed of the scan if you want to test variance
based on head speed.

"""

import math

import man.motion as motion
from man.corpus.sensors import inst as sensors

def nothing(player):
    """
    Do nothing
    """
    if player.firstFrame():
    #player.brain.motion.setDefaultPosition()
        print "Opening FILE"
        player.FILE = open("/tmp/test.xls","w")
        player.fCounter = 0
        
    if player.counter == 10:
        return player.goLater('scan')
    return player.stay()

def record (player):
    if player.firstFrame():
        player.heights = []
        player.widths = []
        player.hSpeeds = []
        player.bearing = []
        player.dists = []
    if player.brain.bgrp.width == 0:
        print "x",
        return player.stay()
    
    obj = player.brain.bgrp
    player.dist = obj.dist
    player.heights.append(obj.height)
    player.widths.append(obj.width)
    player.hSpeeds.append(player.brain.motion.getHeadSpeed())
    player.bearing.append(obj.bearing)
    player.dists.append(obj.dist)
    print len(player.heights), " ",
    if len(player.heights) == 50:
        print " \n"
        return player.goLater('pause')
    else:
        return player.stay()

def pause(player):
    if player.counter == 50:
        player.fCounter +=1
        if player.fCounter > 19:
            print "Closing FILE"
            player.FILE.close()
            return player.goLater('done')
        return player.goLater('record')
    if player.firstFrame():
        """
        player.FILE.write("\n"+str(player.fCounter) + "\t" + str(player.dist) +" \t widths")
        for w in player.widths:
            player.FILE.write("\t" + str(w))

        player.FILE.write("\n"+str(player.fCounter) + "\t" + str(player.dist) +" \t wspeeds")
        for w in player.hSpeeds:
            player.FILE.write("\t" + str(w))

        player.FILE.write("\n"+ str(player.fCounter) + "\t" + str(player.dist) +" \t heights")
        for h in player.heights:
            player.FILE.write("\t" + str(h))

        player.FILE.write("\n" + str(player.fCounter) + "\t" + str(player.dist) +" \t hspeeds")
        for w in player.hSpeeds:
            player.FILE.write("\t" + str(w))
            """
        player.FILE.write("\n" + str(player.fCounter) + "\t" + str(player.dist) +" \t bearings")
        for b in player.bearing:
            player.FILE.write("\t" + str(b))
        player.FILE.write("\n" + str(player.fCounter) + "\t" + str(player.dist) +" \t distances")
        for d in player.dists:
            player.FILE.write("\t" + str(d))

    print 50-player.counter," "
    return player.stay()


def track(player):

    player.brain.tracker.switchTo('tracking', player.brain.ball)
    return player.goLater('done')

def scan(player):
    motionInterface = player.brain.motion

    if player.counter == 0:
        scan1 = motion.HeadJointCommand(1.0, [ 30.0, 0.0], 0)
        scan2 = motion.HeadJointCommand(2.0, [-30.0, 0.0], 0)
        scan3 = motion.HeadJointCommand(1.0, [ 0.0, 0.0], 0)

        headScan = motion.HeadScanCommand([scan1, scan2, scan3], True)
        motionInterface.enqueue(headScan)

    return player.goLater('record')

def done(player):
    return player.stay()


