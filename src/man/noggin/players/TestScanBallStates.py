import man.motion.HeadMoves as HeadMoves
from ..headTracking import TrackingConstants as constants
from man.motion import MotionConstants

def gameInitial(player):
    player.gainsOn()
    return player.stay()

def gameReady(player):
    return player.goLater('standup')

def gameSet(player):
    return player.goLater('standup')

def gamePlaying(player):
    return player.goLater('standup')

def gamePenalized(player):
    return player.goLater('standup')

def standup(player):
    player.gainsOn()
    player.standup()
    return player.goLater('ballTracking')

def ballTracking(player):
    player.brain.tracker.switchTo('ballSpinTracking')
    return player.stay()
#    if player.brain.tracker.target.vis.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
#        return player.brain.tracker.goNow('tracking')
#    else:
#        return player.brain.tracker.goNow('scanBall')

#def tracking(player):
#    player.brain.tracker.helper.trackObject()
#    if not player.brain.tracker.target.vis.on:
#        if player.brain.tracker.target.vis.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
#            return player.brain.tracker.goLater('ballTracking')
#        return player.brain.tracker.stay()
#    return player.brain.tracker.stay()

#def scanBall(player):
#    ball = player.brain.tracker.brain.ball

#    if player.brain.tracker.target == ball and \
#            player.brain.tracker.target.vis.framesOn >= constants.TRACKER_FRAMES_ON_TRACK_THRESH:
#        return player.brain.tracker.goNow('ballTracking')

#    player.brain.tracker.headMove = HeadMoves.SPIN_LEFT_SCAN_BALL

#    return player.brain.tracker.stay()
