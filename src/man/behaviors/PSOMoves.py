pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy# ((LShoulderPitch, LShoulderRoll, LElbowYaw, LElbowRoll),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy#  (LHiawPitch, LHipRoll, LHipPitch, LKneePitch, LAnklePitch, LAnkleRoll),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy#  (RHiawPitch, RHipRoll, RHipPitch, RKneePitch, RAnklePitch, RAnkleRoll),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy#  (RShoulderPitch, RShoulderRoll, RElbowYaw, RElbowRoll),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy#  interp_time, interpolation, stiffness) in positions)
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy  
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypyfrom ..behaviors import StiffnessModes as stiff
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypyLEFT_STRAIGHT_KICK = (
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    #swing to the right
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    ((20.0,30.0,0.0,0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,17.0,-15.0,43.5,-30.0,-20.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,10.0,-27.0,45.0,-22.5,-17.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (80.0,-30.0,0.0,0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     0.8,0.0, stiff.NORMAL_STIFFNESSES),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    # Lift/cock leg
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    ((20.0,30.0,0.0,0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0, 17.0, -40.0, 100.0,-50.0,-25.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0, 10.0,-27.0,45.0,-22.5,-18.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (100.0,-30.0,0.0,0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     0.4,0.0, stiff.NORMAL_STIFFNESSES),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    # Kick?
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    ((43.0,30.0,0.0,0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,17.0, -60.0,70.0,-10.0,-15.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,10.0,-27.0,45.0,-22.5,-18.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (20.0,-30.0,0.0, 0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     0.14,0.0, stiff.NORMAL_STIFFNESSES),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    # Recover
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    # ((80.,30.,-50.,-70.),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    ((90.0,10.0,-90.0,-10.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,25.0,-27.0,43.5,-21.2,-20.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,15.0,-27.0,45.0,-22.5,-18.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (80.0,-30.0,50.0,74.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     0.7,0.0, stiff.NORMAL_STIFFNESSES),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    ((90.0,10.0,-90.0,-10.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     (90.0,-10.0,82.0,13.2),
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy     0.7,0.0,stiff.NORMAL_STIFFNESSES)
pypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypypy    )