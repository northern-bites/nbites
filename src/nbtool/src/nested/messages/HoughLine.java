// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Vision.proto

package messages;

/**
 * Protobuf type {@code messages.HoughLine}
 */
public  final class HoughLine extends
    com.google.protobuf.GeneratedMessage
    implements HoughLineOrBuilder {
  // Use HoughLine.newBuilder() to construct.
  private HoughLine(com.google.protobuf.GeneratedMessage.Builder<?> builder) {
    super(builder);
    this.unknownFields = builder.getUnknownFields();
  }
  private HoughLine(boolean noInit) { this.unknownFields = com.google.protobuf.UnknownFieldSet.getDefaultInstance(); }

  private static final HoughLine defaultInstance;
  public static HoughLine getDefaultInstance() {
    return defaultInstance;
  }

  public HoughLine getDefaultInstanceForType() {
    return defaultInstance;
  }

  private final com.google.protobuf.UnknownFieldSet unknownFields;
  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
      getUnknownFields() {
    return this.unknownFields;
  }
  private HoughLine(
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
            r_ = input.readFloat();
            break;
          }
          case 21: {
            bitField0_ |= 0x00000002;
            t_ = input.readFloat();
            break;
          }
          case 29: {
            bitField0_ |= 0x00000004;
            ep0_ = input.readFloat();
            break;
          }
          case 37: {
            bitField0_ |= 0x00000008;
            ep1_ = input.readFloat();
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
    return messages._File_Vision.internal_static_messages_HoughLine_descriptor;
  }

  protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return messages._File_Vision.internal_static_messages_HoughLine_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            messages.HoughLine.class, messages.HoughLine.Builder.class);
  }

  public static com.google.protobuf.Parser<HoughLine> PARSER =
      new com.google.protobuf.AbstractParser<HoughLine>() {
    public HoughLine parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new HoughLine(input, extensionRegistry);
    }
  };

  @java.lang.Override
  public com.google.protobuf.Parser<HoughLine> getParserForType() {
    return PARSER;
  }

  private int bitField0_;
  // optional float r = 1;
  public static final int R_FIELD_NUMBER = 1;
  private float r_;
  /**
   * <code>optional float r = 1;</code>
   */
  public boolean hasR() {
    return ((bitField0_ & 0x00000001) == 0x00000001);
  }
  /**
   * <code>optional float r = 1;</code>
   */
  public float getR() {
    return r_;
  }

  // optional float t = 2;
  public static final int T_FIELD_NUMBER = 2;
  private float t_;
  /**
   * <code>optional float t = 2;</code>
   */
  public boolean hasT() {
    return ((bitField0_ & 0x00000002) == 0x00000002);
  }
  /**
   * <code>optional float t = 2;</code>
   */
  public float getT() {
    return t_;
  }

  // optional float ep0 = 3;
  public static final int EP0_FIELD_NUMBER = 3;
  private float ep0_;
  /**
   * <code>optional float ep0 = 3;</code>
   */
  public boolean hasEp0() {
    return ((bitField0_ & 0x00000004) == 0x00000004);
  }
  /**
   * <code>optional float ep0 = 3;</code>
   */
  public float getEp0() {
    return ep0_;
  }

  // optional float ep1 = 4;
  public static final int EP1_FIELD_NUMBER = 4;
  private float ep1_;
  /**
   * <code>optional float ep1 = 4;</code>
   */
  public boolean hasEp1() {
    return ((bitField0_ & 0x00000008) == 0x00000008);
  }
  /**
   * <code>optional float ep1 = 4;</code>
   */
  public float getEp1() {
    return ep1_;
  }

  private void initFields() {
    r_ = 0F;
    t_ = 0F;
    ep0_ = 0F;
    ep1_ = 0F;
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
      output.writeFloat(1, r_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      output.writeFloat(2, t_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      output.writeFloat(3, ep0_);
    }
    if (((bitField0_ & 0x00000008) == 0x00000008)) {
      output.writeFloat(4, ep1_);
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
        .computeFloatSize(1, r_);
    }
    if (((bitField0_ & 0x00000002) == 0x00000002)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(2, t_);
    }
    if (((bitField0_ & 0x00000004) == 0x00000004)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(3, ep0_);
    }
    if (((bitField0_ & 0x00000008) == 0x00000008)) {
      size += com.google.protobuf.CodedOutputStream
        .computeFloatSize(4, ep1_);
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

  public static messages.HoughLine parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.HoughLine parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.HoughLine parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static messages.HoughLine parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static messages.HoughLine parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.HoughLine parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }
  public static messages.HoughLine parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input);
  }
  public static messages.HoughLine parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseDelimitedFrom(input, extensionRegistry);
  }
  public static messages.HoughLine parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return PARSER.parseFrom(input);
  }
  public static messages.HoughLine parseFrom(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return PARSER.parseFrom(input, extensionRegistry);
  }

  public static Builder newBuilder() { return Builder.create(); }
  public Builder newBuilderForType() { return newBuilder(); }
  public static Builder newBuilder(messages.HoughLine prototype) {
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
   * Protobuf type {@code messages.HoughLine}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessage.Builder<Builder>
     implements messages.HoughLineOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return messages._File_Vision.internal_static_messages_HoughLine_descriptor;
    }

    protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return messages._File_Vision.internal_static_messages_HoughLine_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              messages.HoughLine.class, messages.HoughLine.Builder.class);
    }

    // Construct using messages.HoughLine.newBuilder()
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
      r_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000001);
      t_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000002);
      ep0_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000004);
      ep1_ = 0F;
      bitField0_ = (bitField0_ & ~0x00000008);
      return this;
    }

    public Builder clone() {
      return create().mergeFrom(buildPartial());
    }

    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return messages._File_Vision.internal_static_messages_HoughLine_descriptor;
    }

    public messages.HoughLine getDefaultInstanceForType() {
      return messages.HoughLine.getDefaultInstance();
    }

    public messages.HoughLine build() {
      messages.HoughLine result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    public messages.HoughLine buildPartial() {
      messages.HoughLine result = new messages.HoughLine(this);
      int from_bitField0_ = bitField0_;
      int to_bitField0_ = 0;
      if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
        to_bitField0_ |= 0x00000001;
      }
      result.r_ = r_;
      if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
        to_bitField0_ |= 0x00000002;
      }
      result.t_ = t_;
      if (((from_bitField0_ & 0x00000004) == 0x00000004)) {
        to_bitField0_ |= 0x00000004;
      }
      result.ep0_ = ep0_;
      if (((from_bitField0_ & 0x00000008) == 0x00000008)) {
        to_bitField0_ |= 0x00000008;
      }
      result.ep1_ = ep1_;
      result.bitField0_ = to_bitField0_;
      onBuilt();
      return result;
    }

    public Builder mergeFrom(com.google.protobuf.Message other) {
      if (other instanceof messages.HoughLine) {
        return mergeFrom((messages.HoughLine)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(messages.HoughLine other) {
      if (other == messages.HoughLine.getDefaultInstance()) return this;
      if (other.hasR()) {
        setR(other.getR());
      }
      if (other.hasT()) {
        setT(other.getT());
      }
      if (other.hasEp0()) {
        setEp0(other.getEp0());
      }
      if (other.hasEp1()) {
        setEp1(other.getEp1());
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
      messages.HoughLine parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (messages.HoughLine) e.getUnfinishedMessage();
        throw e;
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }
    private int bitField0_;

    // optional float r = 1;
    private float r_ ;
    /**
     * <code>optional float r = 1;</code>
     */
    public boolean hasR() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>optional float r = 1;</code>
     */
    public float getR() {
      return r_;
    }
    /**
     * <code>optional float r = 1;</code>
     */
    public Builder setR(float value) {
      bitField0_ |= 0x00000001;
      r_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float r = 1;</code>
     */
    public Builder clearR() {
      bitField0_ = (bitField0_ & ~0x00000001);
      r_ = 0F;
      onChanged();
      return this;
    }

    // optional float t = 2;
    private float t_ ;
    /**
     * <code>optional float t = 2;</code>
     */
    public boolean hasT() {
      return ((bitField0_ & 0x00000002) == 0x00000002);
    }
    /**
     * <code>optional float t = 2;</code>
     */
    public float getT() {
      return t_;
    }
    /**
     * <code>optional float t = 2;</code>
     */
    public Builder setT(float value) {
      bitField0_ |= 0x00000002;
      t_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float t = 2;</code>
     */
    public Builder clearT() {
      bitField0_ = (bitField0_ & ~0x00000002);
      t_ = 0F;
      onChanged();
      return this;
    }

    // optional float ep0 = 3;
    private float ep0_ ;
    /**
     * <code>optional float ep0 = 3;</code>
     */
    public boolean hasEp0() {
      return ((bitField0_ & 0x00000004) == 0x00000004);
    }
    /**
     * <code>optional float ep0 = 3;</code>
     */
    public float getEp0() {
      return ep0_;
    }
    /**
     * <code>optional float ep0 = 3;</code>
     */
    public Builder setEp0(float value) {
      bitField0_ |= 0x00000004;
      ep0_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float ep0 = 3;</code>
     */
    public Builder clearEp0() {
      bitField0_ = (bitField0_ & ~0x00000004);
      ep0_ = 0F;
      onChanged();
      return this;
    }

    // optional float ep1 = 4;
    private float ep1_ ;
    /**
     * <code>optional float ep1 = 4;</code>
     */
    public boolean hasEp1() {
      return ((bitField0_ & 0x00000008) == 0x00000008);
    }
    /**
     * <code>optional float ep1 = 4;</code>
     */
    public float getEp1() {
      return ep1_;
    }
    /**
     * <code>optional float ep1 = 4;</code>
     */
    public Builder setEp1(float value) {
      bitField0_ |= 0x00000008;
      ep1_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>optional float ep1 = 4;</code>
     */
    public Builder clearEp1() {
      bitField0_ = (bitField0_ & ~0x00000008);
      ep1_ = 0F;
      onChanged();
      return this;
    }

    // @@protoc_insertion_point(builder_scope:messages.HoughLine)
  }

  static {
    defaultInstance = new HoughLine(true);
    defaultInstance.initFields();
  }

  // @@protoc_insertion_point(class_scope:messages.HoughLine)
}

