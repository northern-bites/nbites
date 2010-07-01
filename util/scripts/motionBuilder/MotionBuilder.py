#needed to grab Matrix.py in other folder
manDir = "../../../../branches/robotstadium/controllers/nao_team_1"
import sys
sys.path.append(manDir)
import Motion.MotionConstants
import Motion.MotionCore

DEBUG =False
NONE_VAR_NAME = "OFF" # shorter name for NONE
PRECISION = 1 #number of decimals in angles
COUNT_FRAMES = False

"""
ToDo:
 Make this automatically add to SweetMoves.py. Parse the file, find the name if it exists, and replace, or add new

 Add a name attribute to Move class so we don't need to define it in the code (each class names itself).

 Make a more automatic system for doing this from behaviors, so we don't need to copy and paste


"""
class MotionBuilder():
    """
    Class to help build scripted motions. Does not run on the robot.

    See the wiki for an (eventual) guide on how to use
    """
    
    def __init__(self):
        self.builtMotion = [] #stores the sets of angles combined so far
        self.initCommands()
        self.frameCounter = 0
        self.core = Motion.MotionCore.MotionCore()
        self.name = "DEFAULT_MOVE"
        
    def initCommands(self):
        """
        Before we make each frame, each object (x,y,z) needs to be set to None

        Accorind to the format shown in MotionCore.py, None means nothing gets queued to the queue for that chain.

        For the head, we never store (x,y,z) destination, only pan,pitch
        """
        self.head = None
        self.leftArm = None
        self.leftLeg = None
        self.rightLeg= None
        self.rightArm = None

        
    def buildFrame(self,time):
        """
        Takes the destinations stored in self.chainName vars and gets the angles to go there using Kinematics
        """

        if COUNT_FRAMES:
            if self.frameCounter == 0:
                print "processing frame",
            print self.frameCounter,"...",

        self.chains = (self.head,self.leftArm,self.leftLeg,self.rightLeg,self.rightArm)        
        thisBodyCommand = []

        
        for chainID in range (Motion.MotionConstants.HEAD_CHAIN,Motion.MotionConstants.RIGHT_ARM_CHAIN+1): #all chains but the head
            thisCommand = self.chains[chainID]
            if DEBUG: print thisCommand
            if thisCommand == None or len(thisCommand) != 3:
                #it's already None, or in angular form, so we can just add it
                thisBodyCommand.append(thisCommand)
            else:
                #get the angles to go to this xyz
                (x,y,z) = thisCommand
                angles = self.core.getChainAnglesTo(chainID,x,y,z)

                thisBodyCommand.append(angles)
                
        thisBodyCommand.append(time)
        
        if DEBUG:print thisBodyCommand
        self.builtMotion.append(thisBodyCommand)
        
        #reset the storage
        self.initCommands()
        self.frameCounter +=1


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
        pass


    def printMotionCode(self):
        """
        Prints the python code which defines a tuple of 
        (tuples of (None or tuples )) which define a motion.
        See MotionCore for details.
        """

        #print end line for formating reasons
        if COUNT_FRAMES: print "\n"
#loop through each separate command
        
        if len(self.builtMotion) == 0:
            return

        #a whole bunch of body commands?
        cStr = self.name + " = ("
        for i in xrange(len(self.builtMotion)):
            time = self.builtMotion[i][5]
            #a chain commands
            cStr +="("
            for j in xrange(len(self.builtMotion[i]) -1):

                command = self.builtMotion[i][j]

                if command == None:
                    cStr +=NONE_VAR_NAME+","
                else:
                    cStr += "("
                    for k in xrange(len(command)):
                        angle = command[k]
                        if k < len(command) -1:
                            f="%."+str(PRECISION)+"f,"
                            cStr += f%(angle)
                        else:
                            f ="%."+str(PRECISION)+"f"
                            cStr += f%(angle)
                    cStr += "),"
                
            
            #make the end of all 5 chain commands with the time
            if i < len(self.builtMotion) -1:
                cStr += str(time)+"), \\\n                 "
            else:
                cStr += str(time)+") "

        cStr += ")"
        print cStr


if __name__ == "__main__":
    from Switch import selectedMove
    s = selectedMove.Move()
    s.scriptedMotion()
    s.printMotionCode()

