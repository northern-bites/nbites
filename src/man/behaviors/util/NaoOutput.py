"""
NaoOutput.py - File for holding all sorts of output functions
"""

# CONSTANTS
GREEN_COLOR_CODE = '\033[32m'
RESET_COLORS_CODE = '\033[0m'

class NaoOutput:
    def __init__(self, brain):
        """
        class constructor
        """
        self.brain = brain

    def printf(self,outputString):
        """
        Prints string to robot_console
        """
        # Print everything in green
        print GREEN_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
