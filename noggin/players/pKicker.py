
class Player(object):
    def __init__(self, brain):
        self.brain = brain
        self.doOnce = True
        self.doTwice = True
        self.counter = 0

    def behave(self):
        """ Called every TIME_STEP, ie 40 ms"""

        if self.doOnce:

            self.brain.motion.setDefaultPosition()
            self.brain.motion.setMotion(27,-15,0,6)
            
            self.doOnce = not self.doOnce
            
        
        elif self.doTwice and not self.brain.motion.isWalking():
            self.brain.motion.queueLeftKick()
            self.doTwice = not self.doTwice
