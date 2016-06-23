class_<UNSWSensorValues>("UNSWSensorValues")
   .add_property("joints"       , &UNSWSensorValues::joints )
   .add_property("sensors"      , &UNSWSensorValues::sensors)
   .add_property("sonar"        , &UNSWSensorValues::sonar  );

