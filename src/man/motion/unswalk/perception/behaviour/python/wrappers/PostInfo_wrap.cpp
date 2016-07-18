class_<PostInfo>("PostInfo").
   def_readwrite("rr"          , &PostInfo::rr         ).
   def_readwrite("type"        , &PostInfo::type       ).
   def_readwrite("imageCoords" , &PostInfo::imageCoords);

enum_<PostInfo::Type>("PostType").
   value("pNone"       , PostInfo::pNone       ).
   value("pLeft"       , PostInfo::pLeft       ).
   value("pRight"      , PostInfo::pRight      ).
   value("pHome"       , PostInfo::pHome       ).
   value("pAway"       , PostInfo::pAway       ).
   value("pHomeLeft"   , PostInfo::pHomeLeft   ).
   value("pHomeRight"  , PostInfo::pHomeRight  ).
   value("pAwayLeft"   , PostInfo::pAwayLeft   ).
   value("pAwayRight"  , PostInfo::pAwayRight  );

