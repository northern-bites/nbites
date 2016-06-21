// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: FallStatus.proto

package messages;

/**
 * Protobuf type {@code messages.FallStatus}
 */
public final class FallStatus extends
    com.google.protobuf.GeneratedMessage implements
    // @@protoc_insertion_point(message_implements:messages.FallStatus)
    FallStatusOrBuilder {
  // Use FallStatus.newBuilder() to construct.
  private FallStatus(com.google.protobuf.GeneratedMessage.Builder<?> builder) {
    super(builder);
    this.unknownFields = builder.getUnknownFields();
  }
  private FallStatus(boolean noInit) { this.unknownFields = com.google.protobuf.UnknownFieldSet.getDefaultInstance(); }

  private static final FallStatus defaultInstance;
  public static FallStatus getDefaultInstance() {
    return defaultInstance;
  }

  public FallStatus getDefaultInstanceForType() {
    return defaultInstance;
  }

  private final com.google.protobuf.UnknownFieldSet unknownFields;
  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
      getUnknownFields() {
    return this.unknownFields;
  }
  private FallStatus(
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
            falling_ = input.readBool();
            break;
          }
          case 16: {
            bitField0_ |= 0x00000002;
            fallen_ = input.readBool();
            break;
          }
          case 24: {
            bitField0_ |= 0x00000004;
            onFront_ = input.readBool();
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
    return messages._File_FallStatus.internal_static_messages_FallStatus_descriptor;
  }

  protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return messages._File_FallStatus.internal_static_messages_FallStatus_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            messages.FallStatus.class, messages.FallStatus.Builder.class);
  }

  public static com.google.protobuf.Parser<FallStatus> PARSER =
      new com.google.protobuf.AbstractParser<FallStatus>() {
    public FallStatus parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new FallStatus(input, extensionRegistry);
    }
  };

  @java.lang.Override
  public com.google.protobuf.Parser<FallStatus> getParserForType() {
    return PARSER;
  }

  private int bitField0_;
  public static final int FALLING_FIELD_NUMBER = 1;
  private boolean falling_;
  /**
   * <code>optional bool falling = 1;</code>
   */
  public boolean hasFalling() {
    return ((bitField0_ & 0x00000001) == 0x00000001);
  }
  /**
   * <code>optional bool falling = 1;</code>
   */
  public boolean getFalling() {
    return falling_;
  }

  public static final int FALLEN_FIELD_NUMBER = 2;
  private boolean fallen_;
  /**
   * <code>optional bool fallen = 2;</code>
   */
  public boolean hasFallen() {
    return ((bitField0_ & 0x00000002) == 0x00000002);
  }
  /**
   * <code>optional bool fallen = 2;</code>
   */
  public boolean getFallen() {
    return fallen_;
  }

  public static final int ON_FRONT_FIELD_NUMBER = 3;
  private boolean onFront_;
  /**
   * <code>optional bool on_front = 3;</code>
   *
   * <pre>
   *On back if false, undefined if not fallen.
   * </pre>
   */
  public boolean hasOnFront() {
    return ((bitField0_ & 0x00000004) == 0x00000004);
  }
  /**
   * <code>optional bool on_front = 3;</code>
   *
   * <pre>
   *On back if false, undefined if not fallen.
   * </pre>
   */
  public boolean getOnFront() {
    return onFront_;
  }

  private void initFields() {
    falling_ = false;
    fallen_ = false;
    onFront_ = false;
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
      output.writeBool(1, falling_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      output.writeBool(2, fallen_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      output.writeBool(3, onFront_);
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
        .computeBoolSize(1, falling_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      size += com.google.protobuf.CodedOutputStream
        .computeBoolSize(2, fallen_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      size += com.google.protobuf.CodedOutputStream
        .computeBoolSize(3, onFront_);
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

  public static messages.FallStatus parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.FallStatus parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.FallStatus parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.FallStatus parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.FallStatus parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.FallStatus parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }
  public static messages.FallStatus parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input);
  }
  public static messages.FallStatus parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input, extensionRegistry);
  }
  public static messages.FallStatus parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.FallStatus parseFrom(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }

  public static Builder newBuilder() { return Builder.create(); }
  public Builder newBuilderForType() { return newBuilder(); }
  public static Builder newBuilder(messages.FallStatus prototype) {
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
   * Protobuf type {@code messages.FallStatus}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessage.Builder<Builder> implements
      // @@protoc_insertion_point(builder_implements:messages.FallStatus)
      messages.FallStatusOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return messages._File_FallStatus.internal_static_messages_FallStatus_descriptor;
    }

    protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return messages._File_FallStatus.internal_static_messages_FallStatus_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              messages.FallStatus.class, messages.FallStatus.Builder.class);
    }

    // Construct using messages.FallStatus.newBuilder()
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
      falling_ = false;
      bitField0_ = (bitField0_ & ~0x00000001);
      fallen_ = false;
      bitField0_ = (bitField0_ & ~0x00000002);
      onFront_ = false;
      bitField0_ = (bitField0_ & ~0x00000004);
      return this;
    }

    public Builder clone() {
      return create().mergeFrom(buildPartial());
    }

    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return messages._File_FallStatus.internal_static_messages_FallStatus_descriptor;
    }

    public messages.FallStatus getDefaultInstanceForType() {
      return messages.FallStatus.getDefaultInstance();
    }

    public messages.FallStatus build() {
      messages.FallStatus result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    public messages.FallStatus buildPartial() {
      messages.FallStatus result = new messages.FallStatus(this);
      int from_bitField0_ = bitField0_;
      int to_bitField0_ = 0;
      if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
        to_bitField0_ |= 0x00000001;
      }
      result.falling_ = falling_;
      if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
        to_bitField0_ |= 0x00000002;
      }
      result.fallen_ = fallen_;
      if (((from_bitField0_ & 0x00000004) == 0x00000004)) {
        to_bitField0_ |= 0x00000004;
      }
      result.onFront_ = onFront_;
      result.bitField0_ = to_bitField0_;
      onBuilt();
      return result;
    }

    public Builder mergeFrom(com.google.protobuf.Message other) {
      if (other instanceof messages.FallStatus) {
        return mergeFrom((messages.FallStatus)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(messages.FallStatus other) {
      if (other == messages.FallStatus.getDefaultInstance()) return this;
      if (other.hasFalling()) {
        setFalling(other.getFalling());
      }
      if (other.hasFallen()) {
        setFallen(other.getFallen());
      }
      if (other.hasOnFront()) {
        setOnFront(other.getOnFront());
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
      messages.FallStatus parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (messages.FallStatus) e.getUnfinishedMessage();
        throw e;
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }
    private int bitField0_;

    private boolean falling_ ;
    /**
     * <code>optional bool falling = 1;</code>
     */
    public boolean hasFalling() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>optional bool falling = 1;</code>
     */
    public boolean getFalling() {
      return falling_;
    }
    /**
     * <code>optional bool falling = 1;</code>
     */
    public Builder setFalling(boolean value) {
      bitField0_ |= 0x00000001;
      falling_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool falling = 1;</code>
     */
    public Builder clearFalling() {
      bitField0_ = (bitField0_ & ~0x00000001);
      falling_ = false;
      onChanged();
      return this;
    }

    private boolean fallen_ ;
    /**
     * <code>optional bool fallen = 2;</code>
     */
    public boolean hasFallen() {
      return ((bitField0_ & 0x00000002) == 0x00000002);
    }
    /**
     * <code>optional bool fallen = 2;</code>
     */
    public boolean getFallen() {
      return fallen_;
    }
    /**
     * <code>optional bool fallen = 2;</code>
     */
    public Builder setFallen(boolean value) {
      bitField0_ |= 0x00000002;
      fallen_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool fallen = 2;</code>
     */
    public Builder clearFallen() {
      bitField0_ = (bitField0_ & ~0x00000002);
      fallen_ = false;
      onChanged();
      return this;
    }

    private boolean onFront_ ;
    /**
     * <code>optional bool on_front = 3;</code>
     *
     * <pre>
     *On back if false, undefined if not fallen.
     * </pre>
     */
    public boolean hasOnFront() {
      return ((bitField0_ & 0x00000004) == 0x00000004);
    }
    /**
     * <code>optional bool on_front = 3;</code>
     *
     * <pre>
     *On back if false, undefined if not fallen.
     * </pre>
     */
    public boolean getOnFront() {
      return onFront_;
    }
    /**
     * <code>optional bool on_front = 3;</code>
     *
     * <pre>
     *On back if false, undefined if not fallen.
     * </pre>
     */
    public Builder setOnFront(boolean value) {
      bitField0_ |= 0x00000004;
      onFront_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional bool on_front = 3;</code>
     *
     * <pre>
     *On back if false, undefined if not fallen.
     * </pre>
     */
    public Builder clearOnFront() {
      bitField0_ = (bitField0_ & ~0x00000004);
      onFront_ = false;
      onChanged();
      return this;
    }

    // @@protoc_insertion_point(builder_scope:messages.FallStatus)
  }

  static {
    defaultInstance = new FallStatus(true);
    defaultInstance.initFields();
  }

  // @@protoc_insertion_point(class_scope:messages.FallStatus)
}

