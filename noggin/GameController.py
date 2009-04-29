
from man import comm

from . import NogginConstants as Constants
from . import GameStates
from .util import FSA


####### LED DEFINITIONS #############
LED_OFF = 0
LED_ON = 1

#GROUPS
LEFT_FOOT_LED   = "LeftFootLeds"
RIGHT_FOOT_LED  = "RightFootLeds"
CHEST_LED       = "ChestLeds"


LED_GROUPS = (LEFT_FOOT_LED, RIGHT_FOOT_LED,CHEST_LED)

#COLORS
RED   = 0xFF0000
GREEN = 0x00FF00
BLUE  = 0x0000FF
YELLOW= 0xFFFF00
WHITE = 0xFFFFFF
OFF   = 0x000000
NOW = 0.0

###### KICKOFF ##########
HAVE_KICKOFF_LEDS  = ((RIGHT_FOOT_LED, WHITE, NOW),)
NO_KICKOFF_LEDS    = ((RIGHT_FOOT_LED, OFF, NOW),)

###### TEAM COLOR ##########
TEAM_BLUE_LEDS = ((LEFT_FOOT_LED, BLUE, NOW),)
TEAM_RED_LEDS = ((LEFT_FOOT_LED, RED, NOW),)

###### STATES ###########
STATE_INITIAL_LEDS =  ((CHEST_LED, OFF,    NOW),)
STATE_READY_LEDS =    ((CHEST_LED, BLUE,   NOW),)
STATE_SET_LEDS =      ((CHEST_LED, YELLOW, NOW),)
STATE_PLAYING_LEDS =  ((CHEST_LED, GREEN,  NOW),)
STATE_PENALIZED_LEDS =((CHEST_LED, RED,    NOW),)
STATE_FINISHED_LEDS = ((CHEST_LED, OFF,    NOW),)

TEAM_BLUE = 0
TEAM_RED = 1

class GameController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.gc = brain.comm.gc
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(GameStates)
        self.currentState = 'gameInitial'
        self.setName('GameController')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'cyan'
        self.setPrintFunction(self.brain.out.printf)
        self.kickOff = self.gc.kickOff

    def run(self):
        self.setGCLEDS()

        if self.gc.state == comm.STATE_INITIAL:
            self.switchTo('gameInitial')
        elif self.gc.state == comm.STATE_SET:
            self.switchTo('gameSet')
        elif self.gc.state == comm.STATE_READY:
            self.switchTo('gameReady')
        elif self.gc.state == comm.STATE_PLAYING:
            if self.gc.penalty != comm.PENALTY_NONE:
                self.switchTo("gamePenalized")
            else:
                self.switchTo("gamePlaying")
        elif self.gc.state == comm.STATE_FINISHED:
            self.switchTo('gameFinished')


        #Set team color
        if self.gc.color != self.brain.my.teamColor:

            self.brain.my.teamColor = self.gc.color
            self.brain.makeFieldObjectsRelative()
            self.printf("Switching team color to " +
                        Constants.teamColorDict[self.brain.my.teamColor])
        if self.gc.kickOff != self.kickOff:
            self.printf("Switching kickoff to team #%g"%self.gc.kickOff +
                        " from team #%g"% self.kickOff)
            self.kickOff = self.gc.kickOff

        FSA.FSA.run(self)

    def setGCLEDS(self):
        '''
        Method to set the chest and feet according to the current
        GC states and infos
        '''

        #######  KICKOFF  ######
        #if self.gc.kickOff != self.kickOff:
        if self.gc.kickOff == self.gc.team:
            #self.printf("Setting LEDS to KICKOFF (WHITE)")
            self.executeLeds(HAVE_KICKOFF_LEDS)
        else:
            #self.printf("Setting LEDS to KICKOFF (OFF)")
            self.executeLeds(NO_KICKOFF_LEDS)

        ###### TEAM COLOR ######
        if self.gc.color == TEAM_BLUE:
            self.executeLeds(TEAM_BLUE_LEDS)
        else:
            self.executeLeds(TEAM_RED_LEDS)

        ###### GAME STATE ######

        if self.gc.state == comm.STATE_INITIAL:
            self.executeLeds(STATE_INITIAL_LEDS)
        elif self.gc.state == comm.STATE_SET:
            self.executeLeds(STATE_SET_LEDS)
            self.switchTo('gameSet')
        elif self.gc.state == comm.STATE_READY:
            self.executeLeds(STATE_READY_LEDS)
            self.switchTo('gameReady')
        elif self.gc.state == comm.STATE_PLAYING:
            if self.gc.penalty != comm.PENALTY_NONE:
                self.executeLeds(STATE_PENALIZED_LEDS)
            else:
                self.executeLeds(STATE_PLAYING_LEDS)
        elif self.gc.state == comm.STATE_FINISHED:
            self.executeLeds(STATE_FINISHED_LEDS)

    def executeLeds(self,listOfLeds):

        for ledTuple in listOfLeds:
            if len(ledTuple) != 3:
                self.printf("Invalid print command!! " + str(ledTuple))
                continue
            ledName     = ledTuple[0]
            ledHexValue = ledTuple[1]
            ledTime     = ledTuple[2]
            self.brain.leds.fadeRGB(ledName,ledHexValue,ledTime)
