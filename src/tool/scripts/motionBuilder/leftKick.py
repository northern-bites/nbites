from MotionBuilder import MotionBuilder

class Move(MotionBuilder):
    def scriptedMotion(self):
        """
        Here is where we build the motion. The easiest way is to have a variable
        like lLx (leftLegX) which you initially set to -20, for example. Then
        later, you say lLx += 5 to move the left leg 5 mm forward. After you set
        all the chains you want, you need to call buildFrame, which saves the 
        angles.
        
        Notes:
         * None means a chain doesnt get queued. If you want a chain to stay 
         where it is, you must do so explicitly
           
         * self.head should generally only ever take angles.
         """
        time = .2
        (lx,ly,lz) = (-20,50,-300)
        (rx,ry,rz) = (-20,-50,-300)
        
        #default
        self.head = None #'None' commands are optional
        self.leftArm = None
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.rightArm = None
        self.buildFrame(time)

        #shift weight to right leg
        ly += 50
        ry += 50
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.buildFrame(time)

        #foot up
        lz +=20
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.buildFrame(time)

        #foot forward
        lx +=60
        time = .15
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.buildFrame(time)
        time = .2

        #foot back
        lx -=60
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.buildFrame(time)

        #foot down
        lz -=20
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.buildFrame(time)
        #shift weight to middle 
        ly -= 50
        ry -= 50
        self.leftLeg = (lx,ly,lz)
        self.rightLeg = (rx,ry,rz)
        self.buildFrame(time)
