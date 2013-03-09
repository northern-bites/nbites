import os
from . import SoccerFSA
from . import DataStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(DataStates)
        self.setName('pData')
        self.postDistance = 50
        self.lastDistance = 0

        # Specify which object is being studied
        self.objects = (self.brain.ygrp, self.brain.yglp)

    def savePostInfo(self):
        both_zero = True
        for obj in self.objects:
            if obj.vis.dist != 0.0:
                both_zero = False
                break

        if both_zero:
            return

        filename = "/home/root/postDistData" + str(self.postDistance) + ".csv"

        # need to remove it if it exists already and make way
        # for new data
        if self.lastDistance != self.postDistance and \
                os.path.exists(filename):
            self.lastDistance = self.postDistance
            os.remove(filename)
            csv = open(filename,'a+')
            csv.write("dist,bearing\n")
        else :
            csv = open(filename,'a+')

        for obj in self.objects:
            if obj.vis.dist !=0.0 and abs(obj.vis.dist - self.postDistance) < 100:
                csv.write(str(obj.vis.dist) + "," + str(obj.vis.bearing) + '\n')
                print obj.vis.dist, obj.vis.bearing
        csv.close()
