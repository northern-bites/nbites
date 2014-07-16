import noggin_constants as NogginConstants
from objects import RobotLocation

### ROLE CONFIGURATION
# Possibilites are the following:
# Goalie, LeftDefender, RightDefender, FirstChaser, SecondChaser, CherryPicker

moderate = {1: "Goalie",
            2: "LeftDefender",
            3: "RightDefender",
            4: "FirstChaser",
            5: "SecondChaser"}

aggresive = {1: "Goalie",
             2: "LeftDefender",
             3: "CherryPicker",
             4: "FirstChaser",
             5: "SecondChaser"}

cautious = {1: "Goalie",
            2: "LeftDefender",
            3: "RightDefender",
            4: "FirstChaser",
            5: "CherryPicker"}

roleConfiguration = moderate

def getRole(role):
    if role < 1:
        return None
    return roleConfiguration[role]

def isGoalie(role):
    return getRole(role) == "Goalie"

def isLeftDefender(role):
    return getRole(role) == "LeftDefender"

def isRightDefender(role):
    return getRole(role) == "RightDefender"

def isDefender(role):
    return isLeftDefender(role) or isRightDefender(role)

def isFirstChaser(role):
    return getRole(role) == "FirstChaser"

def isSecondChaser(role):
    return getRole(role) == "SecondChaser"

def isChaser(role):
    return isFirstChaser(role) or isSecondChaser(role)

def isCherryPicker(role):
    return getRole(role) == "CherryPicker"

# Could be useful if we decide that the CherryPicker doesn't roleswitch
def willRoleSwitch(role):
    return isDefender(role) or isCherryPicker(role)

# Makes it easy for arbitrary roleswitching
def canRoleSwitchTo(role):
    return isChaser(role)

def twoAttackersOnField(player):
    firstAttacker = False
    secondAttacker = False

    if isFirstChaser(player.role):
        firstAttacker = True
    elif isSecondChaser(player.role) or isCherryPicker(player.role):
        secondAttacker = True

    for mate in player.brain.teamMembers:
        if isFirstChaser(mate.role):
            firstAttacker = True
        elif isSecondChaser(mate.role) or isCherryPicker(mate.role):
            secondAttacker = True
    if firstAttacker and secondAttacker:
        return True

    return False

### RANDOM STUFF
isKickingOff = False # Default is false, changed by pBrunswick or some other if
                     # this is not the case, TODO this is ugly

boxBuffer = 100 # Used for the buffered box when approach ball is potentially
                # going to transition out and into 'positionAtHome'

### HOME POSITIONS
oddDefenderHome = RobotLocation(NogginConstants.BLUE_GOALBOX_RIGHT_X + 50,
                                NogginConstants.MY_GOALBOX_BOTTOM_Y,
                                0)

evenDefenderHome = RobotLocation(NogginConstants.BLUE_GOALBOX_RIGHT_X + 100,
                                 NogginConstants.MY_GOALBOX_TOP_Y,
                                 0)

oddChaserHome = RobotLocation(NogginConstants.CENTER_FIELD_X,
                              NogginConstants.GREEN_PAD_Y + 100,
                              90)

oddChaserKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - 45,
                                 NogginConstants.OPP_GOALBOX_BOTTOM_Y - 100,
                                 0)
evenChaserHome = RobotLocation(NogginConstants.CENTER_FIELD_X,
                               NogginConstants.FIELD_GREEN_HEIGHT - 100,
                               -90)

cherryPickerHome = RobotLocation(NogginConstants.OPP_GOALBOX_LEFT_X,
                                 NogginConstants.OPP_GOALBOX_BOTTOM_Y,
                                 90)

cherryPickerKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - 45,
                                    NogginConstants.OPP_GOALBOX_TOP_Y + 100,
                                    0)

ourKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - 45,
                           NogginConstants.CENTER_FIELD_Y,
                           0)

theirKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - \
                             NogginConstants.CENTER_CIRCLE_RADIUS - 20,
                             NogginConstants.CENTER_FIELD_Y,
                             0)

### BOXES
# These values are in cm, with the origin defined as the bottom corner of the
# field closest to your own goal (when you have your goal to your left)
#
#           ______________________________________
#           |
#           |
#           |
#           o
#           \
# our goal  \ ^
#     --->  \ |
#           o | Y-axis
#           | |
#           | |
#           | -------------> X-axis
# origin--> _______________________________________
#
# A BOX is defined as:
# ((lower-LEFT X, lower-LEFT Y), Width (X value), Height (Y value))
#
# Useful constants can be found in src/share/include/FieldConstants.h

defenderBox = ((0, 0), NogginConstants.CENTER_FIELD_X - 70, NogginConstants.FIELD_GREEN_HEIGHT)

oddDefenderBoxCautious = ((0, 0), NogginConstants.CENTER_FIELD_X - 70, NogginConstants.CENTER_FIELD_Y)

evenDefenderBoxCautious = ((0, NogginConstants.CENTER_FIELD_Y), NogginConstants.CENTER_FIELD_X - 70, \
                           NogginConstants.CENTER_FIELD_Y)

oddDefenderBox = defenderBox
evenDefenderBox = defenderBox

chaserBox = ((0, 0), NogginConstants.FIELD_WIDTH, NogginConstants.FIELD_HEIGHT)

cherryPickerBox = (((0.5*NogginConstants.FIELD_GREEN_WIDTH + 0.25*NogginConstants.FIELD_WHITE_WIDTH), 0),
                    0.25*NogginConstants.FIELD_WHITE_WIDTH + NogginConstants.GREEN_PAD_X, 
                    NogginConstants.FIELD_HEIGHT)

### SETS PLAYER STATE PER ROLE
def setRoleConstants(player, role):
    player.role = role
    if isLeftDefender(role):
        player.homePosition = evenDefenderHome
        player.kickoffPosition = evenDefenderHome
        player.box = evenDefenderBox
        player.isKickingOff = False
    elif isRightDefender(role):
        player.homePosition = oddDefenderHome
        player.kickoffPosition = oddDefenderHome
        player.box = oddDefenderBox
        player.isKickingOff = False
    elif isFirstChaser(role):
        player.homePosition = evenChaserHome
        player.kickoffPosition = theirKickoff
        player.box = chaserBox
        player.isKickingOff = True
    elif isSecondChaser(role):
        player.homePosition = oddChaserHome
        player.kickoffPosition = oddChaserKickoff
        player.box = chaserBox
        player.isKickingOff = False
    elif isCherryPicker(role):
        player.homePosition = cherryPickerHome
        if role == 2: # if there are two chasers
            player.kickoffPosition = cherryPickerKickoff
        else:         # if there is only one chaser, role (should) == 5
            player.kickoffPosition = oddChaserKickoff
        player.box = cherryPickerBox
        player.isKickingOff = False
