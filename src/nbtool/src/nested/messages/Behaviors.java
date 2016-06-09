// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Behaviors.proto

package messages;

/**
 * Protobuf type {@code messages.Behaviors}
 */
public  final class Behaviors extends
    com.google.protobuf.GeneratedMessage
    implements BehaviorsOrBuilder {
  // Use Behaviors.newBuilder() to construct.
  private Behaviors(com.google.protobuf.GeneratedMessage.Builder<?> builder) {
    super(builder);
    this.unknownFields = builder.getUnknownFields();
  }
  private Behaviors(boolean noInit) { this.unknownFields = com.google.protobuf.UnknownFieldSet.getDefaultInstance(); }

  private static final Behaviors defaultInstance;
  public static Behaviors getDefaultInstance() {
    return defaultInstance;
  }

  public Behaviors getDefaultInstanceForType() {
    return defaultInstance;
  }

  private final com.google.protobuf.UnknownFieldSet unknownFields;
  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
      getUnknownFields() {
    return this.unknownFields;
  }
  private Behaviors(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    initFields();
    int mutable_bitField0_ = 0;
    com.google.protobuf.UnknownFieldSet.Builder unknownFields =
        com.google.protobuf.UnknownFieldSet.newBuilder();
    try {
      boolean done = false;
      while (!done) {
        int tag = input.readTag();
        switch (tag) {
          case 0:
            done = true;
            break;
          default: {
            if (!parseUnknownField(input, unknownFields,
                                   extensionRegistry, tag)) {
              done = true;
            }
            break;
          }
          case 8: {
            int rawValue = input.readEnum();
            messages.Behaviors.GameControllerState value = messages.Behaviors.GameControllerState.valueOf(rawValue);
            if (value == null) {
              unknownFields.mergeVarintField(1, rawValue);
            } else {
              bitField0_ |= 0x00000001;
              gCState_ = value;
            }
            break;
          }
          case 16: {
            bitField0_ |= 0x00000002;
            on_ = input.readBool();
            break;
          }
        }
      }
    } catch (com.google.protobuf.InvalidProtocolBufferException e) {
      throw e.setUnfinishedMessage(this);
    } catch (java.io.IOException e) {
      throw new com.google.protobuf.InvalidProtocolBufferException(
          e.getMessage()).setUnfinishedMessage(this);
    } finally {
      this.unknownFields = unknownFields.build();
      makeExtensionsImmutable();
    }
  }
  public static final com.google.protobuf.Descriptors.Descriptor
      getDescriptor() {
    return messages._File_Behaviors.internal_static_messages_Behaviors_descriptor;
  }

  protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return messages._File_Behaviors.internal_static_messages_Behaviors_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            messages.Behaviors.class, messages.Behaviors.Builder.class);
  }

  public static com.google.protobuf.Parser<Behaviors> PARSER =
      new com.google.protobuf.AbstractParser<Behaviors>() {
    public Behaviors parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new Behaviors(input, extensionRegistry);
    }
  };

  @java.lang.Override
  public com.google.protobuf.Parser<Behaviors> getParserForType() {
    return PARSER;
  }

  /**
   * Protobuf enum {@code messages.Behaviors.GameControllerState}
   */
  public enum GameControllerState
      implements com.google.protobuf.ProtocolMessageEnum {
    /**
     * <code>GAMEINITIAL = 1;</code>
     */
    GAMEINITIAL(0, 1),
    /**
     * <code>GAMEREADY = 2;</code>
     */
    GAMEREADY(1, 2),
    /**
     * <code>GAMESET = 3;</code>
     */
    GAMESET(2, 3),
    /**
     * <code>GAMEPLAYING = 4;</code>
     */
    GAMEPLAYING(3, 4),
    /**
     * <code>GAMEFINISHED = 5;</code>
     */
    GAMEFINISHED(4, 5),
    /**
     * <code>GAMEPENALIZED = 6;</code>
     */
    GAMEPENALIZED(5, 6),
    ;

    /**
     * <code>GAMEINITIAL = 1;</code>
     */
    public static final int GAMEINITIAL_VALUE = 1;
    /**
     * <code>GAMEREADY = 2;</code>
     */
    public static final int GAMEREADY_VALUE = 2;
    /**
     * <code>GAMESET = 3;</code>
     */
    public static final int GAMESET_VALUE = 3;
    /**
     * <code>GAMEPLAYING = 4;</code>
     */
    public static final int GAMEPLAYING_VALUE = 4;
    /**
     * <code>GAMEFINISHED = 5;</code>
     */
    public static final int GAMEFINISHED_VALUE = 5;
    /**
     * <code>GAMEPENALIZED = 6;</code>
     */
    public static final int GAMEPENALIZED_VALUE = 6;


    public final int getNumber() { return value; }

    public static GameControllerState valueOf(int value) {
      switch (value) {
        case 1: return GAMEINITIAL;
        case 2: return GAMEREADY;
        case 3: return GAMESET;
        case 4: return GAMEPLAYING;
        case 5: return GAMEFINISHED;
        case 6: return GAMEPENALIZED;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<GameControllerState>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static com.google.protobuf.Internal.EnumLiteMap<GameControllerState>
        internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<GameControllerState>() {
            public GameControllerState findValueByNumber(int number) {
              return GameControllerState.valueOf(number);
            }
          };

    public final com.google.protobuf.Descriptors.EnumValueDescriptor
        getValueDescriptor() {
      return getDescriptor().getValues().get(index);
    }
    public final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptorForType() {
      return getDescriptor();
    }
    public static final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptor() {
      return messages.Behaviors.getDescriptor().getEnumTypes().get(0);
    }

    private static final GameControllerState[] VALUES = values();

    public static GameControllerState valueOf(
        com.google.protobuf.Descriptors.EnumValueDescriptor desc) {
      if (desc.getType() != getDescriptor()) {
        throw new java.lang.IllegalArgumentException(
          "EnumValueDescriptor is not for this type.");
      }
      return VALUES[desc.getIndex()];
    }

    private final int index;
    private final int value;

    private GameControllerState(int index, int value) {
      this.index = index;
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:messages.Behaviors.GameControllerState)
  }

  private int bitField0_;
  // optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];
  public static final int GCSTATE_FIELD_NUMBER = 1;
  private messages.Behaviors.GameControllerState gCState_;
  /**
   * <code>optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];</code>
   */
  public boolean hasGCState() {
    return ((bitField0_ & 0x00000001) == 0x00000001);
  }
  /**
   * <code>optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];</code>
   */
  public messages.Behaviors.GameControllerState getGCState() {
    return gCState_;
  }

  // optional bool on = 2;
  public static final int ON_FIELD_NUMBER = 2;
  private boolean on_;
  /**
   * <code>optional bool on = 2;</code>
   */
  public boolean hasOn() {
    return ((bitField0_ & 0x00000002) == 0x00000002);
  }
  /**
   * <code>optional bool on = 2;</code>
   */
  public boolean getOn() {
    return on_;
  }

  private void initFields() {
    gCState_ = messages.Behaviors.GameControllerState.GAMEINITIAL;
    on_ = false;
  }
  private byte memoizedIsInitialized = -1;
  public final boolean isInitialized() {
    byte isInitialized = memoizedIsInitialized;
    if (isInitialized != -1) return isInitialized == 1;

    memoizedIsInitialized = 1;
    return true;
  }

  public void writeTo(com.google.protobuf.CodedOutputStream output)
                      throws java.io.IOException {
    getSerializedSize();
    if (((bitField0_ & 0x00000001) == 0x00000001)) {
      output.writeEnum(1, gCState_.getNumber());
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      output.writeBool(2, on_);
    }
    getUnknownFields().writeTo(output);
  }

  private int memoizedSerializedSize = -1;
  public int getSerializedSize() {
    int size = memoizedSerializedSize;
    if (size != -1) return size;

    size = 0;
    if (((bitField0_ & 0x00000001) == 0x00000001)) {
      size += com.google.protobuf.CodedOutputStream
        .computeEnumSize(1, gCState_.getNumber());
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      size += com.google.protobuf.CodedOutputStream
        .computeBoolSize(2, on_);
    }
    size += getUnknownFields().getSerializedSize();
    memoizedSerializedSize = size;
    return size;
  }

  private static final long serialVersionUID = 0L;
  @java.lang.Override
  protected java.lang.Object writeReplace()
      throws java.io.ObjectStreamException {
    return super.writeReplace();
  }

  public static messages.Behaviors parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.Behaviors parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.Behaviors parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.Behaviors parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.Behaviors parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.Behaviors parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }
  public static messages.Behaviors parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input);
  }
  public static messages.Behaviors parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input, extensionRegistry);
  }
  public static messages.Behaviors parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.Behaviors parseFrom(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }

  public static Builder newBuilder() { return Builder.create(); }
  public Builder newBuilderForType() { return newBuilder(); }
  public static Builder newBuilder(messages.Behaviors prototype) {
    return newBuilder().mergeFrom(prototype);
  }
  public Builder toBuilder() { return newBuilder(this); }

  @java.lang.Override
  protected Builder newBuilderForType(
      com.google.protobuf.GeneratedMessage.BuilderParent parent) {
    Builder builder = new Builder(parent);
    return builder;
  }
  /**
   * Protobuf type {@code messages.Behaviors}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessage.Builder<Builder>
     implements messages.BehaviorsOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return messages._File_Behaviors.internal_static_messages_Behaviors_descriptor;
    }

    protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return messages._File_Behaviors.internal_static_messages_Behaviors_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              messages.Behaviors.class, messages.Behaviors.Builder.class);
    }

    // Construct using messages.Behaviors.newBuilder()
    private Builder() {
      maybeForceBuilderInitialization();
    }

    private Builder(
        com.google.protobuf.GeneratedMessage.BuilderParent parent) {
      super(parent);
      maybeForceBuilderInitialization();
    }
    private void maybeForceBuilderInitialization() {
      if (com.google.protobuf.GeneratedMessage.alwaysUseFieldBuilders) {
      }
    }
    private static Builder create() {
      return new Builder();
    }

    public Builder clear() {
      super.clear();
      gCState_ = messages.Behaviors.GameControllerState.GAMEINITIAL;
      bitField0_ = (bitField0_ & ~0x00000001);
      on_ = false;
      bitField0_ = (bitField0_ & ~0x00000002);
      return this;
    }

    public Builder clone() {
      return create().mergeFrom(buildPartial());
    }

    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return messages._File_Behaviors.internal_static_messages_Behaviors_descriptor;
    }

    public messages.Behaviors getDefaultInstanceForType() {
      return messages.Behaviors.getDefaultInstance();
    }

    public messages.Behaviors build() {
      messages.Behaviors result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    public messages.Behaviors buildPartial() {
      messages.Behaviors result = new messages.Behaviors(this);
      int from_bitField0_ = bitField0_;
      int to_bitField0_ = 0;
      if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
        to_bitField0_ |= 0x00000001;
      }
      result.gCState_ = gCState_;
      if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
        to_bitField0_ |= 0x00000002;
      }
      result.on_ = on_;
      result.bitField0_ = to_bitField0_;
      onBuilt();
      return result;
    }

    public Builder mergeFrom(com.google.protobuf.Message other) {
      if (other instanceof messages.Behaviors) {
        return mergeFrom((messages.Behaviors)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(messages.Behaviors other) {
      if (other == messages.Behaviors.getDefaultInstance()) return this;
      if (other.hasGCState()) {
        setGCState(other.getGCState());
      }
      if (other.hasOn()) {
        setOn(other.getOn());
      }
      this.mergeUnknownFields(other.getUnknownFields());
      return this;
    }

    public final boolean isInitialized() {
      return true;
    }

    public Builder mergeFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      messages.Behaviors parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (messages.Behaviors) e.getUnfinishedMessage();
        throw e;
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }
    private int bitField0_;

    // optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];
    private messages.Behaviors.GameControllerState gCState_ = messages.Behaviors.GameControllerState.GAMEINITIAL;
    /**
     * <code>optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];</code>
     */
    public boolean hasGCState() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];</code>
     */
    public messages.Behaviors.GameControllerState getGCState() {
      return gCState_;
    }
    /**
     * <code>optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];</code>
     */
    public Builder setGCState(messages.Behaviors.GameControllerState value) {
      if (value == null) {
        throw new NullPointerException();
      }
      bitField0_ |= 0x00000001;
      gCState_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional .messages.Behaviors.GameControllerState GCState = 1 [default = GAMEINITIAL];</code>
     */
    public Builder clearGCState() {
      bitField0_ = (bitField0_ & ~0x00000001);
      gCState_ = messages.Behaviors.GameControllerState.GAMEINITIAL;
      onChanged();
      return this;
    }

    // optional bool on = 2;
    private boolean on_ ;
    /**
     * <code>optional bool on = 2;</code>
     */
    public boolean hasOn() {
      return ((bitField0_ & 0x00000002) == 0x00000002);
    }
    /**
     * <code>optional bool on = 2;</code>
     */
    public boolean getOn() {
      return on_;
    }
    /**
     * <code>optional bool on = 2;</code>
     */
    public Builder setOn(boolean value) {
      bitField0_ |= 0x00000002;
      on_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool on = 2;</code>
     */
    public Builder clearOn() {
      bitField0_ = (bitField0_ & ~0x00000002);
      on_ = false;
      onChanged();
      return this;
    }

    // @@protoc_insertion_point(builder_scope:messages.Behaviors)
  }

  static {
    defaultInstance = new Behaviors(true);
    defaultInstance.initFields();
  }

  // @@protoc_insertion_point(class_scope:messages.Behaviors)
}

