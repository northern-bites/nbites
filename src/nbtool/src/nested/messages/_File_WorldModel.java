// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: WorldModel.proto

package messages;

public final class _File_WorldModel {
  private _File_WorldModel() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
  }
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_WorldModel_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_WorldModel_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\020WorldModel.proto\022\010messages\"\275\003\n\nWorldMo" +
      "del\022\021\n\ttimestamp\030\001 \001(\003\022\014\n\004my_x\030\002 \001(\002\022\014\n\004" +
      "my_y\030\003 \001(\002\022\014\n\004my_h\030\004 \001(\002\022\021\n\tmy_uncert\030\005 " +
      "\001(\002\022\024\n\014walking_to_x\030\006 \001(\002\022\024\n\014walking_to_" +
      "y\030\007 \001(\002\022\017\n\007ball_on\030\010 \001(\010\022\020\n\010ball_age\030\t \001" +
      "(\005\022\021\n\tball_dist\030\n \001(\002\022\024\n\014ball_bearing\030\013 " +
      "\001(\002\022\022\n\nball_vel_x\030\014 \001(\002\022\022\n\nball_vel_y\030\r " +
      "\001(\002\022\023\n\013ball_uncert\030\016 \001(\002\022\014\n\004role\030\017 \001(\005\022\030" +
      "\n\020in_kicking_state\030\020 \001(\010\022\024\n\014kicking_to_x" +
      "\030\021 \001(\002\022\024\n\014kicking_to_y\030\022 \001(\002\022\016\n\006fallen\030\023",
      " \001(\010\022\016\n\006active\030\024 \001(\010\022\024\n\014claimed_ball\030\025 \001" +
      "(\010\022\021\n\tintention\030\026 \001(\005\022\r\n\005alive\030\027 \001(\010B\024B\020" +
      "_File_WorldModelP\001"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
      new com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner() {
        public com.google.protobuf.ExtensionRegistry assignDescriptors(
            com.google.protobuf.Descriptors.FileDescriptor root) {
          descriptor = root;
          internal_static_messages_WorldModel_descriptor =
            getDescriptor().getMessageTypes().get(0);
          internal_static_messages_WorldModel_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_WorldModel_descriptor,
              new java.lang.String[] { "Timestamp", "MyX", "MyY", "MyH", "MyUncert", "WalkingToX", "WalkingToY", "BallOn", "BallAge", "BallDist", "BallBearing", "BallVelX", "BallVelY", "BallUncert", "Role", "InKickingState", "KickingToX", "KickingToY", "Fallen", "Active", "ClaimedBall", "Intention", "Alive", });
          return null;
        }
      };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
        }, assigner);
  }

  // @@protoc_insertion_point(outer_class_scope)
}
