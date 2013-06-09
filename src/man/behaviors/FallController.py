import SweetMoves as SweetMoves

class FallController():
    def __init__(self, brain):
        self.brain = brain
        self.stateChangeColor = 'blue'

        self.falling = False
        self.fell = False
        self.standingUp = False
        self.standDelay = 0
        self.startStandupTime = 0
        self.standupMoveTime = 0

        self.enabled = True

    def run(self):
        if not self.enabled:
            return

        # Check if we have just begun falling.
        if (not self.falling and self.brain.interface.fallStatus.falling):
            # Save the player. We are falling
            self.falling = True
            self.brain.player.gainsOff()
            self.brain.player.switchTo('fallen')
            self.brain.tracker.stopHeadMoves()

        # Check if we have fallen.
        # HACK Guardian's `fallen` is not actually on the ground
        #      Put in a delay to ensure we hit the ground softly
        if (not self.fell and self.brain.interface.fallStatus.fallen):
            self.standDelay += 1
            if (self.standDelay == 14):
                self.brain.interface.motionRequest.reset_providers = True
                self.brain.interface.motionRequest.timestamp = int(self.brain.time*1000)
                self.brain.player.gainsOn()

            if (self.standDelay == 15): # Half a second
                self.fell = True

        # Send a stand up move once.
        elif (not self.standingUp and self.fell):
            self.standingUp = True

            self.brain.tracker.setNeutralHead()

            move = None
            if (self.brain.interface.fallStatus.on_front):
                move = SweetMoves.STAND_UP_FRONT
            else:
                move = SweetMoves.STAND_UP_BACK

            self.brain.player.executeMove(move)

            self.standupMoveTime = SweetMoves.getMoveTime(move)
            self.startStandupTime = self.brain.time

        # If we are standing, check if we are done.
        elif (self.standingUp):
            if (self.brain.time - self.startStandupTime > self.standupMoveTime):
                self.brain.player.stand()
                self.brain.player.switchTo(self.brain.player.gameState)
                self.falling = False
                self.fell = False
                self.standingUp = False
                self.standDelay = 0
