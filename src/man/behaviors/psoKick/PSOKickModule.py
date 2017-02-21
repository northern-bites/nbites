from . import PSOMoves as move
from . import PSOHelper as helper
import random,sys, subprocess

lower_bound_joints = [-119.5,-18.0,-119.5,-88.5, 
					-65.62,-21.74,-88.00,-5.29,-68.15,-22.79,
					-65.62,-45.29,-88.00,-5.90,-67.97,-44.06,
					-119.5,-76.0,-119.5,2.0]

upper_bound_joints = [119.5,76.0,119.5,-2.0,
					42.44,45.29,27.73,121.04,52.86,44.06,
					42.44,21.74,27.73,121.47,121.47,22.80,
					119.5,18.0,119.5,88.5]
Num_Particles = 5
current_best = move.LEFT_STRAIGHT_KICK
kick = move.LEFT_STRAIGHT_KICK
num_groups = 5
num_limbs = 4
num_joints = 20
ROBOT_ADDRESS = "river"
ROBOT_USERNAME = "nao"


def startMe():
	swarm = helper.Swarm(Num_Particles,current_best)
	keepGoing = True
	print("start pso stuff")
	for i in range(0,Num_Particles-1):
		#randomly change positions
		move.startChanging()
		for j in range(0,num_groups):
			for k in range(0,num_limbs):
				l_num = helper.getLimbNumber(k)
				for l in range(0,l_num):
					move.changeJointAngles(j,k, l,move.getRandomJointAngle(l))
		move.stopChanging()
		subprocess.call(['psoKick/scpKick.sh river nao'],shell=True)
		print(i+1)
		myFitness = input("Particle's Fitness Eval:")
		particle = helper.Particle(move.LEFT_STRAIGHT_KICK, i, myFitness)#change current_best so that the position is a little more random
		swarm.particles.append(particle)
	while(keepGoing):
  		print("In generation")
  		for i in swarm.particles:
  			move.startChanging()
			for j in range(0,num_groups):
				for k in range(0,num_limbs):
					l_num = helper.getLimbNumber(k)
					for l in range(0,l_num):
						move.changeJointAngles(j,k, l,i.position[j][k][l])
			move.stopChanging()
			subprocess.call("scpKick.sh")
			particleFitness = input("Particle's Fitness:")
			if particleFitness == 'q':
				keepGoing = False
			if particleFitness > i.pBest:
				pBest = particleFitness
			if pBest>gBest:
				gBest = pBest
		for i in swarm.particle:
			for j in range(0,num_groups):
				for k in range(0,num_limbs):
					l_num = getLimbNumber(l)
					for l in range(0,l_num):
						R1 = random.randrange(0,2.05)
						R2 = random.randrange(0,2.05)
						i.velocity[j][k][l] = 0.7298((i.velocity[j][k][l] + R1) \
							* ((i.pBest_position[j][k][l] - i.position[j][k][l]) * R2) \
							* (gBest_position[j][k][l] - i.position[j][k][l]))
						i.position[j][k][l] = i.position[j][k][l] + i.velocity[j][k][l]


def testChangeJointAngles():
	# kick = move.LEFT_STRAIGHT_KICK
	move.startChanging()
	for j in range(0,num_groups):
		for k in range(0,num_limbs):
			l_num = helper.getLimbNumber(k)
			for l in range(0,l_num):
				move.changeJointAngles(j,k, l)
	move.stopChanging()

startMe()
# testChangeJointAngles()





# For each particle
#     Initialize particle
# Do until maximum iterations or minimum error criteria
#     For each particle
#         Calculate Data fitness value
#         If the fitness value is better than pBest
#             Set pBest = current fitness value
#         If pBest is better than gBest
#             Set gBest = pBest
#     For each particle
#         Calculate particle Velocity
#         Use gBest and Velocity to update particle Data
