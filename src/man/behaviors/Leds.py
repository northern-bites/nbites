import noggin_constants as NogginConstants
import GameController

############################ IMPORTANT ############################
# DO NOT check any values directly from messages in this class.   #
# Leds.run() is called once during Brain.init(), so messages will #
# all be null. Instead, take values from brain.ball,              #
# brain.gameController, etc.                                      #
############################ IMPORTANT ############################

# LED Related #
GC_LEDS = True
FOOT_LEDS = True
BALL_LEDS = True
GOAL_LEDS = True
ROLESWITCH_LEDS = True
CALIBRATION_LEDS = True
SHAREDFLIP_LEDS = True
COMM_LEDS = True

####### LED DEFINITIONS #############
LED_OFF = 0
LED_ON = 1

#The order here must match the order of the string defined in Lights.h
# and in ALLedNames.h
NUM_LED_GROUPS = 29

(LEFT_CALIBRATION_ONE_LED,
LEFT_CALIBRATION_TWO_LED,
LEFT_CALIBRATION_THREE_LED,
LEFT_CALIBRATION_FOUR_LED,
LEFT_CALIBRATION_FIVE_LED,
RIGHT_CALIBRATION_ONE_LED,
RIGHT_CALIBRATION_TWO_LED,
RIGHT_CALIBRATION_THREE_LED,
RIGHT_CALIBRATION_FOUR_LED,
RIGHT_CALIBRATION_FIVE_LED,
LEFT_COMM_ONE_LED,
LEFT_COMM_TWO_LED,
LEFT_COMM_THREE_LED,
LEFT_COMM_FOUR_LED,
LEFT_COMM_FIVE_LED,
RIGHT_COMM_ONE_LED,
RIGHT_COMM_TWO_LED,
RIGHT_COMM_THREE_LED,
RIGHT_COMM_FOUR_LED,
RIGHT_COMM_FIVE_LED,
ROLE_LED,
BALL_LED,
LEFT_GOAL_LED,
RIGHT_GOAL_LED,
GOAL_ID_LED,
CHEST_LED,
LEFT_FOOT_LED,
RIGHT_FOOT_LED,
RIGHT_UNUSED_LED) = range(NUM_LED_GROUPS)

###COLORS
RED   = 0xFF0000
GREEN = 0x00FF00
BLUE  = 0x0000FF
YELLOW= 0xFFFF00
PURPLE= 0xFF00FF
CYAN  = 0x00FFFF
WHITE = 0xFFFFFF
OFF   = 0x000000
PINK  = 0xFF1493
EYE_YELLOW= 0xFF4400  # Need special hex for eye yellow display.

NOW = 0.0

##### Ball  #####
BALL_ON_LEDS = ((BALL_LED, RED, NOW),)
BALL_OFF_LEDS = ((BALL_LED, BLUE, NOW),)

##### Roles #####
ROLE_ONE      = ((ROLE_LED, RED, NOW),)
ROLE_TWO      = ((ROLE_LED, EYE_YELLOW, NOW),)
ROLE_THREE    = ((ROLE_LED, CYAN, NOW),)
ROLE_FOUR     = ((ROLE_LED, BLUE, NOW),)
ROLE_FIVE     = ((ROLE_LED, PURPLE, NOW),)
ROLE_OFF_LEDS = ((ROLE_LED, OFF, NOW),)

ROLE_DICT = {1: ROLE_ONE,
             2: ROLE_TWO,
             3: ROLE_THREE,
             4: ROLE_FOUR,
             5: ROLE_FIVE}

##### GOAL ######
LEFT_POST_ON_LEDS =   ((RIGHT_GOAL_LED, GREEN, NOW),)
LEFT_POST_AMBIGUOUS_LEDS = ((RIGHT_GOAL_LED, EYE_YELLOW, NOW),)
LEFT_POST_OFF_LEDS = ((RIGHT_GOAL_LED, BLUE, NOW),)
RIGHT_POST_ON_LEDS =  ((LEFT_GOAL_LED, GREEN, NOW),)
RIGHT_POST_AMBIGUOUS_LEDS = ((LEFT_GOAL_LED, EYE_YELLOW, NOW),)
RIGHT_POST_OFF_LEDS = ((LEFT_GOAL_LED, BLUE, NOW),)

BLUE_GOAL_LEDS = ((GOAL_ID_LED, BLUE, NOW),)
PINK_GOAL_LEDS = ((GOAL_ID_LED, PINK, NOW),)
NO_GOAL_LEDS =   ((GOAL_ID_LED, OFF, NOW),)

##### CALIBRATION #####
LEFT_CALIBRATION_ONE_LEDS   = ((LEFT_CALIBRATION_ONE_LED, BLUE, NOW),)
LEFT_CALIBRATION_TWO_LEDS   = ((LEFT_CALIBRATION_TWO_LED, BLUE, NOW),)
LEFT_CALIBRATION_THREE_LEDS = ((LEFT_CALIBRATION_THREE_LED, BLUE, NOW),)
LEFT_CALIBRATION_FOUR_LEDS  = ((LEFT_CALIBRATION_FOUR_LED, BLUE, NOW),)
LEFT_CALIBRATION_FIVE_LEDS  = ((LEFT_CALIBRATION_FIVE_LED, BLUE, NOW),)
RIGHT_CALIBRATION_ONE_LEDS  = ((RIGHT_CALIBRATION_ONE_LED, BLUE, NOW),)
RIGHT_CALIBRATION_TWO_LEDS  = ((RIGHT_CALIBRATION_TWO_LED, BLUE, NOW),)
RIGHT_CALIBRATION_THREE_LEDS= ((RIGHT_CALIBRATION_THREE_LED, BLUE, NOW),)
RIGHT_CALIBRATION_FOUR_LEDS = ((RIGHT_CALIBRATION_FOUR_LED, BLUE, NOW),)
RIGHT_CALIBRATION_FIVE_LEDS = ((RIGHT_CALIBRATION_FIVE_LED, BLUE, NOW),)
LEFT_CALIBRATION_ONE_OFF_LEDS   = ((LEFT_CALIBRATION_ONE_LED, OFF, NOW),)
LEFT_CALIBRATION_TWO_OFF_LEDS   = ((LEFT_CALIBRATION_TWO_LED, OFF, NOW),)
LEFT_CALIBRATION_THREE_OFF_LEDS = ((LEFT_CALIBRATION_THREE_LED, OFF, NOW),)
LEFT_CALIBRATION_FOUR_OFF_LEDS  = ((LEFT_CALIBRATION_FOUR_LED, OFF, NOW),)
LEFT_CALIBRATION_FIVE_OFF_LEDS  = ((LEFT_CALIBRATION_FIVE_LED, OFF, NOW),)
RIGHT_CALIBRATION_ONE_OFF_LEDS  = ((RIGHT_CALIBRATION_ONE_LED, OFF, NOW),)
RIGHT_CALIBRATION_TWO_OFF_LEDS  = ((RIGHT_CALIBRATION_TWO_LED, OFF, NOW),)
RIGHT_CALIBRATION_THREE_OFF_LEDS= ((RIGHT_CALIBRATION_THREE_LED, OFF, NOW),)
RIGHT_CALIBRATION_FOUR_OFF_LEDS = ((RIGHT_CALIBRATION_FOUR_LED, OFF, NOW),)
RIGHT_CALIBRATION_FIVE_OFF_LEDS = ((RIGHT_CALIBRATION_FIVE_LED, OFF, NOW),)

##### COMM #####
LEFT_COMM_ONE_LEDS   = ((LEFT_COMM_ONE_LED, BLUE, NOW),)
LEFT_COMM_TWO_LEDS   = ((LEFT_COMM_TWO_LED, BLUE, NOW),)
LEFT_COMM_THREE_LEDS = ((LEFT_COMM_THREE_LED, BLUE, NOW),)
LEFT_COMM_FOUR_LEDS  = ((LEFT_COMM_FOUR_LED, BLUE, NOW),)
LEFT_COMM_FIVE_LEDS  = ((LEFT_COMM_FIVE_LED, BLUE, NOW),)
RIGHT_COMM_ONE_LEDS  = ((RIGHT_COMM_ONE_LED, BLUE, NOW),)
RIGHT_COMM_TWO_LEDS  = ((RIGHT_COMM_TWO_LED, BLUE, NOW),)
RIGHT_COMM_THREE_LEDS= ((RIGHT_COMM_THREE_LED, BLUE, NOW),)
RIGHT_COMM_FOUR_LEDS = ((RIGHT_COMM_FOUR_LED, BLUE, NOW),)
RIGHT_COMM_FIVE_LEDS = ((RIGHT_COMM_FIVE_LED, BLUE, NOW),)
LEFT_COMM_ONE_OFF_LEDS   = ((LEFT_COMM_ONE_LED, OFF, NOW),)
LEFT_COMM_TWO_OFF_LEDS   = ((LEFT_COMM_TWO_LED, OFF, NOW),)
LEFT_COMM_THREE_OFF_LEDS = ((LEFT_COMM_THREE_LED, OFF, NOW),)
LEFT_COMM_FOUR_OFF_LEDS  = ((LEFT_COMM_FOUR_LED, OFF, NOW),)
LEFT_COMM_FIVE_OFF_LEDS  = ((LEFT_COMM_FIVE_LED, OFF, NOW),)
RIGHT_COMM_ONE_OFF_LEDS  = ((RIGHT_COMM_ONE_LED, OFF, NOW),)
RIGHT_COMM_TWO_OFF_LEDS  = ((RIGHT_COMM_TWO_LED, OFF, NOW),)
RIGHT_COMM_THREE_OFF_LEDS= ((RIGHT_COMM_THREE_LED, OFF, NOW),)
RIGHT_COMM_FOUR_OFF_LEDS = ((RIGHT_COMM_FOUR_LED, OFF, NOW),)
RIGHT_COMM_FIVE_OFF_LEDS = ((RIGHT_COMM_FIVE_LED, OFF, NOW),)

##### KICKOFF #####
HAVE_KICKOFF_LEDS  = ((RIGHT_FOOT_LED, WHITE, NOW),)
NO_KICKOFF_LEDS    = ((RIGHT_FOOT_LED, OFF, NOW),)

##### TEAM COLOR #####
TEAM_BLUE_LEDS = ((LEFT_FOOT_LED, BLUE, NOW),)
TEAM_RED_LEDS = ((LEFT_FOOT_LED, RED, NOW),)

##### STATES #####
STATE_INITIAL_LEDS =  ((CHEST_LED, OFF,    NOW),)
STATE_READY_LEDS =    ((CHEST_LED, BLUE,   NOW),)
STATE_SET_LEDS =      ((CHEST_LED, YELLOW, NOW),)
STATE_PLAYING_LEDS =  ((CHEST_LED, GREEN,  NOW),)
STATE_PENALIZED_LEDS =((CHEST_LED, RED,    NOW),)
STATE_FINISHED_LEDS = ((CHEST_LED, OFF,    NOW),)

class Leds():

    def __init__(self, brainPtr):
        #self.lights = _lights.lights
        self.brain = brainPtr
        self.kickoffChange = True
        self.teamChange = True
        self.goalCertainty = 0
        self.facingOpp = -1
        self.numActiveMates = 0
        self.role = -1
        self.calibrationCount = 0
        self.flippingCount = 0
        self.oldFlipTime = 0
        self.flashingCount = 150

    def processLeds(self):

        if BALL_LEDS:
            if self.brain.ball.vis.frames_on == 1:
                self.executeLeds(BALL_ON_LEDS)
            elif self.brain.ball.vis.frames_off == 1:
                self.executeLeds(BALL_OFF_LEDS)

        if GOAL_LEDS:
            newCertainty = self.brain.ygrp.certainty

            if (newCertainty == 0):
                if (self.brain.ygrp.on and
                    (self.brain.ygrp.frames_on == 1 or
                     self.goalCertainty != newCertainty)):
                    #we see an ambiguous post for the first time!
                    self.executeLeds(LEFT_POST_AMBIGUOUS_LEDS)
                    self.executeLeds(RIGHT_POST_AMBIGUOUS_LEDS)
            if(newCertainty == 2 and
               self.brain.ygrp.on and
               (self.brain.ygrp.frames_on == 1 or
                self.goalCertainty != newCertainty)):
                #we see the right post for the first time!
                self.executeLeds(RIGHT_POST_ON_LEDS)
            if(self.brain.yglp.on and
               (self.brain.yglp.frames_on == 1 or
                self.goalCertainty != newCertainty)):
                #we see the left post for the first time!
                self.executeLeds(LEFT_POST_ON_LEDS)
            if(self.brain.ygrp.frames_off == 1):
                #we don't see the right post for the first time
                self.executeLeds(RIGHT_POST_OFF_LEDS)
            if((self.brain.yglp.frames_off == 1 and
                (newCertainty != 1 and
                 self.brain.ygrp.frames_off >= 1)) or
               (self.brain.yglp.frames_off >=1 and
                ((newCertainty != 1 and
                  newCertainty != self.goalCertainty) or
                 self.brain.ygrp.frames_off == 1))):
                #we don't see the left post for the first time
                self.executeLeds(LEFT_POST_OFF_LEDS)

            self.goalCertainty = newCertainty

            newFacingOpp = (-90 < self.brain.loc.h < 90)
            if (newFacingOpp != self.facingOpp or
                self.facingOpp == -1):
                self.facingOpp = newFacingOpp
                if -90 < self.brain.loc.h < 90:
                    if self.brain.gameController.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                        self.executeLeds(PINK_GOAL_LEDS)
                    else:
                        self.executeLeds(BLUE_GOAL_LEDS)
                else:
                    if self.brain.gameController.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                        self.executeLeds(BLUE_GOAL_LEDS)
                    else:
                        self.executeLeds(PINK_GOAL_LEDS)

        if ROLESWITCH_LEDS:
            newRole = self.brain.player.role
            if newRole != self.role:
                if newRole == 0:
                    self.executeLeds(ROLE_OFF_LEDS)
                else:
                    self.executeLeds(ROLE_DICT[newRole])
                self.role = newRole

        if CALIBRATION_LEDS:
            if (self.brain.motion.standing and not self.brain.motion.calibrated
                and self.calibrationCount % 8 < 4):
                self.executeLeds(LEFT_CALIBRATION_ONE_LEDS)
                self.executeLeds(LEFT_CALIBRATION_TWO_LEDS)
                self.executeLeds(LEFT_CALIBRATION_THREE_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FOUR_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FIVE_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_ONE_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_TWO_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_THREE_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FOUR_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FIVE_LEDS)
            else:
                self.executeLeds(LEFT_CALIBRATION_ONE_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_TWO_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_THREE_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FOUR_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FIVE_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_ONE_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_TWO_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_THREE_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FOUR_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FIVE_OFF_LEDS)

            self.calibrationCount = self.calibrationCount + 1

        # TODO this is a hacky solution to using the same LEDs as calibration
        if SHAREDFLIP_LEDS and self.brain.motion.calibrated:
            if ((self.oldFlipTime != self.brain.interface.sharedFlip.timestamp
                 or self.flashingCount < 150) and self.flippingCount % 20 < 10):
                self.oldFlipTime = self.brain.interface.sharedFlip.timestamp
                if self.flashingCount >= 150:
                    self.flashingCount = 0
                else:
                    self.flashingCount = self.flashingCount + 1

                self.executeLeds(LEFT_CALIBRATION_ONE_LEDS)
                self.executeLeds(LEFT_CALIBRATION_TWO_LEDS)
                self.executeLeds(LEFT_CALIBRATION_THREE_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FOUR_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FIVE_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_ONE_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_TWO_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_THREE_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FOUR_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FIVE_LEDS)
            else:
                self.executeLeds(LEFT_CALIBRATION_ONE_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_TWO_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_THREE_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FOUR_OFF_LEDS)
                self.executeLeds(LEFT_CALIBRATION_FIVE_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_ONE_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_TWO_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_THREE_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FOUR_OFF_LEDS)
                self.executeLeds(RIGHT_CALIBRATION_FIVE_OFF_LEDS)
            self.flippingCount = self.flippingCount + 1

        # If more teammates are added, consider making bottom of right
        # eye into localization uncertainty and using whole ear for comm
        if COMM_LEDS:
            newActiveMates = self.brain.activeTeamMates()
            if (newActiveMates != self.numActiveMates):
                self.numActiveMates = newActiveMates
                if(self.numActiveMates > 0):
                    self.executeLeds(LEFT_COMM_ONE_LEDS)
                    self.executeLeds(RIGHT_COMM_ONE_LEDS)
                else:
                    self.executeLeds(LEFT_COMM_ONE_OFF_LEDS)
                    self.executeLeds(RIGHT_COMM_ONE_OFF_LEDS)
                if(self.numActiveMates > 1):
                    self.executeLeds(LEFT_COMM_TWO_LEDS)
                    self.executeLeds(RIGHT_COMM_TWO_LEDS)
                else:
                    self.executeLeds(LEFT_COMM_TWO_OFF_LEDS)
                    self.executeLeds(RIGHT_COMM_TWO_OFF_LEDS)
                if(self.numActiveMates > 2):
                    self.executeLeds(LEFT_COMM_THREE_LEDS)
                    self.executeLeds(RIGHT_COMM_THREE_LEDS)
                else:
                    self.executeLeds(LEFT_COMM_THREE_OFF_LEDS)
                    self.executeLeds(RIGHT_COMM_THREE_OFF_LEDS)
                if(self.numActiveMates > 3):
                    self.executeLeds(LEFT_COMM_FOUR_LEDS)
                    self.executeLeds(RIGHT_COMM_FOUR_LEDS)
                else:
                    self.executeLeds(LEFT_COMM_FOUR_OFF_LEDS)
                    self.executeLeds(RIGHT_COMM_FOUR_OFF_LEDS)
                if(self.numActiveMates > 4):
                    self.executeLeds(LEFT_COMM_FIVE_LEDS)
                    self.executeLeds(RIGHT_COMM_FIVE_LEDS)
                else:
                    self.executeLeds(LEFT_COMM_FIVE_OFF_LEDS)
                    self.executeLeds(RIGHT_COMM_FIVE_OFF_LEDS)

        if GC_LEDS:
            if self.brain.gameController.stateChanged:
                gcState = self.brain.gameController.currentState
                if (self.brain.gameController.penalized):
                    self.executeLeds(STATE_PENALIZED_LEDS)
                elif (gcState == GameController.STATE_INITIAL):
                    self.executeLeds(STATE_INITIAL_LEDS)
                elif (gcState == GameController.STATE_READY):
                    self.executeLeds(STATE_READY_LEDS)
                elif (gcState == GameController.STATE_SET):
                    self.executeLeds(STATE_SET_LEDS)
                elif (gcState == GameController.STATE_PLAYING):
                    self.executeLeds(STATE_PLAYING_LEDS)
                elif (gcState == GameController.STATE_FINISHED):
                    self.executeLeds(STATE_FINISHED_LEDS)

        if FOOT_LEDS:
            if (self.brain.gameController.kickOffChanged
                or self.brain.gameController.teamColorChanged):
                # At starts of halves, either kickOffChanged or
                # teamColorChanged will trigger but not both, and
                # both LEDs should update.
                if self.brain.gameController.ownKickOff:
                    self.executeLeds(HAVE_KICKOFF_LEDS)
                else:
                    self.executeLeds(NO_KICKOFF_LEDS)

                if self.brain.gameController.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                    self.executeLeds(TEAM_BLUE_LEDS)
                else:
                    self.executeLeds(TEAM_RED_LEDS)

    def executeLeds(self,listOfLeds):
        for ledTuple in listOfLeds:
            if len(ledTuple) != 3:
                print "Invalid print command!! " + str(ledTuple)
                continue
            # Add command to brain's out message fields
            self.brain.interface.ledCommand.add_led_id(ledTuple[0])
            self.brain.interface.ledCommand.add_rgb_hex(ledTuple[1])

            # Unnecessary check, never triggered
            #if ledTuple[2] != NOW:
                #print "Invalid timing command in Leds.py"
