
from . import PSOMoves as move
import random

lower_bound_joints = [-119.5,-18.0,-119.5,-88.5, 
					-65.62,-21.74,-88.00,-5.29,-68.15,-22.79,
					-65.62,-45.29,-88.00,-5.90,-67.97,-44.06,
					-119.5,-76.0,-119.5,2.0]

upper_bound_joints = [119.5,76.0,119.5,-2.0,
					42.44,45.29,27.73,121.04,52.86,44.06,
					42.44,21.74,27.73,121.47,121.47,22.80,
					119.5,18.0,119.5,88.5]


Num_Particles = 20
current_best = move.LEFT_STRAIGHT_KICK
kick = move.LEFT_STRAIGHT_KICK
num_groups = 5
num_limbs = 4
num_joints = 20

class Particle:
	def __init__(self,currentPosition, particleID,currBest):
		self.particleID = particleID
		self.pBest = currBest
		self.pBest_position = currentPosition

		#need to change this so that it deals with the weirdness of the kick
		self.pBest_position= [[0]*num_limbs]*num_groups
		self.position = [[0]*num_limbs]*num_groups
		self.velocity =[[0]*num_limbs]*num_groups
		for i in range(0, num_groups):
			for j in range(0,num_limbs):
				joints = getLimbNumber(j)
				self.velocity[i][j] = [0]*joints
				self.position[i][j] = currentPosition[i][j]
				self.pBest_position = currentPosition[i][j]

class Swarm:
	def __init__(self,numParticles,gBest):
		self.particles = []
		self.numParticles = numParticles
		self.gBest = gBest

def getLimbNumber(currentLimb):
	if ((currentLimb == 0) or (currentLimb == 3)):
	    return 4
	else:
		return 6

def changeMove(group,limb, joint):
	move.changeJointAngles(group,limb, joint)

def listit(t):
    return list(map(listit, t)) if isinstance(t, (list, tuple)) else t
def tupleit(t):
    return tuple(map(tupleit, t)) if isinstance(t, (tuple, list)) else t
# def getLimbNumber(currentLimb,num_limbs):
# 	if currentLimb == 0 or currentLimb == 3:
# 		return 4
# 	else:
# 		return 6

# def changeJointAngles(kick, group,limb, joint):
#   kick[group][limb][joint] = kick[group][limb][:joint] + getRandomJointAngle(joint) + kick[group][limb][joint+1:]
#   return kick[group][limb][joint]

# # def changeJointAngles(kick, group,limb, joint):
# #   kick = listit(kick)
# #   kick[group][limb][joint] = kick[group][limb][:joint] + getRandomJointAngle(joint) + kick[group][limb][joint+1:]
# #   kick = tupleit(kick)
# #   return kick

# def getRandomJointAngle(joint):
#   return [float("{0:.2f}".format(random.uniform(lower_bound_joints[joint], upper_bound_joints[joint])))]


# def listit(t):
#     return list(map(listit, t)) if isinstance(t, (list, tuple)) else t
# def tupleit(t):
#     return tuple(map(tupleit, t)) if isinstance(t, (tuple, list)) else t

    # def main():
#   print("----------------------------")
#   changeJointAngles(moves.LEFT_STRAIGHT_KICK,0,0,0)
#   print("----------------------------")
