import man.motion as motion

"""
Gaits loaded from the gaits/ directory
Each *Gait.py file should be self-sufficient

Note that you will need to add your gaits to 
cmake.man.motion/CMakeLists to get them to install
"""

from .gaits.FastGait import FAST_GAIT
from .gaits.LabGait import LAB_GAIT
from .gaits.DuckGait import DUCK_GAIT
from .gaits.SlowGait import SLOW_GAIT
from .gaits.WebotsGait import WEBOTS_GAIT, WEBOTS_GAIT2

# disabled / unused gaits
#from .gaits.ComGait import COM_GAIT 
#from .gaits.MedGait import MEDIUM_GAIT, MARVIN_MEDIUM_GATE


############# DEFAULT GAIT ASSIGNMENTS ##################

CUR_GAIT = LAB_GAIT
CUR_DRIBBLE_GAIT = DUCK_GAIT
CUR_SLOW_GAIT = SLOW_GAIT

MARVIN_CUR_GAIT = LAB_GAIT
MARVIN_CUR_SLOW_GAIT = LAB_GAIT


TRILLIAN_GAIT = CUR_GAIT
ZAPHOD_GAIT   = CUR_GAIT
SLARTI_GAIT   = CUR_GAIT
MARVIN_GAIT   = CUR_GAIT

TRILLIAN_DRIBBLE_GAIT = CUR_DRIBBLE_GAIT
ZAPHOD_DRIBBLE_GAIT   = CUR_DRIBBLE_GAIT
SLARTI_DRIBBLE_GAIT   = CUR_DRIBBLE_GAIT
MARVIN_DRIBBLE_GAIT   = CUR_DRIBBLE_GAIT

TRILLIAN_SLOW_GAIT = CUR_SLOW_GAIT
ZAPHOD_SLOW_GAIT   = CUR_SLOW_GAIT
SLARTI_SLOW_GAIT   = CUR_SLOW_GAIT
MARVIN_SLOW_GAIT   = CUR_SLOW_GAIT


