
from motion import (MotionInterface,
                    BodyJointCommand,
                    HeadJointCommand,
                    HeadScanCommand,
                    WalkCommand)

def test_MotionInterface():
    intface = MotionInterface()

def test_BodyJointCommand():
    cmd1 = BodyJointCommand(0.5, [0.]*22, 0)
    cmd2 = BodyJointCommand(cmd1)
    cmd3 = BodyJointCommand(0.2, 2, [0.]*6, 1)
    cmd4 = BodyJointCommand(2.0, [0.]*4, [0.]*6, [0.]*6, [0.]*4, 1)

    cmd5 = BodyJointCommand(time=0.5, joints=[0.]*22, type=0)
    cmd6 = BodyJointCommand(other=cmd1)
    cmd7 = BodyJointCommand(time=0.2, chain=2, joints=[0.]*6, type=1)
    cmd8 = BodyJointCommand(time=2.0, larm=[0.]*4, lleg=[0.]*6, rleg=[0.]*6,
                            rarm=[0.]*4, type=1)

    cmd9 = BodyJointCommand(time=2.0, lleg=[0.]*6, type=1)

def test_HeadJointCommand():
    cmd1 = HeadJointCommand(1.0, [10., 5.], 0)
    cmd2 = HeadJointCommand(cmd1)

def test_HeadScanCommand():
    cmd1 = HeadJointCommand(1.0, [10., 5.], 0)
    cmd2 = HeadScanCommand(2., [])
    cmd3 = HeadScanCommand(2., [cmd1])
    cmd4 = HeadScanCommand(cmd3)

if __name__ == '__main__':
    test_BodyJointCommand()
    test_HeadJointCommand()
