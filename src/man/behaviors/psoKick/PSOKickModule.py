from . import PSOMoves as move
from . import PSOHelper as helper
import random,sys, subprocess


Num_Particles = 20
current_best = helper.LEFT_STRAIGHT_KICK
theKick = helper.LEFT_STRAIGHT_KICK
num_groups = 5
num_limbs = 4
num_joints = 20
ROBOT_ADDRESS = " river "
ROBOT_USERNAME = "nao"


def startMe():
	global theKick
	random.seed()
	theKick = helper.listit(theKick)
	swarm = helper.Swarm(theKick,Num_Particles,0)
	keepGoing = True
	print("start pso stuff")
	for i in range(0,Num_Particles):
		particle = helper.Particle(theKick, i, 10)#change current_best so that the position is a little more random
		swarm.particles.append(particle)
	for i in swarm.particles:
		print("Initialize #"+str(i.particleID))
		helper.startChanging()
		for j in range(0,num_groups):
			for k in range(0,num_limbs):
				l_num = helper.getLimbNumber(k)
				for l in range(0,l_num):
					if (j != 4):
						helper.changeJointAngles(j,k, l,helper.getRandJointInRange(i.position,j,k,l))
					else:
						helper.changeJointAngles(4,k, l,theKick[4][k][l])
		kick = helper.stopChanging()
		i.updatePosition(kick)
		print(kick)
		helper.writeNewKick()
		subprocess.call(['psoKick/scpKick.sh'+ROBOT_ADDRESS+ROBOT_USERNAME],shell=True)
		myFitness = raw_input("Particle's Fitness Eval:")
		holdStill = raw_input("Stop changing a joint?:")
		while(holdStill == 'y'):
			holdGroup = raw_input("Group:")
			holdStill = raw_input("Stop changing a joint?:")
			for k in range(0,num_limbs):
				l_num = helper.getLimbNumber(k)
				for l in range(0,l_num):
					i.freeze[int(holdGroup)][k][l] = 0
	counter = 1
	while(keepGoing):
  		print("In generation"+ str(counter))
  		for i in swarm.particles:
  			if (randint(0,9) == 0) :
	  			print("Initialize #"+str(i.particleID))
	  			helper.startChanging()
				for j in range(0,num_groups):
					for k in range(0,num_limbs):
						l_num = helper.getLimbNumber(k)
						for l in range(0,l_num):
							if(i.freeze[j][k][l]):
								helper.changeJointAngles(j,k, l,i.position[j][k][l])
				helper.stopChanging()
				subprocess.call(['psoKick/scpKick.sh river nao'],shell=True)
				
				print(counter)
				counter= counter + 1
				
				particleFitness = input("Particle's Fitness:")
				if particleFitness == 'q':
					
					keepGoing = False
				holdStill = raw_input("Control Group?:")
				while(holdStill == 'y'):
					posOrNeg = raw_input("On or off?(1|0): ")
					holdGroup = raw_input("Group:")
					holdStill = raw_input("Stop changing a joint?:")
					for k in range(0,num_limbs):
						l_num = helper.getLimbNumber(k)
						for l in range(0,l_num):
							i.freeze[int(holdGroup)][k][l] = int(posOrNeg)

				particleFitness = int(particleFitness)
				if particleFitness > i.pBest:
					i.pBest = particleFitness
					i.updatepBestPosition(i.position)
					
					if particleFitness>swarm.gBest:
						swarm.gBest = i.pBest
						swarm.updategBestPosition(i.position)

		for i in swarm.particles:
			pos = [[0]*num_limbs]*num_groups
			vel = [[0]*num_limbs]*num_groups
			for j in range(0, num_groups):
				for k in range(0,num_limbs):
					joints = helper.getLimbNumber(k)
					vel[j][k] = [0]*joints
					pos[j][k] = [0]*joints
			for j in range(0,num_groups):
				for k in range(0,num_limbs):
					l_num = helper.getLimbNumber(k)
					for l in range(0,l_num):
						R1 = random.uniform(0, 2.05)
						R2 = random.uniform(0, 2.05)
						vel[j][k][l] = 0.7298*(i.getVelocity(j,k,l) + R1 * (i.getpBest(j,k,l) - i.getPosition(j,k,l)) + R2 * (swarm.getgBest(j,k,l) -  i.getPosition(j,k,l)))
						if(i.freeze[j][k][l]):
							pos[j][k][l] = i.getPosition(j,k,l) + vel[j][k][l]
						else:
							pos[j][k][l] = i.getPosition(j,k,l) 
			i.updateVelocity(vel)
			i.updatePosition(pos)

startMe()



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
