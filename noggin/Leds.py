import _lights

# LED Related #
DEBUG_BALL_LEDS = True
DEBUG_GOAL_LEDS = True

####### LED DEFINITIONS #############
LED_OFF = 0
LED_ON = 1

#The order here must match the order of the string defined in Lights.h 
# and in ALLedNames.h
(LEFT_EAR_LED,
RIGHT_EAR_LED,
LEFT_EYE_LED,
RIGHT_EYE_LED,
CHEST_LED,
LEFT_FOOT_LED,
RIGHT_FOOT_LED) = range(_lights.NUM_LED_GROUPS)


#COLORS
RED   = 0xFF0000
GREEN = 0x00FF00
BLUE  = 0x0000FF
YELLOW= 0xFFFF00
WHITE = 0xFFFFFF
OFF   = 0x000000
NOW = 0.0



###### GC LEDS ##########
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

##### Brain ######
#### Ball #######
BALL_ON_LEDS = ((LEFT_EYE_LED, RED,    NOW),
                (RIGHT_EYE_LED, RED,    NOW),)
BALL_OFF_LEDS = ((LEFT_EYE_LED, BLUE,    NOW),
                 (RIGHT_EYE_LED, BLUE,    NOW),)


#### GOAL ######
GOAL_ON_LEDS = ((LEFT_EAR_LED, BLUE,    NOW),
                (RIGHT_EAR_LED, BLUE,    NOW),)
GOAL_OFF_LEDS = ((LEFT_EAR_LED, OFF,    NOW),
                 (RIGHT_EAR_LED, OFF,    NOW),)



class Leds():
    def __init__(self, brainPtr):
        self.lights = _lights.lights
        self.brain = brainPtr

    def processLeds(self):
        ### for the ball ###
        if DEBUG_BALL_LEDS:
            if self.brain.ball.on:
                self.executeLeds(BALL_ON_LEDS)
            else:
                self.executeLeds(BALL_OFF_LEDS)

        if DEBUG_GOAL_LEDS:
            if self.brain.oppGoalRightPost.on or self.brain.oppGoalLeftPost.on or \
                    self.brain.myGoalRightPost.on or self.brain.myGoalLeftPost.on:
                self.executeLeds(GOAL_ON_LEDS)
            else:
                self.executeLeds(GOAL_OFF_LEDS)

    def executeLeds(self,listOfLeds):

        for ledTuple in listOfLeds:
            if len(ledTuple) != 3:
                self.printf("Invalid print command!! " + str(ledTuple))
                continue
            ledName     = ledTuple[0]
            ledHexValue = ledTuple[1]
            if ledTuple[2] != NOW:
                print "Invalid timing command in Leds.py"
            self.lights.setRGB(ledName,ledHexValue)
