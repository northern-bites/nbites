class_<UNSWRobotInfo>("UNSWRobotInfo")
   .def_readonly("rr"       , &UNSWRobotInfo::rr      )
   .def_readonly("type"     , &UNSWRobotInfo::type    );

enum_<UNSWRobotInfo::Type>("UNSWRobotInfoType")
   .value("rUnknown"  , UNSWRobotInfo::rUnknown    )
   .value("rBlue"     , UNSWRobotInfo::rBlue       )
   .value("rRed"      , UNSWRobotInfo::rRed        );

