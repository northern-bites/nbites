// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include <boost/shared_ptr.hpp>

#include "NaoPose.h"
using namespace std;
using boost::shared_ptr;
using namespace boost::numeric;
using namespace Kinematics;
using namespace NBMath;
using namespace CoordFrame4D;

// From camera docs:
const float NaoPose::IMAGE_WIDTH_MM = 2.36f;
const float NaoPose::IMAGE_HEIGHT_MM = 1.76f;

// Calculated from numbers in camera docs:
const float NaoPose::FOCAL_LENGTH_MM = (float) ((IMAGE_WIDTH_MM / 2)
        / tan(FOV_X / 2));

// e.g. 3 mm * mm_to_pix = 176 pixels
const float NaoPose::MM_TO_PIX_X = IMAGE_WIDTH / IMAGE_WIDTH_MM;
const float NaoPose::MM_TO_PIX_Y = IMAGE_HEIGHT / IMAGE_HEIGHT_MM;
const float NaoPose::PIX_X_TO_MM = 1.0f / MM_TO_PIX_X;
const float NaoPose::PIX_Y_TO_MM = 1.0f / MM_TO_PIX_Y;
const float NaoPose::IMAGE_CENTER_X = (IMAGE_WIDTH - 1) / 2.0f;
const float NaoPose::IMAGE_CENTER_Y = (IMAGE_HEIGHT - 1) / 2.0f;

const float NaoPose::PIX_TO_RAD_X =
        static_cast<float> (FOV_X_DEG / IMAGE_WIDTH) * TO_RAD;
const float NaoPose::PIX_TO_RAD_Y = static_cast<float> (FOV_Y_DEG
        / IMAGE_HEIGHT) * TO_RAD;

const estimate NaoPose::NULL_ESTIMATE;

const float NaoPose::INFTY = 1E+37f;

// Screen edge coordinates in the camera coordinate frame
const ublas::vector<float> NaoPose::topLeft(vector4D(FOCAL_LENGTH_MM,
                                                     IMAGE_WIDTH_MM / 2,
                                                     IMAGE_HEIGHT_MM / 2));

const ublas::vector<float> NaoPose::bottomLeft = vector4D(FOCAL_LENGTH_MM,
                                                          IMAGE_WIDTH_MM / 2,
                                                          -IMAGE_HEIGHT_MM / 2);
const ublas::vector<float> NaoPose::topRight = vector4D(FOCAL_LENGTH_MM,
                                                        -IMAGE_WIDTH_MM / 2,
                                                        IMAGE_HEIGHT_MM / 2);
const ublas::vector<float> NaoPose::bottomRight =
        vector4D(FOCAL_LENGTH_MM, -IMAGE_WIDTH_MM / 2, -IMAGE_HEIGHT_MM / 2);

NaoPose::NaoPose(shared_ptr<Sensors> s) :
    bodyInclinationX(0.0f), bodyInclinationY(0.0f), sensors(s), horizonLeft(0,
                                                                            0),
            horizonRight(0, 0), horizonSlope(0.0f), perpenHorizonSlope(0.0f),
            focalPointInWorldFrame(0.0f, 0.0f, 0.0f), comHeight(0.0f) {
}

/**
 * Calculate all forward transformation matrices from the center of mass to each
 * end effector. For the head this is the focal point of the camera. We also
 * calculate the transformation from the camera frame to the world frame.
 * Then we calculate horizon and camera height which is necessary for the
 * calculation of pix estimates.
 */
void NaoPose::transform() {

    // Make up bogus values
    std::vector<float> headAngles(2, 0.0f);
    std::vector<float> lLegAngles(6, 0.0f);
    std::vector<float> rLegAngles(6, 0.0f);

    std::vector<float> bodyAngles = sensors->getVisionBodyAngles();

    //copy the values into a vector.
    copy(bodyAngles.begin(), bodyAngles.begin() + HEAD_JOINTS,
         headAngles.begin());
    copy(bodyAngles.begin() + HEAD_JOINTS + ARM_JOINTS, bodyAngles.begin()
            + HEAD_JOINTS + ARM_JOINTS + LEG_JOINTS, lLegAngles.begin());
    copy(bodyAngles.begin() + HEAD_JOINTS + ARM_JOINTS + LEG_JOINTS,
         bodyAngles.begin() + HEAD_JOINTS + ARM_JOINTS + 2 * LEG_JOINTS,
         rLegAngles.begin());

    const ublas::vector<float> origin = vector4D(0.0f, 0.0f, 0.0f);

    cameraToBodyTransform = calculateForwardTransform(HEAD_CHAIN, headAngles);
    // factor in camera calibration
    for (int i = 0; i < CameraCalibrate::NUM_PARAMS; i++) {
        cameraToBodyTransform = prod(cameraToBodyTransform,
                                     CameraCalibrate::Transforms[i]);
    }


    ublas::matrix<float> supportLegToBodyTransform;

    //support leg is determined by which leg is further from the body!
    //this should be changed in one or more of the following ways:
    //   * ask the walk engine for the support leg. (doesnt work in cortex)
    //   * ask the gyros/accelerometers for which way is down (doesnt work yet)
    //if (lLegDistance > rLegDistance) {
    if (sensors->getSupportFoot() == LEFT_SUPPORT) {
        supportLegToBodyTransform = calculateForwardTransform(LLEG_CHAIN,
                                                              lLegAngles);
    } else {
        supportLegToBodyTransform = calculateForwardTransform(RLEG_CHAIN,
                                                              rLegAngles);
    }

    const ublas::vector<float>
            supportLegLocation(prod(supportLegToBodyTransform, origin));

    // Now support leg to body is actually world to body. World is explained in
    // the header.
    supportLegToBodyTransform(X_AXIS, W_AXIS) = 0.0f;
    supportLegToBodyTransform(Y_AXIS, W_AXIS) = 0.0f;
    supportLegToBodyTransform(Z_AXIS, W_AXIS) = 0.0f;

    // **************************
    // The code below is old but is kept around just in case the new code does
    // not work. We used to get the body rotation using the leg transform, but
    // now we use the accelerometers. The question is whether we should first
    // apply the x rotation or the y one.
    // **************************
    // We need the inverse but we calculate the transpose because they are
    // equivalent for orthogonal matrices and transpose is faster.
     ublas::matrix<float>bodyToWorldTransform=trans(supportLegToBodyTransform);
    // **************************
    // End old code
    // **************************


    // At this time we trust inertial
    const Inertial inertial = sensors->getInertial();
    bodyInclinationX = inertial.angleX;
    bodyInclinationY = inertial.angleY;

    //cout<<"inertial.x "<<bodyInclinationX<<" y: "<<bodyInclinationY<<endl;

 //   ublas::matrix<float> bodyToWorldTransform =
  //          prod(CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
 //                                         bodyInclinationY),
   //              CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
   //                                           bodyInclinationX));

    ublas::vector<float> torsoLocationInLegFrame = prod(bodyToWorldTransform,
                                                        supportLegLocation);
    // get the Z component of the location
    comHeight = -torsoLocationInLegFrame[Z];

    cameraToWorldFrame = prod(bodyToWorldTransform, cameraToBodyTransform);

    calcImageHorizonLine();
    focalPointInWorldFrame.x = cameraToWorldFrame(X, 3);
    focalPointInWorldFrame.y = cameraToWorldFrame(Y, 3);
    focalPointInWorldFrame.z = cameraToWorldFrame(Z, 3);

    //cout<<"Joints: yaw:"<<headAngles[0]*TO_DEG<<" pitch: "<<headAngles[1]*TO_DEG<<endl;
    //cout<<"comHeight "<<comHeight<<endl;
}

/**
 * Calculates a horizon line for real image via the camera matrix which is a
 * global member of NaoPose. The line is stored as two endpoints on the left and
 * right of the screen in horizonLeft and horizonRight.
 */
void NaoPose::calcImageHorizonLine() {
    // Moving the camera frame to the center of the body lets us compare the
    // rotation of the camera frame relative to the world frame.
    ublas::matrix<float> cameraToHorizonFrame = cameraToWorldFrame;

    cameraToHorizonFrame(X_AXIS, W_AXIS) = 0.0f;
    cameraToHorizonFrame(Y_AXIS, W_AXIS) = 0.0f;
    cameraToHorizonFrame(Z_AXIS, W_AXIS) = 0.0f;

    // We need the inverse but we calculate the transpose because they are
    // equivalent for orthogonal matrices and transpose is faster.
    ublas::matrix<float> horizonToCameraFrame = trans(cameraToHorizonFrame);

    // We defined each edge of the CCD as a line, and solve
    // for where that line intersects the horizon plane ( xy plane level with the
    // ground, at the height of the focal point
    std::vector<ublas::vector<float> > leftEdge, rightEdge;

    leftEdge.push_back(prod(cameraToHorizonFrame, topLeft));
    leftEdge.push_back(prod(cameraToHorizonFrame, bottomLeft));

    rightEdge.push_back(prod(cameraToHorizonFrame, topRight));
    rightEdge.push_back(prod(cameraToHorizonFrame, bottomRight));

    //intersection points in the horizon frame
    ublas::vector<float> intersectionLeft = intersectLineWithXYPlane(leftEdge);
    ublas::vector<float> intersectionRight =
            intersectLineWithXYPlane(rightEdge);

    // Now they are in the camera frame. Result still stored in intersection 1,2
    intersectionLeft = prod(horizonToCameraFrame, intersectionLeft);
    intersectionRight = prod(horizonToCameraFrame, intersectionRight);

    //we are only interested in the height (z axis), not the width

    const float height_mm_left = intersectionLeft(Z);
    const float height_mm_right = intersectionRight(Z);

    const float height_pix_left = -height_mm_left * MM_TO_PIX_Y + IMAGE_HEIGHT
            / 2;
    const float height_pix_right = -height_mm_right * MM_TO_PIX_Y
            + IMAGE_HEIGHT / 2;

    horizonLeft.x = 0;
    horizonLeft.y = ROUND(height_pix_left);
    horizonRight.x = IMAGE_WIDTH - 1;
    horizonRight.y = ROUND(height_pix_right);

    horizonSlope = (float) ((height_pix_right - height_pix_left) / (IMAGE_WIDTH
            - 1.0));
    if (horizonSlope != 0)
        perpenHorizonSlope = -1 / horizonSlope;
    else
        perpenHorizonSlope = INFTY;
}

/**
 * Method to take a vector of two points describing a line, and intersect it with
 * the XYplane of the relevant coordinate frame. Could probably be made faster
 * if dependency on matrix multiplication was removed.
 */
ublas::vector<float> NaoPose::intersectLineWithXYPlane(const std::vector<
        ublas::vector<float> > &aLine) {
    ublas::vector<float> l1 = aLine[0];
    ublas::vector<float> l2 = aLine[1];

    //points on the plane level with the ground in the horizon coord frame
    //normally need 3 points, but since one is the origin, it can get ignored
    ublas::vector<float> unitX = vector4D(1, 0, 0);
    ublas::vector<float> unitY = vector4D(0, 1, 0);

    //we now solve the point of intersection using linear algebra
    //Ax=b, where b is the target, x is the solution of weights (t,u,v)
    //to solve l1 + (l2 -l1)t = o1*u + o2*v
    //Note!: usually a plane is defined by three vectors. e.g. in this case of
    //the target plane goes through the origin of the target
    //frame, so one of the vectors is the zero vector, so we ignore it
    //See http://en.wikipedia.org/wiki/Line-plane_intersection for detail
    ublas::matrix<float> eqSystem(3, 3);
    eqSystem(0, 0) = l1(0) - l2(0);
    eqSystem(0, 1) = unitX(0);
    eqSystem(0, 2) = unitY(0);

    eqSystem(1, 0) = l1(1) - l2(1);
    eqSystem(1, 1) = unitX(1);
    eqSystem(1, 2) = unitY(1);

    eqSystem(2, 0) = l1(2) - l2(2);
    eqSystem(2, 1) = unitX(2);
    eqSystem(2, 2) = unitY(2);

    // Solve for the solution of the weights.
    // Now usually we would solve eqSystem*target = l1 for target, but l1 is
    // defined in homogeneous coordiantes. We need it to be a 3 by 1 vector to
    // solve the system of equations.
    ublas::vector<float> target(3);
    target(0) = l1(0);
    target(1) = l1(1);
    target(2) = l1(2);
    ublas::permutation_matrix<float> P(3);
    // If the matrix is near singular, this value will be != 0
    int singularRow = lu_factorize(eqSystem, P);
    if (singularRow != 0) {
        // The camera is parallel to the ground
        // Since l1 is the top (left/right) of the image, the horizon
        // will be at the top of the screen in this case which works for us.
        return l1;
    }

    ublas::vector<float> result(3);
    result.assign(target);
    lu_substitute(eqSystem, P, result);
    float t = result(0);

    //the first variable in the linear equation was t, so it appears at the top of
    //the vector 'result'. The 't' is such that the point l1 + (l2 -l1)t is on
    //the horizon plane
    //NOTE: this intersection is still in the horizon frame though
    ublas::vector<float> intersection = l2 - l1;
    intersection *= t;
    intersection += l1;

    // The intersection seems to currently have the wrong y coordinate. It's the
    // negative of what it should be.

    return intersection;
}

/**
 * Method to determine where the physical point represented by a pixel is relative
 * to the world frame.
 */
const estimate NaoPose::pixEstimate(const int pixelX, const int pixelY,
                                    const float objectHeight) {

    /*if (pixelX >= IMAGE_WIDTH || pixelX < 0 || pixelY >= IMAGE_HEIGHT || pixelY
            < 0) {
        return NULL_ESTIMATE;
    }
    */
    /*
    // declare x,y,z coordinate of pixel in relation to focal point
    ublas::vector<float> pixelInCameraFrame =
            vector4D(FOCAL_LENGTH_MM, ((float) IMAGE_CENTER_X - (float) pixelX)
                    * (float) PIX_X_TO_MM, ((float) IMAGE_CENTER_Y
                    - (float) pixelY) * (float) PIX_Y_TO_MM);

    // declare x,y,z coordinate of pixel in relation to body center
    ublas::vector<float> pixelInWorldFrame(4);

    // transform camera coordinates to body frame coordinates for a test pixel
    pixelInWorldFrame = prod(cameraToWorldFrame, pixelInCameraFrame);

    // Draw the line between the focal point and the pixel while in the world
    // frame. Our goal is to find the point of intersection of that line and
    // the plane, parallel to the ground, passing through the object height.
    // In most cases, this plane is the ground plane, which is comHeight below the
    // origin of the world frame. If we call this method with objectHeight != 0,
    // then the plane is at a different height.
    float object_z_in_world_frame = -comHeight + objectHeight * CM_TO_MM;

    // We are going to parameterize the line with one variable t. We find the t
    // for which the line goes through the plane, then evaluate the line at t for
    // the x,y,z coordinate
    float t = 0;

    // calculate t knowing object_z_in_body_frame
    if ((focalPointInWorldFrame.z - pixelInWorldFrame(Z)) != 0) {
        t = (object_z_in_world_frame - pixelInWorldFrame(Z))
                / (focalPointInWorldFrame.z - pixelInWorldFrame(Z));
    }

    const float x = pixelInWorldFrame(X) + (focalPointInWorldFrame.x
            - pixelInWorldFrame(X)) * t;
    const float y = pixelInWorldFrame(Y) + (focalPointInWorldFrame.y
            - pixelInWorldFrame(Y)) * t;
    const float z = pixelInWorldFrame(Z) + (focalPointInWorldFrame.z
            - pixelInWorldFrame(Z)) * t;
    ublas::vector<float> objectInWorldFrame = vector4D(x, y, z);
*/
    float FOCAL_LENGTH = 385.54f;
    ufvector4 pixelInCameraFrame =
            vector4D( FOCAL_LENGTH,
                      ((float)IMAGE_CENTER_X - (float)pixelX),
                      ((float)IMAGE_CENTER_Y - (float)pixelY));

    ufmatrix4 cameraToWorldRotation = cameraToWorldFrame;
    cameraToWorldRotation(0, 3) = 0;
    cameraToWorldRotation(1, 3) = 0;
    cameraToWorldRotation(2, 3) = 0;
    //TODO:clean this up and comment
    ufvector4 pixelInWorldFrame = prod(cameraToWorldRotation, pixelInCameraFrame);

    float beta = atan(pixelInWorldFrame(Y)/pixelInWorldFrame(X));

    float alpha = sqrt((pixelInWorldFrame(X)*pixelInWorldFrame(X)) + (pixelInWorldFrame(Y)*pixelInWorldFrame(Y))
                      + (pixelInWorldFrame(Z)*pixelInWorldFrame(Z))) / pixelInWorldFrame(Z);

    float distance3D = alpha * (focalPointInWorldFrame.z + comHeight - objectHeight);
    float distance2D = sqrt(distance3D * distance3D - (focalPointInWorldFrame.z + comHeight - objectHeight) *
                            (focalPointInWorldFrame.z + comHeight - objectHeight));

    estimate est;
//    est.bearing = beta;
    //est.dist = distance2D * MM_TO_CM;

    float distX = distance2D * cos(beta) + focalPointInWorldFrame.x;
    float distY = distance2D * sin(beta) + focalPointInWorldFrame.y;

    float bearing = 0.0f;
    const bool yPos = pixelInWorldFrame(Y) >= 0;
    const bool xPos = pixelInWorldFrame(X) >= 0;
    const float r = distY / distX;
    if (!isnan(r)) {
        //quadrants +x,+y and +x-y
        if (xPos && (yPos || !yPos)) {
            bearing = std::atan(r);
        } else if (yPos) { //quadrant -x+y
            bearing = std::atan(r) + M_PI_FLOAT;
        } else {//quadrant -x+y
            bearing = std::atan(r) - M_PI_FLOAT;
        }
    } else {
        bearing = 0.0f;
    }

    est.bearing = bearing;

    est.x = distX*MM_TO_CM;
    est.y = distY*MM_TO_CM;
    est.dist = sqrt(est.x * est.x + est.y * est.y);

    //TODO: this is prolly not right
    const float temp2 = -(focalPointInWorldFrame.z + comHeight - objectHeight) / distance3D;
    if (temp2 <= 1.0)
        est.elevation = NBMath::safe_asin(temp2);

    // SANITY CHECKS
    //If the plane where the target object is, is below the camera height,
    //then we need to make sure that the pixel in world frame is lower than
    //the focal point, or else, we will get odd results, since the point
    //of intersection with that plane will be behind us.
    if (objectHeight * CM_TO_MM < comHeight + focalPointInWorldFrame.z
            && pixelInWorldFrame(Z) > focalPointInWorldFrame.z) {
        return NULL_ESTIMATE;
    }

    //estimate est = getEstimate(objectInWorldFrame);
    //est.dist = correctDistance(static_cast<float> (est.dist));

    est.distance_variance = getDistanceVariance(est.dist);
    est.bearing_variance = getBearingVariance(est.dist);

    return est;
}

float NaoPose::getDistanceVariance(float distance) {
	return static_cast<float>(0.6499*pow(M_E, 0.0248 * distance));
}

float NaoPose::getBearingVariance(float distance) {
	return max<float>(-0.00002f * distance + 0.0115f, 0);
}

/**
 * Body estimate takes a pixel on the screen, and a vision calculated
 * distance to that pixel, and calculates where that pixel is relative
 * to the world frame.  It then returns an estimate to that position,
 * with units in cm.
 */
const estimate NaoPose::bodyEstimate(const int x, const int y, const float dist) {
    if (dist <= 0.0)
        return NULL_ESTIMATE;

    //all angle signs are according to right hand rule for the major axis
    // get bearing angle in image plane,left pos, right negative
    float object_bearing = (IMAGE_CENTER_X - (float) x) * PIX_TO_RAD_X;
    // get elevation angle in image plane, up negative, down is postive
    float object_elevation = ((float) y - IMAGE_CENTER_Y) * PIX_TO_RAD_Y;
    // convert dist estimate to mm
    float object_dist = dist * 10;

    // object in the camera frame
    ublas::vector<float> objectInCameraFrame = vector4D(object_dist
            * cos(object_bearing) * cos(-object_elevation), object_dist
            * sin(object_bearing), object_dist * cos(object_bearing)
            * sin(-object_elevation));

    // object in world frame
    ublas::vector<float> objectInWorldFrame = prod(cameraToWorldFrame,
                                                   objectInCameraFrame);

    return getEstimate(objectInWorldFrame);
}

/* OBSOLETE (yay)
const float NaoPose::correctDistance(const float uncorrectedDist) {
    if (uncorrectedDist > 706.0f) {
        return uncorrectedDist - 387.0f;
    }
    return -0.000591972f * uncorrectedDist * uncorrectedDist + 0.858283f
            * uncorrectedDist + 2.18768F;
}
*/

/**
 * Method to populate an estimate with an vector4D in homogenous coordinates.
 *
 * Input units are MM, output in estimate is in CM, radians
 *
 */
estimate NaoPose::getEstimate(ublas::vector<float> objInWorldFrame) {
    estimate pix_est;

    //distance as projected onto XY plane - ie bird's eye view

    pix_est.dist = getHypotenuse(objInWorldFrame(X), objInWorldFrame(Y))
            * MM_TO_CM;

    // calculate in radians the bearing to the object from the center of the body
    // since trig functions can't handle 2 Pi, we need to differentiate
    // by quadrant:

    const bool yPos = objInWorldFrame(Y) >= 0;
    const bool xPos = objInWorldFrame(X) >= 0;
    const float temp = objInWorldFrame(Y) / objInWorldFrame(X);
    if (!isnan(temp)) {
        //quadrants +x,+y and +x-y
        if (xPos && (yPos || !yPos)) {
            pix_est.bearing = std::atan(temp);
        } else if (yPos) { //quadrant -x+y
            pix_est.bearing = std::atan(temp) + M_PI_FLOAT;
        } else {//quadrant -x+y
            pix_est.bearing = std::atan(temp) - M_PI_FLOAT;
        }
    } else {
        pix_est.bearing = 0.0f;
    }

    pix_est.x = objInWorldFrame(X) * MM_TO_CM;
    pix_est.y = objInWorldFrame(Y) * MM_TO_CM;

    //need dist in 3D for angular elevation, not birdseye
    float dist3D = getHomLength(objInWorldFrame); //in MM
    const float temp2 = objInWorldFrame(Z) / dist3D;
    if (temp2 <= 1.0)
        pix_est.elevation = NBMath::safe_asin(temp2);

    return pix_est;
}

//TODO: test this (untested as of now)
const float NaoPose::getDistanceBetweenTwoObjects(estimate e1, estimate e2) {

    float x = e1.x - e2.x;
    float y = e1.y - e2.y;
    return sqrt(x * x + y * y);
}

const ublas::matrix<float> NaoPose::calculateForwardTransform(const ChainID id,
                                                              const std::vector<
                                                                      float> &angles) {
    ublas::matrix<float> fullTransform = ublas::identity_matrix<float>(4);

    // Do base transforms
    const int numBaseTransforms = NUM_BASE_TRANSFORMS[id];
    for (int i = 0; i < numBaseTransforms; i++) {
        fullTransform = prod(fullTransform, BASE_TRANSFORMS[id][i]);
    }

    // Do mDH transforms
    const int numTransforms = NUM_JOINTS_CHAIN[id];
    for (int i = 0; i < numTransforms; i++) {
        // Right before we do a transformation, we are in the correct coordianate
        // frame and we need to store it, so we know where all the links of a
        // chain are. We only need to do this if the transformation gives us a new
        // link
        const float *currentmDHParameters = MDH_PARAMS[id];

        //length L - movement along the X(i-1) axis
        if (currentmDHParameters[i * 4 + L] != 0) {
            const ublas::matrix<float>
                    transX = CoordFrame4D::translation4D(currentmDHParameters[i
                            * 4 + L], 0.0f, 0.0f);
            fullTransform = prod(fullTransform, transX);
        }

        //twist: - rotate about the X(i-1) axis
        if (currentmDHParameters[i * 4 + ALPHA] != 0) {
            const ublas::matrix<float>
                    rotX = CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                                                    currentmDHParameters[i * 4
                                                            + ALPHA]);
            fullTransform = prod(fullTransform, rotX);
        }
        //theta - rotate about the Z(i) axis
        if (currentmDHParameters[i * 4 + THETA] + angles[i] != 0) {
            const ublas::matrix<float>
                    rotZ = CoordFrame4D::rotation4D(CoordFrame4D::Z_AXIS,
                                                    currentmDHParameters[i * 4
                                                            + THETA]
                                                            + angles[i]);
            fullTransform = prod(fullTransform, rotZ);
        }
        //offset D movement along the Z(i) axis
        if (currentmDHParameters[i * 4 + D] != 0) {
            const ublas::matrix<float>
                    transZ = CoordFrame4D::translation4D(0.0f, 0.0f,
                                                         currentmDHParameters[i
                                                                 * 4 + D]);
            fullTransform = prod(fullTransform, transZ);
        }
    }

    // Do the end transforms
    const int numEndTransforms = NUM_END_TRANSFORMS[id];
    for (int i = 0; i < numEndTransforms; i++) {
        fullTransform = prod(fullTransform, END_TRANSFORMS[id][i]);
    }

    return fullTransform;
}

const float NaoPose::getHomLength(const ublas::vector<float> &vec) {
    float sum = 0.0f;
    for (ublas::vector<float>::const_iterator i = vec.begin(); i != vec.end()
            - 1; ++i) {
        sum += *i * *i;
    }
    return sqrt(sum);
}

// returns the y coord for a given x coord on the horizon line
const int NaoPose::getHorizonY(const int x) const {
    return (int) (horizonLeft.y + (int) (horizonSlope * (float) x));
}

// returns the x coord for a given y coord on the horizon line
const int NaoPose::getHorizonX(const int y) const {
    return (int) (((float) y - (float) horizonLeft.y) / horizonSlope);
}

// Return the distance to the object based on the image magnification of its
// height
const float NaoPose::pixHeightToDistance(float pixHeight, float cmHeight) const {
    return (FOCAL_LENGTH_MM / (pixHeight * PIX_Y_TO_MM)) * cmHeight;
}

// Return the distance to the object based on the image magnification of its
// height
const float NaoPose::pixWidthToDistance(float pixWidth, float cmWidth) const {
    return (FOCAL_LENGTH_MM / (pixWidth * PIX_X_TO_MM)) * cmWidth;
}

/**
 * getExpectedVisualLinesFromFieldPosition
 *
 * returns the expected visual lines from a known field position
 * x, y, robotAngle are considered from a bird's eye view perspective
 **/
std::vector<VisualLine> NaoPose::getExpectedVisualLinesFromFieldPosition(
        float x, float y, float robotAngle) {

    std::vector<VisualLine> visualLines;
    //translation from the world origin to the robot origin
    ublas::matrix <float> worldOriginToRobotOriginTranslation =
            CoordFrame3D::translation3D(-x, -y);
    //rotation around the angle the robot is oriented relative to the world origin
    ublas::matrix <float> worldToRobotRotation =
            CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS, robotAngle);

    for (vector <const ConcreteLine*>::const_iterator i = ConcreteLine::concreteLines().begin();
            i != ConcreteLine::concreteLines().end(); i++) {

        ublas::vector <float> linePoint1 = CoordFrame3D::vector3D(
                (**i).getFieldX1(), (**i).getFieldY1());
        //get the line point in the robot coordinate system
        linePoint1 = prod(worldOriginToRobotOriginTranslation, linePoint1);
        linePoint1 = prod(worldToRobotRotation, linePoint1);

        ublas::vector <float> pixel1 = worldPointToPixel(linePoint1);
        linePoint visualLinePoint1;
        visualLinePoint1.x = (int) pixel1(X);
        visualLinePoint1.y = (int) pixel1(Y);

        ublas::vector <float> linePoint2 = CoordFrame3D::vector3D(
                (**i).getFieldX2(), (**i).getFieldY2());
        //get the line point in the robot coordinate system
        linePoint2 = prod(worldOriginToRobotOriginTranslation, linePoint2);
        linePoint2 = prod(worldToRobotRotation, linePoint2);

        ublas::vector <float> pixel2 = worldPointToPixel(linePoint2);
        linePoint visualLinePoint2;
        visualLinePoint2.x = (int) pixel2(X);
        visualLinePoint2.y = (int) pixel2(Y);

        list<linePoint> visualLinePoints;
        if (pixel1(X) != 0 && pixel1(Y) != 0)
            visualLinePoints.push_back(visualLinePoint1);
        if (pixel2(X) != 0 && pixel2(Y) != 0)
            visualLinePoints.push_back(visualLinePoint2);
        if (visualLinePoints.size() > 0){
            VisualLine visualLine = VisualLine(visualLinePoints);
            visualLines.push_back(visualLine);}
    }

    return visualLines;
}


/**
 * worldPointToPixel
 * gets a point from the world coordinate and transforms it into a pixel in the camera image
 * @params point - the coordinates of the point in a vector
 * @return image coordinates
 **/
const ublas::vector <float> NaoPose::worldPointToPixel(ublas::vector <float> point)
{

    ublas::vector <float> pointVectorInWorldFrame =
            CoordFrame4D::vector4D(point(X) * CM_TO_MM, point(Y) * CM_TO_MM, -comHeight);
    //transform it from the world frame to the camera frame
    pointVectorInWorldFrame(X) = pointVectorInWorldFrame(X) - focalPointInWorldFrame.x;
    pointVectorInWorldFrame(Y) = pointVectorInWorldFrame(Y) - focalPointInWorldFrame.y;
    pointVectorInWorldFrame(Z) = pointVectorInWorldFrame(Z) - focalPointInWorldFrame.z;

    //now transform the point from camera frame to image frame
    ufmatrix4 cameraToWorldRotation = cameraToWorldFrame;
    cameraToWorldRotation(0, 3) = 0;
    cameraToWorldRotation(1, 3) = 0;
    cameraToWorldRotation(2, 3) = 0;
    pointVectorInWorldFrame = prod(trans(cameraToWorldRotation), pointVectorInWorldFrame);

    float FOCAL_LENGTH = 385.54f;

    //scale to image size
    float t = FOCAL_LENGTH / pointVectorInWorldFrame(X);
    float x = -(t * pointVectorInWorldFrame(Y)) + IMAGE_CENTER_X;
    float y = -(t * pointVectorInWorldFrame(Z)) + IMAGE_CENTER_Y;

    //if t is negatve, then object is behind, cannnot put that in image
    if (t < 0) {x = 0;  y = 0;}

    return CoordFrame3D::vector3D(x, y);
}

const estimate NaoPose::sizeBasedEstimate(int pixelX, int pixelY, float objectHeight, float pixelSize, float realSize) {

    float PIXEL_FOCAL_LENGTH = 385.54f;
    if (pixelSize <= 0 || realSize <= 0)
        return NULL_ESTIMATE;
    float ratio = realSize / pixelSize;
    //cout<<"ratio "<<ratio<<endl;

    ufvector4 pixelInCameraFrame =
                vector4D( PIXEL_FOCAL_LENGTH,
                          ((float)IMAGE_CENTER_X - (float)pixelX),
                          ((float)IMAGE_CENTER_Y - (float)pixelY));

    float pixelDistance = length(pixelInCameraFrame);

    float distance3D = ratio * pixelDistance;

    float distance2D = 0.0f;
    //safe to assume that if the focal distance is smaller than the height, then the ball is a few pixel to big
    if (distance3D < focalPointInWorldFrame.z + comHeight - objectHeight) {
        return pixEstimate(pixelX, pixelY, objectHeight);
    }

    distance2D = sqrt(abs(distance3D * distance3D - (focalPointInWorldFrame.z + comHeight - objectHeight) *
                          (focalPointInWorldFrame.z + comHeight - objectHeight)));
    //cout<<"shit "<<distance3D<<" "<<(focalPointInWorldFrame.z + comHeight - objectHeight)<<endl;

    ufmatrix4 cameraToWorldRotation = cameraToWorldFrame;
    cameraToWorldRotation(0, 3) = 0;
    cameraToWorldRotation(1, 3) = 0;
    cameraToWorldRotation(2, 3) = 0;
    //TODO: clean this up and comment
    ufvector4 pixelInWorldFrame = prod(cameraToWorldRotation, pixelInCameraFrame);

    float beta = atan(pixelInWorldFrame(Y)/pixelInWorldFrame(X));

    float distX = distance2D * cos(beta) + focalPointInWorldFrame.x;
    float distY = distance2D * sin(beta) + focalPointInWorldFrame.y;

    //    float bearing = atan(pixelInWorldFrame(Y) / pixelInWorldFrame(X));
    estimate est;
    //est.dist = distance2D * MM_TO_CM;

    float bearing = 0.0f;
    const bool yPos = distY >= 0;
    const bool xPos = distX >= 0;
    const float r = distY / distX;
    if (!isnan(r)) {
        //quadrants +x,+y and +x-y
        if (xPos && (yPos || !yPos)) {
            bearing = std::atan(r);
        } else if (yPos) { //quadrant -x+y
            bearing = std::atan(r) + M_PI_FLOAT;
        } else {//quadrant -x+y
            bearing = std::atan(r) - M_PI_FLOAT;
        }
    } else {
        bearing = 0.0f;
    }

    est.bearing = bearing;


    //TODO: this is prolly not important
    const float temp2 = (focalPointInWorldFrame.z + comHeight - objectHeight) / distance3D;
    if (temp2 <= 1.0) {
        est.elevation = NBMath::safe_asin(temp2);
    }
    est.x = distX*MM_TO_CM;
    est.y = distY*MM_TO_CM;
    est.dist = sqrt(est.x * est.x + est.y * est.y);

    return est;
}

