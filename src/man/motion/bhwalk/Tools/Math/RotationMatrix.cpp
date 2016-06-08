/**
* @file RotationMatrixBH.cpp
* Implementation of class RotationMatrixBH
* @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
* @author Max Risler
*/

#include "RotationMatrix.h"

using namespace std;

RotationMatrixBH::RotationMatrixBH(const Vector3BH<>& a, float angle)
{
  Vector3BH<> axis = a;
  const float axisLen = axis.abs();
  if(axisLen != 0.f)
    axis *= 1.f / axisLen; // normalizeBH a, rotation is only possible with unit vectors
  const float& x = axis.x, &y = axis.y, &z = axis.z;
  //compute sine and cosine of angle because it is needed quite often for complete matrix
  const float si = sin(angle), co = cos(angle);
  //compute all components needed more than once for complete matrix
  const float v = 1 - co;
  const float xyv = x * y * v;
  const float xzv = x * z * v;
  const float yzv = y * z * v;
  const float xs = x * si;
  const float ys = y * si;
  const float zs = z * si;
  //compute matrix
  c0.x = x * x * v + co;
  c1.x = xyv - zs;
  c2.x = xzv + ys;
  c0.y = xyv + zs;
  c1.y = y * y * v + co;
  c2.y = yzv - xs;
  c0.z = xzv - ys;
  c1.z = yzv + xs;
  c2.z = z * z * v + co;
}

RotationMatrixBH::RotationMatrixBH(const Vector3BH<>& a)
{
  const float angle = a.abs();
  Vector3BH<> axis = a;
  if(angle != 0.f)
    axis *= 1.f / angle; // normalizeBH a, rotation is only possible with unit vectors
  const float& x = axis.x, &y = axis.y, &z = axis.z;
  //compute sine and cosine of angle because it is needed quite often for complete matrix
  const float si = sin(angle), co = cos(angle);
  //compute all components needed more than once for complete matrix
  const float v = 1 - co;
  const float xyv = x * y * v;
  const float xzv = x * z * v;
  const float yzv = y * z * v;
  const float xs = x * si;
  const float ys = y * si;
  const float zs = z * si;
  //compute matrix
  c0.x = x * x * v + co;
  c1.x = xyv - zs;
  c2.x = xzv + ys;
  c0.y = xyv + zs;
  c1.y = y * y * v + co;
  c2.y = yzv - xs;
  c0.z = xzv - ys;
  c1.z = yzv + xs;
  c2.z = z * z * v + co;
}

RotationMatrixBH& RotationMatrixBH::rotateX(const float angle)
{
  /*
  const float c = cos(angle), s = sin(angle);
  *this = RotationMatrixold(this->c[0], this->c[1] * c + this->c[2] * s, this->c[2] * c - this->c[1] * s);
  return *this;
  */

  const float c = cos(angle), s = sin(angle);
  const float c1X = c1.x, c1Y = c1.y, c1Z = c1.z;
  c1.x = c1X * c + c2.x * s;
  c1.y = c1Y * c + c2.y * s;
  c1.z = c1Z * c + c2.z * s;
  c2.x = c2.x * c - c1X * s;
  c2.y = c2.y * c - c1Y * s;
  c2.z = c2.z * c - c1Z * s;
  return *this;
}

RotationMatrixBH& RotationMatrixBH::rotateY(const float angle)
{
  /*
  const float c = cos(angle), s = sin(angle);
  *this = RotationMatrixBH(this->c0 * c - this->c2 * s, this->c1, this->c2 * c + this->c0 * s);
  return *this;
  */

  const float c = cos(angle), s = sin(angle);
  const float c0X = c0.x, c0Y = c0.y, c0Z = c0.z;
  c0.x = c0X * c - c2.x * s;
  c0.y = c0Y * c - c2.y * s;
  c0.z = c0Z * c - c2.z * s;
  c2.x = c2.x * c + c0X * s;
  c2.y = c2.y * c + c0Y * s;
  c2.z = c2.z * c + c0Z * s;
  return *this;
}

RotationMatrixBH& RotationMatrixBH::rotateZ(const float angle)
{
  /*
  const float c = cos(angle), s = sin(angle);
  *this = RotationMatrixBH(this->c0 * c + this->c1 * s, this->c1 * c - this->c0 * s, this->c2);
  return *this;
  */

  const float c = cos(angle), s = sin(angle);
  const float c0X = c0.x, c0Y = c0.y, c0Z = c0.z;
  c0.x = c0X * c + c1.x * s;
  c0.y = c0Y * c + c1.y * s;
  c0.z = c0Z * c + c1.z * s;
  c1.x = c1.x * c - c0X * s;
  c1.y = c1.y * c - c0Y * s;
  c1.z = c1.z * c - c0Z * s;
  return *this;
}

float RotationMatrixBH::getXAngle() const
{
  const float h = sqrt(c2.y * c2.y + c2.z * c2.z);
  return h != 0.f ? acos(c2.z / h) * (c2.y > 0 ? -1 : 1) : 0;
}

float RotationMatrixBH::getYAngle() const
{
  const float h = sqrt(c0.x * c0.x + c0.z * c0.z);
  return h != 0.f ? acos(c0.x / h) * (c0.z > 0 ? -1 : 1) : 0;
}

float RotationMatrixBH::getZAngle() const
{
  const float h = sqrt(c0.x * c0.x + c0.y * c0.y);
  return h != 0.f ? acos(c0.x / h) * (c0.y < 0 ? -1 : 1) : 0;
}

Vector3BH<> RotationMatrixBH::getAngleAxis() const
{
  float co = (c0.x + c1.y + c2.z - 1.f) * 0.5f;
  if(co > 1.f)
    co = 1.f;
  else if(co < -1.f)
    co = 1.f;
  const float angle = acos(co);
  if(angle == 0.f)
    return Vector3BH<>();
  Vector3BH<> result(
    c1.z - c2.y,
    c2.x - c0.z,
    c0.y - c1.x);
  result *= angle / (2.f * sin(angle));
  return result;
}

RotationMatrixBH RotationMatrixBH::fromRotationX(const float angle)
{
  const float c = cos(angle), s = sin(angle);
  return RotationMatrixBH(Vector3BH<>(1.f, 0.f, 0.f), Vector3BH<>(0.f, c, s), Vector3BH<>(0.f, -s, c));
}

RotationMatrixBH RotationMatrixBH::fromRotationY(const float angle)
{
  const float c = cos(angle), s = sin(angle);
  return RotationMatrixBH(Vector3BH<>(c, 0.f, -s), Vector3BH<>(0.f, 1.f, 0.f), Vector3BH<>(s, 0.f, c));
}

RotationMatrixBH RotationMatrixBH::fromRotationZ(const float angle)
{
  const float c = cos(angle), s = sin(angle);
  return RotationMatrixBH(Vector3BH<>(c, s, 0.f), Vector3BH<>(-s, c, 0.f), Vector3BH<>(0.f, 0.f, 1.f));
}
