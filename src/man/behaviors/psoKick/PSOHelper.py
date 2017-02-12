
class Particle:
	def __init__(self,currentPosition, particleID,currBest):
		self.particleID = particleID
		self.position = currentPosition
		self.pBest = currBest
		self.pBest_position = currentPosition
		self.inertial = MAX_INERTIAL * random.random() #shoutout to wikipedia for this

		#need to change this so that it deals with the weirdness of the kick
			
		self.velocity =[[0]*num_limbs]*num_groups
		for i in range(0, num_groups-1):
			for j in range(0,num_limbs):
				joints = getLimbNumber(j,num_limbs)
				self.velocity[i][j] = [0]*joints

class Swarm:
	def __init__(self,numParticles,gBest):
		self.particles = []
		self.numParticles = numParticles
		self.gBest = gBest


def getLimbNumber(currentLimb,num_limbs):
	if currentLimb == 0 or currentLimb == 3:
		return 4
	else:
		return 6

def changeJointAngles(kick, group,limb, joint):
  kick = listit(kick)
  kick[group][limb][joint] = kick[group][limb][:joint] + getRandomJointAngle(joint) + kick[group][limb][joint+1:]
  kick = tupleit(kick)

def getRandomJointAngle(joint):
  return [float("{0:.2f}".format(random.uniform(lower_bound_joints[joint], upper_bound_joints[joint])))]


def listit(t):
    return list(map(listit, t)) if isinstance(t, (list, tuple)) else t
def tupleit(t):
    return tuple(map(tupleit, t)) if isinstance(t, (tuple, list)) else t

    # def main():
#   print("----------------------------")
#   changeJointAngles(moves.LEFT_STRAIGHT_KICK,0,0,0)
#   print("----------------------------")
