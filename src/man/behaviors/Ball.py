# This is a very simple class to hold data about the ball.

class Ball():

    def __init__(self):
        self.visDist = 0
        self.visBearing = 0
        self.locDist = 0
        self.locBearing = 0
        self.framesOn = 0
        self.framesOff = 0

    # @param ballInfo: must be a filteredBall message
    def updateBallInfo(self, ballInfo):
        self.visDist = ballInfo.distance
        self.visBearing = ballInfo.bearing
        # self.locDist = ballInfo.filter_distance
        # self.locBearing = ballInfo.filter_bearing
        if ballInfo.on:
            self.framesOff = 0
            self.framesOn += 1
        else:
            self.framesOn = 0
            self.framesOff += 1
