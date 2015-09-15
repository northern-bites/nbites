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
GOALBOX_LEDS = True
CENTER_CIRCLE_LEDS = True
ROLESWITCH_LEDS = True
CALIBRATION_LEDS = True
DODGE_LEDS = True
SHAREDFLIP_LEDS = True
COMM_LEDS = True

####### LED DEFINITIONS #############
LED_OFF = 0
LED_ON = 1

#The order here must match the order of the string defined in Lights.h
# and in ALLedNames.h
NUM_LED_GROUPS = 26

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
GOALBOX_LED,
CHEST_LED,
LEFT_FOOT_LED,
RIGHT_FOOT_LED) = range(NUM_LED_GROUPS)

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
BALL_OFF_LEDS = ((BALL_LED, OFF, NOW),)

##### GoalBox #####
GOALBOX_ON_LEDS = ((GOALBOX_LED, GREEN, NOW),)
GOALBOX_OFF_LEDS = ((GOALBOX_LED, OFF, NOW),)

##### Center Circle Leds #####
CENTER_CIRCLE_ON_LEDS = ((GOALBOX_LED, WHITE, NOW),)

##### Roles #####
ROLE_ONE      = ((ROLE_LED, GREEN, NOW),)
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

##### DODGE #####
LEFT_DODGE_ONE_LEDS   = ((LEFT_CALIBRATION_ONE_LED, BLUE, NOW),)
LEFT_DODGE_TWO_LEDS   = ((LEFT_CALIBRATION_TWO_LED, BLUE, NOW),)
LEFT_DODGE_THREE_LEDS = ((LEFT_CALIBRATION_THREE_LED, BLUE, NOW),)
LEFT_DODGE_FOUR_LEDS  = ((LEFT_CALIBRATION_FOUR_LED, BLUE, NOW),)
LEFT_DODGE_FIVE_LEDS  = ((LEFT_CALIBRATION_FIVE_LED, BLUE, NOW),)
RIGHT_DODGE_ONE_LEDS  = ((RIGHT_CALIBRATION_ONE_LED, BLUE, NOW),)
RIGHT_DODGE_TWO_LEDS  = ((RIGHT_CALIBRATION_TWO_LED, BLUE, NOW),)
RIGHT_DODGE_THREE_LEDS= ((RIGHT_CALIBRATION_THREE_LED, BLUE, NOW),)
RIGHT_DODGE_FOUR_LEDS = ((RIGHT_CALIBRATION_FOUR_LED, BLUE, NOW),)
RIGHT_DODGE_FIVE_LEDS = ((RIGHT_CALIBRATION_FIVE_LED, BLUE, NOW),)

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
        self.facingOpp = -1
        self.numActiveMates = 0
        self.role = -1
        self.calibrationCount = 0
        self.dodgeCount = 0
        self.flippingCount = 0
        self.oldFlipTime = 0
        self.flashingCount = 150

    def processLeds(self):

        if BALL_LEDS:
            if self.brain.ball.vis.frames_on == 1:
                self.executeLeds(BALL_ON_LEDS)
            elif self.brain.ball.vis.frames_off == 1:
                self.executeLeds(BALL_OFF_LEDS)

        if CENTER_CIRCLE_LEDS and self.brain.vision.circle.on:
            self.executeLeds(CENTER_CIRCLE_ON_LEDS)
        # Only do goalbox if we didn't see a center circle
        elif GOALBOX_LEDS:
            gbOn = False
            for i in range(0, self.brain.vision.line_size()):
                if self.brain.vision.line(i).id == 7:
                    self.executeLeds(GOALBOX_ON_LEDS)
                    gbOn = True
                    break
            if not gbOn:
                self.executeLeds(GOALBOX_OFF_LEDS)
        else:
            self.executeLeds(GOALBOX_OFF_LEDS)

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
            elif (DODGE_LEDS and self.brain.nav.currentState=="dodge" and self.dodgeCount % 4 < 2):
                self.executeLeds(LEFT_DODGE_ONE_LEDS)
                self.executeLeds(LEFT_DODGE_TWO_LEDS)
                self.executeLeds(LEFT_DODGE_THREE_LEDS)
                self.executeLeds(LEFT_DODGE_FOUR_LEDS)
                self.executeLeds(LEFT_DODGE_FIVE_LEDS)
                self.executeLeds(RIGHT_DODGE_ONE_LEDS)
                self.executeLeds(RIGHT_DODGE_TWO_LEDS)
                self.executeLeds(RIGHT_DODGE_THREE_LEDS)
                self.executeLeds(RIGHT_DODGE_FOUR_LEDS)
                self.executeLeds(RIGHT_DODGE_FIVE_LEDS)
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
            self.dodgeCount = self.dodgeCount + 1

        # TODO this is a hacky solution to using the same LEDs as calibration
        # if SHAREDFLIP_LEDS and self.brain.motion.calibrated:
        #     if ((self.oldFlipTime != self.brain.interface.sharedFlip.timestamp
        #          or self.flashingCount < 150) and self.flippingCount % 20 < 10):
        #         self.oldFlipTime = self.brain.interface.sharedFlip.timestamp
        #         if self.flashingCount >= 150:
        #             self.flashingCount = 0
        #         else:
        #             self.flashingCount = self.flashingCount + 1

        #         self.executeLeds(LEFT_CALIBRATION_ONE_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_TWO_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_THREE_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_FOUR_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_FIVE_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_ONE_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_TWO_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_THREE_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_FOUR_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_FIVE_LEDS)
        #     else:
        #         self.executeLeds(LEFT_CALIBRATION_ONE_OFF_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_TWO_OFF_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_THREE_OFF_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_FOUR_OFF_LEDS)
        #         self.executeLeds(LEFT_CALIBRATION_FIVE_OFF_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_ONE_OFF_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_TWO_OFF_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_THREE_OFF_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_FOUR_OFF_LEDS)
        #         self.executeLeds(RIGHT_CALIBRATION_FIVE_OFF_LEDS)
        #     self.flippingCount = self.flippingCount + 1

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
