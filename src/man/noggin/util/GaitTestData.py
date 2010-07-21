# Simple container class so we can pickle our Gait Testing
# state easily and cleanly(ish)

class GaitTestData:
    def __init__(self, gaits, walks):
        self.gaitTest = gaits
        self.unitTest = walks

        self.gaitTestResults = []

        self.testCounter = 0
        self.gaitCounter = 0
