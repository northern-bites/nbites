# ((LShoulderPitch, LShoulderRoll, LElbowYaw, LElbowRoll),
#  (LHipYawPitch, LHipRoll, LHipPitch, LKneePitch, LAnklePitch, LAnkleRoll),
#  (RHipYawPitch, RHipRoll, RHipPitch, RKneePitch, RAnklePitch, RAnkleRoll),
#  (RShoulderPitch, RShoulderRoll, RElbowYaw, RElbowRoll),
#  interp_time, interpolation, stiffness) in positions)
  
import StiffnessModes as stiff
import random
lower_bound_joints = [-119.5,-18.0,-119.5,-88.5, 
                    -65.62,-21.74,-88.00,-5.29,-68.15,-22.79,
                    -65.62,-45.29,-88.00,-5.90,-67.97,-44.06,
                    -119.5,-76.0,-119.5,2.0]

upper_bound_joints = [119.5,76.0,119.5,-2.0,
                    42.44,45.29,27.73,121.04,52.86,44.06,
                    42.44,21.74,27.73,121.47,121.47,22.80,
                    119.5,18.0,119.5,88.5]

    # return kick[group][limb][joint]

# LEFT_STRAIGHT_KICK = (
#     #swing to the right
#     ((20.0,30.0,0.0,0.0),
#      (0.0,17.0,-15.0,43.5,-30.0,-20.0),
#      (0.0,10.0,-27.0,45.0,-22.5,-17.0),
#      (80.0,-30.0,0.0,0.0),
#      0.8,0.0, stiff.NORMAL_STIFFNESSES),

#     # Lift/cock leg
#     ((20.0,30.0,0.0,0.0),
#      (0.0, 17.0, -40.0, 100.0,-50.0,-25.0),
#      (0.0, 10.0,-27.0,45.0,-22.5,-18.0),
#      (100.0,-30.0,0.0,0.0),
#      0.4,0.0, stiff.NORMAL_STIFFNESSES),

#     # Kick?
#     ((43.0,30.0,0.0,0.0),
#      (0.0,17.0, -60.0,70.0,-10.0,-15.0),
#      (0.0,10.0,-27.0,45.0,-22.5,-18.0),
#      (20.0,-30.0,0.0, 0.0),
#      0.14,0.0, stiff.NORMAL_STIFFNESSES),

#     # Recover
#     # ((80.,30.,-50.,-70.),
#     ((90.0,10.0,-90.0,-10.0),
#      (0.0,25.0,-27.0,43.5,-21.2,-20.0),
#      (0.0,15.0,-27.0,45.0,-22.5,-18.0),
#      (80.0,-30.0,50.0,74.0),
#      0.7,0.0, stiff.NORMAL_STIFFNESSES),

#     ((90.0,10.0,-90.0,-10.0),
#      (0.0,0.0,-22.3,43.5,-21.2, 0.0),
#      (0.0,0.0,-22.3,43.5,-21.2, 0.0),
#      (90.0,-10.0,82.0,13.2),
#      0.7,0.0,stiff.NORMAL_STIFFNESSES)
#     )

LEFT_STRAIGHT_KICK= [
            [[20.0, 30.0, 0.0, 0.0], 
            [0.0, 17.0, -15.0, 43.5, -30.0, -20.0], 
            [0.0, 10.0, -27.0, 45.0, -22.5, -17.0], 
            [80.0, -30.0, 0.0, 0.0], 0.8, 0.0,stiff.NORMAL_STIFFNESSES], 
            
            [[20.0, 30.0, 0.0, 0.0], 
            [0.0, 17.0, -40.0, 100.0, -50.0, -25.0], 
            [0.0, 10.0, -27.0, 45.0, -22.5, -18.0], 
            [100.0, -30.0, 0.0, 0.0], 0.4, 0.0, stiff.NORMAL_STIFFNESSES], 
            
            [[43.0, 30.0, 0.0, 0.0], 
            [0.0, 17.0, -60.0, 70.0, -10.0, -15.0], 
            [0.0, 10.0, -27.0, 45.0, -22.5, -18.0], 
            [20.0, -30.0, 0.0, 0.0], 0.14, 0.0, stiff.NORMAL_STIFFNESSES], 
            
            [[90.0, 10.0, -90.0, -10.0], 
            [0.0, 25.0, -27.0, 43.5, -21.2, -20.0], 
            [0.0, 15.0, -27.0, 45.0, -22.5, -18.0], 
            [80.0, -30.0, 50.0, 74.0], 
            0.7, 0.0,stiff.NORMAL_STIFFNESSES], 

            [[90.0, 10.0, -90.0, -10.0], 
            [0.0, 0.0, -22.3, 43.5, -21.2, 0.0], 
            [0.0, 0.0, -22.3, 43.5, -21.2, 0.0], 
            [90.0, -10.0, 82.0, 13.2], 0.7, 0.0, stiff.NORMAL_STIFFNESSES]]


def listit(t):
    return list(map(listit, t)) if isinstance(t, (list, tuple)) else t
def tupleit(t):
    return tuple(map(tupleit, t)) if isinstance(t, (tuple, list)) else t
def getRandomJointAngle(joint):
    return float("{0:.2f}".format(random.uniform(lower_bound_joints[joint], upper_bound_joints[joint])))
# #   kick = listit(kick)
# #   kick[group][limb][joint] = kick[group][limb][:joint] + getRandomJointAngle(joint) + kick[group][limb][joint+1:]
# #   kick = tupleit(kick)
def startChanging():
    global LEFT_STRAIGHT_KICK
    # LEFT_STRAIGHT_KICK = listit(LEFT_STRAIGHT_KICK)
def stopChanging():
    global LEFT_STRAIGHT_KICK
    # LEFT_STRAIGHT_KICK = tupleit(LEFT_STRAIGHT_KICK)
    print(LEFT_STRAIGHT_KICK)

def changeJointAngles(group,limb, joint,NewPos):
    global LEFT_STRAIGHT_KICK
    LEFT_STRAIGHT_KICK[group][limb][joint] = NewPos
