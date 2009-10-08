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
        self.object = self.brain.ygrp

    def savePostInfo(self):
        if self.object.dist == 0.0:
            return

        filename = "/home/root/postDistData" + str(self.postDistance) + ".csv"

        # need to remove it if it exists already and make way
        # for new data
        if self.lastDistance != self.postDistance and \
                os.path.exists(filename):
            self.lastDistance = self.postDistance
            os.remove(filename)
        csv = open(filename,'a+')

        csv.write(str(self.object.dist) + "," + str(self.object.bearing) + '\n')
        csv.close()
