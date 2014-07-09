/**
* @file ImageCoordinateSystem.h
* Declaration of a class that provides transformations on image coordinates.
* Parts of this class were copied from class ImageInfo.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a>
*/

#pragma once

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Matrix2x2.h"
#include "Tools/Streams/AutoStreamable.h"

/**
* @class ImageCoordinateSystemBH
* A class that provides transformations on image coordinates.
*/
STREAMABLE(ImageCoordinateSystemBH,
{
  CameraInfoBH cameraInfo;
  int* xTable;
  int* yTable;
  int table[6144];

public:
  ~ImageCoordinateSystemBH();

  void setCameraInfo(const CameraInfoBH& cameraInfo);

  /**
  * Converts image coordintates into coordinates in the horizon-aligned coordinate system.
  * @param imageCoords The point in image coordinates.
  * @return The point in horizon-aligned coordinates.
  */
  Vector2BH<> toHorizonAligned(const Vector2BH<int>& imageCoords) const
  {
    return invRotation * Vector2BH<>(imageCoords);
  }

  /**
  * Converts image coordintates into coordinates in the horizon-aligned coordinate system.
  * @param imageCoords The point in image coordinates.
  * @return The point in horizon-aligned coordinates.
  */
  Vector2BH<> toHorizonAligned(const Vector2BH<>& imageCoords) const
  {
    return invRotation * imageCoords;
  }

  /**
  * Converts coordinates in the horizon-aligned coordinate system into image coordinates.
  * No clipping is done.
  * @param horizonAlignedCoords The point in horizon-aligned coordinates.
  * @return The point in image coordinates.
  */
  Vector2BH<int> fromHorizonAligned(const Vector2BH<>& horizonAlignedCoords) const
  {
    Vector2BH<> result = rotation * horizonAlignedCoords;
    return Vector2BH<int>(result);
  }

  /**
  * Converts image coordintates into coordinates in the horizon-based coordinate system,
  * i.e. a system of coordinates, in which the origin of the horizon is mapped to (0, 0).
  * @param imageCoords The point in image coordinates.
  * @return The point in horizon-based coordinates.
  */
  Vector2BH<> toHorizonBased(const Vector2BH<int>& imageCoords) const
  {
    return invRotation * (Vector2BH<>(imageCoords) - origin);
  }

  /**
  * Converts image coordintates into coordinates in the horizon-based coordinate system,
  * i.e. a system of coordinates, in which the origin of the horizon is mapped to (0, 0).
  * @param imageCoords The point in image coordinates.
  * @return The point in horizon-based coordinates.
  */
  Vector2BH<> toHorizonBased(const Vector2BH<>& imageCoords) const
  {
    return invRotation * (imageCoords - origin);
  }

  /**
  * Converts coordinates in the horizon-based coordinate system into image coordinates.
  * No clipping is done.
  * @param horizonBasedCoords The point in horizon-based coordinates.
  * @return The point in image coordinates.
  */
  Vector2BH<int> fromHorizonBased(const Vector2BH<>& horizonBasedCoords) const
  {
    Vector2BH<> result = rotation * horizonBasedCoords;
    return Vector2BH<int>(int(result.x + origin.x), int(result.y + origin.y));
  }

  /**
  * Corrects image coordinates so that the distortion resulting from the rolling
  * shutter is compensated.
  * No clipping is done.
  * @param imageCoords The point in image coordinates.
  * @return The corrected point.
  */
  Vector2BH<> toCorrected(const Vector2BH<>& imageCoords) const
  {
    float factor = a + imageCoords.y * b;
    return Vector2BH<>(float(cameraInfo.opticalCenter.x - tan(atan((cameraInfo.opticalCenter.x - imageCoords.x) / cameraInfo.focalLength) - factor * offset.x) * cameraInfo.focalLength),
                     float(cameraInfo.opticalCenter.y + tan(atan((imageCoords.y - cameraInfo.opticalCenter.y) / cameraInfo.focalLength) - factor * offset.y) * cameraInfo.focalLength));
  }

  /**
  * Corrects image coordinates so that the distortion resulting from the rolling
  * shutter is compensated.
  * No clipping is done.
  * @param imageCoords The point in image coordinates.
  * @return The corrected point.
  */
  inline Vector2BH<> toCorrected(const Vector2BH<int>& imageCoords) const
  {
    return toCorrected(Vector2BH<float>(float(imageCoords.x), float(imageCoords.y)));
  }

  /**
  * "Incorrects" image coordinates so that the result contains an approximation of distortion from the rolling shutter.
  * @param coords The point in corrected image coordinates.
  * @return The incorrected point.
  */
  Vector2BH<> fromCorrectedApprox(const Vector2BH<>& coords) const
  {
    float factor = a + cameraInfo.height / 2 * b;
    Vector2BH<> v(factor * offset.x - std::atan((coords.x - cameraInfo.opticalCenter.x) / cameraInfo.focalLength),
                factor * offset.y + std::atan((coords.y - cameraInfo.opticalCenter.y) / cameraInfo.focalLength));
    const static float EPSILON = 0.1f;
    if(v.x < (float) -pi_2 + EPSILON)
      v.x = (float) -pi_2 + EPSILON;
    else if(v.x > pi_2 - EPSILON)
      v.x = pi_2 - EPSILON;

    if(v.y < (float) -pi_2 + EPSILON)
      v.y = (float) -pi_2 + EPSILON;
    else if(v.y > pi_2 - EPSILON)
      v.y = pi_2 - EPSILON;

    return Vector2BH<>(cameraInfo.opticalCenter.x - std::tan(v.x) * cameraInfo.focalLength,
                     cameraInfo.opticalCenter.y + std::tan(v.y) * cameraInfo.focalLength);
  }

  /**
  * "Incorrects" image coordinates so that the result contains an approximation of distortion from the rolling shutter.
  * @param coords The point in corrected image coordinates.
  * @return The incorrected point.
  */
  inline Vector2BH<> fromCorrectedApprox(const Vector2BH<int>& coords) const
  {
    return fromCorrectedApprox(Vector2BH<float>(float(coords.x), float(coords.y)));
  }

  /**
  * "Incorrects" image coordinates using a linearized version of the motion distortion model.
  * @param coords The point in corrected image coordinates.
  * @return The incorrected point.
  */
  Vector2BH<> fromCorrectedLinearized(const Vector2BH<>& coords) const
  {
    const float temp1 = cameraInfo.focalLength * offset.y;
    const float temp2 = 1.0f / (1.0f - temp1 * b);
    return Vector2BH<>(coords.x - cameraInfo.focalLength * offset.x * (a + coords.y * b) * temp2, (coords.y + temp1 * a) * temp2);
  }

  /**
  * "Incorrects" image coordinates using a linearized version of the motion distortion model.
  * @param coords The point in corrected image coordinates.
  * @return The incorrected point.
  */
  Vector2BH<> fromCorrectedLinearized(const Vector2BH<int>& coords) const
  {
    return fromCorrectedLinearized(Vector2BH<>(coords));
  }

  /**
  * Corrects image coordinates using a linearized version of the motion distortion model.
  * @param coords The point in corrected image coordinates.
  * @return The incorrected point.
  */
  Vector2BH<> toCorrectedLinearized(const Vector2BH<>& coords) const
  {
    const float temp = cameraInfo.focalLength * (a + coords.y * b);
    return Vector2BH<>(coords.x + temp * offset.x, coords.y - temp * offset.y);
  }

  /**
  * Corrects image coordinates using a linearized version of the motion distortion model.
  * @param coords The point in corrected image coordinates.
  * @return The incorrected point.
  */
  Vector2BH<> toCorrectedLinearized(const Vector2BH<int>& coords) const
  {
    return toCorrectedLinearized(Vector2BH<>(coords));
  }

  /**
  * Corrects image coordinates so that the distortion resulting from the rolling
  * shutter is compensated.
  * No clipping is done.
  * @param x The x coordinate of the point in image coordinates.
  * @param y The y coordinate of the point in image coordinates.
  * @return The corrected point relative to the image center with negated signs.
  */
  Vector2BH<int> toCorrectedCenteredNeg(int x, int y) const
  {
    float factor = a + y * b;
    x = xTable[x * maxResolutionWidth / cameraInfo.width] - int(factor * offset.x);
    y = yTable[y * maxResolutionHeight / cameraInfo.height] - int(factor * offset.y);
    if(x < -3072)
      x = -3072;
    else if(x > 3071)
      x = 3071;
    if(y < -3072)
      y = -3072;
    else if(y > 3071)
      y = 3071;
    return Vector2BH<int>(table[x + 3072], -table[y + 3072]) * cameraInfo.width / maxResolutionWidth;
  }

  /**
  * Some coordinate system debug drawings.
  */
  void draw() const,

  /**
   * The rotation from a horizon-aligned coordinate system to the image coordinate
   * system. The horizon-aligned coordinate system is defined as follows:
   *  - the x-coordinate points parallel to the horizon to the right
   *  - the y-coordinate points perpendicular to the horizon downwards
   *  - the origin is the top left corner of the image, i.e. the same as the the origin
   *    of the image coordinate system. Thus the transformation from horizon-aligned to
   *    image coordinates only requires the rotation matrix.
   * The direction of the horizon is c[0], the direction downwards is c[1].
   */
  (Matrix2x2BH<>) rotation,
  (Matrix2x2BH<>) invRotation, /**< The rotation from the image coordinates to the horizon-aligned coordinates. */
  (Vector2BH<>) origin, /**< The origin of the horizon in image coordinates. */
  (Vector2BH<>) offset, /**< The offset of the previous image to the current one. */
  (float)(0) a, /**< Constant part of equation to motion distortion. */
  (float)(0) b, /**< Linear part of equation to motion distortion. */

  // Initialization
  xTable = yTable = 0;
});
