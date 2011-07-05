import _lights
from playbook import PBConstants

# LED Related #
DEBUG_BALL_LEDS = True
DEBUG_GOAL_LEDS = True
DEBUG_ROLE_LEDS = False
DEBUG_DEFENDER_LEDS = False

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
PURPLE= 0xFF00FF
CYAN  = 0x00FFFF
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
BALL_ON_LEDS = ((LEFT_EYE_LED, RED, NOW),)
BALL_OFF_LEDS = ((LEFT_EYE_LED, BLUE, NOW),)

############### Roles ###############
#### Chaser #######
CHASER_ON_LEDS = ((RIGHT_EYE_LED, GREEN, NOW),)
LOC_CHASER_ON_LEDS = ((RIGHT_EYE_LED, WHITE, NOW),)
#### Other ####
MIDDIE_ON_LEDS = ((RIGHT_EYE_LED, PURPLE, NOW),)
DEFENDER_ON_LEDS = ((RIGHT_EYE_LED, CYAN, NOW),)
OFFENDER_ON_LEDS =  ((RIGHT_EYE_LED, YELLOW, NOW),)
CHASER_OFF_LEDS = ((RIGHT_EYE_LED, BLUE, NOW),)

#### GOAL ######
LEFT_GOAL_ON_LEDS = ((LEFT_EAR_LED, BLUE, NOW),)
LEFT_GOAL_OFF_LEDS = ((LEFT_EAR_LED, OFF, NOW),)
RIGHT_GOAL_ON_LEDS = ((RIGHT_EAR_LED, BLUE, NOW),)
RIGHT_GOAL_OFF_LEDS = ((RIGHT_EAR_LED, OFF, NOW),)

#### FLASH ####
FLASH_ON_LEDS = ((LEFT_EYE_LED,  GREEN, NOW),
                 (RIGHT_EYE_LED, GREEN, NOW),
                 (LEFT_EAR_LED,  BLUE, NOW),
                 (RIGHT_EAR_LED, BLUE, NOW),)
FLASH_OFF_LEDS = ((LEFT_EYE_LED,  OFF, NOW),
                  (RIGHT_EYE_LED, OFF, NOW),
                  (LEFT_EAR_LED,  OFF, NOW),
                  (RIGHT_EAR_LED, OFF, NOW),)



class Leds():
    def __init__(self, brainPtr):
        self.lights = _lights.lights
        self.brain = brainPtr
        self.flashing = False
        self.flashOn = False
        self.counter = 0

    def processLeds(self):
        self.counter += 1

        if self.flashing and self.counter % 5 == 0:
            self.flashLeds()
            return

        ### for the ball ###
        if DEBUG_BALL_LEDS:
            if self.brain.ball.vis.framesOn == 1:
                self.executeLeds(BALL_ON_LEDS)
            elif self.brain.ball.vis.framesOff == 1:
                self.executeLeds(BALL_OFF_LEDS)

        if DEBUG_GOAL_LEDS:
            if (self.brain.myGoalLeftPost.vis.framesOn == 1 or
                self.brain.oppGoalLeftPost.vis.framesOn == 1 or
                self.brain.myGoalRightPost.vis.framesOn == 1 or
                self.brain.oppGoalRightPost.vis.framesOn == 1):

                self.executeLeds(LEFT_GOAL_ON_LEDS)
                self.executeLeds(RIGHT_GOAL_ON_LEDS)

            elif (self.brain.myGoalLeftPost.vis.framesOff == 1 or
                self.brain.oppGoalLeftPost.vis.framesOff == 1 or
                self.brain.myGoalRightPost.vis.framesOff == 1 or
                self.brain.oppGoalRightPost.vis.framesOff == 1):

                self.executeLeds(LEFT_GOAL_OFF_LEDS)
                self.executeLeds(RIGHT_GOAL_OFF_LEDS)

        if DEBUG_ROLE_LEDS:
            if self.playbook.roleChanged:
                if self.brain.play.isRole(PBConstants.CHASER):
                        self.executeLeds(CHASER_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.DEFENDER):
                        self.executeLeds(DEFENDER_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.OFFENDER):
                        self.executeLeds(OFFENDER_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.MIDDIE):
                        self.executeLeds(MIDDIE_ON_LEDS)
                elif self.brain.play.roleChanged():
                        self.executeLeds(CHASER_OFF_LEDS)

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

    def startFlashing(self):
        self.flashing = True
        self.flashOn = False
        self.flashLeds()

    def stopFlashing(self):
        self.flashing = False
        self.flashOn = False

    def flashLeds(self):
        if self.flashOn:
            self.executeLeds(FLASH_OFF_LEDS)
        else:
            self.executeLeds(FLASH_ON_LEDS)

        self.flashOn = not self.flashOn
