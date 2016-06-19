class_<RobotObstacle>("RobotObstacle")
   .def_readonly("rr", &RobotObstacle::rr)
   .def_readonly("type", &RobotObstacle::type)
   .def_readonly("rrc", &RobotObstacle::rrc)
   .def_readonly("pos", &RobotObstacle::pos)
   .def_readonly("tangentHeadingLeft", &RobotObstacle::tangentHeadingLeft)
   .def_readonly("tangentHeadingRight", &RobotObstacle::tangentHeadingRight)
   .def_readonly("evadeVectorLeft", &RobotObstacle::evadeVectorLeft)
   .def_readonly("evadeVectorRight", &RobotObstacle::evadeVectorRight);
