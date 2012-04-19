try:
    from _comm import (Comm,
                        GameController,
                        inst,
                        STATE_FINISHED,
                        STATE_INITIAL,
                        STATE_PLAYING,
                        STATE_READY,
                        STATE_SET,
                        STATE2_NORMAL,
                        STATE2_PENALTYSHOOT,
                        PENALTY_NONE,
                        PENALTY_BALL_HOLDING,
                        PENALTY_PLAYER_PUSHING,
                        PENALTY_OBSTRUCTION,
                        PENALTY_INACTIVE_PLAYER,
                        PENALTY_ILLEGAL_DEFENDER,
                        PENALTY_LEAVING,
                        PENALTY_PLAYING_WITH_HANDS,
                        PENALTY_REQUEST_FOR_PICKUP,
                        PENALTY_MANUAL)
except ImportError:
    import sys
    print >>sys.stderr, "**** WARNING - No backend _comm module located ****"
    inst = None
