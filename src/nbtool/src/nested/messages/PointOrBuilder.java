// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: VisionField.proto

package messages;

public interface PointOrBuilder extends
    // @@protoc_insertion_point(interface_extends:messages.Point)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>optional float x = 1;</code>
   */
  boolean hasX();
  /**
   * <code>optional float x = 1;</code>
   */
  float getX();

  /**
   * <code>optional float y = 2;</code>
   */
  boolean hasY();
  /**
   * <code>optional float y = 2;</code>
   */
  float getY();

  /**
   * <code>optional float field_angle = 3;</code>
   *
   * <pre>
   *Used only for corners... HACK
   * </pre>
   */
  boolean hasFieldAngle();
  /**
   * <code>optional float field_angle = 3;</code>
   *
   * <pre>
   *Used only for corners... HACK
   * </pre>
   */
  float getFieldAngle();
}