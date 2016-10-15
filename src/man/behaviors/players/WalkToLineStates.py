import vision
import man.motion.HeadMoves as HeadMoves
from objects import RelRobotLocation

# Currently, this is just a copy of the example walkToBall behavior given
# on the Wiki. Obviously, this will need to implement the walkToLine behavior.

def gameSet(player):
    if player.firstFrame():
        player.brain.nav.stand()

    return player.stay()

def gamePlaying(player):
    player.firstFrame():
    player.gainsOn()
    player.brain.nav.stand()
    player.brain.tracker.lookToAngleFixedPitch(0)

    while (not player.brain.motion.calibrated()):
        return player.stay()

    return player.goNow('walkToBall')

def walkToBall(player):
    ball = player.brain.ball

    if (not ball.vis.on):
        print "no ball"
        dest = RelRobotLocation(10, 0, 0)
        player.brain.nav.goTo(dest)
        return player.stay()

    elif (ball.vis.dest < 30):
        print "saw the ball, stopping"
        return player.goNow('gameSet')
    else:
        print "see ball, going towards it"
        player.brain.nav.goTo(ball.loc)
        return player.stay()
