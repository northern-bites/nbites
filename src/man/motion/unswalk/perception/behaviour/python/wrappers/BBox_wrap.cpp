class_<BBox>("BBox")
   .add_property("a"          , make_getter(&BBox::a, return_value_policy<return_by_value>()))
   .add_property("b"          , make_getter(&BBox::b, return_value_policy<return_by_value>()))
   .def_readonly("width"      , &BBox::width     )
   .def_readonly("height"     , &BBox::height    )
   .def         ("within"     , &BBox::within    )
   .def         ("validIndex" , &BBox::validIndex);

