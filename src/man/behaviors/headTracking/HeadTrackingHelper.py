from . import TrackingConstants as constants
from .. import MotionConstants
from ..util import MyMath as MyMath
from .. import StiffnessModes
from math import fabs
import HeadMoves

class HeadTrackingHelper(object):
    def __init__(self, tracker):
        self.tracker = tracker

    def executeHeadMove(self, headMove):
        """performs a sweetmove"""
        command = self.tracker.brain.interface.headMotionCommand
        command.type = command.CommandType.SCRIPTED_HEAD_COMMAND

        for position in headMove:
            if len(position) == 4:
                move = command.scripted_command.add_command()

                # Set most recent command
                move.time = position[1]
                if position[2] == 1:
                    move.interpolation = move.InterpolationType.LINEAR
                else:
                    move.interpolation = move.InterpolationType.SMOOTH

                # Only set the head angles
                move.angles.head_yaw   = position[0][0]
                move.angles.head_pitch = position[0][1]

                # Set all stiffnesses, since this command specifies them all
                move.stiffness.head_yaw =         position[3][0]
                move.stiffness.head_pitch =       position[3][1]

                move.stiffness.l_shoulder_pitch = position[3][2]
                move.stiffness.l_shoulder_roll =  position[3][3]
                move.stiffness.l_elbow_yaw =      position[3][4]
                move.stiffness.l_elbow_roll =     position[3][5]

                move.stiffness.r_shoulder_pitch = position[3][18]
                move.stiffness.r_shoulder_roll =  position[3][19]
                move.stiffness.r_elbow_yaw =      position[3][20]
                move.stiffness.r_elbow_roll =     position[3][21]

                move.stiffness.l_hip_yaw_pitch =  position[3][6]
                move.stiffness.l_hip_roll =       position[3][7]
                move.stiffness.l_hip_pitch =      position[3][8]
                move.stiffness.l_knee_pitch =     position[3][9]
                move.stiffness.l_ankle_pitch =    position[3][10]
                move.stiffness.l_ankle_roll =     position[3][11]

                move.stiffness.r_hip_yaw_pitch =  position[3][12]
                move.stiffness.r_hip_roll =       position[3][13]
                move.stiffness.r_hip_pitch =      position[3][14]
                move.stiffness.r_knee_pitch =     position[3][15]
                move.stiffness.r_ankle_pitch =    position[3][16]
                move.stiffness.r_ankle_roll =     position[3][17]

            else:
                self.tracker.printf("What kind of sweet ass-Move is this?")

        command.processed_by_motion = False
        # Returns the last HJC in the HeadMove for keeping track of
        # when a move is done
        return move

    def startingPan(self, headMove):
        """Calculates the first part of a fixed pitch pan to get there quickly."""
        if len(headMove) < 2:
            # Not a normal pan: there's only 1 headMove.
            # Don't do a starting move.
            return

        headMoveYaw = headMove[1][0][0]
        headMovePitch = headMove[1][0][1]

        #TODO do this math in C++
        curYaw = self.tracker.brain.interface.joints.head_yaw
        degreesPerSecond = 80 #fast, but hopefully won't destabilize the walk much
        yawDiff = MyMath.fabs(curYaw - headMoveYaw)
        totalTime = yawDiff/degreesPerSecond

        newHeadMove = ( ((headMoveYaw,headMovePitch), totalTime, 1,
                         StiffnessModes.LOW_HEAD_STIFFNESSES), )

        self.executeHeadMove(newHeadMove)

    # Should be generalized.
    def convertKickPan(self, headMove, invert):
        """
        Converts the first step of the kick pan to have the same speed
        as the second step, regardless of starting yaw.
        ASSERT: 2 step headMove.
        """
        headMoveYaw = headMove[0][0][0]
        headMovePitch = headMove[0][0][1]

        motionAngles = self.tracker.brain.interface.joints
        curYaw = self.tracker.brain.interface.joints.head_yaw
        degreesPerSecond = (headMoveYaw*2)/headMove[0][1]
        yawDiff = MyMath.fabs(curYaw-headMoveYaw)
        totalTime = yawDiff/degreesPerSecond

        if invert is True:
            newHeadMove = ( ((-1*headMoveYaw,headMovePitch), totalTime, 1,
                             StiffnessModes.LOW_HEAD_STIFFNESSES),
                            ((-1*headMove[1][0][0],headMove[1][0][1]),
                             headMove[1][1], headMove[1][2], headMove[1][3]) )
        else:
            newHeadMove = ( ((headMoveYaw,headMovePitch), totalTime, 1,
                             StiffnessModes.LOW_HEAD_STIFFNESSES),
                            headMove[1] )

        return newHeadMove

    def trackObject(self):
        """
        Method to actually perform the tracking.
        Should only be called explicitly from state
        methods in TrackingStates.py
        """
        target = self.tracker.target

        changeX, changeY = 0.0 , 0.0

        # If we don't see it, let's try to use our model of it to find
        # it and track it

        # TODO: generalize this to objects which we cannot see.
        if not target or \
                (target.loc.relX == 0.0 and target.loc.relY == 0.0):
            return

        # If we haven't seen the object in the very recent past, look
        # towards where the model says it is. The framesOff > 3
        # provides a buffer to ensure that it's not just a flickering
        # image problem (prevents twitchy robot)
        if target.vis.framesOff > 3:
            self.lookToPoint(target)
            return

        # Find the target's angular distance from the center of the screen
        # if we have an object, track that
        changeX = target.vis.angleX
        changeY = target.vis.angleY #the pitch is pos = down

        motionAngles = self.tracker.brain.interface.joints
        curYaw   = self.tracker.brain.interface.joints.head_yaw
        curPitch = self.tracker.brain.interface.joints.head_pitch

        maxChange = 13.0

        # Warning- no gain is applied currently!
        safeChangeX = MyMath.clip(changeX, -maxChange, maxChange )
        safeChangeY = MyMath.clip(changeY, -maxChange, maxChange )

        newYaw = curYaw + safeChangeX
        newPitch = curPitch - safeChangeY

        newYaw = MyMath.clip(newYaw, -80., 80.)

        maxSpeed = 2.0

        # Set message fields
        command = self.tracker.brain.interface.headMotionCommand
        command.type = command.CommandType.POS_HEAD_COMMAND

        command.pos_command.head_yaw = newYaw
        command.pos_command.head_pitch = newPitch
        command.pos_command.max_speed_yaw = maxSpeed
        command.pos_command.max_speed_pitch = maxSpeed

        command.processed_by_motion = False

    # Fixed Pitch
    def trackObjectFixedPitch(self):
        """
        Analogous to trackObject, but with a fixed value for pitch.
        """
        target = self.tracker.target
        changeX, changeY = 0.0, 0.0

        # If we cannot see the target, abort.
        if not target or \
                (target.loc.relX == 0.0 and target.loc.relY == 0.0) or \
                (target.vis.framesOff > 0 and target.vis.framesOff < 3):
            return

        # If we haven't seen the target, look towards loc model.
        if target.vis.framesOff > 3:
            self.lookToPointFixedPitch(target)
            return

        # Assert: target is visible.

        # Find the target's angular distance from yaw center.
        changeX = target.vis.angleX
        # ignore changeY: pitch is fixed

        motionAngles = self.tracker.brain.interface.joints
        curYaw   = self.tracker.brain.interface.joints.head_yaw
        curPitch = self.tracker.brain.interface.joints.head_pitch
        maxChange = 13.0

        # Warning- no gain is applied currently!
        safeChangeX = MyMath.clip(changeX, -maxChange, maxChange)
        # ignore safeChangeY: pitch is fixed

        newYaw = curYaw + safeChangeX
        # ignore newPitch: pitch is fixed

        maxSpeed = 2.0

        # Set message fields
        command = self.tracker.brain.interface.headMotionCommand
        command.type = command.CommandType.POS_HEAD_COMMAND

        command.pos_command.head_yaw = newYaw
        command.pos_command.head_pitch = 20.0 # MAKE A CONSTANT FOR THIS
        command.pos_command.max_speed_yaw = maxSpeed
        command.pos_command.max_speed_pitch = maxSpeed

        command.processed_by_motion = False

    # Not called anywhere in the code.
    def lookToTargetAngles(self, target):
        """
        Uses setHeadCommands to bring given target to center of frame.
        """
        motionAngles = self.tracker.brain.interface.joints
        yaw = self.tracker.brain.interface.joints.head_yaw
        pitch = self.tracker.brain.interface.joints.head_pitch

        # Find the target's angular distance from the center of vision
        if not target is None and target.vis.on:
            yaw += target.vis.angleX
            pitch -= target.vis.angleY
            #note: angleY is positive up from center of vision frame
            #note: angleX is positive left from center of vision frame
        else:
            # by default, do nothing
            return

        # Set message fields
        command = self.tracker.brain.interface.headMotionCommand
        command.type = command.CommandType.POS_HEAD_COMMAND

        command.pos_command.head_yaw = yaw
        command.pos_command.head_pitch = pitch
        # Leave max speeds to default
        command.processed_by_motion = False

    def panTo(self, heads):
        """
        Pan heads at appropriate speed to given heads
        """
        headYaw = self.tracker.brain.interface.joints.head_yaw
        headPitch = self.tracker.brain.interface.joints.head_pitch

        yawDiff = fabs(heads[0] - headYaw)
        pitchDiff = fabs(heads[1] - headPitch)

        maxDiff = max(pitchDiff, yawDiff)
        panTime = maxDiff/constants.MAX_PAN_SPEED
        self.executeHeadMove(((heads, panTime, 0,
                               StiffnessModes.LOW_HEAD_STIFFNESSES), ))

    # Unsafe to call... TODO: CoordHeadCommands for messages.
    def lookToPoint(self, target):
        if hasattr(target, "height"):
            height = target.height
        else:
            height = 0

        if hasattr(target, "loc"):
            target = target.loc

        print str(target.relX) + " " + str(target.relY)

        headMove = motion.CoordHeadCommand(relX = target.relX,
                                           relY = target.relY,
                                           relZ = height)

        self.tracker.brain.motion.coordHead(headMove)
        return headMove

    # Fixed Pitch
    def lookToPointFixedPitch(self, target):
        """
        If the relative y is positive, look left. Otherwise, look right.
        """
        if hasattr(target, "height"):
            height = target.height
        else:
            height = 0

        if hasattr(target, "loc"):
            target = target.loc

        if target.relY > 0:
            self.executeHeadMove(HeadMoves.FIXED_PITCH_LOOK_LEFT)
        else:
            self.executeHeadMove(HeadMoves.FIXED_PITCH_LOOK_RIGHT)

    # Fixed Pitch
    def lookToAngleFixedPitch(self, yaw):
        """
        Looks to the given yaw at the appropriate fixed pitch.
        NOTE: should call brain.motion.stopHeadMoves() first!
        """
        if yaw > 55 or yaw < -55:
            pitch = 11.0
        else:
            pitch = 17.0

        self.executeHeadMove((((yaw, pitch), 2.0, 1,
                               StiffnessModes.LOW_HEAD_STIFFNESSES),))

    # Consider updating this for new loc and vision systems (summer 2012)
    def calculateClosestLandmark(self):
        brain = self.tracker.brain
        posts = [brain.yglp, brain.ygrp, brain.bgrp, brain.bglp]


        currYaw = brain.interface.joints.head_yaw

        minDiff = 1000000000
        bestPost = None

        for p in posts:
            diff = MyMath.sub180Angle(currYaw - p.bearing)

            if diff < minDiff:
                bestPost = p
                minDiff = diff
        return bestPost
