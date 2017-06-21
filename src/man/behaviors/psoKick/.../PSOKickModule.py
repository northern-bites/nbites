#INFO:
# This code was written as a way to learn sweetmove kicks more effectively.

# USAGE:
# Change ROBOT_ADDRESS to robot you are using
# Particle's Fitness = q if you want to quit, r if you want to redo the install

from . import PSOMoves as move
from . import PSOHelper as helper
import random,sys, subprocess

ROBOT_ADDRESS = "buzz"

Num_Particles = 20
current_best = helper.LEFT_STRAIGHT_KICK
theKick = helper.LEFT_STRAIGHT_KICK
num_groups = 5
num_limbs = 4
num_joints = 20
ROBOT_USERNAME = "nao"
cmd ='psoKick/scpKick.sh '+ROBOT_ADDRESS+' '+ROBOT_USERNAME


# def evaluate(kick):
# 	totalSum = 0
# 	for j in range(0,num_groups):
# 		for k in range(0,num_limbs):
# 			l_num = helper.getLimbNumber(k)
# 			for l in range(0,l_num):
# 				if (j != 4):
# 					totalSum = totalSum + kick[j][k][l]
# 	return totalSum

def startMe():
	global theKick
	random.seed()
	theKick = helper.listit(theKick)
	swarm = helper.Swarm(theKick,Num_Particles,0)
	keepGoing = True
	# print("start pso stuff")
	for i in range(0,Num_Particles):
		particle = helper.Particle(theKick, i, 10)#change current_best so that the position is a little more random
		swarm.particles.append(particle)
	for i in swarm.particles:
		# print("Initialize #"+str(i.particleID))
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
		
		subprocess.call([cmd],shell=True)
		
		print("MY KICK:")
		helper.printKick(kick)
		print
		particleFitness = raw_input("Particle's Fitness:")
		# particleFitness = evaluate(kick)

		i.pBest = int(particleFitness)
		if i.pBest>swarm.gBest:
			swarm.gBest = i.pBest
			swarm.updategBestPosition(i.position)
	counter = 0
	# while(keepGoing):
	while(counter < 1000):
  		if(counter != 0):
  			# print("In generation"+ str(counter))

	  		for i in swarm.particles:
	  			r = random.randint(0,9)
	  			r = 0 #HACKKKK
	  			if (r == 0) :
		  			helper.startChanging()
					for j in range(0,num_groups):
						for k in range(0,num_limbs):
							l_num = helper.getLimbNumber(k)
							for l in range(0,l_num):
								if (j != 4):
									helper.changeJointAngles(j,k, l,i.position[j][k][l])
								else:
									helper.changeJointAngles(4,k, l,theKick[4][k][l])
					kick = helper.stopChanging()
					helper.writeNewKick()

					subprocess.call([cmd],shell=True)
					particleFitness = int(raw_input("Particle's Fitness:"))
					# particleFitness = evaluate(kick)
					if(particleFitness > i.pBest):
						i.pBest = particleFitness
						i.updatepBestPosition(i.position)
						# sys.stdout.write(str(i.particleID)+" "+str(evaluate(i.position)) + "\n")

						if i.pBest>swarm.gBest:

							swarm.gBest = i.pBest
							swarm.updategBestPosition(i.position)

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
			
			# sys.stdout.write(str(evaluate(i.position)) + " ")
			# print(i.particleID)
			# print(evaluate(i.position))
		# print
		counter= counter + 1





startMe()





# def startMe():
# 	global theKick
# 	random.seed()
# 	theKick = helper.listit(theKick)
# 	swarm = helper.Swarm(theKick,Num_Particles,0)
# 	keepGoing = True
# 	print("start pso stuff")
# 	for i in range(0,Num_Particles):
# 		particle = helper.Particle(theKick, i, 10)#change current_best so that the position is a little more random
# 		swarm.particles.append(particle)
# 	for i in swarm.particles:
# 		print("Initialize #"+str(i.particleID))
# 		helper.startChanging()
# 		for j in range(0,num_groups):
# 			for k in range(0,num_limbs):
# 				l_num = helper.getLimbNumber(k)
# 				for l in range(0,l_num):
# 					if (j != 4):
# 						helper.changeJointAngles(j,k, l,helper.getRandJointInRange(i.position,j,k,l))
# 					else:
# 						helper.changeJointAngles(4,k, l,theKick[4][k][l])
# 		kick = helper.stopChanging()
# 		i.updatePosition(kick)
# 		helper.writeNewKick()
		
# #		subprocess.call([cmd],shell=True)

# 		particleFitness = raw_input("Particle's Fitness:")
# 		# particleFitness = testEvaluate(i.position)

# 		while particleFitness.isdigit() == False:
# 			particleFitness = raw_input("Try again: Particle's Fitness:")
# 		holdStill = raw_input("Changing joint status?:")
# 		while holdStill != 'n' and holdStill != 'y':
# 					holdStill = raw_input("Try again: Changing joint status?:")
# 		while(holdStill == 'y'):
# 			holdGroup = raw_input("Group:")
# 			while holdGroup.isdigit() == False and holdGroup > num_groups or holdGroup < 0:
# 					holdStill = raw_input("Try again: Group?:")
# 			holdStill = raw_input("Stop changing a joint?:")
# 			while holdStill != 'n' and holdStill != 'y':
# 					holdStill = raw_input("Try again: Stop changing joint?:")
# 			for k in range(0,num_limbs):
# 				l_num = helper.getLimbNumber(k)
# 				for l in range(0,l_num):
# 					i.freeze[int(holdGroup)][k][l] = 0

# 		i.pBest = int(particleFitness)
# 		if i.pBest>swarm.gBest:
# 			swarm.gBest = i.pBest
# 			swarm.updategBestPosition(i.position)
# 	counter = 1
# 	while(keepGoing):
# 	# while(counter < 1000):
#   		print("In generation"+ str(counter))
#   		if(counter != 1):
# 	  		for i in swarm.particles:
# 	  			r = random.randint(0,9)
# 	  			r = 0
# 	  			#print(str(i.particleID)+str(i.position))
# 	  			if (r == 0) :
# 		  			helper.startChanging()
# 					for j in range(0,num_groups):
# 						for k in range(0,num_limbs):
# 							l_num = helper.getLimbNumber(k)
# 							for l in range(0,l_num):
# 								if (j != 4):
# 									#if(i.freeze[j][k][l]):
# 									helper.changeJointAngles(j,k, l,i.position[j][k][l])
# 								else:
# 									helper.changeJointAngles(4,k, l,theKick[4][k][l])
# 					kick = helper.stopChanging()
# 				helper.writeNewKick()

# 					#subprocess.call(['psoKick/scpKick.sh river nao'],shell=True)
# 					particleFitness = raw_input("Particle's Fitness:")
# 					# particleFitness = testEvaluate(i.position)
# 					while particleFitness.isdigit() == False:
# 						if particleFitness == 'q':
# 							return
# 						if particleFitness == 'r':
# 							subprocess.call(['psoKick/scpKick.sh river nao'],shell=True)
# 						particleFitness = raw_input("Try again: Particle's Fitness:")
# 					holdStill = raw_input("Changing joint status?:")
# 					while holdStill != 'n' and holdStill != 'y':
# 						holdStill = raw_input("Try again: Changing joint status?:")
# 					while(holdStill == 'y'):
# 						posOrNeg = raw_input("On or off?(1|0): ")
# 						while posOrNeg != '1' and posOrNeg != '0':
# 							posOrNeg = raw_input("Try again: On or off?(1|0):")
# 						holdGroup = raw_input("Group:")
# 						while holdGroup.isdigit() == False and holdGroup > num_groups or holdGroup < 0:
# 							holdStill = raw_input("Try again: Group?:")
# 						holdStill = raw_input("Stop changing a joint?:")
# 						while holdStill != 'n' and holdStill != 'y':
# 							holdStill = raw_input("Try again: Stop changing joint?:")
# 						for k in range(0,num_limbs):
# 							l_num = helper.getLimbNumber(k)
# 							for l in range(0,l_num):
# 								i.freeze[int(holdGroup)][k][l] = int(posOrNeg)

# 					particleFitness = int(particleFitness)
# 					if(particleFitness > i.pBest):
# 						i.pBest = particleFitness
# 						i.updatepBestPosition(i.position)
# 						#print("updated pBest")
						
# 						if i.pBest>swarm.gBest:
# 							swarm.gBest = i.pBest
# 							swarm.updategBestPosition(i.position)
# 							#print("updated gBest")

# 		for i in swarm.particles:
# 			for j in range(0,num_groups):
# 				for k in range(0,num_limbs):
# 					l_num = helper.getLimbNumber(k)
# 					for l in range(0,l_num):
# 						if (j != 4):
# 							#if(i.freeze[j][k][l]):
# 							R1 = random.uniform(0, 2.05)
# 							R2 = random.uniform(0, 2.05)
# 							newVel = 0.7298*(i.getVelocity(j,k,l) + R1 * (i.getpBest(j,k,l) - i.getPosition(j,k,l)) + R2 * (swarm.getgBest(j,k,l) -  i.getPosition(j,k,l)))
# 							i.updateVelocityAt(j,k,l,float("{0:.2f}".format(newVel)))
# 							i.updatePositionAt(j,k,l,float("{0:.2f}".format(newVel + i.getPosition(j,k,l))))
# 						else:
# 							i.updatePositionAt(4,k, l,theKick[4][k][l])
# 			print(i.velocity)

# 			print(i.position)
# 			print
# 		counter= counter + 1