class_<BallInfo>("BallInfo")
   .def_readonly("rr", &BallInfo::rr)
   .def_readonly("radius", &BallInfo::radius)
   .add_property("imageCoords", make_getter(&BallInfo::imageCoords, return_value_policy<return_by_value>()))
   .add_property("neckRelative", &BallInfo::neckRelative)
   .def_readonly("visionVar", &BallInfo::visionVar)
   .def_readonly("topCamera", &BallInfo::topCamera);

class_<BallHint>("BallHint")
   .def_readonly("type"        , &BallHint::type       );

enum_<BallHint::Type>("BallHintType")
   .value("bLeft"              , BallHint::bLeft       )
   .value("bRight"             , BallHint::bRight      )
   .value("bHidden"            , BallHint::bHidden     )
   .value("bNone"              , BallHint::bNone       );
