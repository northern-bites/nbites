class_<GameControllerBlackboard>("GameControllerBlackboard").
   def_readonly("connect"      , &GameControllerBlackboard::connect      ).
   def_readonly("connected"    , &GameControllerBlackboard::connected    ).
   def_readonly("data"         , &GameControllerBlackboard::data         ).
   def_readonly("our_team"     , &GameControllerBlackboard::our_team     ).
   def_readonly("team_red"     , &GameControllerBlackboard::team_red     ).
   def_readonly("player_number", &GameControllerBlackboard::player_number).
   def_readonly("game_type"    , &GameControllerBlackboard::game_type    );
