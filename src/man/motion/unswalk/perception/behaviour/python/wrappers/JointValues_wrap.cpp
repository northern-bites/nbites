class_<JointValues>("JointValues")
   .add_property("angles"       , &JointValues::angles      )
   .add_property("stiffnesses"  , &JointValues::stiffnesses )
   .add_property("temperatures" , &JointValues::temperatures);

