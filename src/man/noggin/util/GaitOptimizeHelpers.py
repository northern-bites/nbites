# contains some functions that previously lived elsewhere
# These are both necessary for gait optimization
#
# @author Nathan Merritt
# @date May 2011

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
    zmp = tuple(array[18:24])
    hack = (array[25], array[25]) # joint hack is always the same L/R!
    sensor = tuple(array[26:34])
    stiffness = tuple(array[34:40])
    odo = tuple(array[40:43])
    arm = tuple(array[43:44])

    return (stance, step, zmp, hack, sensor, stiffness, odo, arm)
