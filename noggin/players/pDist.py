
import math

class Player(object):
    def __init__(self, brain):
        self.brain = brain
	self.doOnce = True

        self.counter = 0
    def behave(self):
        obj = self.brain.vision.yglp
        
        #save frame when object is not on, for calibration purposes
        #if self.counter % 25 == 0: self.brain.out.saveFrame()

	if self.doOnce:

	    self.brain.motion.setDefaultPosition()
	    #self.brain.motion.setSpeed(0,0,10,60)

            #self.headPan(10)
            self.brain.out.printf("\t w \t ht\t d")
	    self.doOnce = not self.doOnce
	#self.brain.motion.printFSR()

	#self.brain.out.saveFrame()
        self.counter += 1

        if self.counter % 15 == 0 and obj.dist > 0: self.brain.out.printf("\t %i \t %i \t %f " % (obj.width,obj.height,obj.dist))

    """
    def headPan(self,times):
        self.brain.motion.enqueueChain(Motion.HEAD, (60,10),.5)
        for i in xrange(times):
            self.brain.motion.enqueueChain(Motion.HEAD, (0,45),0.5)
            self.brain.motion.enqueueChain(Motion.HEAD, (-60,10),0.5)
            self.brain.motion.enqueueChain(Motion.HEAD, (60,10),.8)
            
        self.brain.motion.enqueueChain(Motion.HEAD, (0,10),0.5)
    """
