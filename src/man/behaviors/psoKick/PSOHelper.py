
from . import PSOMoves as move
from . import StiffnessModes as stiff
import random

lower_bound_joints = [-119.5,-18.0,-119.5,-88.5, 
					-65.62,-21.74,-88.00,-5.29,-68.15,-22.79,
					-65.62,-45.29,-88.00,-5.90,-67.97,-44.06,
					-119.5,-76.0,-119.5,2.0]

upper_bound_joints = [119.5,76.0,119.5,-2.0,
					42.44,45.29,27.73,121.04,52.86,44.06,
					42.44,21.74,27.73,121.47,121.47,22.80,
					119.5,18.0,119.5,88.5]
LEFT_STRAIGHT_KICK = (
    #swing to the right
    ((20.0,30.0,0.0,0.0),
     (0.0,17.0,-15.0,43.5,-30.0,-20.0),
     (0.0,10.0,-27.0,45.0,-22.5,-17.0),
     (80.0,-30.0,0.0,0.0),
     0.8,0.0, stiff.NORMAL_STIFFNESSES),

    # Lift/cock leg
    ((20.0,30.0,0.0,0.0),
     (0.0, 17.0, -40.0, 100.0,-50.0,-25.0),
     (0.0, 10.0,-27.0,45.0,-22.5,-18.0),
     (100.0,-30.0,0.0,0.0),
     0.4,0.0, stiff.NORMAL_STIFFNESSES),

    # Kick?
    ((43.0,30.0,0.0,0.0),
     (0.0,17.0, -60.0,70.0,-10.0,-15.0),
     (0.0,10.0,-27.0,45.0,-22.5,-18.0),
     (20.0,-30.0,0.0, 0.0),
     0.14,0.0, stiff.NORMAL_STIFFNESSES),

    # Recover
    # ((80.,30.,-50.,-70.),
    ((90.0,10.0,-90.0,-10.0),
     (0.0,25.0,-27.0,43.5,-21.2,-20.0),
     (0.0,15.0,-27.0,45.0,-22.5,-18.0),
     (80.0,-30.0,50.0,74.0),
     0.7,0.0, stiff.NORMAL_STIFFNESSES),

    ((90.0,10.0,-90.0,-10.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.0,-10.0,82.0,13.2),
     0.7,0.0,stiff.NORMAL_STIFFNESSES)
    )

Num_Particles = 20
current_best = move.LEFT_STRAIGHT_KICK
kick = move.LEFT_STRAIGHT_KICK
num_groups = 5
num_limbs = 4
num_joints = 20
threshold= 5

def getLimbNumber(currentLimb):
	if ((currentLimb == 0) or (currentLimb == 3)):
	    return 4
	else:
		return 6

def listit(t):
    return list(map(listit, t)) if isinstance(t, (list, tuple)) else t
def tupleit(t):
    return tuple(map(tupleit, t)) if isinstance(t, (tuple, list)) else t

def getRandomJointAngle(joint):
    return float("{0:.2f}".format(random.uniform(lower_bound_joints[joint], upper_bound_joints[joint])))

def getRandJointInRange(currPosition,group,limb, joint):
	basis = listit(LEFT_STRAIGHT_KICK)
	lower = basis[group][limb][joint]-threshold
	upper = basis[group][limb][joint]+threshold
	if (lower < lower_bound_joints[joint]):
		lower = lower_bound_joints[joint]+1
	if (upper > upper_bound_joints[joint]):
		upper = upper_bound_joints[joint]-1
	return float("{0:.2f}".format(random.uniform(lower, upper)))

def startChanging():
    global kick
    kick = listit(kick)
def stopChanging():
    global kick
    kick = tupleit(kick)
    return kick

def writeNewKick():

	with open("psoKick/PSOMoves.py", 'w+') as f:
		f.write("import StiffnessModes as stiff\n\n")
		f.write("LEFT_STRAIGHT_KICK="+str(kick))
		f.close()

def changeJointAngles(group,limb, joint,NewPos):
    global kick
    kick[group][limb][joint] = NewPos



class Particle:
	def __init__(self,currentPosition, particleID,currBest):
		self.particleID = particleID
		self.pBest = int(currBest)
		self.basis = listit(LEFT_STRAIGHT_KICK)
		#need to change this so that it deals with the weirdness of the kick
		self.pBest_position= [[0]*num_limbs]*num_groups
		self.position = [[0]*num_limbs]*num_groups
		self.velocity =[[0]*num_limbs]*num_groups
		self.freeze = [[1]*num_limbs]*num_groups
		for i in range(0, num_groups):
			for j in range(0,num_limbs):
				joints = getLimbNumber(j)
				self.velocity[i][j] = [0]*joints
				self.freeze[i][j] = [1]*joints

		self.position = listit(currentPosition)
		self.pBest_position = listit(currentPosition)
		
	def updatePosition(self, newPosition):
		newPosition = listit(newPosition)
		for j in range(0,num_groups):
			for k in range(0,num_limbs):
				l_num = getLimbNumber(k)
				for l in range(0,l_num):
					lower = self.basis[j][k][l]-threshold
					upper = self.basis[j][k][l]+threshold
					if (lower < lower_bound_joints[l]):
						lower = lower_bound_joints[l]+1
					if (upper > upper_bound_joints[l]):
						upper = upper_bound_joints[l]-1
					if newPosition[j][k][l]<lower:
						newPosition[j][k][l]=lower
					if newPosition[j][k][l]>upper:
						newPosition[j][k][l]=upper
		self.position  = listit(newPosition)
	def updatePositionAt(self, j,k,l,new):
		lower = self.basis[j][k][l]-threshold
		upper = self.basis[j][k][l]+threshold
		if (lower < lower_bound_joints[l]):
			lower = lower_bound_joints[l]+1
		if (upper > upper_bound_joints[l]):
			upper = upper_bound_joints[l]-1
		if new<lower:
			new=lower
		if new>upper:
			new=upper
		self.position[j][k][l]  = new 


	def getPosition(self, j,k,l):
		return self.position[j][k][l]

	def updatepBestPosition(self, newPosition):
		self.position = listit(newPosition)


	def getpBest(self, j,k,l):
		return self.pBest_position[j][k][l]
	def updateVelocity(self, newPosition):
		self.velocity = listit(newPosition)

	def updateVelocityAt(self, j,k,l,new):
		self.velocity[j][k][l]  = new 

	def getVelocity(self, j,k,l):
		return self.velocity[j][k][l]


class Swarm:
	def __init__(self,currentPosition,numParticles,gBest):
		self.particles = []
		self.numParticles = numParticles
		self.gBest = int(gBest)
		self.gBest_position = listit(currentPosition)
		

	def updategBestPosition(self, newPosition):
		self.gBest_position = listit(newPosition)
	def getgBest(self, j,k,l):
		return self.gBest_position[j][k][l]



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
