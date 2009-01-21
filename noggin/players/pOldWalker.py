
from math import sqrt


NUM_STATES = 5
(NOTHING, INITIAL, WALK, IDLE, SPIN) = range(NUM_STATES)

STATES = (NOTHING, INITIAL, WALK, IDLE, SPIN)

STATE_DICT = dict(zip(range(NUM_STATES),
                      ("NOTHING","INITIAL","WALK","IDLE","SPIN")))

# dist to the post at which the dog stops walking (in cm)
STOP_DIST = 90


#WALK_DIST = (Constants.LANDMARK_RIGHT_BEACON_X - 
#             Constants.LANDMARK_LEFT_BEACON_X) - STOP_DIST*2 + 20 # 20 buffer

WALK_DIST = 200

ADJUST_ANGLE = 5.0 # adjust bearing thresh for WALK state
ALIGN_ANGLE = 3.0 # align angle for SPIN state
APPROACH_FACT = -0.0009 # factor by which we spin when aligning during WALK
SPIN_FACT = -0.0013 # factor by which we spin when aligning during spin
TIMEOUT = 30 # seconds by which SPIN times out
BEACON_LOST_FRAMESOFF = 60 # walk times out after losing beacon, in frames
SPIN_SPEED = 0.03 # default spin raw param in SPIN state
IDLE_DELAY = 20 # how long IDLE state lasts in frames


class Player(object):
    def __init__(self, brain):
        self.brain = brain
        self.state = NOTHING
        self.lastFrameState = NOTHING
        self.lastDiffState = NOTHING
        self.counter = 0

        self.timer = 0
        self.startTimer = 0
        
        self.speed = 9

        self.obj1 = self.brain.bglp
        self.obj2 = self.brain.ygrp
        
        self.object = self.obj2 #default
        self.objects = (self.obj1,self.obj2)

        #store all the methods in a tuple so its easy to switch
        self.stateMethods =  (self.nothing,\
                                  self.initial,\
                                  self.walk,\
                                  self.idle,\
                                  self.spin)

        self.objectJitter = 0
        self.lastObjectX = 0
        self.lastObjectY = 0

    def behave(self):
        ''' do walk learning behavior'''
        #pick the next method to call
        self.nextMethod = self.stateMethods[self.state]
        self.nextMethod()
        
        #self.brain.out.printf( "Counter, %d state :" %(self.state) + STATE_DICT[self.state])
        
        #since we don't have a sensor, just wait a sec and start

        if self.counter == 30 and self.state == NOTHING:
            self.state = INITIAL

        self.update()
        
    
    def update(self):
        '''
        Handles all the mechanics at the end of  frame, like storing last state
        '''
        if self.state != self.lastFrameState:
            self.brain.out.printf( "In state :" +STATE_DICT[self.state])
            self.counter = 0
            self.startTimer = self.brain.nao.getSimulatedTime()
            self.timer = 0
            self.lastDiffState = self.lastFrameState
        else:
            self.counter +=1
            self.timer = self.brain.nao.getSimulatedTime() - self.startTimer
        self.lastFrameState = self.state
        pass

    def nothing(self):
        '''
        on the first frame, stops moving, osets heads to higher then normal
        (for seeing objects)
        '''
        if self.firstFrame():
            self.brain.motion.stopWalking()
            self.brain.motion.setDefaultPosition()
        
    def initial(self):
        if self.firstFrame():
            self.runPartial = True
        
        for obj in self.objects:
            if obj.on:
                self.object = obj
                self.state = WALK
                 
    def walk(self):
        if self.firstFrame():
             self.runStart = self.brain.nao.getSimulatedTime()
             
        if self.object.on:
            pass #need to have object tracking

        
        #do jitter calculations
        if self.lastObjectX != 0 and self.lastObjectY != 0:
            self.objectJitter += \
                sqrt((self.lastObjectX-self.object.centerX)**2 +\
                         (self.lastObjectY-self.object.centerY)**2)

        self.lastObjectX = self.object.centerX
        self.lastObjectY = self.object.centerY
        
        # dist is lower than threshold, so stop
        if self.object.dist <= STOP_DIST:
            self.brain.motion.stopWalking() # stop moving
            self.state = IDLE # switch to IDLE state
            # if just a partial run, just spin
            if self.runPartial:
                self.runPartial = False # turn off runPartial bool
            # else, figure out speed of last run
            else:
                #how much jitter was there
                jitter = self.objectJitter
                self.objectJitter = 0
                # figure out time in seconds
                time = float(self.brain.nao.getSimmulatedTime()
                       - self.runStart)/1000.0
                # figure out distance
                dist = WALK_DIST
                # get speed of run
                speed = dist/time - jitter*0.0016
                # set speed to tcp string
                #brain.setTcpString(str(speed))
                print ("pWalker:: jitter: %g run dist: %g time: %g speed: %g" %
                       (jitter, dist, time, speed))

        # not close enough to object to stop, so walk
        else:
            # straight motion
            if abs(self.object.bearing) < ADJUST_ANGLE:
                self.brain.motion.setMotion(self.speed,0,0)
            # align motion
            else:
                self.brain.motion.setMotion(self.speed,0,
                                            self.object.bearing*APPROACH_FACT)


    def idle(self):
        pass

    def spin(self):
        pass

    def firstFrame(self):
        '''
        returns boolean of whether or not this is the first 
        frame of the curr. state
        '''
        return (self.counter == 0)

    def nextObject(self,obj):
        '''switches object variable'''
        if object == self.obj1:
            return self.obj2
        elif object == self.obj2:
            return self.obj1
        return None

    """
    self.brain.motion.setDefaultPosition()
    self.brain.motion.setMotion(0,0,20)
    """




