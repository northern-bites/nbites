/**
 * @file Pose2D.h
 * Contains class Pose2DBH
 *
 * @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
 * @author Max Risler
 */

#pragma once

#include "Vector2.h"

template <class T> class RangeBH;

/** representation for 2D Transformation and Position (Location + Orientation)*/
class Pose2DBH : public Streamable
{
protected:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN;
    STREAM(rotation);
    STREAM(translation);
    STREAM_REGISTER_FINISH;
  }
public:

  /** Rotation as an angle*/
  float rotation;

  /** translation as an vector2*/
  Vector2BH<> translation;

  /** noargs-constructor*/
  Pose2DBH(): rotation(0), translation(0, 0) {}

  /** constructor from rotation and translation
   * \param rotation rotation (float)
   * \param translation translation (Vector2BH)
   */
  Pose2DBH(const float rotation, const Vector2BH<>& translation): rotation(rotation), translation(translation) {}

  /** constructor from rotation and translation
   * \param rot rotation (float)
   * \param x translation.x (float)
   * \param y translation.y (float)
   */
  Pose2DBH(const float rot, const float x, const float y): rotation(rot), translation(x, y)
  {}

  /** constructor from rotation
   * \param rotation rotation (float)
   */
  Pose2DBH(const float rotation): rotation(rotation), translation(0, 0) {}

  /** constructor from translation
   * \param translation translation (Vector2BH)
   */
  Pose2DBH(const Vector2BH<>& translation): rotation(0), translation(translation) {}

  /** constructor from translation
   * \param translation translation (Vector2BH)
   */
  Pose2DBH(const Vector2BH<int>& translation): rotation(0), translation((float) translation.x, (float) translation.y) {}

  /** constructor from two translation values
   * \param x translation x component
   * \param y translation y component
   */
  Pose2DBH(const float x, const float y): rotation(0), translation(x, y) {}

  /** Assignment operator
  *\param other The other Pose2DBH that is assigned to this one
  *\return A reference to this object after the assignment.
  */
  Pose2DBH& operator=(const Pose2DBH& other)
  {
    rotation = other.rotation;
    translation = other.translation;
    return *this;
  }

  /** Copy constructor
  *\param other The other vector that is copied to this one
  */
  Pose2DBH(const Pose2DBH& other) {*this = other;}

  /** Multiplication of a Vector2BH with this Pose2DBH.
   *
   * Same as (point.rotate(Pose2DBH.rotation) + Pose2DBH.translation)
   *
  *\param point The Vector2BH that will be multiplicated with this Pose2DBH
  *\return The resulting Vector2BH
  */
  Vector2BH<> operator*(const Vector2BH<>& point) const
  {
    float s = std::sin(rotation);
    float c = std::cos(rotation);
    return (Vector2BH<>(point.x * c - point.y * s , point.x * s + point.y * c) + translation);
  }

  /** Comparison of another pose with this one.
  *\param other The other pose that will be compared to this one
  *\return Whether the two poses are equal.
  */
  bool operator==(const Pose2DBH& other) const
  {
    return ((translation == other.translation) && (rotation == other.rotation));
  }

  /** Comparison of another pose with this one.
  *\param other The other pose that will be compared to this one
  *\return Whether the two poses are unequal.
  */
  bool operator!=(const Pose2DBH& other) const
  {return !(*this == other);}

  /**Concatenation of this pose with another pose.
  *\param other The other pose that will be concatenated to this one.
  *\return A reference to this pose after concatenation.
  */
  Pose2DBH& operator+=(const Pose2DBH& other)
  {
    translation = *this * other.translation;
    rotation += other.rotation;
    rotation = normalizeBH(rotation);
    return *this;
  }

  /**A concatenation of this pose and another pose.
  *\param other The other pose that will be concatenated to this one.
  *\return The resulting pose.
  */
  Pose2DBH operator+(const Pose2DBH& other) const
  {return Pose2DBH(*this) += other;}

  /**Difference of this pose relative to another pose. So if A+B=C is the addition/concatenation, this calculates C-A=B.
  *\param other The other pose that will be used as origin for the new pose.
  *\return A reference to this pose after calculating the difference.
  */
  Pose2DBH& operator-=(const Pose2DBH& other)
  {
    translation -= other.translation;
    Pose2DBH p(-other.rotation);
    return *this = p + *this;
  }

  /**Difference of this pose relative to another pose.
  *\param other The other pose that will be used as origin for the new pose.
  *\return The resulting pose.
  */
  Pose2DBH operator-(const Pose2DBH& other) const
  {return Pose2DBH(*this) -= other;}

  /**Concatenation of this pose with another pose
  *\param other The other pose that will be concatenated to this one.
  *\return A reference to this pose after concatenation
  */
  Pose2DBH& conc(const Pose2DBH& other)
  {return *this += other;}

  /**Translate this pose by a translation vector
  *\param trans Vector to translate with
  *\return A reference to this pose after translation
  */
  Pose2DBH& translate(const Vector2BH<>& trans)
  {
    translation = *this * trans;
    return *this;
  }

  /**Translate this pose by a translation vector
  *\param x x component of vector to translate with
  *\param y y component of vector to translate with
  *\return A reference to this pose after translation
  */
  Pose2DBH& translate(const float x, const float y)
  {
    translation = *this * Vector2BH<>(x, y);
    return *this;
  }


  /**Rotate this pose by a rotation
  *\param angle Angle to rotate.
  *\return A reference to this pose after rotation
  */
  Pose2DBH& rotate(const float angle)
  {
    rotation += angle;
    return *this;
  }


  /** Calculates the inverse transformation from the current pose
  * @return The inverse transformation pose.
  */
  Pose2DBH invert() const
  {
    const float& invRotation = -rotation;
    return Pose2DBH(invRotation, (Vector2BH<>() - translation).rotate(invRotation));
  }

  /**
  * The function creates a random pose.
  * @param x The range for x-values of the pose.
  * @param y The range for y-values of the pose.
  * @param angle The range for the rotation of the pose.
  */
  static Pose2DBH random(const RangeBH<float>& x, const RangeBH<float>& y, const RangeBH<float>& angle);
};
