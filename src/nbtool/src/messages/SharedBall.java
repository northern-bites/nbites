// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BallModel.proto

package messages;

/**
 * Protobuf type {@code messages.SharedBall}
 */
public final class SharedBall extends
    com.google.protobuf.GeneratedMessage implements
    // @@protoc_insertion_point(message_implements:messages.SharedBall)
    SharedBallOrBuilder {
  // Use SharedBall.newBuilder() to construct.
  private SharedBall(com.google.protobuf.GeneratedMessage.Builder<?> builder) {
    super(builder);
    this.unknownFields = builder.getUnknownFields();
  }
  private SharedBall(boolean noInit) { this.unknownFields = com.google.protobuf.UnknownFieldSet.getDefaultInstance(); }

  private static final SharedBall defaultInstance;
  public static SharedBall getDefaultInstance() {
    return defaultInstance;
  }

  public SharedBall getDefaultInstanceForType() {
    return defaultInstance;
  }

  private final com.google.protobuf.UnknownFieldSet unknownFields;
  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
      getUnknownFields() {
    return this.unknownFields;
  }
  private SharedBall(
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
          case 13: {
            bitField0_ |= 0x00000001;
            x_ = input.readFloat();
            break;
          }
          case 21: {
            bitField0_ |= 0x00000002;
            y_ = input.readFloat();
            break;
          }
          case 24: {
            bitField0_ |= 0x00000004;
            ballOn_ = input.readBool();
            break;
          }
          case 32: {
            bitField0_ |= 0x00000008;
            reliability_ = input.readInt32();
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
    return messages._File_BallModel.internal_static_messages_SharedBall_descriptor;
  }

  protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return messages._File_BallModel.internal_static_messages_SharedBall_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            messages.SharedBall.class, messages.SharedBall.Builder.class);
  }

  public static com.google.protobuf.Parser<SharedBall> PARSER =
      new com.google.protobuf.AbstractParser<SharedBall>() {
    public SharedBall parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new SharedBall(input, extensionRegistry);
    }
  };

  @java.lang.Override
  public com.google.protobuf.Parser<SharedBall> getParserForType() {
    return PARSER;
  }

  private int bitField0_;
  public static final int X_FIELD_NUMBER = 1;
  private float x_;
  /**
   * <code>optional float x = 1;</code>
   */
  public boolean hasX() {
    return ((bitField0_ & 0x00000001) == 0x00000001);
  }
  /**
   * <code>optional float x = 1;</code>
   */
  public float getX() {
    return x_;
  }

  public static final int Y_FIELD_NUMBER = 2;
  private float y_;
  /**
   * <code>optional float y = 2;</code>
   */
  public boolean hasY() {
    return ((bitField0_ & 0x00000002) == 0x00000002);
  }
  /**
   * <code>optional float y = 2;</code>
   */
  public float getY() {
    return y_;
  }

  public static final int BALL_ON_FIELD_NUMBER = 3;
  private boolean ballOn_;
  /**
   * <code>optional bool ball_on = 3;</code>
   */
  public boolean hasBallOn() {
    return ((bitField0_ & 0x00000004) == 0x00000004);
  }
  /**
   * <code>optional bool ball_on = 3;</code>
   */
  public boolean getBallOn() {
    return ballOn_;
  }

  public static final int RELIABILITY_FIELD_NUMBER = 4;
  private int reliability_;
  /**
   * <code>optional int32 reliability = 4;</code>
   */
  public boolean hasReliability() {
    return ((bitField0_ & 0x00000008) == 0x00000008);
  }
  /**
   * <code>optional int32 reliability = 4;</code>
   */
  public int getReliability() {
    return reliability_;
  }

  private void initFields() {
    x_ = 0F;
    y_ = 0F;
    ballOn_ = false;
    reliability_ = 0;
  }
  private byte memoizedIsInitialized = -1;
  public final boolean isInitialized() {
    byte isInitialized = memoizedIsInitialized;
    if (isInitialized == 1) return true;
    if (isInitialized == 0) return false;

    memoizedIsInitialized = 1;
    return true;
  }

  public void writeTo(com.google.protobuf.CodedOutputStream output)
                      throws java.io.IOException {
    getSerializedSize();
    if (((bitField0_ & 0x00000001) == 0x00000001)) {
      output.writeFloat(1, x_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      output.writeFloat(2, y_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      output.writeBool(3, ballOn_);
    }
    if (((bitField0_ & 0x00000008) == 0x00000008)) {
      output.writeInt32(4, reliability_);
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
        .computeFloatSize(1, x_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(2, y_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      size += com.google.protobuf.CodedOutputStream
        .computeBoolSize(3, ballOn_);
    }
    if (((bitField0_ & 0x00000008) == 0x00000008)) {
      size += com.google.protobuf.CodedOutputStream
        .computeInt32Size(4, reliability_);
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

  public static messages.SharedBall parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.SharedBall parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.SharedBall parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.SharedBall parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.SharedBall parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.SharedBall parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }
  public static messages.SharedBall parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input);
  }
  public static messages.SharedBall parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input, extensionRegistry);
  }
  public static messages.SharedBall parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.SharedBall parseFrom(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }

  public static Builder newBuilder() { return Builder.create(); }
  public Builder newBuilderForType() { return newBuilder(); }
  public static Builder newBuilder(messages.SharedBall prototype) {
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
   * Protobuf type {@code messages.SharedBall}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessage.Builder<Builder> implements
      // @@protoc_insertion_point(builder_implements:messages.SharedBall)
      messages.SharedBallOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return messages._File_BallModel.internal_static_messages_SharedBall_descriptor;
    }

    protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return messages._File_BallModel.internal_static_messages_SharedBall_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              messages.SharedBall.class, messages.SharedBall.Builder.class);
    }

    // Construct using messages.SharedBall.newBuilder()
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
      x_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000001);
      y_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000002);
      ballOn_ = false;
      bitField0_ = (bitField0_ & ~0x00000004);
      reliability_ = 0;
      bitField0_ = (bitField0_ & ~0x00000008);
      return this;
    }

    public Builder clone() {
      return create().mergeFrom(buildPartial());
    }

    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return messages._File_BallModel.internal_static_messages_SharedBall_descriptor;
    }

    public messages.SharedBall getDefaultInstanceForType() {
      return messages.SharedBall.getDefaultInstance();
    }

    public messages.SharedBall build() {
      messages.SharedBall result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    public messages.SharedBall buildPartial() {
      messages.SharedBall result = new messages.SharedBall(this);
      int from_bitField0_ = bitField0_;
      int to_bitField0_ = 0;
      if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
        to_bitField0_ |= 0x00000001;
      }
      result.x_ = x_;
      if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
        to_bitField0_ |= 0x00000002;
      }
      result.y_ = y_;
      if (((from_bitField0_ & 0x00000004) == 0x00000004)) {
        to_bitField0_ |= 0x00000004;
      }
      result.ballOn_ = ballOn_;
      if (((from_bitField0_ & 0x00000008) == 0x00000008)) {
        to_bitField0_ |= 0x00000008;
      }
      result.reliability_ = reliability_;
      result.bitField0_ = to_bitField0_;
      onBuilt();
      return result;
    }

    public Builder mergeFrom(com.google.protobuf.Message other) {
      if (other instanceof messages.SharedBall) {
        return mergeFrom((messages.SharedBall)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(messages.SharedBall other) {
      if (other == messages.SharedBall.getDefaultInstance()) return this;
      if (other.hasX()) {
        setX(other.getX());
      }
      if (other.hasY()) {
        setY(other.getY());
      }
      if (other.hasBallOn()) {
        setBallOn(other.getBallOn());
      }
      if (other.hasReliability()) {
        setReliability(other.getReliability());
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
      messages.SharedBall parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (messages.SharedBall) e.getUnfinishedMessage();
        throw e;
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }
    private int bitField0_;

    private float x_ ;
    /**
     * <code>optional float x = 1;</code>
     */
    public boolean hasX() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>optional float x = 1;</code>
     */
    public float getX() {
      return x_;
    }
    /**
     * <code>optional float x = 1;</code>
     */
    public Builder setX(float value) {
      bitField0_ |= 0x00000001;
      x_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float x = 1;</code>
     */
    public Builder clearX() {
      bitField0_ = (bitField0_ & ~0x00000001);
      x_ = 0F;
      onChanged();
      return this;
    }

    private float y_ ;
    /**
     * <code>optional float y = 2;</code>
     */
    public boolean hasY() {
      return ((bitField0_ & 0x00000002) == 0x00000002);
    }
    /**
     * <code>optional float y = 2;</code>
     */
    public float getY() {
      return y_;
    }
    /**
     * <code>optional float y = 2;</code>
     */
    public Builder setY(float value) {
      bitField0_ |= 0x00000002;
      y_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float y = 2;</code>
     */
    public Builder clearY() {
      bitField0_ = (bitField0_ & ~0x00000002);
      y_ = 0F;
      onChanged();
      return this;
    }

    private boolean ballOn_ ;
    /**
     * <code>optional bool ball_on = 3;</code>
     */
    public boolean hasBallOn() {
      return ((bitField0_ & 0x00000004) == 0x00000004);
    }
    /**
     * <code>optional bool ball_on = 3;</code>
     */
    public boolean getBallOn() {
      return ballOn_;
    }
    /**
     * <code>optional bool ball_on = 3;</code>
     */
    public Builder setBallOn(boolean value) {
      bitField0_ |= 0x00000004;
      ballOn_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool ball_on = 3;</code>
     */
    public Builder clearBallOn() {
      bitField0_ = (bitField0_ & ~0x00000004);
      ballOn_ = false;
      onChanged();
      return this;
    }

    private int reliability_ ;
    /**
     * <code>optional int32 reliability = 4;</code>
     */
    public boolean hasReliability() {
      return ((bitField0_ & 0x00000008) == 0x00000008);
    }
    /**
     * <code>optional int32 reliability = 4;</code>
     */
    public int getReliability() {
      return reliability_;
    }
    /**
     * <code>optional int32 reliability = 4;</code>
     */
    public Builder setReliability(int value) {
      bitField0_ |= 0x00000008;
      reliability_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional int32 reliability = 4;</code>
     */
    public Builder clearReliability() {
      bitField0_ = (bitField0_ & ~0x00000008);
      reliability_ = 0;
      onChanged();
      return this;
    }

    // @@protoc_insertion_point(builder_scope:messages.SharedBall)
  }

  static {
    defaultInstance = new SharedBall(true);
    defaultInstance.initFields();
  }

  // @@protoc_insertion_point(class_scope:messages.SharedBall)
}
