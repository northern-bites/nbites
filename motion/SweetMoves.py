
#constants file to store all our sweet ass-moves for the Nao
import MotionConstants


OFF = None #OFF means the joint chain doesnt get enqueued during this motion


INITIAL_POS = (((80.,40.,-50.,-70.),(0.,0.,-50.,100.,-50.,0.),(0.,0.,-50.,100.,-50.,0.),(80.,-40.,50.,70.),4.0,1),)

SIT_POS = (((90.,0.,-65.,-57.),(0.,0.,-55.,130.,-75.,0.),(0.,0.,-55.,130.,-75.,0.),(90.,0.,65.,57.),4.0,1),)

PENALIZED_POS = INITIAL_POS

SET_POS = INITIAL_POS

READY_POS = INITIAL_POS

NEUT_HEADS = (((0.,20.),2.0,1),)
