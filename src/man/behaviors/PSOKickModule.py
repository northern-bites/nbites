import PSOMoves as move
import random


# ((LShoulderPitch -119.5 119.5, LShoulderRoll -18 to 76, LElbowYaw -119.5 to 119.5, LElbowRoll -88.5 to -2),
#  (LHipYawPitch -65.62 to 42.44, LHipRoll -21.74 to 45.29, LHipPitch -88.00 to 27.73, LKneePitch -5.29 to 121.04, LAnklePitch -68.15 to 52.86, LAnkleRoll -22.79 to 44.06),
#  (RHipYawPitch -65.62 to 42.44, RHipRoll -45.29 to 21.74, RHipPitch -88.00 to 27.73, RKneePitch -5.90 to 121.47, RAnklePitch -67.97 to 121.47, RAnkleRoll -44.06 to 22.80),
#  (RShoulderPitch -119.5 to 119.5, RShoulderRoll -76 to 18, RElbowYaw -119.5 to 119.5, RElbowRoll 	2 to 88.5),
#  interp_time, interpolation, stiffness))

#20 possible joint angles

lower_bound_joints = [-119.5,-18.0,-119.5,-88.5, 
					-65.62,-21.74,-88.00,-5.29,-68.15,-22.79,
					-65.62,-45.29,-88.00,-5.90,-67.97,-44.06,
					-119.5,-76.0,-119.5,2.0]

upper_bound_joints = [119.5,76.0,119.5,-2.0,
					42.44,45.29,27.73,121.04,52.86,44.06,
					42.44,21.74,27.73,121.47,121.47,22.80,
					119.5,18.0,119.5,88.5]


def changeJointAngles(self, kick, group, where):





def getRandomJointAngle(self, group, where):

	random.uniform(lower_bound_joints[where], upper_bound_joints[where])	

