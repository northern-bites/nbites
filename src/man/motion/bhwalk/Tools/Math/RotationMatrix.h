/**
* \file RotationMatrix.h
* Delcaration of class RotationMatrixBH
* \author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
* \author <a href="mailto:thomas.kindler@gmx.de">Thomas Kindler</a>
* \author Max Risler
*/

#pragma once

#include "Matrix3x3.h"

/**
 * Representation for 3x3 RotationMatrices
 */
class RotationMatrixBH : public Matrix3x3BH<>
{
protected:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN;
    STREAM_BASE(Matrix3x3BH<>);
    STREAM_REGISTER_FINISH;
  }

public:
  /**
   * Default constructor.
   */
  RotationMatrixBH() {}

  /**
   * Constructor.
   *
   * \param  c0  the first column of the matrix.
   * \param  c1  the second column of the matrix.
   * \param  c2  the third column of the matrix.
   */
  RotationMatrixBH(
    const Vector3BH<>& c0,
    const Vector3BH<>& c1,
    const Vector3BH<>& c2) : Matrix3x3BH<>(c0, c1, c2) {}

  /**
   * Copy constructor.
   *
   * \param  other  The other matrix that is copied to this one
   */
  explicit RotationMatrixBH(const Matrix3x3BH<>& other) : Matrix3x3BH<>(other) {}

  /**
   * Assignment operator.
   *
   * \param  other  The other matrix that is assigned to this one
   * \return        A reference to this object after the assignment.
   */
  RotationMatrixBH& operator=(const Matrix3x3BH<>& other)
  {
    c0 = other.c0;
    c1 = other.c1;
    c2 = other.c2;
    return *this;
  }

  /**
   * RotationMatrixBH from rotation around any axis.
  * \param axis The axis.
   * \param angle The angle to rotate around the axis.
   */
  RotationMatrixBH(const Vector3BH<>& axis, float angle);

  /**
   * RotationMatrixBH from rotation around any axis with an angle given as the length of the axis.
   * \param axis The axis.
   */
  RotationMatrixBH(const Vector3BH<>& axis);

  /**
   * Invert the matrix.
   *
   * \note: Inverted rotation matrix is transposed matrix.
   */
  inline RotationMatrixBH invert() const
  {
    return RotationMatrixBH(
             Vector3BH<>(c0.x, c1.x, c2.x),
             Vector3BH<>(c0.y, c1.y, c2.y),
             Vector3BH<>(c0.z, c1.z, c2.z)
           );
  }

  /**
   * Multiplication of this rotation matrix by another matrix.
   * This function is reimplemented here to avoid hidden Matrix3x3BH<> to RotationMatrixBH (copy) casts.
   * \param  other  The other matrix this one is multiplied by
   * \return        A new matrix containing the result
   *                of the calculation.
  */
  RotationMatrixBH operator*(const Matrix3x3BH<>& other) const
  {
    RotationMatrixBH result;
    result.c0.x = c0.x * other.c0.x + c1.x * other.c0.y + c2.x * other.c0.z;
    result.c0.y = c0.y * other.c0.x + c1.y * other.c0.y + c2.y * other.c0.z;
    result.c0.z = c0.z * other.c0.x + c1.z * other.c0.y + c2.z * other.c0.z;
    result.c1.x = c0.x * other.c1.x + c1.x * other.c1.y + c2.x * other.c1.z;
    result.c1.y = c0.y * other.c1.x + c1.y * other.c1.y + c2.y * other.c1.z;
    result.c1.z = c0.z * other.c1.x + c1.z * other.c1.y + c2.z * other.c1.z;
    result.c2.x = c0.x * other.c2.x + c1.x * other.c2.y + c2.x * other.c2.z;
    result.c2.y = c0.y * other.c2.x + c1.y * other.c2.y + c2.y * other.c2.z;
    result.c2.z = c0.z * other.c2.x + c1.z * other.c2.y + c2.z * other.c2.z;
    return result;
  }

  /**
  * Multiplication of this matrix by vector.
  * This function is reimpletened here since the RotationMatrixBH * Matrix3x3BH<> reimplementation makes Matrix3x3BH<> * Vector3BH<> inaccessible
  * \param  vector  The vector this one is multiplied by
  * \return         A new vector containing the result
  *                 of the calculation.
  */
  Vector3BH<> operator*(const Vector3BH<>& vector) const
  {
    return Vector3BH<>(
             c0.x * vector.x + c1.x * vector.y + c2.x * vector.z,
             c0.y * vector.x + c1.y * vector.y + c2.y * vector.z,
             c0.z * vector.x + c1.z * vector.y + c2.z * vector.z);
  }

  /**
   * Rotation around the x-axis.
   *
   * \param   angle  The angle this pose will be rotated by
   * \return  A reference to this object after the calculation.
   */
  RotationMatrixBH& rotateX(const float angle);

  /**
   * Rotation around the y-axis.
   *
   * \param   angle  The angle this pose will be rotated by
   * \return  A reference to this object after the calculation.
   */
  RotationMatrixBH& rotateY(const float angle);

  /**
   * Rotation around the z-axis.
   *
   * \param   angle  The angle this pose will be rotated by
   * \return  A reference to this object after the calculation.
   */
  RotationMatrixBH& rotateZ(const float angle);

  /**
   * Get the x-angle of a RotationMatrixBH.
   *
   * \return  The angle around the x-axis between the original
   *          and the rotated z-axis projected on the y-z-plane
   */
  float getXAngle() const;

  /**
   * Get the y-angle of a RotationMatrixBH.
   *
   * \return  The angle around the y-axis between the original
   *          and the rotated x-axis projected on the x-z-plane
   */
  float getYAngle() const;

  /**
   * Get the z-angle of a RotationMatrixBH.
   *
   * \return  The angle around the z-axis between the original
   *          and the rotated x-axis projected on the x-y-plane
   */
  float getZAngle() const;

  /**
   * Create and return a RotationMatrixBH, rotated around x-axis
   *
   * \param   angle
   * \return  rotated RotationMatrixBH
   */
  static RotationMatrixBH fromRotationX(const float angle);

  /**
   * Create and return a RotationMatrixBH, rotated around y-axis
   *
   * \param   angle
   * \return  rotated RotationMatrixBH
   */
  static RotationMatrixBH fromRotationY(const float angle);

  /**
   * Create and return a RotationMatrixBH, rotated around z-axis
   *
   * \param   angle
   * \return  rotated RotationMatrixBH
   */
  static RotationMatrixBH fromRotationZ(const float angle);

  /**
  * Converts the rotation matrix into the single vector format.
  * @return The rotation matrix as angleAxis.
  */
  Vector3BH<> getAngleAxis() const;
};
