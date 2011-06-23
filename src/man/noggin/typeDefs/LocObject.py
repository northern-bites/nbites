from .Location import Location

class LocObject(Location):
    """LocObject is a class for objects that need location information
    when vision information is lacking."""

    def __init__(self):
        Location.__init__(self, 0,0,0)
        self.locDist = 0
        self.locBearing = 0
        self.relX = 0
        self.relY = 0
        self.trackingFitness = 0   # Used in ActiveLoc

    def __lt__(self, other):
        return (self.trackingFitness < other.trackingFitness)
