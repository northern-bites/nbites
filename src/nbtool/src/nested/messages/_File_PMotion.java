// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PMotion.proto

package messages;

public final class _File_PMotion {
  private _File_PMotion() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
  }
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_MotionRequest_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_MotionRequest_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_MotionCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_MotionCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_ScriptedMove_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_ScriptedMove_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_BodyJointCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_BodyJointCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_DestinationWalk_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_DestinationWalk_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_OdometryWalk_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_OdometryWalk_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_WalkCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_WalkCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_MotionKick_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_MotionKick_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_Kick_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_Kick_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_HeadMotionCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_HeadMotionCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_PositionHeadCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_PositionHeadCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_ScriptedHeadCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_ScriptedHeadCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_HeadJointCommand_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_HeadJointCommand_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_JointAngles_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_JointAngles_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\rPMotion.proto\022\010messages\"\255\001\n\rMotionRequ" +
      "est\022\021\n\tstop_body\030\001 \001(\010\022\021\n\tstop_head\030\002 \001(" +
      "\010\022\026\n\016reset_odometry\030\003 \001(\010\022\030\n\020remove_stif" +
      "fness\030\004 \001(\010\022\030\n\020enable_stiffness\030\005 \001(\010\022\027\n" +
      "\017reset_providers\030\006 \001(\010\022\021\n\ttimestamp\030\007 \001(" +
      "\003\"\200\003\n\rMotionCommand\0221\n\004type\030\001 \001(\0162#.mess" +
      "ages.MotionCommand.CommandType\022&\n\006script" +
      "\030\002 \001(\0132\026.messages.ScriptedMove\022\'\n\004dest\030\003" +
      " \001(\0132\031.messages.DestinationWalk\022$\n\005speed" +
      "\030\004 \001(\0132\025.messages.WalkCommand\022-\n\rodometr",
      "y_dest\030\005 \001(\0132\026.messages.OdometryWalk\022\034\n\004" +
      "kick\030\006 \001(\0132\016.messages.Kick\022\021\n\ttimestamp\030" +
      "\007 \001(\003\"e\n\013CommandType\022\024\n\020DESTINATION_WALK" +
      "\020\000\022\020\n\014WALK_COMMAND\020\001\022\021\n\rSCRIPTED_MOVE\020\002\022" +
      "\021\n\rODOMETRY_WALK\020\003\022\010\n\004KICK\020\004\";\n\014Scripted" +
      "Move\022+\n\007command\030\001 \003(\0132\032.messages.BodyJoi" +
      "ntCommand\"\343\001\n\020BodyJointCommand\022\014\n\004time\030\001" +
      " \001(\002\022%\n\006angles\030\002 \001(\0132\025.messages.JointAng" +
      "les\022(\n\tstiffness\030\003 \001(\0132\025.messages.JointA" +
      "ngles\022C\n\rinterpolation\030\004 \001(\0162,.messages.",
      "BodyJointCommand.InterpolationType\"+\n\021In" +
      "terpolationType\022\n\n\006SMOOTH\020\000\022\n\n\006LINEAR\020\001\"" +
      "p\n\017DestinationWalk\022\r\n\005rel_x\030\001 \001(\002\022\r\n\005rel" +
      "_y\030\002 \001(\002\022\r\n\005rel_h\030\003 \001(\002\022\014\n\004gain\030\004 \001(\002\022\"\n" +
      "\004kick\030\005 \001(\0132\024.messages.MotionKick\"I\n\014Odo" +
      "metryWalk\022\r\n\005rel_x\030\001 \001(\002\022\r\n\005rel_y\030\002 \001(\002\022" +
      "\r\n\005rel_h\030\003 \001(\002\022\014\n\004gain\030\004 \001(\002\"F\n\013WalkComm" +
      "and\022\021\n\tx_percent\030\001 \001(\002\022\021\n\ty_percent\030\002 \001(" +
      "\002\022\021\n\th_percent\030\003 \001(\002\"<\n\nMotionKick\022\033\n\023pe" +
      "rform_motion_kick\030\001 \001(\010\022\021\n\tkick_type\030\002 \001",
      "(\005\"d\n\004Kick\022%\n\004type\030\002 \001(\0162\027.messages.Kick" +
      ".KickType\"5\n\010KickType\022\023\n\017kickForwardLeft" +
      "\020\000\022\024\n\020kickForwardRight\020\001\"\212\002\n\021HeadMotionC" +
      "ommand\0225\n\004type\030\001 \001(\0162\'.messages.HeadMoti" +
      "onCommand.CommandType\0222\n\013pos_command\030\002 \001" +
      "(\0132\035.messages.PositionHeadCommand\0227\n\020scr" +
      "ipted_command\030\003 \001(\0132\035.messages.ScriptedH" +
      "eadCommand\022\021\n\ttimestamp\030\004 \001(\003\">\n\013Command" +
      "Type\022\024\n\020POS_HEAD_COMMAND\020\000\022\031\n\025SCRIPTED_H" +
      "EAD_COMMAND\020\001\"s\n\023PositionHeadCommand\022\020\n\010",
      "head_yaw\030\001 \001(\002\022\022\n\nhead_pitch\030\002 \001(\002\022\031\n\rma" +
      "x_speed_yaw\030\003 \001(\002:\002-1\022\033\n\017max_speed_pitch" +
      "\030\004 \001(\002:\002-1\"B\n\023ScriptedHeadCommand\022+\n\007com" +
      "mand\030\001 \003(\0132\032.messages.HeadJointCommand\"\343" +
      "\001\n\020HeadJointCommand\022\014\n\004time\030\001 \001(\002\022%\n\006ang" +
      "les\030\002 \001(\0132\025.messages.JointAngles\022(\n\tstif" +
      "fness\030\003 \001(\0132\025.messages.JointAngles\022C\n\rin" +
      "terpolation\030\004 \001(\0162,.messages.HeadJointCo" +
      "mmand.InterpolationType\"+\n\021Interpolation" +
      "Type\022\n\n\006SMOOTH\020\000\022\n\n\006LINEAR\020\001\"\303\004\n\013JointAn",
      "gles\022\020\n\010head_yaw\030\001 \001(\002\022\022\n\nhead_pitch\030\002 \001" +
      "(\002\022\030\n\020l_shoulder_pitch\030\003 \001(\002\022\027\n\017l_should" +
      "er_roll\030\004 \001(\002\022\023\n\013l_elbow_yaw\030\005 \001(\002\022\024\n\014l_" +
      "elbow_roll\030\006 \001(\002\022\023\n\013l_wrist_yaw\030\007 \001(\002\022\016\n" +
      "\006l_hand\030\010 \001(\002\022\030\n\020r_shoulder_pitch\030\t \001(\002\022" +
      "\027\n\017r_shoulder_roll\030\n \001(\002\022\023\n\013r_elbow_yaw\030" +
      "\013 \001(\002\022\024\n\014r_elbow_roll\030\014 \001(\002\022\023\n\013r_wrist_y" +
      "aw\030\r \001(\002\022\016\n\006r_hand\030\016 \001(\002\022\027\n\017l_hip_yaw_pi" +
      "tch\030\017 \001(\002\022\027\n\017r_hip_yaw_pitch\030\020 \001(\002\022\022\n\nl_" +
      "hip_roll\030\021 \001(\002\022\023\n\013l_hip_pitch\030\022 \001(\002\022\024\n\014l",
      "_knee_pitch\030\023 \001(\002\022\025\n\rl_ankle_pitch\030\024 \001(\002" +
      "\022\024\n\014l_ankle_roll\030\025 \001(\002\022\022\n\nr_hip_roll\030\026 \001" +
      "(\002\022\023\n\013r_hip_pitch\030\027 \001(\002\022\024\n\014r_knee_pitch\030" +
      "\030 \001(\002\022\025\n\rr_ankle_pitch\030\031 \001(\002\022\024\n\014r_ankle_" +
      "roll\030\032 \001(\002B\021B\r_File_PMotionP\001"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
      new com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner() {
        public com.google.protobuf.ExtensionRegistry assignDescriptors(
            com.google.protobuf.Descriptors.FileDescriptor root) {
          descriptor = root;
          internal_static_messages_MotionRequest_descriptor =
            getDescriptor().getMessageTypes().get(0);
          internal_static_messages_MotionRequest_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_MotionRequest_descriptor,
              new java.lang.String[] { "StopBody", "StopHead", "ResetOdometry", "RemoveStiffness", "EnableStiffness", "ResetProviders", "Timestamp", });
          internal_static_messages_MotionCommand_descriptor =
            getDescriptor().getMessageTypes().get(1);
          internal_static_messages_MotionCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_MotionCommand_descriptor,
              new java.lang.String[] { "Type", "Script", "Dest", "Speed", "OdometryDest", "Kick", "Timestamp", });
          internal_static_messages_ScriptedMove_descriptor =
            getDescriptor().getMessageTypes().get(2);
          internal_static_messages_ScriptedMove_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_ScriptedMove_descriptor,
              new java.lang.String[] { "Command", });
          internal_static_messages_BodyJointCommand_descriptor =
            getDescriptor().getMessageTypes().get(3);
          internal_static_messages_BodyJointCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_BodyJointCommand_descriptor,
              new java.lang.String[] { "Time", "Angles", "Stiffness", "Interpolation", });
          internal_static_messages_DestinationWalk_descriptor =
            getDescriptor().getMessageTypes().get(4);
          internal_static_messages_DestinationWalk_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_DestinationWalk_descriptor,
              new java.lang.String[] { "RelX", "RelY", "RelH", "Gain", "Kick", });
          internal_static_messages_OdometryWalk_descriptor =
            getDescriptor().getMessageTypes().get(5);
          internal_static_messages_OdometryWalk_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_OdometryWalk_descriptor,
              new java.lang.String[] { "RelX", "RelY", "RelH", "Gain", });
          internal_static_messages_WalkCommand_descriptor =
            getDescriptor().getMessageTypes().get(6);
          internal_static_messages_WalkCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_WalkCommand_descriptor,
              new java.lang.String[] { "XPercent", "YPercent", "HPercent", });
          internal_static_messages_MotionKick_descriptor =
            getDescriptor().getMessageTypes().get(7);
          internal_static_messages_MotionKick_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_MotionKick_descriptor,
              new java.lang.String[] { "PerformMotionKick", "KickType", });
          internal_static_messages_Kick_descriptor =
            getDescriptor().getMessageTypes().get(8);
          internal_static_messages_Kick_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_Kick_descriptor,
              new java.lang.String[] { "Type", });
          internal_static_messages_HeadMotionCommand_descriptor =
            getDescriptor().getMessageTypes().get(9);
          internal_static_messages_HeadMotionCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_HeadMotionCommand_descriptor,
              new java.lang.String[] { "Type", "PosCommand", "ScriptedCommand", "Timestamp", });
          internal_static_messages_PositionHeadCommand_descriptor =
            getDescriptor().getMessageTypes().get(10);
          internal_static_messages_PositionHeadCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_PositionHeadCommand_descriptor,
              new java.lang.String[] { "HeadYaw", "HeadPitch", "MaxSpeedYaw", "MaxSpeedPitch", });
          internal_static_messages_ScriptedHeadCommand_descriptor =
            getDescriptor().getMessageTypes().get(11);
          internal_static_messages_ScriptedHeadCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_ScriptedHeadCommand_descriptor,
              new java.lang.String[] { "Command", });
          internal_static_messages_HeadJointCommand_descriptor =
            getDescriptor().getMessageTypes().get(12);
          internal_static_messages_HeadJointCommand_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_HeadJointCommand_descriptor,
              new java.lang.String[] { "Time", "Angles", "Stiffness", "Interpolation", });
          internal_static_messages_JointAngles_descriptor =
            getDescriptor().getMessageTypes().get(13);
          internal_static_messages_JointAngles_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_JointAngles_descriptor,
              new java.lang.String[] { "HeadYaw", "HeadPitch", "LShoulderPitch", "LShoulderRoll", "LElbowYaw", "LElbowRoll", "LWristYaw", "LHand", "RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RElbowRoll", "RWristYaw", "RHand", "LHipYawPitch", "RHipYawPitch", "LHipRoll", "LHipPitch", "LKneePitch", "LAnklePitch", "LAnkleRoll", "RHipRoll", "RHipPitch", "RKneePitch", "RAnklePitch", "RAnkleRoll", });
          return null;
        }
      };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
        }, assigner);
    internal_static_messages_MotionRequest_descriptor =
      getDescriptor().getMessageTypes().get(0);
    internal_static_messages_MotionRequest_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_MotionRequest_descriptor,
        new java.lang.String[] { "StopBody", "StopHead", "ResetOdometry", "RemoveStiffness", "EnableStiffness", "ResetProviders", "Timestamp", });
    internal_static_messages_MotionCommand_descriptor =
      getDescriptor().getMessageTypes().get(1);
    internal_static_messages_MotionCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_MotionCommand_descriptor,
        new java.lang.String[] { "Type", "Script", "Dest", "Speed", "OdometryDest", "Kick", "Timestamp", });
    internal_static_messages_ScriptedMove_descriptor =
      getDescriptor().getMessageTypes().get(2);
    internal_static_messages_ScriptedMove_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_ScriptedMove_descriptor,
        new java.lang.String[] { "Command", });
    internal_static_messages_BodyJointCommand_descriptor =
      getDescriptor().getMessageTypes().get(3);
    internal_static_messages_BodyJointCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_BodyJointCommand_descriptor,
        new java.lang.String[] { "Time", "Angles", "Stiffness", "Interpolation", });
    internal_static_messages_DestinationWalk_descriptor =
      getDescriptor().getMessageTypes().get(4);
    internal_static_messages_DestinationWalk_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_DestinationWalk_descriptor,
        new java.lang.String[] { "RelX", "RelY", "RelH", "Gain", "Kick", });
    internal_static_messages_OdometryWalk_descriptor =
      getDescriptor().getMessageTypes().get(5);
    internal_static_messages_OdometryWalk_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_OdometryWalk_descriptor,
        new java.lang.String[] { "RelX", "RelY", "RelH", "Gain", });
    internal_static_messages_WalkCommand_descriptor =
      getDescriptor().getMessageTypes().get(6);
    internal_static_messages_WalkCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_WalkCommand_descriptor,
        new java.lang.String[] { "XPercent", "YPercent", "HPercent", });
    internal_static_messages_MotionKick_descriptor =
      getDescriptor().getMessageTypes().get(7);
    internal_static_messages_MotionKick_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_MotionKick_descriptor,
        new java.lang.String[] { "PerformMotionKick", "KickType", });
    internal_static_messages_Kick_descriptor =
      getDescriptor().getMessageTypes().get(8);
    internal_static_messages_Kick_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_Kick_descriptor,
        new java.lang.String[] { "Type", });
    internal_static_messages_HeadMotionCommand_descriptor =
      getDescriptor().getMessageTypes().get(9);
    internal_static_messages_HeadMotionCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_HeadMotionCommand_descriptor,
        new java.lang.String[] { "Type", "PosCommand", "ScriptedCommand", "Timestamp", });
    internal_static_messages_PositionHeadCommand_descriptor =
      getDescriptor().getMessageTypes().get(10);
    internal_static_messages_PositionHeadCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_PositionHeadCommand_descriptor,
        new java.lang.String[] { "HeadYaw", "HeadPitch", "MaxSpeedYaw", "MaxSpeedPitch", });
    internal_static_messages_ScriptedHeadCommand_descriptor =
      getDescriptor().getMessageTypes().get(11);
    internal_static_messages_ScriptedHeadCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_ScriptedHeadCommand_descriptor,
        new java.lang.String[] { "Command", });
    internal_static_messages_HeadJointCommand_descriptor =
      getDescriptor().getMessageTypes().get(12);
    internal_static_messages_HeadJointCommand_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_HeadJointCommand_descriptor,
        new java.lang.String[] { "Time", "Angles", "Stiffness", "Interpolation", });
    internal_static_messages_JointAngles_descriptor =
      getDescriptor().getMessageTypes().get(13);
    internal_static_messages_JointAngles_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_messages_JointAngles_descriptor,
        new java.lang.String[] { "HeadYaw", "HeadPitch", "LShoulderPitch", "LShoulderRoll", "LElbowYaw", "LElbowRoll", "LWristYaw", "LHand", "RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RElbowRoll", "RWristYaw", "RHand", "LHipYawPitch", "RHipYawPitch", "LHipRoll", "LHipPitch", "LKneePitch", "LAnklePitch", "LAnkleRoll", "RHipRoll", "RHipPitch", "RKneePitch", "RAnklePitch", "RAnkleRoll", });
  }

  // @@protoc_insertion_point(outer_class_scope)
}
