"""
An FSA for detecting communication problems and changing to the appropriate strategy.

The third to top level in player FSA.
"""

import CommMonitorTransitions as transitions
import RoleConstants
from ..util import *


# TODO stop defaulting to player number everywhere
# This is where we can switch our strategies based on comm quality
# Should add time stamps for how we switch strategies but also how we determine comm quality

@defaultState('gameControllerResponder')
@superState('roleSwitcher')
def commMonitor(player):

    # role switching off for drop in player
    if player.dropIn:
        return