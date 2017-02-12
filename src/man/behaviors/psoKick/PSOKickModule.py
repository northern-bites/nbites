from . import PSOMoves as move
from . import PSOHelper 
import random,sys, subprocess

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
MAX_INERTIAL = 0.5


def start():
  swarm = PSOHelper.Swarm(Num_Particles,current_best)
  keepGoing = True
  for i in range(0,Num_Particles-1):

    #randomly change positions
    subprocess.call("scpKick.sh")
    currBest = input("Particle's Fitness:")
    particle = PSOHelper.Particle(currPosition, i, currBest)
    swarm.particle.append(particle)

  while(keepGoing):

    for i in swarm.particle:
      for j in range(0,num_groups-1):
        for k in range(0,num_limbs):
          l_num = getLimbNumber(l,num_limbs)
          for l in range(0,l_num-1):
            changeJointAngles(kick, j,k, l)

      subprocess.call("scpKick.sh")
      particleFitness = input("Particle's Fitness:")
      if particleFitness = "stop":
        keepGoing = False

      if particleFitness > i.pBest:
        pBest = particleFitness
      if pBest>gBest:
        gBest = pBest
    for i in swarm.particle:
      for j in range(0,num_groups-1):
        for k in range(0,num_limbs):
          l_num = getLimbNumber(l,num_limbs)
          for l in range(0,l_num-1):
            R1 = random.random()
            R2 = random.random()
            i.velocity[j][k][l] = self.INERTIAL*i.velocity[j][k][l] \
                + COG * R1 * (i.pBest_position[j][k][l] - i.position[j][k][l]) \
                + SOC * R2 * (gBest_position[j][k][l] - i.position[j][k][l])
            i.position[j][k][l] = i.position[j][k][l] + i.velocity[j][k][l]







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
