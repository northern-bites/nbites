from ..headTracker import HeadMoves
from ..psoKick import PSOMoves
#from .. import PSOKickModule
from ..util import *
import PMotion_proto

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    return player.goNow('kick')

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    return player.goNow('kick')

@superState('gameControllerResponder')
def gamePenalized(player):
    #if player.firstFrame:
        #PSOKickModule.main()
    return player.stay()

@superState('gameControllerResponder')
def kick(player):
    if player.firstFrame():
        # player.brain.nav.callKickEngine(PMotion_proto.messages.Kick.M_Left_Chip_Shot)
        player.executeMove(PSOMoves.LEFT_STRAIGHT_KICK)
        print("FROM kickstate"+str(PSOMoves.LEFT_STRAIGHT_KICK))

    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
