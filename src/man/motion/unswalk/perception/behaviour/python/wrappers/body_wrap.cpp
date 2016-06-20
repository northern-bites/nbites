enum_<Joints::JointCodesEnum>("Joints")
   .value("HeadYaw"             , Joints::HeadYaw         )
   .value("HeadPitch"           , Joints::HeadPitch       )
   .value("LShoulderPitch"      , Joints::LShoulderPitch  )
   .value("LShoulderRoll"       , Joints::LShoulderRoll   )
   .value("LElbowYaw"           , Joints::LElbowYaw       )
   .value("LElbowRoll"          , Joints::LElbowRoll      )
   .value("LWristYaw"           , Joints::LWristYaw       )
   .value("LHand"               , Joints::LHand           )
   .value("LHipYawPitch"        , Joints::LHipYawPitch    )
   .value("LHipRoll"            , Joints::LHipRoll        )
   .value("LHipPitch"           , Joints::LHipPitch       )
   .value("LKneePitch"          , Joints::LKneePitch      )
   .value("LAnklePitch"         , Joints::LAnklePitch     )
   .value("LAnkleRoll"          , Joints::LAnkleRoll      )
   .value("RHipRoll"            , Joints::RHipRoll        )
   .value("RHipPitch"           , Joints::RHipPitch       )
   .value("RKneePitch"          , Joints::RKneePitch      )
   .value("RAnklePitch"         , Joints::RAnklePitch     )
   .value("RAnkleRoll"          , Joints::RAnkleRoll      )
   .value("RShoulderPitch"      , Joints::RShoulderPitch  )
   .value("RShoulderRoll"       , Joints::RShoulderRoll   )
   .value("RElbowYaw"           , Joints::RElbowYaw       )
   .value("RElbowRoll"          , Joints::RElbowRoll      )
   .value("RWristYaw"           , Joints::RWristYaw       )
   .value("RHand"               , Joints::RHand           )
   .value("NUMBER_OF_JOINTS"    , Joints::NUMBER_OF_JOINTS);

   scope().attr("HeadYaw_Min"       ) = Joints::Radians::HeadYaw_Min;
   scope().attr("HeadYaw_Max"       ) = Joints::Radians::HeadYaw_Max;
   scope().attr("HeadPitch_Min"     ) = Joints::Radians::HeadPitch_Min;
   scope().attr("HeadPitch_Max"     ) = Joints::Radians::HeadPitch_Max;
   scope().attr("LShoulderPitch_Min") = Joints::Radians::LShoulderPitch_Min;
   scope().attr("LShoulderPitch_Max") = Joints::Radians::LShoulderPitch_Max;
   scope().attr("LShoulderRoll_Max" ) = Joints::Radians::LShoulderRoll_Max;
   scope().attr("LShoulderRoll_Min" ) = Joints::Radians::LShoulderRoll_Min;
   scope().attr("LElbowYaw_Min"     ) = Joints::Radians::LElbowYaw_Min;
   scope().attr("LElbowYaw_Max"     ) = Joints::Radians::LElbowYaw_Max;
   scope().attr("LElbowRoll_Min"    ) = Joints::Radians::LElbowRoll_Min;
   scope().attr("LElbowRoll_Max"    ) = Joints::Radians::LElbowRoll_Max;
   scope().attr("LWristYaw_Min"     ) = Joints::Radians::LWristYaw_Min;
   scope().attr("LWristYaw_Max"     ) = Joints::Radians::LWristYaw_Max;
   scope().attr("LHand_Min"         ) = Joints::Radians::LHand_Min;
   scope().attr("LHand_Max"         ) = Joints::Radians::LHand_Max;
   scope().attr("LHipYawPitch_Min"  ) = Joints::Radians::LHipYawPitch_Min;
   scope().attr("LHipYawPitch_Max"  ) = Joints::Radians::LHipYawPitch_Max;
   scope().attr("LHipPitch_Min"     ) = Joints::Radians::LHipPitch_Min;
   scope().attr("LHipPitch_Max"     ) = Joints::Radians::LHipPitch_Max;
   scope().attr("LHipRoll_Min"      ) = Joints::Radians::LHipRoll_Min;
   scope().attr("LHipRoll_Max"      ) = Joints::Radians::LHipRoll_Max;
   scope().attr("LKneePitch_Min"    ) = Joints::Radians::LKneePitch_Min;
   scope().attr("LKneePitch_Max"    ) = Joints::Radians::LKneePitch_Max;
   scope().attr("LAnklePitch_Min"   ) = Joints::Radians::LAnklePitch_Min;
   scope().attr("LAnklePitch_Max"   ) = Joints::Radians::LAnklePitch_Max;
   scope().attr("LAnkleRoll_Min"    ) = Joints::Radians::LAnkleRoll_Min;
   scope().attr("LAnkleRoll_Max"    ) = Joints::Radians::LAnkleRoll_Max;
   scope().attr("RHipPitch_Min"     ) = Joints::Radians::RHipPitch_Min;
   scope().attr("RHipPitch_Max"     ) = Joints::Radians::RHipPitch_Max;
   scope().attr("RHipRoll_Min"      ) = Joints::Radians::RHipRoll_Min;
   scope().attr("RHipRoll_Max"      ) = Joints::Radians::RHipRoll_Max;
   scope().attr("RKneePitch_Min"    ) = Joints::Radians::RKneePitch_Min;
   scope().attr("RKneePitch_Max"    ) = Joints::Radians::RKneePitch_Max;
   scope().attr("RAnklePitch_Min"   ) = Joints::Radians::RAnklePitch_Min;
   scope().attr("RAnklePitch_Max"   ) = Joints::Radians::RAnklePitch_Max;
   scope().attr("RAnkleRoll_Min"    ) = Joints::Radians::RAnkleRoll_Min;
   scope().attr("RAnkleRoll_Max"    ) = Joints::Radians::RAnkleRoll_Max;
   scope().attr("RShoulderPitch_Min") = Joints::Radians::RShoulderPitch_Min;
   scope().attr("RShoulderPitch_Max") = Joints::Radians::RShoulderPitch_Max;
   scope().attr("RShoulderRoll_Min" ) = Joints::Radians::RShoulderRoll_Min;
   scope().attr("RShoulderRoll_Max" ) = Joints::Radians::RShoulderRoll_Max;
   scope().attr("RElbowYaw_Min"     ) = Joints::Radians::RElbowYaw_Min;
   scope().attr("RElbowYaw_Max"     ) = Joints::Radians::RElbowYaw_Max;
   scope().attr("RElbowRoll_Min"    ) = Joints::Radians::RElbowRoll_Min;
   scope().attr("RElbowRoll_Max"    ) = Joints::Radians::RElbowRoll_Max;
   scope().attr("RWristYaw_Min"     ) = Joints::Radians::RWristYaw_Min;
   scope().attr("RWristYaw_Max"     ) = Joints::Radians::RWristYaw_Max;
   scope().attr("RHand_Min"         ) = Joints::Radians::RHand_Min;
   scope().attr("RHand_Max"         ) = Joints::Radians::RHand_Max;

enum_<Sensors::SensorCodesEnum>("Sensors")
   .value("InertialSensor_AccX"           , Sensors::InertialSensor_AccX         )
   .value("InertialSensor_AccY"           , Sensors::InertialSensor_AccY         )
   .value("InertialSensor_AccZ"           , Sensors::InertialSensor_AccZ         )
   .value("InertialSensor_GyrX"           , Sensors::InertialSensor_GyrX         )
   .value("InertialSensor_GyrY"           , Sensors::InertialSensor_GyrY         ) 
   .value("InertialSensor_GyrRef"         , Sensors::InertialSensor_GyrRef       )
   .value("InertialSensor_AngleX"         , Sensors::InertialSensor_AngleX       )
   .value("InertialSensor_AngleY"         , Sensors::InertialSensor_AngleY       )
   .value("LFoot_FSR_FrontLeft"           , Sensors::LFoot_FSR_FrontLeft         )
   .value("LFoot_FSR_FrontRight"          , Sensors::LFoot_FSR_FrontRight        )
   .value("LFoot_FSR_RearLeft"            , Sensors::LFoot_FSR_RearLeft          )
   .value("LFoot_FSR_RearRight"           , Sensors::LFoot_FSR_RearRight         )
   .value("LFoot_FSR_CenterOfPressure_X"  , Sensors::LFoot_FSR_CenterOfPressure_X)
   .value("LFoot_FSR_CenterOfPressure_Y"  , Sensors::LFoot_FSR_CenterOfPressure_Y)
   .value("RFoot_FSR_FrontLeft"           , Sensors::RFoot_FSR_FrontLeft         )
   .value("RFoot_FSR_FrontRight"          , Sensors::RFoot_FSR_FrontRight        )
   .value("RFoot_FSR_RearLeft"            , Sensors::RFoot_FSR_RearLeft          )
   .value("RFoot_FSR_RearRight"           , Sensors::RFoot_FSR_RearRight         )
   .value("RFoot_FSR_CenterOfPressure_X"  , Sensors::RFoot_FSR_CenterOfPressure_X)
   .value("RFoot_FSR_CenterOfPressure_Y"  , Sensors::RFoot_FSR_CenterOfPressure_Y)
   .value("LFoot_Bumper_Left"             , Sensors::LFoot_Bumper_Left           )
   .value("LFoot_Bumper_Right"            , Sensors::LFoot_Bumper_Right          )
   .value("RFoot_Bumper_Left"             , Sensors::RFoot_Bumper_Left           )
   .value("RFoot_Bumper_Right"            , Sensors::RFoot_Bumper_Right          )
   .value("ChestBoard_Button"             , Sensors::ChestBoard_Button           )
   .value("Battery_Charge"                , Sensors::Battery_Charge              )
   .value("Battery_Current"               , Sensors::Battery_Current             )
   .value("US"                            , Sensors::US                          )
   .value("NUMBER_OF_SENSORS"             , Sensors::NUMBER_OF_SENSORS           );
