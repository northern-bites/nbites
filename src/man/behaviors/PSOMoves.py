# ((LShoulderPitch, LShoulderRoll, LElbowYaw, LElbowRoll),
#  (LHipYawPitch, LHipRoll, LHipPitch, LKneePitch, LAnklePitch, LAnkleRoll),
#  (RHipYawPitch, RHipRoll, RHipPitch, RKneePitch, RAnklePitch, RAnkleRoll),
#  (RShoulderPitch, RShoulderRoll, RElbowYaw, RElbowRoll),
#  interp_time, interpolation, stiffness) in positions)
  
import StiffnessModes as stiff




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