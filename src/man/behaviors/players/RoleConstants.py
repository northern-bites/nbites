import noggin_constants as NogginConstants
from objects import RobotLocation
from objects import Location

isKickingOff = False # default is false, changed by pBrunswick or some other if
                     # this is not the case

boxBuffer = 100 # Used for the buffered box when approach ball is potentially
                # going to transition out and into 'positionAtHome'

oddDefenderHome = RobotLocation(NogginConstants.BLUE_GOALBOX_RIGHT_X + 50,
                                NogginConstants.MY_GOALBOX_BOTTOM_Y,
                                20)

evenDefenderHome = RobotLocation(NogginConstants.BLUE_GOALBOX_RIGHT_X + 100,
                                 NogginConstants.MY_GOALBOX_TOP_Y,
                                 -20)

oddChaserHome = RobotLocation(NogginConstants.CENTER_FIELD_X,
                              NogginConstants.GREEN_PAD_Y + 100,
                              90)

evenChaserHome = RobotLocation(NogginConstants.CENTER_FIELD_X,
                               NogginConstants.FIELD_GREEN_HEIGHT - 100,
                               -90)

cherryPickerHome = RobotLocation(NogginConstants.OPP_GOALBOX_LEFT_X,
                                 NogginConstants.OPP_GOALBOX_BOTTOM_Y,
                                 90)

cherryPickerKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - 10,
                                    NogginConstants.OPP_GOALBOX_BOTTOM_Y,
                                    0)

ourKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - 20,
                           NogginConstants.CENTER_FIELD_Y,
                           0)

theirKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - \
                                 NogginConstants.CENTER_CIRCLE_RADIUS - 10,
                             NogginConstants.CENTER_FIELD_Y,
                             0)

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

defenderBox = ((0, 0), NogginConstants.CENTER_FIELD_X, NogginConstants.FIELD_WHITE_HEIGHT + NogginConstants.GREEN_PAD_Y)

oddDefenderBox = defenderBox
evenDefenderBox = defenderBox

quadrant1 = ((0, 0), NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y)
quadrant2 = ((0, NogginConstants.CENTER_FIELD_Y), NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y)
quadrant3 = ((NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y), NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y)
quadrant4 = ((NogginConstants.CENTER_FIELD_X, 0), NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y)

quad1Center = Location(NogginConstants.CENTER_FIELD_X * .5, NogginConstants.CENTER_FIELD_Y * .5)
quad2Center = Location(NogginConstants.CENTER_FIELD_X * .5, NogginConstants.CENTER_FIELD_Y * 1.5)
quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.5, NogginConstants.CENTER_FIELD_Y * 1.5)
quad4Center = Location(NogginConstants.CENTER_FIELD_X * 1.5, NogginConstants.CENTER_FIELD_Y * .5)

# oddDefenderBox = ((0, 0), NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y + 75)

# evenDefenderBox = ((0, NogginConstants.CENTER_FIELD_Y - 75), NogginConstants.CENTER_FIELD_X, \
#                    NogginConstants.FIELD_WHITE_HEIGHT + NogginConstants.GREEN_PAD_Y)

chaserBox = ((0, 0), 1100, 800)

cherryPickerBox = (((NogginConstants.FIELD_WHITE_HEIGHT + NogginConstants.CENTER_FIELD_X)/2, 0), \
                    NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X, NogginConstants.FIELD_WHITE_HEIGHT)

def setRoleConstants(player, role):
    player.role = role
    if role == 2:
        player.homePosition = evenDefenderHome
        player.kickoffPosition = evenDefenderHome
        player.box = evenDefenderBox
        player.isKickingOff = False
    elif role == 3:
        player.homePosition = oddDefenderHome
        player.kickoffPosition = oddDefenderHome
        player.box = oddDefenderBox
        player.isKickingOff = False
    elif role == 4:
        player.homePosition = evenChaserHome
        player.kickoffPosition = theirKickoff
        player.box = chaserBox
        player.isKickingOff = True
    elif role == 5:
        player.homePosition = oddChaserHome
        player.kickoffPosition = cherryPickerKickoff
        player.box = chaserBox
        player.isKickingOff = False
