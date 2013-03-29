from playbook import PBConstants
import noggin_constants as NogginConstants

NUM_LED_GROUPS = 31

# LED Related #
GC_LEDS = True
FOOT_LEDS = True
BALL_LEDS = True
GOAL_LEDS = True
PLAYBOOK_LEDS = True
LOC_LEDS = False
COMM_LEDS = True

####### LED DEFINITIONS #############
LED_OFF = 0
LED_ON = 1

#The order here must match the order of the string defined in Lights.h
# and in ALLedNames.h
(LEFT_LOC_ONE_LED,
LEFT_LOC_TWO_LED,
LEFT_LOC_THREE_LED,
LEFT_LOC_FOUR_LED,
LEFT_LOC_FIVE_LED,
RIGHT_LOC_ONE_LED,
RIGHT_LOC_TWO_LED,
RIGHT_LOC_THREE_LED,
RIGHT_LOC_FOUR_LED,
RIGHT_LOC_FIVE_LED,
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
SUBROLE_LED,
ROLE_LED,
BALL_LED,
LEFT_GOAL_LED,
RIGHT_GOAL_LED,
GOAL_ID_LED,
CHEST_LED,
LEFT_FOOT_LED,
RIGHT_FOOT_LED,
LEFT_UNUSED_LED,
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
EYE_YELLOW= 0xFF4400

NOW = 0.0

##### Ball  #####
BALL_ON_LEDS = ((BALL_LED, RED, NOW),)
BALL_OFF_LEDS = ((BALL_LED, BLUE, NOW),)

##### Roles #####
CHASER_ON_LEDS =   ((ROLE_LED, RED, NOW),)
OFFENDER_ON_LEDS = ((ROLE_LED, EYE_YELLOW, NOW),)
MIDDIE_ON_LEDS =   ((ROLE_LED, GREEN, NOW),)
DEFENDER_ON_LEDS = ((ROLE_LED, BLUE, NOW),)
DUB_DEFEND_ON_LEDS=((ROLE_LED, CYAN, NOW),)
GOALIE_ON_LEDS =   ((ROLE_LED, PURPLE, NOW),)
ROLE_OFF_LEDS =    ((ROLE_LED, OFF, NOW),)

##### SubRoles #####
AGGRESSIVE_LEDS = ((SUBROLE_LED, RED, NOW),)
BOLD_LEDS =       ((SUBROLE_LED, EYE_YELLOW, NOW),)
CAUTIOUS_LEDS =   ((SUBROLE_LED, GREEN, NOW),)
GUARDED_LEDS =    ((SUBROLE_LED, BLUE, NOW),)

GOALIE_CHASE_LEDS =  ((SUBROLE_LED, PURPLE, NOW),)
GOALIE_CENTER_LEDS = ((SUBROLE_LED, BLUE, NOW),)
GOALIE_LEFT_LEDS =   ((SUBROLE_LED, RED, NOW),)
GOALIE_RIGHT_LEDS =  ((SUBROLE_LED, GREEN, NOW),)

OTHER_SUBROLE_LEDS= ((SUBROLE_LED, OFF, NOW),)

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

##### LOC #####
LEFT_LOC_ONE_LEDS   = ((LEFT_LOC_ONE_LED, BLUE, NOW),)
LEFT_LOC_TWO_LEDS   = ((LEFT_LOC_TWO_LED, BLUE, NOW),)
LEFT_LOC_THREE_LEDS = ((LEFT_LOC_THREE_LED, BLUE, NOW),)
LEFT_LOC_FOUR_LEDS  = ((LEFT_LOC_FOUR_LED, BLUE, NOW),)
LEFT_LOC_FIVE_LEDS  = ((LEFT_LOC_FIVE_LED, BLUE, NOW),)
RIGHT_LOC_ONE_LEDS  = ((RIGHT_LOC_ONE_LED, BLUE, NOW),)
RIGHT_LOC_TWO_LEDS  = ((RIGHT_LOC_TWO_LED, BLUE, NOW),)
RIGHT_LOC_THREE_LEDS= ((RIGHT_LOC_THREE_LED, BLUE, NOW),)
RIGHT_LOC_FOUR_LEDS = ((RIGHT_LOC_FOUR_LED, BLUE, NOW),)
RIGHT_LOC_FIVE_LEDS = ((RIGHT_LOC_FIVE_LED, BLUE, NOW),)
LEFT_LOC_ONE_OFF_LEDS   = ((LEFT_LOC_ONE_LED, OFF, NOW),)
LEFT_LOC_TWO_OFF_LEDS   = ((LEFT_LOC_TWO_LED, OFF, NOW),)
LEFT_LOC_THREE_OFF_LEDS = ((LEFT_LOC_THREE_LED, OFF, NOW),)
LEFT_LOC_FOUR_OFF_LEDS  = ((LEFT_LOC_FOUR_LED, OFF, NOW),)
LEFT_LOC_FIVE_OFF_LEDS  = ((LEFT_LOC_FIVE_LED, OFF, NOW),)
RIGHT_LOC_ONE_OFF_LEDS  = ((RIGHT_LOC_ONE_LED, OFF, NOW),)
RIGHT_LOC_TWO_OFF_LEDS  = ((RIGHT_LOC_TWO_LED, OFF, NOW),)
RIGHT_LOC_THREE_OFF_LEDS= ((RIGHT_LOC_THREE_LED, OFF, NOW),)
RIGHT_LOC_FOUR_OFF_LEDS = ((RIGHT_LOC_FOUR_LED, OFF, NOW),)
RIGHT_LOC_FIVE_OFF_LEDS = ((RIGHT_LOC_FIVE_LED, OFF, NOW),)

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

    def processLeds(self):

        if BALL_LEDS:
            if self.brain.ball.framesOn == 1:
                self.executeLeds(BALL_ON_LEDS)
            elif self.brain.ball.framesOff == 1:
                self.executeLeds(BALL_OFF_LEDS)

        if False:#GOAL_LEDS:
            newCertainty = self.brain.ygrp.vis.certainty

            if (newCertainty == vision.certainty.NOT_SURE):
                if (self.brain.ygrp.vis.on and
                    (self.brain.ygrp.vis.framesOn == 1 or
                     self.goalCertainty != newCertainty)):
                    #we see an ambiguous post for the first time!
                    self.executeLeds(LEFT_POST_AMBIGUOUS_LEDS)
                    self.executeLeds(RIGHT_POST_AMBIGUOUS_LEDS)
            if(newCertainty == vision.certainty._SURE and
               self.brain.ygrp.vis.on and
               (self.brain.ygrp.vis.framesOn == 1 or
                self.goalCertainty != newCertainty)):
                #we see the right post for the first time!
                self.executeLeds(RIGHT_POST_ON_LEDS)
            if(self.brain.yglp.vis.on and
               (self.brain.yglp.vis.framesOn == 1 or
                self.goalCertainty != newCertainty)):
                #we see the left post for the first time!
                self.executeLeds(LEFT_POST_ON_LEDS)
            if(self.brain.ygrp.vis.framesOff == 1):
                #we don't see the right post for the first time
                self.executeLeds(RIGHT_POST_OFF_LEDS)
            if((self.brain.yglp.vis.framesOff == 1 and
                (newCertainty != vision.certainty.NOT_SURE and
                 self.brain.ygrp.vis.framesOff >= 1)) or
               (self.brain.yglp.vis.framesOff >=1 and
                ((newCertainty != vision.certainty.NOT_SURE and
                  newCertainty != self.goalCertainty) or
                 self.brain.ygrp.vis.framesOff == 1))):
                #we don't see the left post for the first time
                self.executeLeds(LEFT_POST_OFF_LEDS)

            self.goalCertainty = newCertainty


            # TODO make this part actually tell us if we are looking at our goal or not
            # via flag in loc (???)
            newFacingOpp = (-90 < self.brain.my.h < 90)
            if (newFacingOpp != self.facingOpp or
                self.facingOpp == -1):
                self.facingOpp = newFacingOpp
                if -90 < self.brain.my.h < 90:
                    if self.brain.my.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                        self.executeLeds(PINK_GOAL_LEDS)
                    else:
                        self.executeLeds(BLUE_GOAL_LEDS)
                else:
                    if self.brain.my.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                        self.executeLeds(BLUE_GOAL_LEDS)
                    else:
                        self.executeLeds(PINK_GOAL_LEDS)
#                self.executeLeds(NO_GOAL_LEDS)

        if PLAYBOOK_LEDS:
            if self.brain.playbook.roleChanged():
                if self.brain.play.isRole(PBConstants.CHASER):
                    self.executeLeds(CHASER_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.OFFENDER):
                    self.executeLeds(OFFENDER_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.MIDDIE):
                    self.executeLeds(MIDDIE_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.DEFENDER):
                    self.executeLeds(DEFENDER_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.DEFENDER_DUB_D):
                    print "IN DUB D"
                    self.executeLeds(DUB_DEFEND_ON_LEDS)
                elif self.brain.play.isRole(PBConstants.GOALIE):
                    self.executeLeds(GOALIE_ON_LEDS)
                else:
                    self.executeLeds(ROLE_OFF_LEDS)

            if not self.brain.playbook.subRoleUnchanged():
                play = self.brain.play
                if play.isRole(PBConstants.GOALIE):
                    if (play.isSubRole(PBConstants.GOALIE_CENTER)):
                        self.executeLeds(GOALIE_CENTER_LEDS)
                    #elif (play.isSubRole(PBConstants.GOALIE_LEFT)):
                    #    self.executeLeds(GOALIE_LEFT_LEDS)
                    #elif (play.isSubRole(PBConstants.GOALIE_RIGHT)):
                    #    self.executeLeds(GOALIE_RIGHT_LEDS)
                    elif (play.isSubRole(PBConstants.GOALIE_CHASER)):
                        self.executeLeds(GOALIE_CHASE_LEDS)
                    else:
                        self.executeLeds(OTHER_SUBROLE_LEDS)
                else:
                    if (play.isSubRole(PBConstants.CHASE_NORMAL) or
                        play.isSubRole(PBConstants.PICKER) or
                        play.isSubRole(PBConstants.DEFENSIVE_MIDDIE) or
                        play.isSubRole(PBConstants.STOPPER)):
                        self.executeLeds(AGGRESSIVE_LEDS)
                    elif (play.isSubRole(PBConstants.LEFT_WING) or
                          play.isSubRole(PBConstants.RIGHT_WING) or
                          play.isSubRole(PBConstants.KICKOFF_STRIKER) or
                          play.isSubRole(PBConstants.CENTER_BACK)):
                        self.executeLeds(BOLD_LEDS)
                    elif (play.isSubRole(PBConstants.STRIKER) or
                          play.isSubRole(PBConstants.OFFENSIVE_MIDDIE) or
                          play.isSubRole(PBConstants.SWEEPER)):
                        self.executeLeds(CAUTIOUS_LEDS)
                    elif (play.isSubRole(PBConstants.FORWARD) or
                          play.isSubRole(PBConstants.DUB_D_MIDDIE) or
                          play.isSubRole(PBConstants.KICKOFF_SWEEPER) or
                          play.isSubRole(PBConstants.LEFT_DEEP_BACK) or
                          play.isSubRole(PBConstants.RIGHT_DEEP_BACK)):
                        self.executeLeds(GUARDED_LEDS)
                    else:
                        self.executeLeds(OTHER_SUBROLE_LEDS)
        if LOC_LEDS:
            # TODO: show loc uncertainty via LEDS
            pass

        if False:#COMM_LEDS:
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
            if self.brain.gameController.counter == 1:
                gcState = self.brain.gameController.currentState
                if (gcState == 'gameInitial' or
                    gcState == 'penaltyShotsGameInitial'):
                    self.executeLeds(STATE_INITIAL_LEDS)
                elif (gcState == 'gameReady' or
                      gcState == 'penaltyShotsGameReady'):
                    self.executeLeds(STATE_READY_LEDS)
                elif (gcState == 'gameSet' or
                      gcState == 'penaltyShotsGameSet'):
                    self.executeLeds(STATE_SET_LEDS)
                elif (gcState == 'gamePlaying' or
                      gcState == 'penaltyShotsGamePlaying'):
                    self.executeLeds(STATE_PLAYING_LEDS)
                elif (gcState == 'gamePenalized' or
                      gcState == 'penaltyShotsGamePenalized'):
                    self.executeLeds(STATE_PENALIZED_LEDS)
                elif (gcState == 'gameFinished' or
                      gcState == 'penaltyShotsGameFinished'):
                    self.executeLeds(STATE_FINISHED_LEDS)


        if FOOT_LEDS:
            if self.kickoffChange:
                if self.brain.gameController.ownKickOff:
                    self.executeLeds(HAVE_KICKOFF_LEDS)
                else:
                    self.executeLeds(NO_KICKOFF_LEDS)
                self.kickoffChange = False
            if self.teamChange:
                if self.brain.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                    self.executeLeds(TEAM_BLUE_LEDS)
                else:
                    self.executeLeds(TEAM_RED_LEDS)
                self.teamChange = False

    def executeLeds(self,listOfLeds):
        self.brain.out.printf("executing leds.")
        for ledTuple in listOfLeds:
            if len(ledTuple) != 3:
                self.printf("Invalid print command!! " + str(ledTuple))
                continue
            # Add command to brain's out message fields
            self.brain.interface.ledCommand.add_led_id(ledTuple[0])
            self.brain.interface.ledCommand.add_rgb_hex(ledTuple[1])

            # Unnecessary check, never triggered
            #if ledTuple[2] != NOW:
                #print "Invalid timing command in Leds.py"
