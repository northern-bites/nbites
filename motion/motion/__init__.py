

from _motion import (MotionInterface,
                     BodyJointCommand,
                     HeadJointCommand,
                     HeadScanCommand,
                     WalkCommand,
                     #WalkParameters,
                     WALK_STRAIGHT,
                     WALK_SIDEWAYS,
                     WALK_TURN,
                     WALK_ARC,
                     DEFAULT_SAMPLES_PER_STEP,
                    )


class WalkStraight(WalkCommand):
    def __init__(self, dist, samples=DEFAULT_SAMPLES_PER_STEP):
        WalkCommand.__init__(self, WALK_STRAIGHT, dist, samples)

class WalkSideways(WalkCommand):
    def __init__(self, dist, samples=DEFAULT_SAMPLES_PER_STEP):
        WalkCommand.__init__(self, WALK_SIDEWAYS, dist, samples)

class WalkTurn(WalkCommand):
    def __init__(self, angle, samples=DEFAULT_SAMPLES_PER_STEP):
        WalkCommand.__init__(self, WALK_TURN, angle, samples)

class WalkArc(WalkCommand):
    def __init__(self, angle, radius, samples=DEFAULT_SAMPLES_PER_STEP):
        WalkCommand.__init__(self, WALK_ARC, angle, radius, samples)

