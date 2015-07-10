// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BallModel.proto

package messages;

public interface FilteredBallOrBuilder extends
    // @@protoc_insertion_point(interface_extends:messages.FilteredBall)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>optional .messages.VisionBall vis = 1;</code>
   */
  boolean hasVis();
  /**
   * <code>optional .messages.VisionBall vis = 1;</code>
   */
  messages.VisionBall getVis();
  /**
   * <code>optional .messages.VisionBall vis = 1;</code>
   */
  messages.VisionBallOrBuilder getVisOrBuilder();

  /**
   * <code>optional float distance = 2;</code>
   */
  boolean hasDistance();
  /**
   * <code>optional float distance = 2;</code>
   */
  float getDistance();

  /**
   * <code>optional float bearing = 3;</code>
   */
  boolean hasBearing();
  /**
   * <code>optional float bearing = 3;</code>
   */
  float getBearing();

  /**
   * <code>optional float rel_x = 4;</code>
   *
   * <pre>
   * State of best filter
   * </pre>
   */
  boolean hasRelX();
  /**
   * <code>optional float rel_x = 4;</code>
   *
   * <pre>
   * State of best filter
   * </pre>
   */
  float getRelX();

  /**
   * <code>optional float rel_y = 5;</code>
   */
  boolean hasRelY();
  /**
   * <code>optional float rel_y = 5;</code>
   */
  float getRelY();

  /**
   * <code>optional float vel_x = 6;</code>
   */
  boolean hasVelX();
  /**
   * <code>optional float vel_x = 6;</code>
   */
  float getVelX();

  /**
   * <code>optional float vel_y = 7;</code>
   */
  boolean hasVelY();
  /**
   * <code>optional float vel_y = 7;</code>
   */
  float getVelY();

  /**
   * <code>optional float var_rel_x = 8;</code>
   */
  boolean hasVarRelX();
  /**
   * <code>optional float var_rel_x = 8;</code>
   */
  float getVarRelX();

  /**
   * <code>optional float var_rel_y = 9;</code>
   */
  boolean hasVarRelY();
  /**
   * <code>optional float var_rel_y = 9;</code>
   */
  float getVarRelY();

  /**
   * <code>optional float var_vel_x = 10;</code>
   */
  boolean hasVarVelX();
  /**
   * <code>optional float var_vel_x = 10;</code>
   */
  float getVarVelX();

  /**
   * <code>optional float var_vel_y = 11;</code>
   */
  boolean hasVarVelY();
  /**
   * <code>optional float var_vel_y = 11;</code>
   */
  float getVarVelY();

  /**
   * <code>optional bool is_stationary = 12;</code>
   */
  boolean hasIsStationary();
  /**
   * <code>optional bool is_stationary = 12;</code>
   */
  boolean getIsStationary();

  /**
   * <code>optional float bearing_deg = 13;</code>
   */
  boolean hasBearingDeg();
  /**
   * <code>optional float bearing_deg = 13;</code>
   */
  float getBearingDeg();

  /**
   * <code>optional float x = 14;</code>
   */
  boolean hasX();
  /**
   * <code>optional float x = 14;</code>
   */
  float getX();

  /**
   * <code>optional float y = 15;</code>
   */
  boolean hasY();
  /**
   * <code>optional float y = 15;</code>
   */
  float getY();

  /**
   * <code>optional float rel_x_dest = 16;</code>
   */
  boolean hasRelXDest();
  /**
   * <code>optional float rel_x_dest = 16;</code>
   */
  float getRelXDest();

  /**
   * <code>optional float rel_y_dest = 17;</code>
   */
  boolean hasRelYDest();
  /**
   * <code>optional float rel_y_dest = 17;</code>
   */
  float getRelYDest();

  /**
   * <code>optional float speed = 18;</code>
   *
   * <pre>
   * For Goalie, know where the ball is going to intersect y-axis
   * </pre>
   */
  boolean hasSpeed();
  /**
   * <code>optional float speed = 18;</code>
   *
   * <pre>
   * For Goalie, know where the ball is going to intersect y-axis
   * </pre>
   */
  float getSpeed();

  /**
   * <code>optional float rel_y_intersect_dest = 19;</code>
   */
  boolean hasRelYIntersectDest();
  /**
   * <code>optional float rel_y_intersect_dest = 19;</code>
   */
  float getRelYIntersectDest();

  /**
   * <code>optional float stat_rel_x = 20;</code>
   */
  boolean hasStatRelX();
  /**
   * <code>optional float stat_rel_x = 20;</code>
   */
  float getStatRelX();

  /**
   * <code>optional float stat_rel_y = 21;</code>
   */
  boolean hasStatRelY();
  /**
   * <code>optional float stat_rel_y = 21;</code>
   */
  float getStatRelY();

  /**
   * <code>optional float stat_distance = 22;</code>
   */
  boolean hasStatDistance();
  /**
   * <code>optional float stat_distance = 22;</code>
   */
  float getStatDistance();

  /**
   * <code>optional float stat_bearing = 23;</code>
   */
  boolean hasStatBearing();
  /**
   * <code>optional float stat_bearing = 23;</code>
   */
  float getStatBearing();

  /**
   * <code>optional float mov_rel_x = 24;</code>
   */
  boolean hasMovRelX();
  /**
   * <code>optional float mov_rel_x = 24;</code>
   */
  float getMovRelX();

  /**
   * <code>optional float mov_rel_y = 25;</code>
   */
  boolean hasMovRelY();
  /**
   * <code>optional float mov_rel_y = 25;</code>
   */
  float getMovRelY();

  /**
   * <code>optional float mov_distance = 26;</code>
   */
  boolean hasMovDistance();
  /**
   * <code>optional float mov_distance = 26;</code>
   */
  float getMovDistance();

  /**
   * <code>optional float mov_bearing = 27;</code>
   */
  boolean hasMovBearing();
  /**
   * <code>optional float mov_bearing = 27;</code>
   */
  float getMovBearing();

  /**
   * <code>optional float mov_vel_x = 28;</code>
   */
  boolean hasMovVelX();
  /**
   * <code>optional float mov_vel_x = 28;</code>
   */
  float getMovVelX();

  /**
   * <code>optional float mov_vel_y = 29;</code>
   */
  boolean hasMovVelY();
  /**
   * <code>optional float mov_vel_y = 29;</code>
   */
  float getMovVelY();

  /**
   * <code>optional float mov_speed = 30;</code>
   */
  boolean hasMovSpeed();
  /**
   * <code>optional float mov_speed = 30;</code>
   */
  float getMovSpeed();
}