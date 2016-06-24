class_<ReceiverBlackboard>("ReceiverBlackboard")
	.def_readonly("incapacitated"  , &ReceiverBlackboard::incapacitated  )
	.def_readonly("lastReceived"   , &ReceiverBlackboard::lastReceived   )
	.def_readonly("data"           , &ReceiverBlackboard::data           );
