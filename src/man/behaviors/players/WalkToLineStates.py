import vision
import man.motion.HeadMoves as HeadMoves
from objects import RelRobotLocation

def gameSet(player):
    if player.firstFrame():
        player.brain.nav.stand()

    return player.stay()

