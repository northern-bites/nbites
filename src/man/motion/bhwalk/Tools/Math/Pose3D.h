/**
* @file Pose3D.h
* Contains class Pose3DBH
* @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
* @author Max Risler
*/

#pragma once

#include "RotationMatrix.h"

/** representation for 3D Transformation (Location + Orientation)*/
class Pose3DBH : public Streamable
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

  /** rotation as a RotationMatrixBH*/
  RotationMatrixBH rotation;

  /** translation as a Vector3BH*/
  Vector3BH<> translation;

  /** constructor*/
  inline Pose3DBH() {}

  /** constructor from rotation and translation
   * \param rot Rotation
   * \param trans Translation
   */
  inline Pose3DBH(const RotationMatrixBH& rot, const Vector3BH<>& trans) : rotation(rot), translation(trans) {}

  /** constructor from rotation
   * \param rot Rotation
   */
  inline Pose3DBH(const RotationMatrixBH& rot) : rotation(rot) {}

  /** constructor from translation
   * \param trans Translation
   */
  inline Pose3DBH(const Vector3BH<>& trans) : translation(trans) {}

  /** constructor from three translation values
   * \param x translation x component
   * \param y translation y component
   * \param z translation z component
   */
  inline Pose3DBH(const float x, const float y, const float z) : translation(x, y, z) {}

  /** Copy constructor
  *\param other The other vector that is copied to this one
  */
  inline Pose3DBH(const Pose3DBH& other) : rotation(other.rotation), translation(other.translation) {}

  /** Assignment operator
  *\param other The other Pose3DBH that is assigned to this one
  *\return A reference to this object after the assignment.
  */
  inline Pose3DBH& operator=(const Pose3DBH& other)
  {
    rotation = other.rotation;
    translation = other.translation;
    return *this;
  }

  /** Multiplication with Point
  *\param point (Vector3BH&lt;float&gt;)
  */
  inline Vector3BH<> operator*(const Vector3BH<>& point) const
  {
    return rotation * point + translation;
  }

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are equal.
  */
  inline bool operator==(const Pose3DBH& other) const
  {
    return translation == other.translation && rotation == other.rotation;
  }

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are unequal.
  */
  inline bool operator!=(const Pose3DBH& other) const
  {
    return translation != other.translation || rotation != other.rotation;
  }

  /**Concatenation of this pose with another pose
  *\param other The other pose that will be concatenated to this one.
  *\return A reference to this pose after concatenation
  */
  inline Pose3DBH& conc(const Pose3DBH& other)
  {
    translation = *this * other.translation;
    rotation *= other.rotation;
    return *this;
  }

  /** Calculates the inverse transformation from the current pose
  * @return The inverse transformation pose.
  */
  inline Pose3DBH invert() const
  {
    Pose3DBH result;
    result.rotation = rotation.invert();
    result.translation = result.rotation * (-translation);
    return result;
  }

  /**Translate this pose by a translation vector
  *\param trans Vector to translate with
  *\return A reference to this pose after translation
  */
  inline Pose3DBH& translate(const Vector3BH<>& trans)
  {
    translation = *this * trans;
    return *this;
  }

  /**Translate this pose by a translation vector
  *\param x x component of vector to translate with
  *\param y y component of vector to translate with
  *\param z z component of vector to translate with
  *\return A reference to this pose after translation
  */
  inline Pose3DBH& translate(const float x, const float y, const float z)
  {
    translation = *this * Vector3BH<>(x, y, z);
    return *this;
  }

  /**Rotate this pose by a rotation
  *\param rot Rotationmatrix to rotate with
  *\return A reference to this pose after rotation
  */
  inline Pose3DBH& rotate(const RotationMatrixBH& rot)
  {
    rotation *= rot;
    return *this;
  }

  /**Rotate this pose around its x-axis
  *\param angle angle to rotate with
  *\return A reference to this pose after rotation
  */
  inline Pose3DBH& rotateX(const float angle)
  {
    rotation.rotateX(angle);
    return *this;
  }

  /**Rotate this pose around its y-axis
  *\param angle angle to rotate with
  *\return A reference to this pose after rotation
  */
  inline Pose3DBH& rotateY(const float angle)
  {
    rotation.rotateY(angle);
    return *this;
  }

  /**Rotate this pose around its z-axis
  *\param angle angle to rotate with
  *\return A reference to this pose after rotation
  */
  inline Pose3DBH& rotateZ(const float angle)
  {
    rotation.rotateZ(angle);
    return *this;
  }
};
