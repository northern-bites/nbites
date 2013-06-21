from ..util import MyMath
from ..typeDefs import TeamMember
from . import PBConstants
from objects import Location, RobotLocation, RelRobotLocation
from math import degrees, ceil
import noggin_constants as NogginConstants
import time
from PlaybookTable import playbookTable

# ANSI terminal color codes
# http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html
RESET_COLORS_CODE = '\033[0m'
RED_COLOR_CODE = '\033[31m'
GREEN_COLOR_CODE = '\033[32m'
YELLOW_COLOR_CODE = '\033[33m'
BLUE_COLOR_CODE = '\033[34m'
PURPLE_COLOR_CODE = '\033[35m'
CYAN_COLOR_CODE = '\033[36m'

class GoTeam:
    """This is the class which controls all of our coordinated
       behavior system."""
    def __init__(self, brain):
        self.brain = brain
        self.printStateChanges = True
        self.time = time.time()

        # Information about teammates
        self.me = self.brain.teamMembers[self.brain.playerNumber - 1]
        self.me.playerNumber = self.brain.playerNumber
        self.goalieIsActive = False
        self.activeFieldPlayers = []
        self.numActiveFieldPlayers = 0

        self.potentialBallX = -1
        self.potentialBallXFrames = 0
        self.potentialBallY = -1
        self.potentialBallYFrames = 0
        self.lastBallX = -1
        self.lastBallY = -1

    def run(self, play):
        """We run this each frame to get the latest info"""
        self.aPrioriTeammateUpdate()

        play.changed = False
        self.strategize(play)

        # If the play has changed, print.
        self.updateStateInfo(play)

    def strategize(self, play):
        """
        creates a play, picks the strategy to run, returns the play after
        it is modified by Strategies
        """
        currentGCState = self.brain.player.gameState
        # We don't control anything in initial or finished
        if (currentGCState == 'gameInitial' or
            currentGCState == 'gameFinished'):
            play.setRole(PBConstants.INIT_ROLE)

            self.lastBallX = -1
            self.lastBallY = -1
            return

        # Have a separate strategy to easily deal with being penalized
        elif not self.me.active:
            play.setRole(PBConstants.PENALTY_ROLE)

            self.lastBallX = -1
            self.lastBallY = -1
            return

        # Special case for the goalie
        if (self.me.isDefaultGoalie()):
            # Make sure the goalie's role is set
            play.setRole(PBConstants.GOALIE)
            dest = Location(PBConstants.GOALIE_HOME_X,
                            PBConstants.GOALIE_HOME_Y)
            h = dest.headingTo(Location(self.brain.ball.x, self.brain.ball.y))
            pos = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)
            play.setPosition(pos)
            return

        # Have a separate ready section to make things simpler
        if (currentGCState == 'gameReady' or currentGCState =='gameSet'):
            if (currentGCState == 'gameReady'):
                self.lastBallX = -1
                self.lastBallY = -1
            self.readyPosition(play)
            return

        # Update the current grid square that the ball is.
        self.ballUpdate()

        test = False
        # Check test cases
        if (PBConstants.TEST_DEFENDER or PBConstants.TEST_OFFENDER
            or PBConstants.TEST_MIDDIE or PBConstants.TEST_CHASER):
            test = True

        # Use the playbook table to determine position.
        self.priorityPositions(
            self.tableLookup(self.lastBallX, self.lastBallY, test) , play)

    def ballUpdate(self):
        # Sometimes the ball filter produces infinity values when starting.
        # HACK: Clip brain.ball.x and brain.ball.y until Global Ball
        # Model does it for us.
        if self.brain.ball.x == float("inf") or self.brain.ball.x == float("-inf"):
            self.brain.ball.x = NogginConstants.MIDFIELD_X
        if self.brain.ball.y == float("inf") or self.brain.ball.y == float("-inf"):
            self.brain.ball.y = NogginConstants.MIDFIELD_Y

        #Which grid entry is the ball in?
        newBallX = int(ceil((self.brain.ball.x - NogginConstants.GREEN_PAD_X) /
                            PBConstants.TABLE_GRID_SIZE))
        newBallY = int(ceil((self.brain.ball.y - NogginConstants.GREEN_PAD_Y) /
                            PBConstants.TABLE_GRID_SIZE))

        # For robustness, clip these values
        newBallX = MyMath.clip(newBallX, 0, PBConstants.TABLE_GRID_WIDTH)
        newBallY = MyMath.clip(newBallY, 0, PBConstants.TABLE_GRID_HEIGHT)

        # If lastBallX or lastBallY is -1, our best guess is newBallX and newBallY
        if (self.lastBallX == -1 and self.lastBallY == -1):
            self.lastBallX = newBallX
            self.lastBallY = newBallY

        # Buffer the x coordinate for the grid to avoid rapid oscillations
        if newBallX != self.lastBallX:
            if newBallX == self.potentialBallX:
                self.potentialBallXFrames += 1
                if self.potentialBallXFrames > 10:
                    self.lastBallX = self.potentialBallX
            else:
                self.potentialBallX = newBallX
                self.potentialBallXFrames = 1

        # Buffer the y coordinate for the grid to avoid rapid oscillations
        if newBallY != self.lastBallY:
            if newBallY == self.potentialBallY:
                self.potentialBallYFrames += 1
                if self.potentialBallYFrames > 10:
                    self.lastBallY = self.potentialBallY
            else:
                self.potentialBallY = newBallY
                self.potentialBallYFrames = 1

    def tableLookup(self, ball_x, ball_y, test = False):
        """
        Given where we think the ball is on the field (from loc),
        use the playbook table to look up where we should position.
        @return: list of position tuples in order of priority.
        """
        entry = playbookTable[ball_x][ball_y]
        offset = 0
        positions = []

        if self.numActiveFieldPlayers == 3:
            offset = 4
        elif self.numActiveFieldPlayers == 2:
            offset = 7
        elif self.numActiveFieldPlayers == 1:
            offset = 9

        if test:
            offset = 0

        for i in range(self.numActiveFieldPlayers):
            if self.goalieIsActive:
                positions.append(entry[1][i+offset])
            else:
                positions.append(entry[0][i+offset])

        return positions

    def priorityPositions(self, positions, play):
        """
        Determine which player should go to each position.
        """
        locations = map(self.mapPositionToRobotLocation, positions)

        roles = [pos[3] for pos in positions]

        # Check test cases
        if PBConstants.TEST_DEFENDER:
            for i, role in enumerate(roles):
                if role == PBConstants.DEFENDER:
                    play.setRole(role)
                    play.setPosition(locations[i])
                    return
            print "COULD NOT FIND DEFENDER TO TEST"
        elif PBConstants.TEST_OFFENDER:
            for i, role in enumerate(roles):
                if role == PBConstants.OFFENDER:
                    play.setRole(role)
                    play.setPosition(locations[i])
                    return
            print "COULD NOT FIND OFFENDER TO TEST"
        elif PBConstants.TEST_MIDDIE:
            for i, role in enumerate(roles):
                if role == PBConstants.MIDDIE:
                    play.setRole(role)
                    play.setPosition(locations[i])
                    return
            print "COULD NOT FIND MIDDIE TO TEST"
        elif PBConstants.TEST_CHASER:
            for i, role in enumerate(roles):
                if role == PBConstants.CHASER:
                    play.setRole(role)
                    play.setPosition(locations[i])
                    return
            print "COULD NOT FIND CHASER TO TEST"

        self.updateMyTimes(locations, roles)

        # Set the chaser!
        chaser_mate = self.determineChaser()
        if chaser_mate.playerNumber == self.brain.playerNumber:
            play.setRole(roles[-1])
            play.setPosition(locations[-1])
            return

        # Find which active field player should go to each position
        if(roles[0] == PBConstants.CHASER):
            print "Hey. You're an idiot. Priorities are fucked."
        firstPlayer = self.findClosestPlayer(roles[0],
                                             [chaser_mate])
        if firstPlayer.playerNumber == self.brain.playerNumber:
            play.setRole(roles[0])
            play.setPosition(locations[0])
        else:
            if(roles[1] == PBConstants.CHASER):
                print "Hey. You're an idiot. Priorities are fucked."
            secondPlayer = self.findClosestPlayer(roles[1],
                                                  [firstPlayer,
                                                   chaser_mate])
            if secondPlayer.playerNumber == self.brain.playerNumber:
                play.setRole(roles[1])
                play.setPosition(locations[1])
            else:
                if(roles[2] == PBConstants.CHASER):
                    print "Hey. You're an idiot. Priorities are fucked."
                play.setRole(roles[2])
                play.setPosition(locations[2])

    def mapPositionToRobotLocation(self, position):
        """
        Position must be a tuple with x, y, heading, role.
        @return: that position as a Robot Location
        """
        return RobotLocation(position[0], position[1], position[2])


    def readyPosition(self, play):
        kickoff = self.brain.gameController.ownKickOff
        locations = None

        if kickoff:
            locations = (PBConstants.READY_O_CHASER_LOCATION,
                         PBConstants.READY_O_DEFENDER_LOCATION,
                         PBConstants.READY_O_OFFENDER_LOCATION,
                         PBConstants.READY_O_MIDDIE_LOCATION)
        else:
            locations = (PBConstants.READY_D_CHASER_LOCATION,
                         PBConstants.READY_D_DEFENDER_LOCATION,
                         PBConstants.READY_D_OFFENDER_LOCATION,
                         PBConstants.READY_D_MIDDIE_LOCATION)

        # Check test cases
        if PBConstants.TEST_DEFENDER:
            play.setRole(PBConstants.DEFENDER)
            play.setPosition(locations[1])
            return
        elif PBConstants.TEST_OFFENDER:
            play.setRole(PBConstants.OFFENDER)
            play.setPosition(locations[2])
            return
        elif PBConstants.TEST_MIDDIE:
            play.setRole(PBConstants.MIDDIE)
            play.setPosition(locations[3])
            return
        elif PBConstants.TEST_CHASER:
            play.setRole(PBConstants.CHASER)
            play.setPosition(locations[0])
            return

        self.updateMyTimes(locations, ready = True)

        chaser = self.determineChaser()
        if chaser.playerNumber == self.brain.playerNumber:
            play.setRole(PBConstants.CHASER)
            play.setPosition(locations[0])
        else:
            defender = self.determineDefender([chaser])
            if defender.playerNumber == self.brain.playerNumber:
                play.setRole(PBConstants.DEFENDER)
                play.setPosition(locations[1])
            else:
                offender = self.determineOffender([chaser, defender])
                if offender.playerNumber == self.brain.playerNumber:
                    play.setRole(PBConstants.OFFENDER)
                    play.setPosition(locations[2])
                else: # Middie
                    play.setRole(PBConstants.MIDDIE)
                    play.setPosition(locations[3])


    def determineChaser(self):
        """return the team member who is the chaser"""
        chaser_mate = self.activeFieldPlayers[0]

        if PBConstants.DEBUG_DET_CHASER:
            self.printf("chaser det: me == #%g"% self.brain.playerNumber)

        # save processing time and skip the rest if we have the ball
        if self.brain.player.inKickingState:
            if PBConstants.DEBUG_DET_CHASER:
                self.printf("I should Chase")
            return self.me

        # loop through the teammates
        for mate in self.activeFieldPlayers:
            if PBConstants.DEBUG_DET_CHASER:
                self.printf("\t mate #%g"% mate.playerNumber)

            # We can skip computation if the mate we are now considering
            # is the chaser_mate.
            if (mate == chaser_mate):
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g is chaser_mate already."
                                % mate.playerNumber)
                continue

            elif(mate.inKickingState):
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g has ball" % mate.playerNumber)
                return mate

            else:
                if self.shouldCallOff(chaser_mate.chaseTime, mate.chaseTime,
                                      mate.isTeammateRole(PBConstants.CHASER)):
                    if PBConstants.DEBUG_DET_CHASER:
                        self.printf("\t #%d @ %g < #%d @ %g, shouldCallOff" %
                                    (mate.playerNumber, mate.chaseTime,
                                     chaser_mate.playerNumber,
                                     chaser_mate.chaseTime))
                    chaser_mate = mate

                elif PBConstants.DEBUG_DET_CHASER:
                    self.printf (("\t #%d @ %g > #%d @ %g, didn't call off" %
                                  (mate.playerNumber, mate.chaseTime,
                                   chaser_mate.playerNumber,
                                   chaser_mate.chaseTime)))

        if PBConstants.DEBUG_DET_CHASER:
            self.printf ("\t ---- MATE %g WINS" % (chaser_mate.playerNumber))
        # returns teammate instance (could be mine)

        return chaser_mate

    def determineDefender(self, except_players = []):
        possible_mates = [mate for mate in self.activeFieldPlayers\
                          if mate not in except_players]

        defender_mate = possible_mates[0]

        # loop through the teammates
        for mate in possible_mates:
            # We can skip computation if the mate we are now considering
            # is the chaser_mate.
            if (mate == defender_mate):
                continue

            if self.shouldCallOff(defender_mate.defenderTime, mate.defenderTime,
                                  mate.isTeammateRole(PBConstants.DEFENDER)):
                defender_mate = mate

        return defender_mate

    def determineOffender(self, except_players = []):
        possible_mates = [mate for mate in self.activeFieldPlayers\
                          if mate not in except_players]

        offender_mate = possible_mates[0]

        # loop through the teammates
        for mate in possible_mates:
            # We can skip computation if the mate we are now considering
            # is the chaser_mate.
            if (mate == offender_mate):
                continue

            # Skip exceptions
            if (mate in except_players):
                continue

            if self.shouldCallOff(offender_mate.offenderTime, mate.offenderTime,
                                  mate.isTeammateRole(PBConstants.OFFENDER)):
                        offender_mate = mate

        return offender_mate

    def determineMiddie(self, except_players = []):
        possible_mates = [mate for mate in self.activeFieldPlayers\
                          if mate not in except_players]

        middie_mate = possible_mates[0]

        # loop through the teammates
        for mate in possible_mates:
            # We can skip computation if the mate we are now considering
            # is the chaser_mate.
            if (mate == middie_mate):
                continue

            # Skip exceptions
            if (mate in except_players):
                continue

            if self.shouldCallOff(middie_mate.middieTime, mate.middieTime,
                                  mate.isTeammateRole(PBConstants.MIDDIE)):
                middie_mate = mate

        return middie_mate

    def shouldCallOff(self, chosen_mate_time, mate_time, mate_prev_chosen):
        """Decides if mate shouldCallOff the chosen_mate"""
        # mate = A, chaser_mate = B.
        # A will become chaser_mate if:
        # [ (chaseTime(A) - chaseTime(B) < e) or
        #   (chaseTime(A) - chaseTime(B) < d and A is already chasing)]
        # Note: d > e
        # A is higher robot that has decided to be chaser.
        return(((mate_time - chosen_mate_time) <
                PBConstants.CALL_OFF_THRESH) or
               ((mate_time - chosen_mate_time) <
                PBConstants.STOP_CALLING_THRESH and
                mate_prev_chosen))

    def findClosestPlayer(self, role, exceptPlayers = []):
        """
        Calls appropriate subroutine to determine mate that is closest
        to a given role. Ignores players in exceptPlayers list.
        Returns the player
        """
        chosen_mate = self.me
        if (role == PBConstants.CHASER):
            print "BAD NEWS IN PLAYBOOK"
        elif (role == PBConstants.DEFENDER):
            chosen_mate = self.determineDefender(exceptPlayers)
        elif (role == PBConstants.OFFENDER):
            chosen_mate = self.determineOffender(exceptPlayers)
        else: # Middie
            chosen_mate = self.determineMiddie(exceptPlayers)

        return chosen_mate

    def aPrioriTeammateUpdate(self):
        """Here we update information about teammates
        before running a new frame"""

        # update my own information for role switching
        self.time = time.time()
        self.me.updateMe()

        self.goalieIsActive = False
        # loop through teammates
        self.activeFieldPlayers = []
        append = self.activeFieldPlayers.append

        self.numActiveFieldPlayers = 0

        for mate in self.brain.teamMembers:
            # don't check inactive mates
            if mate.active:
                # the goalie isn't a field players
                if mate.isDefaultGoalie():
                    self.goalieIsActive = True
                else:
                    append(mate)
                    self.numActiveFieldPlayers += 1

    def updateMyTimes(self, locations, roles = [PBConstants.CHASER,
                                                PBConstants.DEFENDER,
                                                PBConstants.OFFENDER,
                                                PBConstants.MIDDIE],
                      ready = False):
        """
        Update my role times. Here for convenience
        """
        for role in roles:
            target = self.brain.loc.relativeRobotLocationOf(locations[role])
            if role == PBConstants.CHASER and ready:
                self.me.chaseTime = self.me.determineTimeToDest(target)
            elif role == PBConstants.DEFENDER:
                self.me.defenderTime = self.me.determineTimeToDest(target)
            elif role == PBConstants.OFFENDER:
                self.me.offenderTime = self.me.determineTimeToDest(target)
            else: # MIDDIE
                self.me.middieTime = self.me.determineTimeToDest(target)

    def updateStateInfo(self, play):
        """
        Update information specific to the coordinated behaviors
        """
        # Print changes and Say changes
        if play.changed and self.printStateChanges:
            self.printf("Play switched to Field players : {0}, {1}".format(
                self.numActiveFieldPlayers,
                play.__str__()))
            self.me.updateMe() # make sure roles get set this frame

    def printf(self, outputString, printingColor='purple'):
        """FSA print function that allows colors to be specified"""
        if printingColor == 'red':
            print RED_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'blue':
            print BLUE_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'yellow':
            print YELLOW_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'cyan':
            print CYAN_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'purple':
            print PURPLE_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        else:
            print(str(outputString))
