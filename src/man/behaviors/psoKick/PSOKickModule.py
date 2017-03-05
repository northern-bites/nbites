#INFO:
# This code was written as a way to learn sweetmove kicks more effectively.

# USAGE:
# Change ROBOT_ADDRESS to robot you are using
# Particle's Fitness = q if you want to quit, r if you want to redo the install

from . import PSOMoves as move
from . import PSOHelper as helper
import random,sys, subprocess


Num_Particles = 20
current_best = helper.LEFT_STRAIGHT_KICK
theKick = helper.LEFT_STRAIGHT_KICK
num_groups = 5
num_limbs = 4
num_joints = 20
ROBOT_ADDRESS = "river"
ROBOT_USERNAME = "nao"
cmd ='psoKick/scpKick.sh '+ROBOT_ADDRESS+' '+ROBOT_USERNAME

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
		helper.writeNewKick()
		
		subprocess.call(['psoKick/scpKick.sh'+ROBOT_ADDRESS+ROBOT_USERNAME],shell=True)

		particleFitness = raw_input("Particle's Fitness:")
		# particleFitness = testEvaluate(i.position)

		while particleFitness.isdigit() == False:
			particleFitness = raw_input("Try again: Particle's Fitness:")
		holdStill = raw_input("Changing joint status?:")
		while holdStill != 'n' and holdStill != 'y':
					holdStill = raw_input("Try again: Changing joint status?:")
		while(holdStill == 'y'):
			holdGroup = raw_input("Group:")
			while holdGroup.isdigit() == False and holdGroup > num_groups or holdGroup < 0:
					holdStill = raw_input("Try again: Group?:")
			holdStill = raw_input("Stop changing a joint?:")
			while holdStill != 'n' and holdStill != 'y':
					holdStill = raw_input("Try again: Stop changing joint?:")
			for k in range(0,num_limbs):
				l_num = helper.getLimbNumber(k)
				for l in range(0,l_num):
					i.freeze[int(holdGroup)][k][l] = 0

		i.pBest = int(particleFitness)
		if i.pBest>swarm.gBest:
			swarm.gBest = i.pBest
			swarm.updategBestPosition(i.position)
	counter = 1
	while(keepGoing):
	# while(counter < 1000):
  		print("In generation"+ str(counter))
  		if(counter != 1):
	  		for i in swarm.particles:
	  			r = random.randint(0,9)
	  			#print(str(i.particleID)+str(i.position))
	  			if (r == 0) :
		  			helper.startChanging()
					for j in range(0,num_groups):
						for k in range(0,num_limbs):
							l_num = helper.getLimbNumber(k)
							for l in range(0,l_num):
								if (j != 4):
									#if(i.freeze[j][k][l]):
									helper.changeJointAngles(j,k, l,i.position[j][k][l])
								else:
									helper.changeJointAngles(4,k, l,theKick[4][k][l])
					helper.stopChanging()

					subprocess.call(['psoKick/scpKick.sh river nao'],shell=True)
					particleFitness = raw_input("Particle's Fitness:")
					# particleFitness = testEvaluate(i.position)
					while particleFitness.isdigit() == False:
						if particleFitness == 'q':
							return
						if particleFitness == 'r':
							subprocess.call(['psoKick/scpKick.sh river nao'],shell=True)
						particleFitness = raw_input("Try again: Particle's Fitness:")
					holdStill = raw_input("Changing joint status?:")
					while holdStill != 'n' and holdStill != 'y':
						holdStill = raw_input("Try again: Changing joint status?:")
					while(holdStill == 'y'):
						posOrNeg = raw_input("On or off?(1|0): ")
						while posOrNeg != '1' and posOrNeg != '0':
							posOrNeg = raw_input("Try again: On or off?(1|0):")
						holdGroup = raw_input("Group:")
						while holdGroup.isdigit() == False and holdGroup > num_groups or holdGroup < 0:
							holdStill = raw_input("Try again: Group?:")
						holdStill = raw_input("Stop changing a joint?:")
						while holdStill != 'n' and holdStill != 'y':
							holdStill = raw_input("Try again: Stop changing joint?:")
						for k in range(0,num_limbs):
							l_num = helper.getLimbNumber(k)
							for l in range(0,l_num):
								i.freeze[int(holdGroup)][k][l] = int(posOrNeg)

					particleFitness = int(particleFitness)
					if(particleFitness > i.pBest):
						i.pBest = particleFitness
						i.updatepBestPosition(i.position)
						#print("updated pBest")
						
						if i.pBest>swarm.gBest:
							swarm.gBest = i.pBest
							swarm.updategBestPosition(i.position)
							#print("updated gBest")

		for i in swarm.particles:
			for j in range(0,num_groups):
				for k in range(0,num_limbs):
					l_num = helper.getLimbNumber(k)
					for l in range(0,l_num):
						if (j != 4):
							#if(i.freeze[j][k][l]):
							R1 = random.uniform(0, 2.05)
							R2 = random.uniform(0, 2.05)
							newVel = 0.7298*(i.getVelocity(j,k,l) + R1 * (i.getpBest(j,k,l) - i.getPosition(j,k,l)) + R2 * (swarm.getgBest(j,k,l) -  i.getPosition(j,k,l)))
							i.updateVelocityAt(j,k,l,float("{0:.2f}".format(newVel)))
							i.updatePositionAt(j,k,l,float("{0:.2f}".format(newVel + i.getPosition(j,k,l))))
						else:
							i.updatePositionAt(4,k, l,theKick[4][k][l])
		counter= counter + 1
# 	print(swarm.gBest)
# 	print(swarm.gBest_position[0][0][0])
			

# def testEvaluate(pos):
# 	return int(pos[0][0][0])



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


# print
			# pos = [[0]*num_limbs]*num_groups
			# vel = [[0]*num_limbs]*num_groups
			# for j in range(0, num_groups):
			# 	for k in range(0,num_limbs):
			# 		joints = helper.getLimbNumber(k)
			# 		vel[j][k] = [0]*joints
			# 		pos[j][k] = [0]*joints
			# print("CURR: "+ str(i.position))
			# print
			# for j in range(0,num_groups):
			# 	for k in range(0,num_limbs):
			# 		l_num = helper.getLimbNumber(k)
			# 		for l in range(0,l_num):
			# 			print("INDEX: "+str(j)+str(k)+str(l))
			# 			#if (j != 4):							
			# 			R1 = random.uniform(0, 2.05)
			# 			R2 = random.uniform(0, 2.05)
			# 			myVel = 0.7298*(i.getVelocity(j,k,l) + R1 * (i.getpBest(j,k,l) - i.getPosition(j,k,l)) + R2 * (swarm.getgBest(j,k,l) -  i.getPosition(j,k,l)))
			# 				# if(i.freeze[j][k][l]):
			# 			print("myVEL: "+str(myVel)+ " "+ str(vel[j][k][l]) +" "+ str(vel[j][k]))
			# 			vel[j][k][l] = myVel
			# 			print
			# 			print(vel)
			# 			print
			# 			pos[j][k][l] = i.getPosition(j,k,l) + vel[j][k][l]
			# 			print(pos)
			# 			print
			# 				# else:
			# 					# pos[j][k][l] = i.getPosition(j,k,l) 
			# 			#else:
			# 				#pos[j][k][l] = i.getPosition(4,k,l)
							
			# i.updateVelocity(vel)
			# print("OLD POS: "+str(pos))
			# i.updatePosition(pos)
			# print("NEW POS:"+str(i.position))
