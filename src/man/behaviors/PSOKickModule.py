import PSOMoves as move
import random


#20 possible joint angles

lower_bound_joints = [-119.5,-18.0,-119.5,-88.5, 
					-65.62,-21.74,-88.00,-5.29,-68.15,-22.79,
					-65.62,-45.29,-88.00,-5.90,-67.97,-44.06,
					-119.5,-76.0,-119.5,2.0]

upper_bound_joints = [119.5,76.0,119.5,-2.0,
					42.44,45.29,27.73,121.04,52.86,44.06,
					42.44,21.74,27.73,121.47,121.47,22.80,
					119.5,18.0,119.5,88.5]


def main():
  changeJointAngles(moves.LEFT_STRAIGHT_KICK,0,0,0)
  print("Finish")

def changeJointAngles(kick, group,limb, where):
  kick = listit(kick)
  kick[group][limb][where] = kick[group][limb][:where] + getRandomJointAngle(where) + kick[group][limb][where+1:]
  kick = tupleit(kick)
  print(kick)
	  #kick[group][limb][:where] + getRandomJointAngle(where) + joint[group][limb][where+1:]
def getRandomJointAngle(where):
  return [float("{0:.2f}".format(random.uniform(lower_bound_joints[where], upper_bound_joints[where])))]


def listit(t):
    return list(map(listit, t)) if isinstance(t, (list, tuple)) else t
def tupleit(t):
    return tuple(map(tupleit, t)) if isinstance(t, (tuple, list)) else t
    
    
  