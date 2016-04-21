// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Vision.proto

package messages;

public interface NaiveBallOrBuilder extends
    // @@protoc_insertion_point(interface_extends:messages.NaiveBall)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>optional float velocity = 1;</code>
   */
  boolean hasVelocity();
  /**
   * <code>optional float velocity = 1;</code>
   */
  float getVelocity();

  /**
   * <code>optional bool stationary = 2;</code>
   */
  boolean hasStationary();
  /**
   * <code>optional bool stationary = 2;</code>
   */
  boolean getStationary();

  /**
   * <code>optional float yintercept = 3;</code>
   */
  boolean hasYintercept();
  /**
   * <code>optional float yintercept = 3;</code>
   */
  float getYintercept();

  /**
   * <code>repeated .messages.NaiveBall.Position position = 4;</code>
   */
  java.util.List<messages.NaiveBall.Position> 
      getPositionList();
  /**
   * <code>repeated .messages.NaiveBall.Position position = 4;</code>
   */
  messages.NaiveBall.Position getPosition(int index);
  /**
   * <code>repeated .messages.NaiveBall.Position position = 4;</code>
   */
  int getPositionCount();
  /**
   * <code>repeated .messages.NaiveBall.Position position = 4;</code>
   */
  java.util.List<? extends messages.NaiveBall.PositionOrBuilder> 
      getPositionOrBuilderList();
  /**
   * <code>repeated .messages.NaiveBall.Position position = 4;</code>
   */
  messages.NaiveBall.PositionOrBuilder getPositionOrBuilder(
      int index);

  /**
   * <code>repeated .messages.NaiveBall.Position dest_buffer = 5;</code>
   */
  java.util.List<messages.NaiveBall.Position> 
      getDestBufferList();
  /**
   * <code>repeated .messages.NaiveBall.Position dest_buffer = 5;</code>
   */
  messages.NaiveBall.Position getDestBuffer(int index);
  /**
   * <code>repeated .messages.NaiveBall.Position dest_buffer = 5;</code>
   */
  int getDestBufferCount();
  /**
   * <code>repeated .messages.NaiveBall.Position dest_buffer = 5;</code>
   */
  java.util.List<? extends messages.NaiveBall.PositionOrBuilder> 
      getDestBufferOrBuilderList();
  /**
   * <code>repeated .messages.NaiveBall.Position dest_buffer = 5;</code>
   */
  messages.NaiveBall.PositionOrBuilder getDestBufferOrBuilder(
      int index);

  /**
   * <code>optional float x_vel = 6;</code>
   */
  boolean hasXVel();
  /**
   * <code>optional float x_vel = 6;</code>
   */
  float getXVel();

  /**
   * <code>optional float y_vel = 7;</code>
   */
  boolean hasYVel();
  /**
   * <code>optional float y_vel = 7;</code>
   */
  float getYVel();

  /**
   * <code>optional float start_avg_x = 8;</code>
   */
  boolean hasStartAvgX();
  /**
   * <code>optional float start_avg_x = 8;</code>
   */
  float getStartAvgX();

  /**
   * <code>optional float start_avg_y = 9;</code>
   */
  boolean hasStartAvgY();
  /**
   * <code>optional float start_avg_y = 9;</code>
   */
  float getStartAvgY();

  /**
   * <code>optional float end_avg_x = 10;</code>
   */
  boolean hasEndAvgX();
  /**
   * <code>optional float end_avg_x = 10;</code>
   */
  float getEndAvgX();

  /**
   * <code>optional float end_avg_y = 11;</code>
   */
  boolean hasEndAvgY();
  /**
   * <code>optional float end_avg_y = 11;</code>
   */
  float getEndAvgY();

  /**
   * <code>optional float avg_start_index = 12;</code>
   */
  boolean hasAvgStartIndex();
  /**
   * <code>optional float avg_start_index = 12;</code>
   */
  float getAvgStartIndex();

  /**
   * <code>optional float avg_end_index = 13;</code>
   */
  boolean hasAvgEndIndex();
  /**
   * <code>optional float avg_end_index = 13;</code>
   */
  float getAvgEndIndex();

  /**
   * <code>optional float alt_x_vel = 14;</code>
   */
  boolean hasAltXVel();
  /**
   * <code>optional float alt_x_vel = 14;</code>
   */
  float getAltXVel();

  /**
   * <code>optional float x_v_1 = 15;</code>
   */
  boolean hasXV1();
  /**
   * <code>optional float x_v_1 = 15;</code>
   */
  float getXV1();

  /**
   * <code>optional float x_v_2 = 16;</code>
   */
  boolean hasXV2();
  /**
   * <code>optional float x_v_2 = 16;</code>
   */
  float getXV2();

  /**
   * <code>optional float x_v_3 = 17;</code>
   */
  boolean hasXV3();
  /**
   * <code>optional float x_v_3 = 17;</code>
   */
  float getXV3();
}