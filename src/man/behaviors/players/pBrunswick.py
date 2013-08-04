import time

from . import SoccerFSA
from . import ChaseBallStates
from . import PositionStates
from . import PenaltyStates
from . import FindBallStates
from . import KickingStates
from . import PenaltyKickStates
from . import GoaliePositionStates
from . import GoalieSaveStates
from . import BrunswickStates
from . import DribbleStates
from . import BoxPositionStates

import noggin_constants as NogginConstants

from . import BoxPositionConstants as BPConstants
from ..playbook import PBConstants

from objects import Location

COUNT_FPS = False

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(PenaltyKickStates)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieSaveStates)
        self.addStates(PositionStates)
        self.addStates(PenaltyStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.addStates(ChaseBallStates)
        self.addStates(BrunswickStates)
        self.addStates(DribbleStates)
        self.addStates(BoxPositionStates)

        self.setName('pBrunswick')

        self.inKickingState = False

        # If true, robots will use a simpler positioning system than playbook,
        # based on boxes on the field robots are responsible for, and a more static
        # sense of positions (defender, chaser, etc.)
        # NOT COMPLETE AS OF SUMMER 2013 SO STAY OFF!
        self.usingBoxPositions = False

        if self.usingBoxPositions:
            #Figure out home & kickoff, even/odd player.
            #All that good stuff...
            if brain.playerNumber == 2:
                self.homePosition = BPConstants.evenDefenderHome
                self.kickoffPosition = self.homePosition
                self.box = BPConstants.evenDefenderBox
                self.isDefender = True
            elif brain.playerNumber == 3:
                self.homePosition = BPConstants.oddDefenderHome
                self.kickoffPosition = self.homePosition
                self.box = BPConstants.oddDefenderBox
                self.isDefender = True
            elif brain.playerNumber == 4:
                self.homePosition = BPConstants.evenChaserHome
                self.kickoffPosition = BPConstants.theirKickoff
                self.box = BPConstants.chaserBox
                self.isDefender = False

        #GOALIE COUNTERS AND BOOLEANS
        # Counters for goalie dive decision making
        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0

        self.isSaving = False
        self.shouldSaveCounter = 0

        #END GOALIE COUNTERS AND BOOLEANS

        self.frameCounter = 0

        self.shouldRelocalizeCounter = 0

        # Penalty kick player variables
        self.penaltyKicking = False

        # Kickoff kick
        self.shouldKickOff = False

    def run(self):
        self.play = self.brain.play
        gcState = self.gameState

        if (gcState == 'gamePlaying' and
            not self.currentState == 'afterPenalty' and
            not self.currentState == 'gamePenalized' and
            not self.currentState == 'gamePlaying'):

            self.shouldKickOff = False

            roleState = self.getNextState()

            if roleState != self.currentState:
                self.switchTo(roleState)

        # takes our average fps over 1000 frames (without profiling)
        if COUNT_FPS:
            if self.counter == 0:
                self.startTime = time.time()
                print "time at start: {0}".format(self.startTime)
            if self.counter == 1000:
                self.stopTime = time.time()
                print "time at end: {0}".format(self.stopTime)
                print "{0} s for 1000 frames = {1} fps" \
                      .format(self.stopTime - self.startTime,
                              1000/(self.stopTime - self.startTime))

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        if not self.brain.play.changed:
            return self.currentState

        elif self.inKickingState:
            return self.currentState

        else:
            return self.getRoleState()

    def getRoleState(self):
        if(self.play.isRole(PBConstants.CHASER) and
           not BPConstants.isDefender):
            if self.brain.gameController.timeSincePlaying < 10:
                if (self.brain.gameController.ownKickOff):
                    self.shouldKickOff = True

                return 'kickoff'
            return 'chase'
        elif self.brain.gameController.penalized:
            return 'gamePenalized'
        else:
            return 'playbookPosition'

