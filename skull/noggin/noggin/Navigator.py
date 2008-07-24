import util.FSA as FSA
import util.MyMath as MyMath
import NavStates
import NavConstants

class Navigator(FSA.FSA):
    def __init__(self,brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(NavStates)
        self.currentState = 'nothing'
        self.setName('Navigator, bitch!')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)

        self.lastDestX=self.lastDestY=self.lastDestH = self.destX=self.destY=self.destH = 0


    def run (self):
        ''' 
        Overload the FSA version of run, so we can exectue our own updates 
        '''

        #Each frame we need to update where we are going
        #and calculate some facts about how far away we are

        #unnecessary?
        self.lastDestX,self.lastDestY,self.lastDestH = self.destX,self.destY,self.destH

        self.bearingToDest = MyMath.getRelativeBearing(self.brain.my.x,
                                                  self.brain.my.y,
                                                  self.brain.my.h,
                                                  self.destX,
                                                  self.destY)
        self.distToDest = MyMath.dist(self.brain.my.x,
                                 self.brain.my.y,
                                 self.destX,
                                 self.destY)

        self.bearingToDestHeading = self.destH - self.brain.my.h

        FSA.FSA.run(self)


    def goTo(self,x,y,h=None):
        self.forever = False
        if h==None:
            self.destH = self.brain.my.h
        else:
            self.destH = h
        self.destX,self.destY = x,y

        self.switchTo('walkStraight')

    def spinLeft(self):
        self.switchTo('turnLeftForever')

    def spinRight(self):
        self.switchTo('turnRightForever')

    def walkStraight(self):
        self.switchTo('walkStraightForever')

    def stopWalking(self):
        self.switchTo('stop')

    def getErrors(self):
        ''' Returns X,Y,H errors for how close we are to our destination positoning'''
        return (self.destX - self.brain.my.x,
                self.destY - self.brain.my.Y,
                self.destH - self.brain.my.h)
    

    def arrived(self):
        return self.distToDest < NavConstants.DIST_ARRIVED_THRESH

    def shouldInitialTurnLeft(self):
        return self.bearingToDest > NavConstants.INITIAL_BEARING_THRESH and not self.arrived()

    def shouldInitialTurnRight(self):
        return self.bearingToDest < NavConstants.INITIAL_BEARING_THRESH and not self.arrived()
        
    def shouldFinalTurnLeft(self):
        return self.bearingToDestHeading > NavConstants.HEADING_ARRIVED_THRESH and self.arrived()
    
    def shouldFinalTurnRight(self):
        return self.bearingToDestHeading < NavConstants.HEADING_ARRIVED_THRESH and self.arrived()
    
    def shouldWalkStraight(self):
        return ( not self.arrived() and
                 not self.shouldInitialTurnRight() and 
                 not self.shouldInitialTurnLeft() )

    def shouldStop(self):
        return ( self.arrived and 
                 not self.shouldFinalTurnRight() and 
                 not self.shouldFinalTurnLeft() ) 
    

    def turnTo(self,degrees):
        self.degreesToTurn = degrees
        self.goNow('turn')

    def walkStraightTo(self,cm):
        self.distToDest = cm
        self.switchTo('walkStraightTo')

    def walkSidewaysTo(self,cm):
        self.distToDest = cm
        self.switchTo('walkSidewaysTo')

