import _lights
from playbook import PBConstants
import noggin_constants as NogginConstants

# LED Related #
GC_LEDS = True
FOOT_LEDS = True
BALL_LEDS = True
GOAL_LEDS = False
PLAYBOOK_LEDS = False
LOC_LEDS = False
COMM_LEDS = False

####### LED DEFINITIONS #############
LED_OFF = 0
LED_ON = 1

#The order here must match the order of the string defined in Lights.h
# and in ALLedNames.h
(LEFT_LOC_LED,
LEFT_COMM_LED,
RIGHT_LOC_LED,
RIGHT_COMM_LED,
SUBROLE_LED,
ROLE_LED,
BALL_LED,
LEFT_GOAL_LED,
RIGHT_GOAL_LED,
GOAL_ID_LED,
CHEST_LED,
LEFT_FOOT_LED,
RIGHT_FOOT_LED) = range(_lights.NUM_LED_GROUPS)

###COLORS
RED   = 0xFF0000
GREEN = 0x00FF00
BLUE  = 0x0000FF
YELLOW= 0xFFFF00
PURPLE= 0xFF00FF
CYAN  = 0x00FFFF
WHITE = 0xFFFFFF
OFF   = 0x000000
NOW = 0.0

##### Ball  #####
BALL_ON_LEDS = ((BALL_LED, RED, NOW),)
BALL_OFF_LEDS = ((BALL_LED, BLUE, NOW),)

##### Roles #####
CHASER_ON_LEDS =   ((ROLE_LED, RED, NOW),)
OFFENDER_ON_LEDS = ((ROLE_LED, YELLOW, NOW),)
MIDDIE_ON_LEDS =   ((ROLE_LED, GREEN, NOW),)
DEFENDER_ON_LEDS = ((ROLE_LED, BLUE, NOW),)
GOALIE_ON_LEDS =   ((ROLE_LED, PURPLE, NOW),)
OTHER_ROLE_LEDS =    ((ROLE_LED, WHITE, NOW),)

##### SubRoles #####
AGGRESSIVE_LEDS = ((SUBROLE_LED, RED, NOW),)
BOLD_LEDS =       ((SUBROLE_LED, YELLOW, NOW),)
CAUTIOUS_LEDS =   ((SUBROLE_LED, GREEN, NOW),)
GUARDED_LEDS =    ((SUBROLE_LED, BLUE, NOW),)
OTHER_SUBROLE_LEDS= ((SUBROLE_LED, WHITE, NOW),)

##### GOAL ######
LEFT_POST_ON_LEDS =   ((LEFT_GOAL_LED, YELLOW, NOW),)
LEFT_POST_AMBIGUOUS_LEDS = ((LEFT_GOAL_LED, GREEN, NOW),)
LEFT_POST_OFF_LEDS = ((LEFT_GOAL_LED, BLUE, NOW),)
RIGHT_POST_ON_LEDS =  ((RIGHT_GOAL_LED, YELLOW, NOW),)
RIGHT_POST_AMBIGUOUS_LEDS = ((RIGHT_GOAL_LED, GREEN, NOW),)
RIGHT_POST_OFF_LEDS = ((RIGHT_GOAL_LED, BLUE, NOW),)

##### LOC #####
LEFT_LOC_LEDS = ((LEFT_LOC_LED, BLUE, NOW),)
RIGHT_LOC_LEDS = ((RIGHT_LOC_LED, BLUE, NOW),)

##### COMM #####
LEFT_COMM_LEDS = ((LEFT_COMM_LED, BLUE, NOW),)
RIGHT_COMM_LEDS = ((RIGHT_COMM_LED, BLUE, NOW),)

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
        self.lights = _lights.lights
        self.brain = brainPtr
        self.kickoffChange = True
        self.teamChange = True

    def processLeds(self):
        if BALL_LEDS:
            if self.brain.ball.vis.framesOn == 1:
                self.executeLeds(BALL_ON_LEDS)
            elif self.brain.ball.vis.framesOff == 1:
                self.executeLeds(BALL_OFF_LEDS)

        if GOAL_LEDS:
            if (self.brain.myGoalLeftPost.vis.framesOn == 1 or
                self.brain.oppGoalLeftPost.vis.framesOn == 1 or
                self.brain.myGoalRightPost.vis.framesOn == 1 or
                self.brain.oppGoalRightPost.vis.framesOn == 1):

                self.executeLeds(LEFT_POST_ON_LEDS)
                self.executeLeds(RIGHT_POST_ON_LEDS)

            elif (self.brain.myGoalLeftPost.vis.framesOff == 1 or
                self.brain.oppGoalLeftPost.vis.framesOff == 1 or
                self.brain.myGoalRightPost.vis.framesOff == 1 or
                self.brain.oppGoalRightPost.vis.framesOff == 1):

                self.executeLeds(LEFT_POST_OFF_LEDS)
                self.executeLeds(RIGHT_POST_OFF_LEDS)

        if PLAYBOOK_LEDS:
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
                        self.executeLeds(OTHER_ROLE_LEDS)
        if LOC_LEDS:
            pass

        if COMM_LEDS:
            pass

        if GC_LEDS:
            if self.brain.gameController.firstFrame():
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
            if self.teamChange:
                if self.my.teamColor == NogginConstants.teamColor.TEAM_BLUE:
                    self.executeLeds(TEAM_BLUE_LEDS)
                else:
                    self.executeLeds(TEAM_RED_LEDS)

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
