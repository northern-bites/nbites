class_<RoboCupGameControlData>("RoboCupGameControlData")
   .def_readonly("header"        , &RoboCupGameControlData::header        )
   .def_readonly("version"       , &RoboCupGameControlData::version       )
   .def_readonly("playersPerTeam", &RoboCupGameControlData::playersPerTeam)
   .def_readonly("state"         , &RoboCupGameControlData::state         )
   .def_readonly("firstHalf"     , &RoboCupGameControlData::firstHalf     )
   .def_readonly("kickOffTeam"   , &RoboCupGameControlData::kickOffTeam   )
   .def_readonly("secondaryState", &RoboCupGameControlData::secondaryState)
   .def_readonly("dropInTeam"    , &RoboCupGameControlData::dropInTeam    )
   .def_readonly("dropInTime"    , &RoboCupGameControlData::dropInTime    )
   .def_readonly("secsRemaining" , &RoboCupGameControlData::secsRemaining )
   .add_property("teams"         , &RoboCupGameControlData::teams         );

class_<PlayerInfo>("PlayerInfo")
   .def_readonly("penalty"              , &PlayerInfo::penalty            )
   .def_readonly("secsTillUnpenalised"  , &PlayerInfo::secsTillUnpenalised);

class_<TeamInfo>("TeamInfo")
   .def_readonly("teamNumber"           , &TeamInfo::teamNumber           )
   .def_readonly("teamColour"           , &TeamInfo::teamColour           )
   .def_readonly("score"                , &TeamInfo::score                )
   .add_property("players"              , &TeamInfo::players              );

enum_<GameType>("GameType")
   .value("MATCH"                       , MATCH                           )
   .value("DRIBBLE"                     , DRIBBLE                         )
   .value("OPEN"                        , OPEN                            )
   .value("PASSING"                     , PASSING                         );

   scope().attr("MAX_NUM_PLAYERS"                   ) = MAX_NUM_PLAYERS;
   scope().attr("TEAM_BLUE"                         ) = TEAM_BLUE;
   scope().attr("TEAM_CYAN"                         ) = TEAM_CYAN;
   scope().attr("TEAM_RED"                          ) = TEAM_RED;
   scope().attr("TEAM_MAGENTA"                      ) = TEAM_MAGENTA;
   scope().attr("GOAL_BLUE"                         ) = GOAL_BLUE;
   scope().attr("GOAL_YELLOW"                       ) = GOAL_YELLOW;
   scope().attr("STATE_INITIAL"                     ) = STATE_INITIAL;
   scope().attr("STATE_READY"                       ) = STATE_READY;
   scope().attr("STATE_SET"                         ) = STATE_SET;
   scope().attr("STATE_PLAYING"                     ) = STATE_PLAYING;
   scope().attr("STATE_FINISHED"                    ) = STATE_FINISHED;
   scope().attr("STATE_INVALID"                     ) = STATE_INVALID;
   scope().attr("STATE_PENALISED"                   ) = STATE_PENALISED;
   scope().attr("STATE2_NORMAL"                     ) = STATE2_NORMAL;
   scope().attr("STATE2_PENALTYSHOOT"               ) = STATE2_PENALTYSHOOT;
   scope().attr("PENALTY_NONE"                      ) = PENALTY_NONE;
   scope().attr("PENALTY_SPL_BALL_HOLDING"          ) = PENALTY_SPL_BALL_HOLDING;
   scope().attr("PENALTY_SPL_PLAYER_PUSHING"        ) = PENALTY_SPL_PLAYER_PUSHING;
   scope().attr("PENALTY_SPL_OBSTRUCTION"           ) = PENALTY_SPL_OBSTRUCTION;
   scope().attr("PENALTY_SPL_INACTIVE_PLAYER"       ) = PENALTY_SPL_INACTIVE_PLAYER;
   scope().attr("PENALTY_SPL_ILLEGAL_DEFENDER"      ) = PENALTY_SPL_ILLEGAL_DEFENDER;
   scope().attr("PENALTY_SPL_LEAVING_THE_FIELD"     ) = PENALTY_SPL_LEAVING_THE_FIELD;
   scope().attr("PENALTY_SPL_PLAYING_WITH_HANDS"    ) = PENALTY_SPL_PLAYING_WITH_HANDS;
   scope().attr("PENALTY_SPL_REQUEST_FOR_PICKUP"    ) = PENALTY_SPL_REQUEST_FOR_PICKUP;
   scope().attr("PENALTY_HL_KID_BALL_MANIPULATION"  ) = PENALTY_HL_KID_BALL_MANIPULATION;
   scope().attr("PENALTY_HL_KID_PHYSICAL_CONTACT"   ) = PENALTY_HL_KID_PHYSICAL_CONTACT;
   scope().attr("PENALTY_HL_KID_ILLEGAL_ATTACK"     ) = PENALTY_HL_KID_ILLEGAL_ATTACK;
   scope().attr("PENALTY_HL_KID_ILLEGAL_DEFENSE"    ) = PENALTY_HL_KID_ILLEGAL_DEFENSE;
   scope().attr("PENALTY_HL_KID_REQUEST_FOR_PICKUP" ) = PENALTY_HL_KID_REQUEST_FOR_PICKUP;
   scope().attr("PENALTY_HL_TEEN_BALL_MANIPULATION" ) = PENALTY_HL_TEEN_BALL_MANIPULATION;
   scope().attr("PENALTY_HL_TEEN_PHYSICAL_CONTACT"  ) = PENALTY_HL_TEEN_PHYSICAL_CONTACT;
   scope().attr("PENALTY_HL_TEEN_ILLEGAL_ATTACK"    ) = PENALTY_HL_TEEN_ILLEGAL_ATTACK;
   scope().attr("PENALTY_HL_TEEN_ILLEGAL_DEFENSE"   ) = PENALTY_HL_TEEN_ILLEGAL_DEFENSE;
   scope().attr("PENALTY_HL_TEEN_REQUEST_FOR_PICKUP") = PENALTY_HL_TEEN_REQUEST_FOR_PICKUP;
   scope().attr("PENALTY_MANUAL"                    ) = PENALTY_MANUAL;

