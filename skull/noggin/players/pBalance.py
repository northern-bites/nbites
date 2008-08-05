
import code
import man.motion as motion

for var in nao.deviceNames:
    globals()[var] = getattr(nao.motion, var)

class Player(object):
    def __init__(self,brain):
        self.brain = brain
	self.on = True 
	self.jointTime = .5
        self.NUMBER_OF_FSR = 8
	self.frameCount = 0
        self.joints = [0] * nao.NUMBER_OF_JOINTS
        self.fsrVals = [0] * self.NUMBER_OF_FSR
        self.leftFootForce = 0
        self.rightFootForce = 0

        self.doneAlready = False

        # fsrPos [(x positions, y positions)]
        self.fsrPositions = [(0.06993,0.02998),(0.06991,-0.02317),(-0.03002,-0.01911),(-0.03062,0.02996),(0.06991,0.02317),(0.06993,-0.02998),(-0.03062,-0.02996),(0.03002,0.01911)]

        self.copVals = [0,0]
        self.copFootVals = [[0,0],[0,0]]

    def behave(self):
         
        self.initialize()
	



        nao.next(2)
        self.fsrVals = nao.fsr.gets()
        self.getFeetCOP()

        if not self.doneAlready:
            self.brain.motion.enqueueChain(2,(0,0,-15,0,0,16),.5)
            self.brain.motion.enqueueChain(3,(0,0,-15,0,15,15),.5)
            #self.brain.motion.enqueueChain(2,(0,0,0,0,15,0),.25)
            #self.brain.motion.enqueueChain(2,(0,-40,0,0,0,0),1.0)
            #self.brain.motion.enqueueChain(3,(0,-40,0,0,0,0),1.0)
            self.doneAlready = True
    def checkMoment(self):
        while (True):
            nao.sleep(.25)
            self.fsrVals = nao.fsr.gets()
            
            # COP x-value
            
            
                
    def initialize(self):
        self.leftFootForce = 0
        self.rightFootForce = 0
        self.copFootVals[0][0] = 0
        self.copFootVals[0][1] = 0
        self.copFootVals[1][0] = 0
        self.copFootVals[1][1] = 0


    def getFeetCOP(self):
        
        for x in xrange(3):
            self.leftFootForce += self.fsrVals[x]
            if self.fsrVals[x] != 0:
                self.copFootVals[0][0] += self.fsrVals[x]*self.fsrPositions[x][0]
                self.copFootVals[0][1] += self.fsrVals[x]*self.fsrPositions[x][1]
            
        if self.leftFootForce != 0: 
            self.copFootVals[0][0] = self.copFootVals[0][0]/self.leftFootForce
            self.copFootVals[0][1] = self.copFootVals[0][1]/self.leftFootForce
        
            
       
        for x in xrange(4,7):
            self.rightFootForce += self.fsrVals[x]
            if self.fsrVals[x] !=0:
                self.copFootVals[1][0] += self.fsrVals[x]*self.fsrPositions[x][0]
                self.copFootVals[1][1] += self.fsrVals[x]*self.fsrPositions[x][1]
            

        if self.rightFootForce !=0:            
            self.copFootVals[1][0] = self.copFootVals[1][0]/self.rightFootForce
            self.copFootVals[1][1] = self.copFootVals[1][1]/self.rightFootForce
        


        print "left foot total force", self.leftFootForce
        print "right foot total force", self.rightFootForce
        print "left foot x cop =", self.copFootVals[0][0]
        print "left foot y cop =", self.copFootVals[0][1]
        print "right foot x cop =", self.copFootVals[1][0]
        print "right foot y cop =", self.copFootVals[1][1]

            



