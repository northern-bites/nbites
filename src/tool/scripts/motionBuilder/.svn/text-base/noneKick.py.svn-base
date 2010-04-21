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

        #Frame 1: defualt position
        time = .2
        (lLx,lLy,lLz) = (-20,50,-300)
        (rLx,rLy,rLz) = (-20,-50,-300)
        self.leftLeg = (lLx,lLy,lLz)
        self.rightLeg = (rLx,rLy,rLz)
        #uncommnet to create a frame
        #self.buildFrame(time)
        
