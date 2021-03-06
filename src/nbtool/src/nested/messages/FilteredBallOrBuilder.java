// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Vision.proto

package messages;

public interface FilteredBallOrBuilder
    extends com.google.protobuf.MessageOrBuilder {

  // optional .messages.VBall vis = 1;
  /**
   * <code>optional .messages.VBall vis = 1;</code>
   */
  boolean hasVis();
  /**
   * <code>optional .messages.VBall vis = 1;</code>
   */
  messages.VBall getVis();
  /**
   * <code>optional .messages.VBall vis = 1;</code>
   */
  messages.VBallOrBuilder getVisOrBuilder();

  // optional float distance = 2;
  /**
   * <code>optional float distance = 2;</code>
   */
  boolean hasDistance();
  /**
   * <code>optional float distance = 2;</code>
   */
  float getDistance();

  // optional float bearing = 3;
  /**
   * <code>optional float bearing = 3;</code>
   */
  boolean hasBearing();
  /**
   * <code>optional float bearing = 3;</code>
   */
  float getBearing();

  // optional float rel_x = 4;
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

  // optional float rel_y = 5;
  /**
   * <code>optional float rel_y = 5;</code>
   */
  boolean hasRelY();
  /**
   * <code>optional float rel_y = 5;</code>
   */
  float getRelY();

  // optional float vel_x = 6;
  /**
   * <code>optional float vel_x = 6;</code>
   */
  boolean hasVelX();
  /**
   * <code>optional float vel_x = 6;</code>
   */
  float getVelX();

  // optional float vel_y = 7;
  /**
   * <code>optional float vel_y = 7;</code>
   */
  boolean hasVelY();
  /**
   * <code>optional float vel_y = 7;</code>
   */
  float getVelY();

  // optional float var_rel_x = 8;
  /**
   * <code>optional float var_rel_x = 8;</code>
   */
  boolean hasVarRelX();
  /**
   * <code>optional float var_rel_x = 8;</code>
   */
  float getVarRelX();

  // optional float var_rel_y = 9;
  /**
   * <code>optional float var_rel_y = 9;</code>
   */
  boolean hasVarRelY();
  /**
   * <code>optional float var_rel_y = 9;</code>
   */
  float getVarRelY();

  // optional float var_vel_x = 10;
  /**
   * <code>optional float var_vel_x = 10;</code>
   */
  boolean hasVarVelX();
  /**
   * <code>optional float var_vel_x = 10;</code>
   */
  float getVarVelX();

  // optional float var_vel_y = 11;
  /**
   * <code>optional float var_vel_y = 11;</code>
   */
  boolean hasVarVelY();
  /**
   * <code>optional float var_vel_y = 11;</code>
   */
  float getVarVelY();

  // optional bool is_stationary = 12;
  /**
   * <code>optional bool is_stationary = 12;</code>
   */
  boolean hasIsStationary();
  /**
   * <code>optional bool is_stationary = 12;</code>
   */
  boolean getIsStationary();

  // optional float bearing_deg = 13;
  /**
   * <code>optional float bearing_deg = 13;</code>
   */
  boolean hasBearingDeg();
  /**
   * <code>optional float bearing_deg = 13;</code>
   */
  float getBearingDeg();

  // optional float x = 14;
  /**
   * <code>optional float x = 14;</code>
   */
  boolean hasX();
  /**
   * <code>optional float x = 14;</code>
   */
  float getX();

  // optional float y = 15;
  /**
   * <code>optional float y = 15;</code>
   */
  boolean hasY();
  /**
   * <code>optional float y = 15;</code>
   */
  float getY();

  // optional float rel_x_dest = 16;
  /**
   * <code>optional float rel_x_dest = 16;</code>
   */
  boolean hasRelXDest();
  /**
   * <code>optional float rel_x_dest = 16;</code>
   */
  float getRelXDest();

  // optional float rel_y_dest = 17;
  /**
   * <code>optional float rel_y_dest = 17;</code>
   */
  boolean hasRelYDest();
  /**
   * <code>optional float rel_y_dest = 17;</code>
   */
  float getRelYDest();

  // optional float speed = 18;
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

  // optional float rel_y_intersect_dest = 19;
  /**
   * <code>optional float rel_y_intersect_dest = 19;</code>
   */
  boolean hasRelYIntersectDest();
  /**
   * <code>optional float rel_y_intersect_dest = 19;</code>
   */
  float getRelYIntersectDest();

  // optional float stat_rel_x = 20;
  /**
   * <code>optional float stat_rel_x = 20;</code>
   */
  boolean hasStatRelX();
  /**
   * <code>optional float stat_rel_x = 20;</code>
   */
  float getStatRelX();

  // optional float stat_rel_y = 21;
  /**
   * <code>optional float stat_rel_y = 21;</code>
   */
  boolean hasStatRelY();
  /**
   * <code>optional float stat_rel_y = 21;</code>
   */
  float getStatRelY();

  // optional float stat_distance = 22;
  /**
   * <code>optional float stat_distance = 22;</code>
   */
  boolean hasStatDistance();
  /**
   * <code>optional float stat_distance = 22;</code>
   */
  float getStatDistance();

  // optional float stat_bearing = 23;
  /**
   * <code>optional float stat_bearing = 23;</code>
   */
  boolean hasStatBearing();
  /**
   * <code>optional float stat_bearing = 23;</code>
   */
  float getStatBearing();

  // optional float mov_rel_x = 24;
  /**
   * <code>optional float mov_rel_x = 24;</code>
   */
  boolean hasMovRelX();
  /**
   * <code>optional float mov_rel_x = 24;</code>
   */
  float getMovRelX();

  // optional float mov_rel_y = 25;
  /**
   * <code>optional float mov_rel_y = 25;</code>
   */
  boolean hasMovRelY();
  /**
   * <code>optional float mov_rel_y = 25;</code>
   */
  float getMovRelY();

  // optional float mov_distance = 26;
  /**
   * <code>optional float mov_distance = 26;</code>
   */
  boolean hasMovDistance();
  /**
   * <code>optional float mov_distance = 26;</code>
   */
  float getMovDistance();

  // optional float mov_bearing = 27;
  /**
   * <code>optional float mov_bearing = 27;</code>
   */
  boolean hasMovBearing();
  /**
   * <code>optional float mov_bearing = 27;</code>
   */
  float getMovBearing();

  // optional float mov_vel_x = 28;
  /**
   * <code>optional float mov_vel_x = 28;</code>
   */
  boolean hasMovVelX();
  /**
   * <code>optional float mov_vel_x = 28;</code>
   */
  float getMovVelX();

  // optional float mov_vel_y = 29;
  /**
   * <code>optional float mov_vel_y = 29;</code>
   */
  boolean hasMovVelY();
  /**
   * <code>optional float mov_vel_y = 29;</code>
   */
  float getMovVelY();

  // optional float mov_speed = 30;
  /**
   * <code>optional float mov_speed = 30;</code>
   */
  boolean hasMovSpeed();
  /**
   * <code>optional float mov_speed = 30;</code>
   */
  float getMovSpeed();
}
