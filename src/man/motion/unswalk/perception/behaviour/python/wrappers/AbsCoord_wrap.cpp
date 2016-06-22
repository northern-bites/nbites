class_<AbsCoord>("AbsCoord").
   def(init<float, float, float>()).
   add_property("x"      , abs_get_v_const_type(&AbsCoord::x     )).
   add_property("y"      , abs_get_v_const_type(&AbsCoord::y     )).
   add_property("theta"  , abs_get_v_const_type(&AbsCoord::theta )).
   def_readonly("weight" , &AbsCoord::weight                      ).
   add_property("var"    , make_getter(&AbsCoord::var, return_value_policy<return_by_value>()));

