// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Behaviors.proto

package messages;

public final class _File_Behaviors {
  private _File_Behaviors() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
  }
  static com.google.protobuf.Descriptors.Descriptor
    internal_static_messages_Behaviors_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_messages_Behaviors_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\017Behaviors.proto\022\010messages\"\276\002\n\tBehavior" +
      "s\0228\n\007GCState\030\001 \001(\0162\'.messages.Behaviors." +
      "GameControllerState\022\021\n\trobotName\030\002 \001(\t\022\014" +
      "\n\004team\030\003 \001(\005\022\016\n\006player\030\004 \001(\005\022\014\n\004role\030\005 \001" +
      "(\005\022\022\n\ngcstatestr\030\006 \001(\t\022\026\n\016headtrackerstr" +
      "\030\007 \001(\t\022\024\n\014navigatorstr\030\010 \001(\t\"v\n\023GameCont" +
      "rollerState\022\017\n\013GAMEINITIAL\020\001\022\r\n\tGAMEREAD" +
      "Y\020\002\022\013\n\007GAMESET\020\003\022\017\n\013GAMEPLAYING\020\004\022\017\n\013GAM" +
      "EPENALTY\020\005\022\020\n\014GAMEFINISHED\020\006B\023B\017_File_Be" +
      "haviorsP\001"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
      new com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner() {
        public com.google.protobuf.ExtensionRegistry assignDescriptors(
            com.google.protobuf.Descriptors.FileDescriptor root) {
          descriptor = root;
          internal_static_messages_Behaviors_descriptor =
            getDescriptor().getMessageTypes().get(0);
          internal_static_messages_Behaviors_fieldAccessorTable = new
            com.google.protobuf.GeneratedMessage.FieldAccessorTable(
              internal_static_messages_Behaviors_descriptor,
              new java.lang.String[] { "GCState", "RobotName", "Team", "Player", "Role", "Gcstatestr", "Headtrackerstr", "Navigatorstr", });
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