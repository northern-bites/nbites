class_<XYZ_Coord>("XYZ_Coord")
   .def_readonly("x", &XYZ_Coord::x)
   .def_readonly("y", &XYZ_Coord::y)
   .def_readonly("z", &XYZ_Coord::z);
