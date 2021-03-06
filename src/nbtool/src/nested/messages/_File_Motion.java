// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Motion.proto

package messages;

public final class _File_Motion {
  private _File_Motion() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
  }
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_WalkProvider_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_WalkProvider_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_WalkProvider_BHDebug_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_WalkProvider_BHDebug_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_ScriptedProvider_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_ScriptedProvider_fieldAccessorTable;
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_Motion_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_Motion_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\014Motion.proto\022\010messages\032\023RobotLocation." +
      "proto\"\331\003\n\014WalkProvider\022\016\n\006active\030\001 \001(\010\022\022" +
      "\n\nis_walking\030\002 \001(\010\022\020\n\010stopping\030\003 \001(\010\022\031\n\021" +
      "requested_to_stop\030\004 \001(\010\022\023\n\013is_standing\030\005" +
      " \001(\010\022\022\n\ncalibrated\030\006 \001(\010\022\024\n\014command_type" +
      "\030\010 \001(\005\022/\n\007bhdebug\030\007 \001(\0132\036.messages.WalkP" +
      "rovider.BHDebug\032\207\002\n\007BHDebug\022\023\n\013motion_ty" +
      "pe\030\001 \001(\r\022\023\n\013motion_name\030\002 \001(\t\022\034\n\024selecte" +
      "d_motion_type\030\003 \001(\r\022\034\n\024selected_motion_n" +
      "ame\030\004 \001(\t\022\'\n\006target\030\005 \001(\0132\027.messages.Rob",
      "otLocation\022\'\n\006speeds\030\006 \001(\0132\027.messages.Ro" +
      "botLocation\022\025\n\rselect_ratios\030\007 \003(\002\022\033\n\023gr" +
      "ound_contact_safe\030\010 \001(\010\022\020\n\010instable\030\t \001(" +
      "\010\"4\n\020ScriptedProvider\022\016\n\006active\030\001 \001(\010\022\020\n" +
      "\010stopping\030\002 \001(\010\"\375\001\n\006Motion\022\021\n\ttimestamp\030" +
      "\001 \001(\003\022\035\n\025current_body_provider\030\002 \001(\t\022\032\n\022" +
      "next_body_provider\030\003 \001(\t\022)\n\010odometry\030\004 \001" +
      "(\0132\027.messages.RobotLocation\022-\n\rwalk_prov" +
      "ider\030\005 \001(\0132\026.messages.WalkProvider\0225\n\021sc" +
      "ripted_provider\030\006 \001(\0132\032.messages.Scripte",
      "dProvider\022\024\n\014support_foot\030\007 \001(\010B\020B\014_File" +
      "_MotionP\001"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
      new com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner() {
        public com.google.protobuf.ExtensionRegistry assignDescriptors(
            com.google.protobuf.Descriptors.FileDescriptor root) {
          descriptor = root;
          internal_static_messages_WalkProvider_descriptor =
            getDescriptor().getMessageTypes().get(0);
          internal_static_messages_WalkProvider_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_WalkProvider_descriptor,
              new java.lang.String[] { "Active", "IsWalking", "Stopping", "RequestedToStop", "IsStanding", "Calibrated", "CommandType", "Bhdebug", });
          internal_static_messages_WalkProvider_BHDebug_descriptor =
            internal_static_messages_WalkProvider_descriptor.getNestedTypes().get(0);
          internal_static_messages_WalkProvider_BHDebug_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_WalkProvider_BHDebug_descriptor,
              new java.lang.String[] { "MotionType", "MotionName", "SelectedMotionType", "SelectedMotionName", "Target", "Speeds", "SelectRatios", "GroundContactSafe", "Instable", });
          internal_static_messages_ScriptedProvider_descriptor =
            getDescriptor().getMessageTypes().get(1);
          internal_static_messages_ScriptedProvider_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_ScriptedProvider_descriptor,
              new java.lang.String[] { "Active", "Stopping", });
          internal_static_messages_Motion_descriptor =
            getDescriptor().getMessageTypes().get(2);
          internal_static_messages_Motion_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_Motion_descriptor,
              new java.lang.String[] { "Timestamp", "CurrentBodyProvider", "NextBodyProvider", "Odometry", "WalkProvider", "ScriptedProvider", "SupportFoot", });
          return null;
        }
      };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
          messages._File_RobotLocation.getDescriptor(),
        }, assigner);
  }

  // @@protoc_insertion_point(outer_class_scope)
}
