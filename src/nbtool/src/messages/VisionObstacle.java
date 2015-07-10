// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: VisionRobot.proto

package messages;

/**
 * Protobuf type {@code messages.VisionObstacle}
 */
public final class VisionObstacle extends
    com.google.protobuf.GeneratedMessage implements
    // @@protoc_insertion_point(message_implements:messages.VisionObstacle)
    VisionObstacleOrBuilder {
  // Use VisionObstacle.newBuilder() to construct.
  private VisionObstacle(com.google.protobuf.GeneratedMessage.Builder<?> builder) {
    super(builder);
    this.unknownFields = builder.getUnknownFields();
  }
  private VisionObstacle(boolean noInit) { this.unknownFields = com.google.protobuf.UnknownFieldSet.getDefaultInstance(); }

  private static final VisionObstacle defaultInstance;
  public static VisionObstacle getDefaultInstance() {
    return defaultInstance;
  }

  public VisionObstacle getDefaultInstanceForType() {
    return defaultInstance;
  }

  private final com.google.protobuf.UnknownFieldSet unknownFields;
  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
      getUnknownFields() {
    return this.unknownFields;
  }
  private VisionObstacle(
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
            bitField0_ |= 0x00000001;
            onLeft_ = input.readBool();
            break;
          }
          case 16: {
            bitField0_ |= 0x00000002;
            onRight_ = input.readBool();
            break;
          }
          case 24: {
            bitField0_ |= 0x00000004;
            offField_ = input.readBool();
            break;
          }
          case 32: {
            bitField0_ |= 0x00000008;
            blockLeft_ = input.readSInt32();
            break;
          }
          case 40: {
            bitField0_ |= 0x00000010;
            blockMid_ = input.readSInt32();
            break;
          }
          case 48: {
            bitField0_ |= 0x00000020;
            blockRight_ = input.readSInt32();
            break;
          }
          case 61: {
            bitField0_ |= 0x00000040;
            leftDist_ = input.readFloat();
            break;
          }
          case 69: {
            bitField0_ |= 0x00000080;
            midDist_ = input.readFloat();
            break;
          }
          case 77: {
            bitField0_ |= 0x00000100;
            rightDist_ = input.readFloat();
            break;
          }
          case 85: {
            bitField0_ |= 0x00000200;
            leftBearing_ = input.readFloat();
            break;
          }
          case 93: {
            bitField0_ |= 0x00000400;
            midBearing_ = input.readFloat();
            break;
          }
          case 101: {
            bitField0_ |= 0x00000800;
            rightBearing_ = input.readFloat();
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
    return messages._File_VisionRobot.internal_static_messages_VisionObstacle_descriptor;
  }

  protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return messages._File_VisionRobot.internal_static_messages_VisionObstacle_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            messages.VisionObstacle.class, messages.VisionObstacle.Builder.class);
  }

  public static com.google.protobuf.Parser<VisionObstacle> PARSER =
      new com.google.protobuf.AbstractParser<VisionObstacle>() {
    public VisionObstacle parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new VisionObstacle(input, extensionRegistry);
    }
  };

  @java.lang.Override
  public com.google.protobuf.Parser<VisionObstacle> getParserForType() {
    return PARSER;
  }

  private int bitField0_;
  public static final int ON_LEFT_FIELD_NUMBER = 1;
  private boolean onLeft_;
  /**
   * <code>optional bool on_left = 1;</code>
   */
  public boolean hasOnLeft() {
    return ((bitField0_ & 0x00000001) == 0x00000001);
  }
  /**
   * <code>optional bool on_left = 1;</code>
   */
  public boolean getOnLeft() {
    return onLeft_;
  }

  public static final int ON_RIGHT_FIELD_NUMBER = 2;
  private boolean onRight_;
  /**
   * <code>optional bool on_right = 2;</code>
   */
  public boolean hasOnRight() {
    return ((bitField0_ & 0x00000002) == 0x00000002);
  }
  /**
   * <code>optional bool on_right = 2;</code>
   */
  public boolean getOnRight() {
    return onRight_;
  }

  public static final int OFF_FIELD_FIELD_NUMBER = 3;
  private boolean offField_;
  /**
   * <code>optional bool off_field = 3;</code>
   */
  public boolean hasOffField() {
    return ((bitField0_ & 0x00000004) == 0x00000004);
  }
  /**
   * <code>optional bool off_field = 3;</code>
   */
  public boolean getOffField() {
    return offField_;
  }

  public static final int BLOCK_LEFT_FIELD_NUMBER = 4;
  private int blockLeft_;
  /**
   * <code>optional sint32 block_left = 4;</code>
   */
  public boolean hasBlockLeft() {
    return ((bitField0_ & 0x00000008) == 0x00000008);
  }
  /**
   * <code>optional sint32 block_left = 4;</code>
   */
  public int getBlockLeft() {
    return blockLeft_;
  }

  public static final int BLOCK_MID_FIELD_NUMBER = 5;
  private int blockMid_;
  /**
   * <code>optional sint32 block_mid = 5;</code>
   */
  public boolean hasBlockMid() {
    return ((bitField0_ & 0x00000010) == 0x00000010);
  }
  /**
   * <code>optional sint32 block_mid = 5;</code>
   */
  public int getBlockMid() {
    return blockMid_;
  }

  public static final int BLOCK_RIGHT_FIELD_NUMBER = 6;
  private int blockRight_;
  /**
   * <code>optional sint32 block_right = 6;</code>
   */
  public boolean hasBlockRight() {
    return ((bitField0_ & 0x00000020) == 0x00000020);
  }
  /**
   * <code>optional sint32 block_right = 6;</code>
   */
  public int getBlockRight() {
    return blockRight_;
  }

  public static final int LEFT_DIST_FIELD_NUMBER = 7;
  private float leftDist_;
  /**
   * <code>optional float left_dist = 7;</code>
   */
  public boolean hasLeftDist() {
    return ((bitField0_ & 0x00000040) == 0x00000040);
  }
  /**
   * <code>optional float left_dist = 7;</code>
   */
  public float getLeftDist() {
    return leftDist_;
  }

  public static final int MID_DIST_FIELD_NUMBER = 8;
  private float midDist_;
  /**
   * <code>optional float mid_dist = 8;</code>
   */
  public boolean hasMidDist() {
    return ((bitField0_ & 0x00000080) == 0x00000080);
  }
  /**
   * <code>optional float mid_dist = 8;</code>
   */
  public float getMidDist() {
    return midDist_;
  }

  public static final int RIGHT_DIST_FIELD_NUMBER = 9;
  private float rightDist_;
  /**
   * <code>optional float right_dist = 9;</code>
   */
  public boolean hasRightDist() {
    return ((bitField0_ & 0x00000100) == 0x00000100);
  }
  /**
   * <code>optional float right_dist = 9;</code>
   */
  public float getRightDist() {
    return rightDist_;
  }

  public static final int LEFT_BEARING_FIELD_NUMBER = 10;
  private float leftBearing_;
  /**
   * <code>optional float left_bearing = 10;</code>
   */
  public boolean hasLeftBearing() {
    return ((bitField0_ & 0x00000200) == 0x00000200);
  }
  /**
   * <code>optional float left_bearing = 10;</code>
   */
  public float getLeftBearing() {
    return leftBearing_;
  }

  public static final int MID_BEARING_FIELD_NUMBER = 11;
  private float midBearing_;
  /**
   * <code>optional float mid_bearing = 11;</code>
   */
  public boolean hasMidBearing() {
    return ((bitField0_ & 0x00000400) == 0x00000400);
  }
  /**
   * <code>optional float mid_bearing = 11;</code>
   */
  public float getMidBearing() {
    return midBearing_;
  }

  public static final int RIGHT_BEARING_FIELD_NUMBER = 12;
  private float rightBearing_;
  /**
   * <code>optional float right_bearing = 12;</code>
   */
  public boolean hasRightBearing() {
    return ((bitField0_ & 0x00000800) == 0x00000800);
  }
  /**
   * <code>optional float right_bearing = 12;</code>
   */
  public float getRightBearing() {
    return rightBearing_;
  }

  private void initFields() {
    onLeft_ = false;
    onRight_ = false;
    offField_ = false;
    blockLeft_ = 0;
    blockMid_ = 0;
    blockRight_ = 0;
    leftDist_ = 0F;
    midDist_ = 0F;
    rightDist_ = 0F;
    leftBearing_ = 0F;
    midBearing_ = 0F;
    rightBearing_ = 0F;
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
      output.writeBool(1, onLeft_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      output.writeBool(2, onRight_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      output.writeBool(3, offField_);
    }
    if (((bitField0_ & 0x00000008) == 0x00000008)) {
      output.writeSInt32(4, blockLeft_);
    }
    if (((bitField0_ & 0x00000010) == 0x00000010)) {
      output.writeSInt32(5, blockMid_);
    }
    if (((bitField0_ & 0x00000020) == 0x00000020)) {
      output.writeSInt32(6, blockRight_);
    }
    if (((bitField0_ & 0x00000040) == 0x00000040)) {
      output.writeFloat(7, leftDist_);
    }
    if (((bitField0_ & 0x00000080) == 0x00000080)) {
      output.writeFloat(8, midDist_);
    }
    if (((bitField0_ & 0x00000100) == 0x00000100)) {
      output.writeFloat(9, rightDist_);
    }
    if (((bitField0_ & 0x00000200) == 0x00000200)) {
      output.writeFloat(10, leftBearing_);
    }
    if (((bitField0_ & 0x00000400) == 0x00000400)) {
      output.writeFloat(11, midBearing_);
    }
    if (((bitField0_ & 0x00000800) == 0x00000800)) {
      output.writeFloat(12, rightBearing_);
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
        .computeBoolSize(1, onLeft_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      size += com.google.protobuf.CodedOutputStream
        .computeBoolSize(2, onRight_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      size += com.google.protobuf.CodedOutputStream
        .computeBoolSize(3, offField_);
    }
    if (((bitField0_ & 0x00000008) == 0x00000008)) {
      size += com.google.protobuf.CodedOutputStream
        .computeSInt32Size(4, blockLeft_);
    }
    if (((bitField0_ & 0x00000010) == 0x00000010)) {
      size += com.google.protobuf.CodedOutputStream
        .computeSInt32Size(5, blockMid_);
    }
    if (((bitField0_ & 0x00000020) == 0x00000020)) {
      size += com.google.protobuf.CodedOutputStream
        .computeSInt32Size(6, blockRight_);
    }
    if (((bitField0_ & 0x00000040) == 0x00000040)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(7, leftDist_);
    }
    if (((bitField0_ & 0x00000080) == 0x00000080)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(8, midDist_);
    }
    if (((bitField0_ & 0x00000100) == 0x00000100)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(9, rightDist_);
    }
    if (((bitField0_ & 0x00000200) == 0x00000200)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(10, leftBearing_);
    }
    if (((bitField0_ & 0x00000400) == 0x00000400)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(11, midBearing_);
    }
    if (((bitField0_ & 0x00000800) == 0x00000800)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(12, rightBearing_);
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

  public static messages.VisionObstacle parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.VisionObstacle parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.VisionObstacle parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.VisionObstacle parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.VisionObstacle parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.VisionObstacle parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }
  public static messages.VisionObstacle parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input);
  }
  public static messages.VisionObstacle parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input, extensionRegistry);
  }
  public static messages.VisionObstacle parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.VisionObstacle parseFrom(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }

  public static Builder newBuilder() { return Builder.create(); }
  public Builder newBuilderForType() { return newBuilder(); }
  public static Builder newBuilder(messages.VisionObstacle prototype) {
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
   * Protobuf type {@code messages.VisionObstacle}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessage.Builder<Builder> implements
      // @@protoc_insertion_point(builder_implements:messages.VisionObstacle)
      messages.VisionObstacleOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return messages._File_VisionRobot.internal_static_messages_VisionObstacle_descriptor;
    }

    protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return messages._File_VisionRobot.internal_static_messages_VisionObstacle_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              messages.VisionObstacle.class, messages.VisionObstacle.Builder.class);
    }

    // Construct using messages.VisionObstacle.newBuilder()
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
      onLeft_ = false;
      bitField0_ = (bitField0_ & ~0x00000001);
      onRight_ = false;
      bitField0_ = (bitField0_ & ~0x00000002);
      offField_ = false;
      bitField0_ = (bitField0_ & ~0x00000004);
      blockLeft_ = 0;
      bitField0_ = (bitField0_ & ~0x00000008);
      blockMid_ = 0;
      bitField0_ = (bitField0_ & ~0x00000010);
      blockRight_ = 0;
      bitField0_ = (bitField0_ & ~0x00000020);
      leftDist_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000040);
      midDist_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000080);
      rightDist_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000100);
      leftBearing_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000200);
      midBearing_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000400);
      rightBearing_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000800);
      return this;
    }

    public Builder clone() {
      return create().mergeFrom(buildPartial());
    }

    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return messages._File_VisionRobot.internal_static_messages_VisionObstacle_descriptor;
    }

    public messages.VisionObstacle getDefaultInstanceForType() {
      return messages.VisionObstacle.getDefaultInstance();
    }

    public messages.VisionObstacle build() {
      messages.VisionObstacle result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    public messages.VisionObstacle buildPartial() {
      messages.VisionObstacle result = new messages.VisionObstacle(this);
      int from_bitField0_ = bitField0_;
      int to_bitField0_ = 0;
      if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
        to_bitField0_ |= 0x00000001;
      }
      result.onLeft_ = onLeft_;
      if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
        to_bitField0_ |= 0x00000002;
      }
      result.onRight_ = onRight_;
      if (((from_bitField0_ & 0x00000004) == 0x00000004)) {
        to_bitField0_ |= 0x00000004;
      }
      result.offField_ = offField_;
      if (((from_bitField0_ & 0x00000008) == 0x00000008)) {
        to_bitField0_ |= 0x00000008;
      }
      result.blockLeft_ = blockLeft_;
      if (((from_bitField0_ & 0x00000010) == 0x00000010)) {
        to_bitField0_ |= 0x00000010;
      }
      result.blockMid_ = blockMid_;
      if (((from_bitField0_ & 0x00000020) == 0x00000020)) {
        to_bitField0_ |= 0x00000020;
      }
      result.blockRight_ = blockRight_;
      if (((from_bitField0_ & 0x00000040) == 0x00000040)) {
        to_bitField0_ |= 0x00000040;
      }
      result.leftDist_ = leftDist_;
      if (((from_bitField0_ & 0x00000080) == 0x00000080)) {
        to_bitField0_ |= 0x00000080;
      }
      result.midDist_ = midDist_;
      if (((from_bitField0_ & 0x00000100) == 0x00000100)) {
        to_bitField0_ |= 0x00000100;
      }
      result.rightDist_ = rightDist_;
      if (((from_bitField0_ & 0x00000200) == 0x00000200)) {
        to_bitField0_ |= 0x00000200;
      }
      result.leftBearing_ = leftBearing_;
      if (((from_bitField0_ & 0x00000400) == 0x00000400)) {
        to_bitField0_ |= 0x00000400;
      }
      result.midBearing_ = midBearing_;
      if (((from_bitField0_ & 0x00000800) == 0x00000800)) {
        to_bitField0_ |= 0x00000800;
      }
      result.rightBearing_ = rightBearing_;
      result.bitField0_ = to_bitField0_;
      onBuilt();
      return result;
    }

    public Builder mergeFrom(com.google.protobuf.Message other) {
      if (other instanceof messages.VisionObstacle) {
        return mergeFrom((messages.VisionObstacle)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(messages.VisionObstacle other) {
      if (other == messages.VisionObstacle.getDefaultInstance()) return this;
      if (other.hasOnLeft()) {
        setOnLeft(other.getOnLeft());
      }
      if (other.hasOnRight()) {
        setOnRight(other.getOnRight());
      }
      if (other.hasOffField()) {
        setOffField(other.getOffField());
      }
      if (other.hasBlockLeft()) {
        setBlockLeft(other.getBlockLeft());
      }
      if (other.hasBlockMid()) {
        setBlockMid(other.getBlockMid());
      }
      if (other.hasBlockRight()) {
        setBlockRight(other.getBlockRight());
      }
      if (other.hasLeftDist()) {
        setLeftDist(other.getLeftDist());
      }
      if (other.hasMidDist()) {
        setMidDist(other.getMidDist());
      }
      if (other.hasRightDist()) {
        setRightDist(other.getRightDist());
      }
      if (other.hasLeftBearing()) {
        setLeftBearing(other.getLeftBearing());
      }
      if (other.hasMidBearing()) {
        setMidBearing(other.getMidBearing());
      }
      if (other.hasRightBearing()) {
        setRightBearing(other.getRightBearing());
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
      messages.VisionObstacle parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (messages.VisionObstacle) e.getUnfinishedMessage();
        throw e;
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }
    private int bitField0_;

    private boolean onLeft_ ;
    /**
     * <code>optional bool on_left = 1;</code>
     */
    public boolean hasOnLeft() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>optional bool on_left = 1;</code>
     */
    public boolean getOnLeft() {
      return onLeft_;
    }
    /**
     * <code>optional bool on_left = 1;</code>
     */
    public Builder setOnLeft(boolean value) {
      bitField0_ |= 0x00000001;
      onLeft_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool on_left = 1;</code>
     */
    public Builder clearOnLeft() {
      bitField0_ = (bitField0_ & ~0x00000001);
      onLeft_ = false;
      onChanged();
      return this;
    }

    private boolean onRight_ ;
    /**
     * <code>optional bool on_right = 2;</code>
     */
    public boolean hasOnRight() {
      return ((bitField0_ & 0x00000002) == 0x00000002);
    }
    /**
     * <code>optional bool on_right = 2;</code>
     */
    public boolean getOnRight() {
      return onRight_;
    }
    /**
     * <code>optional bool on_right = 2;</code>
     */
    public Builder setOnRight(boolean value) {
      bitField0_ |= 0x00000002;
      onRight_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool on_right = 2;</code>
     */
    public Builder clearOnRight() {
      bitField0_ = (bitField0_ & ~0x00000002);
      onRight_ = false;
      onChanged();
      return this;
    }

    private boolean offField_ ;
    /**
     * <code>optional bool off_field = 3;</code>
     */
    public boolean hasOffField() {
      return ((bitField0_ & 0x00000004) == 0x00000004);
    }
    /**
     * <code>optional bool off_field = 3;</code>
     */
    public boolean getOffField() {
      return offField_;
    }
    /**
     * <code>optional bool off_field = 3;</code>
     */
    public Builder setOffField(boolean value) {
      bitField0_ |= 0x00000004;
      offField_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool off_field = 3;</code>
     */
    public Builder clearOffField() {
      bitField0_ = (bitField0_ & ~0x00000004);
      offField_ = false;
      onChanged();
      return this;
    }

    private int blockLeft_ ;
    /**
     * <code>optional sint32 block_left = 4;</code>
     */
    public boolean hasBlockLeft() {
      return ((bitField0_ & 0x00000008) == 0x00000008);
    }
    /**
     * <code>optional sint32 block_left = 4;</code>
     */
    public int getBlockLeft() {
      return blockLeft_;
    }
    /**
     * <code>optional sint32 block_left = 4;</code>
     */
    public Builder setBlockLeft(int value) {
      bitField0_ |= 0x00000008;
      blockLeft_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional sint32 block_left = 4;</code>
     */
    public Builder clearBlockLeft() {
      bitField0_ = (bitField0_ & ~0x00000008);
      blockLeft_ = 0;
      onChanged();
      return this;
    }

    private int blockMid_ ;
    /**
     * <code>optional sint32 block_mid = 5;</code>
     */
    public boolean hasBlockMid() {
      return ((bitField0_ & 0x00000010) == 0x00000010);
    }
    /**
     * <code>optional sint32 block_mid = 5;</code>
     */
    public int getBlockMid() {
      return blockMid_;
    }
    /**
     * <code>optional sint32 block_mid = 5;</code>
     */
    public Builder setBlockMid(int value) {
      bitField0_ |= 0x00000010;
      blockMid_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional sint32 block_mid = 5;</code>
     */
    public Builder clearBlockMid() {
      bitField0_ = (bitField0_ & ~0x00000010);
      blockMid_ = 0;
      onChanged();
      return this;
    }

    private int blockRight_ ;
    /**
     * <code>optional sint32 block_right = 6;</code>
     */
    public boolean hasBlockRight() {
      return ((bitField0_ & 0x00000020) == 0x00000020);
    }
    /**
     * <code>optional sint32 block_right = 6;</code>
     */
    public int getBlockRight() {
      return blockRight_;
    }
    /**
     * <code>optional sint32 block_right = 6;</code>
     */
    public Builder setBlockRight(int value) {
      bitField0_ |= 0x00000020;
      blockRight_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional sint32 block_right = 6;</code>
     */
    public Builder clearBlockRight() {
      bitField0_ = (bitField0_ & ~0x00000020);
      blockRight_ = 0;
      onChanged();
      return this;
    }

    private float leftDist_ ;
    /**
     * <code>optional float left_dist = 7;</code>
     */
    public boolean hasLeftDist() {
      return ((bitField0_ & 0x00000040) == 0x00000040);
    }
    /**
     * <code>optional float left_dist = 7;</code>
     */
    public float getLeftDist() {
      return leftDist_;
    }
    /**
     * <code>optional float left_dist = 7;</code>
     */
    public Builder setLeftDist(float value) {
      bitField0_ |= 0x00000040;
      leftDist_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float left_dist = 7;</code>
     */
    public Builder clearLeftDist() {
      bitField0_ = (bitField0_ & ~0x00000040);
      leftDist_ = 0F;
      onChanged();
      return this;
    }

    private float midDist_ ;
    /**
     * <code>optional float mid_dist = 8;</code>
     */
    public boolean hasMidDist() {
      return ((bitField0_ & 0x00000080) == 0x00000080);
    }
    /**
     * <code>optional float mid_dist = 8;</code>
     */
    public float getMidDist() {
      return midDist_;
    }
    /**
     * <code>optional float mid_dist = 8;</code>
     */
    public Builder setMidDist(float value) {
      bitField0_ |= 0x00000080;
      midDist_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float mid_dist = 8;</code>
     */
    public Builder clearMidDist() {
      bitField0_ = (bitField0_ & ~0x00000080);
      midDist_ = 0F;
      onChanged();
      return this;
    }

    private float rightDist_ ;
    /**
     * <code>optional float right_dist = 9;</code>
     */
    public boolean hasRightDist() {
      return ((bitField0_ & 0x00000100) == 0x00000100);
    }
    /**
     * <code>optional float right_dist = 9;</code>
     */
    public float getRightDist() {
      return rightDist_;
    }
    /**
     * <code>optional float right_dist = 9;</code>
     */
    public Builder setRightDist(float value) {
      bitField0_ |= 0x00000100;
      rightDist_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float right_dist = 9;</code>
     */
    public Builder clearRightDist() {
      bitField0_ = (bitField0_ & ~0x00000100);
      rightDist_ = 0F;
      onChanged();
      return this;
    }

    private float leftBearing_ ;
    /**
     * <code>optional float left_bearing = 10;</code>
     */
    public boolean hasLeftBearing() {
      return ((bitField0_ & 0x00000200) == 0x00000200);
    }
    /**
     * <code>optional float left_bearing = 10;</code>
     */
    public float getLeftBearing() {
      return leftBearing_;
    }
    /**
     * <code>optional float left_bearing = 10;</code>
     */
    public Builder setLeftBearing(float value) {
      bitField0_ |= 0x00000200;
      leftBearing_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float left_bearing = 10;</code>
     */
    public Builder clearLeftBearing() {
      bitField0_ = (bitField0_ & ~0x00000200);
      leftBearing_ = 0F;
      onChanged();
      return this;
    }

    private float midBearing_ ;
    /**
     * <code>optional float mid_bearing = 11;</code>
     */
    public boolean hasMidBearing() {
      return ((bitField0_ & 0x00000400) == 0x00000400);
    }
    /**
     * <code>optional float mid_bearing = 11;</code>
     */
    public float getMidBearing() {
      return midBearing_;
    }
    /**
     * <code>optional float mid_bearing = 11;</code>
     */
    public Builder setMidBearing(float value) {
      bitField0_ |= 0x00000400;
      midBearing_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float mid_bearing = 11;</code>
     */
    public Builder clearMidBearing() {
      bitField0_ = (bitField0_ & ~0x00000400);
      midBearing_ = 0F;
      onChanged();
      return this;
    }

    private float rightBearing_ ;
    /**
     * <code>optional float right_bearing = 12;</code>
     */
    public boolean hasRightBearing() {
      return ((bitField0_ & 0x00000800) == 0x00000800);
    }
    /**
     * <code>optional float right_bearing = 12;</code>
     */
    public float getRightBearing() {
      return rightBearing_;
    }
    /**
     * <code>optional float right_bearing = 12;</code>
     */
    public Builder setRightBearing(float value) {
      bitField0_ |= 0x00000800;
      rightBearing_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float right_bearing = 12;</code>
     */
    public Builder clearRightBearing() {
      bitField0_ = (bitField0_ & ~0x00000800);
      rightBearing_ = 0F;
      onChanged();
      return this;
    }

    // @@protoc_insertion_point(builder_scope:messages.VisionObstacle)
  }

  static {
    defaultInstance = new VisionObstacle(true);
    defaultInstance.initFields();
  }

  // @@protoc_insertion_point(class_scope:messages.VisionObstacle)
}
