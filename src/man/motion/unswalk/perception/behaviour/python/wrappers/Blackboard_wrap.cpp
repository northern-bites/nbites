class_<Blackboard>("Blackboard")
   .def_readonly("motion"        , &Blackboard::motion        )
   .def_readonly("vision"        , &Blackboard::vision        )
   .def_readonly("localisation"  , &Blackboard::localisation  )
   .def_readonly("behaviour"     , &Blackboard::behaviour     )
   .def_readonly("gameController", &Blackboard::gameController)
   .def_readonly("receiver"      , &Blackboard::receiver      )
   .def_readonly("kinematics"    , &Blackboard::kinematics    )
   ;
