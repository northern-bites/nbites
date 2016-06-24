class_<RRCoord>("RRCoord").
   def(init<float, float, float>()).
   add_property("distance"   , rr_get_v_const_type(&RRCoord::distance)   ).
   add_property("heading"    , rr_get_v_const_type(&RRCoord::heading)    ).
   add_property("orientation", rr_get_v_const_type(&RRCoord::orientation)).
   add_property("var"        , make_getter(&RRCoord::var, return_value_policy<return_by_value>()));

