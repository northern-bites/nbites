# contains some functions that previously lived elsewhere
# These are both necessary for gait optimization
#
# @author Nathan Merritt
# @date May 2011

import time

def gaitToArray(gait):
    list = []
    for section in gait:
        for value in section:
            list.append(value)
    return list

# the opposite of gaitToArray
# parses a list of the correct arity
def arrayToGaitTuple(array):
    assert len(array) == 44

    stance = tuple(array[0:6])
    step = tuple(array[6:18])
    # zmp off same L/R
    zmp = (array[18], array[19], array[20], array[20], array[22], array[23])
    hack = (array[25], array[25]) # joint hack is always the same L/R!
    sensor = tuple(array[26:34])
    stiffness = tuple(array[34:40])
    odo = tuple(array[40:43])
    arm = tuple(array[43:44])

    return (stance, step, zmp, hack, sensor, stiffness, odo, arm)

GAIT_SECTION_NAMES = ("stance", "step", "zmp", "joint_hack", "sensor", "stiffness", "odo", "arm")

def writeGaitToFile(file, gait, gaitNumber):
    '''
    Writes a gait tuple out, so that it can be loaded by a robot later
    The file will need indentation tuneups, but otherwise it's good to go
    '''

    file.write("import man.motion as motion\n\n")

    for i in range (0, len(GAIT_SECTION_NAMES)):
        declaration =  GAIT_SECTION_NAMES[i] + "_" + str(gaitNumber) + " = ("
        file.write(declaration)

        for value in gait[i]:
            file.write(str(value))
            file.write(",\n")
        file.write(")\n\n")

    file.write("\n")

    gait_declaration = "gait_" + str(gaitNumber) + " = motion.GaitCommand(\n"
    file.write(gait_declaration)

    for section in GAIT_SECTION_NAMES:
        # as we declared it before
        declaration = section + "_" + str(gaitNumber) + ",\n"
        file.write(declaration)

    file.write(")\n\n")

