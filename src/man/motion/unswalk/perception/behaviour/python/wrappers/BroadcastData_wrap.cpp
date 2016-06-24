class_<BroadcastData>("BroadcastData")
    .def_readonly("playerNum", &BroadcastData::playerNum)
    .def_readonly("team", &BroadcastData::team)
    .def_readonly("robotPos", &BroadcastData::robotPos)
    .def_readonly("lostCount", &BroadcastData::lostCount)
    .def_readonly("ballPos", &BroadcastData::ballPosAbs)
    .def_readonly("ballPosRR", &BroadcastData::ballPosRR)
    .def_readonly("behaviourSharedData", &BroadcastData::behaviourSharedData)
    .def_readonly("uptime", &BroadcastData::uptime);
