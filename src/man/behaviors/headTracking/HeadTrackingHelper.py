from . import TrackingConstants as constants
from .. import MotionConstants
from ..util import MyMath as MyMath
from .. import StiffnessModes
from math import fabs

class HeadTrackingHelper(object):
    def __init__(self, tracker):
        self.tracker = tracker

    def executeHeadMove(self, headMove):
        """performs a sweetmove"""
        for position in headMove:
            if len(position) == 4:
                move = motion.HeadJointCommand(position[1] ,# time
                                               position[0], # head pos
                                               position[3], # chain stiffnesses
                                               position[2], # interpolation type
                                               )
            else:
                self.tracker.printf("What kind of sweet ass-Move is this?")

            self.tracker.brain.motion.enqueue(move)

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

        motionAngles = self.tracker.brain.sensors.motionAngles
        curYaw = motionAngles[MotionConstants.HeadYaw]
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

        motionAngles = self.tracker.brain.sensors.motionAngles
        curYaw = motionAngles[MotionConstants.HeadYaw]
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

        motionAngles = self.tracker.brain.sensors.motionAngles
        curPitch = motionAngles[MotionConstants.HeadPitch]
        curYaw = motionAngles[MotionConstants.HeadYaw]

        maxChange = 13.0

        # Warning- no gain is applied currently!
        safeChangeX = MyMath.clip(changeX, -maxChange, maxChange )
        safeChangeY = MyMath.clip(changeY, -maxChange, maxChange )

        newYaw = curYaw + safeChangeX
        newPitch = curPitch - safeChangeY

        newYaw = MyMath.clip(newYaw, -80., 80.)

        maxSpeed = 2.0
        headMove = motion.SetHeadCommand(newYaw, newPitch,
                                         maxSpeed, maxSpeed)
        self.tracker.brain.motion.setHead(headMove)

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

        motionAngles = self.tracker.brain.sensors.motionAngles
        curPitch = motionAngles[MotionConstants.HeadPitch]
        curYaw = motionAngles[MotionConstants.HeadYaw]

        maxChange = 13.0

        # Warning- no gain is applied currently!
        safeChangeX = MyMath.clip(changeX, -maxChange, maxChange)
        # ignore safeChangeY: pitch is fixed

        newYaw = curYaw + safeChangeX
        # ignore newPitch: pitch is fixed

        maxSpeed = 2.0
        headMove = motion.SetHeadCommand(newYaw, 20.0, # MAKE A CONSTANT FOR THIS
                                         maxSpeed, maxSpeed)
        self.tracker.brain.motion.setHead(headMove)

    # Not called anywhere in the code.
    def lookToTargetAngles(self, target):
        """
        Uses setHeadCommands to bring given target to center of frame.
        """
        motionAngles = self.tracker.brain.sensors.motionAngles
        yaw = motionAngles[MotionConstants.HeadYaw]
        pitch = motionAngles[MotionConstants.HeadPitch]

        # Find the target's angular distance from the center of vision
        if not target is None and target.vis.on:
            yaw += target.vis.angleX
            pitch -= target.vis.angleY
            #note: angleY is positive up from center of vision frame
            #note: angleX is positive left from center of vision frame
        else:
            # by default, do nothing
            return

        headMove = motion.SetHeadCommand(yaw,pitch)
        self.tracker.brain.motion.setHead(headMove)

    def panTo(self, heads):
        """
        Pan heads at appropriate speed to given heads
        """
        motionAngles = self.tracker.brain.sensors.motionAngles

        headPitch = motionAngles[MotionConstants.HeadPitch]
        headYaw = motionAngles[MotionConstants.HeadYaw]

        yawDiff = fabs(heads[0] - headYaw)
        pitchDiff = fabs(heads[1] - headPitch)

        maxDiff = max(pitchDiff, yawDiff)
        panTime = maxDiff/constants.MAX_PAN_SPEED
        self.executeHeadMove(((heads, panTime, 0,
                               StiffnessModes.LOW_HEAD_STIFFNESSES), ))

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
            self.executeHeadMove(motion.HeadMoves.FIXED_PITCH_LOOK_LEFT)
        else:
            self.executeHeadMove(motion.HeadMoves.FIXED_PITCH_LOOK_RIGHT)

    # broken?
    # Not called anywhere in code.
    def lookToAngles(self, yaw=0, pitch=0):
        headMove = motion.SetHeadCommand(MyMath.degrees(yaw),
                                         MyMath.degrees(pitch))
        self.tracker.brain.motion.setHead(headMove)

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

        currYaw = brain.sensors.motionAngles[MotionConstants.HeadYaw]

        minDiff = 1000000000
        bestPost = None

        for p in posts:
            diff = MyMath.sub180Angle(currYaw - p.bearing)

            if diff < minDiff:
                bestPost = p
                minDiff = diff
        return bestPost

    # Debug method to print current head pitch and yaw.
    def printHeadAngles(self):

        # hijacked for vision testing
        curPitch = self.tracker.brain.sensors.motionAngles[MotionConstants.HeadPitch]
        curYaw = self.tracker.brain.sensors.motionAngles[MotionConstants.HeadYaw]

        self.tracker.printf("Head pitch is:")
        self.tracker.printf(curPitch)
        self.tracker.printf("Head yaw is:")
        self.tracker.printf(curYaw)
