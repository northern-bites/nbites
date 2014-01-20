import noggin_constants as NogginConstants
from objects import RobotLocation

isDefender = False # default is false, changed by pBrunswick or some other if
                   # this is not the case

oddDefenderHome = RobotLocation(NogginConstants.BLUE_GOALBOX_RIGHT_X + 20,
                                NogginConstants.MY_GOALBOX_BOTTOM_Y,
                                20)

evenDefenderHome = RobotLocation(NogginConstants.BLUE_GOALBOX_RIGHT_X + 20,
                                 NogginConstants.MY_GOALBOX_TOP_Y,
                                 -20)

oddChaserHome = RobotLocation(NogginConstants.CENTER_FIELD_X,
                              NogginConstants.GREEN_PAD_Y + 10,
                              90)

evenChaserHome = RobotLocation(NogginConstants.CENTER_FIELD_X,
                               NogginConstants.FIELD_GREEN_HEIGHT - 10,
                               -90)

ourKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - 10,
                           NogginConstants.CENTER_FIELD_Y,
                           0)

theirKickoff = RobotLocation(NogginConstants.CENTER_FIELD_X - \
                                 NogginConstants.CENTER_CIRCLE_RADIUS - 10,
                             NogginConstants.CENTER_FIELD_Y,
                             0)

# These values are in cm, with the origin defined as the right corner of the
# field closest to your own goal

#   |                   |   ^
#   |                   |   |
#   |                   |   | X-axis
#   |                   |   |
#   |                   |
#   |                   |
#   |______o=====o______| <--origin
#          our goal
#          <-------Y-axis

# A BOX is defined as:
# ((lower-RIGHT X, lower-RIGHT Y), Height (X value), Width (Y value))

oddDefenderBox = ((0, 0), NogginConstants.CENTER_FIELD_X, NogginConstants.CENTER_FIELD_Y)

evenDefenderBox = ((0, NogginConstants.CENTER_FIELD_Y), NogginConstants.CENTER_FIELD_X, \
                   NogginConstants.FIELD_WHITE_HEIGHT + NogginConstants.GREEN_PAD_Y)

chaserBox = ((0, 0), 900, 600)

