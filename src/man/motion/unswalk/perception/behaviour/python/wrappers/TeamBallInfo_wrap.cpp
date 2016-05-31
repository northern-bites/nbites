class_<TeamBallInfo>("TeamBallInfo")
   .def_readonly("pos"            , &TeamBallInfo::pos           )
   .def_readonly("status"         , &TeamBallInfo::status        )
   .def_readonly("contributors"   , &TeamBallInfo::contributors  );

enum_<TeamBallInfo::Status>("TeamBallInfoStatus")
   .value("tNeutral"  , TeamBallInfo::tNeutral  )
   .value("tAgree"    , TeamBallInfo::tAgree    )
   .value("tDisagree" , TeamBallInfo::tDisagree );
