class_<ReadySkillPositionAllocation>("ReadySkillPositionAllocation")
    .def_readonly("fromPlayerNum", &ReadySkillPositionAllocation::fromPlayerNum)
    .def_readonly("readyPositionAllocation0", &ReadySkillPositionAllocation::readyPositionAllocation0)
    .def_readonly("readyPositionAllocation1", &ReadySkillPositionAllocation::readyPositionAllocation1)
    .def_readonly("readyPositionAllocation2", &ReadySkillPositionAllocation::readyPositionAllocation2)
    .def_readonly("readyPositionAllocation3", &ReadySkillPositionAllocation::readyPositionAllocation3)
    .def_readonly("readyPositionAllocation4", &ReadySkillPositionAllocation::readyPositionAllocation4);
