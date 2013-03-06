# This is a very simple class to hold data about the ball.

class Ball():

    def __init__(self):
        self.visDist = 0
        self.visBearing = 0
        self.locDist = 0
        self.locBearing = 0
        self.framesOn = 0

    def updateFramesOn(self, onThisFrame):
        if onThisFrame:
            self.framesOn += 1
        else:
            self.framesOn = 0

