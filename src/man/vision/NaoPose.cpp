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
using namespace ::messages;
using boost::shared_ptr;
using namespace boost::numeric;
using namespace Kinematics;
using namespace NBMath;
using namespace CoordFrame4D;
using namespace angle;

// From camera docs:
const float NaoPose::IMAGE_WIDTH_MM = 2.45f;
const float NaoPose::IMAGE_HEIGHT_MM = 1.84f;

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

NaoPose::NaoPose() :
    bodyInclinationX(0.0f), bodyInclinationY(0.0f), horizonLeft(0,
                                                                            0),
            horizonRight(0, 0), horizonSlope(0.0f), perpenHorizonSlope(0.0f),
            cameraInWorldFrame(0.0f, 0.0f, 0.0f), comHeight(0.0f) {
}

/**
 * Calculate all forward transformation matrices from the center of mass to each
 * end effector. For the head this is the focal point of the camera. We also
 * calculate the transformation from the camera frame to the world frame.
 * Then we calculate horizon and camera height which is necessary for the
 * calculation of pix estimates.
 */

void NaoPose::transform(bool _isTopCam, const JointAngles& ja, 
			const InertialState& inert) {

    isTopCam = _isTopCam;


    // Make up bogus values
    std::vector<float> headAngles(2, 0.0f);
    std::vector<float> lLegAngles(6, 0.0f);
    std::vector<float> rLegAngles(6, 0.0f);

    std::vector<float> bodyAngles(22);

    bodyAngles[0] = ja.head_yaw();
    bodyAngles[1] = ja.head_pitch();
    bodyAngles[2] = ja.l_shoulder_pitch();
    bodyAngles[3] = ja.l_shoulder_roll();
    bodyAngles[4] = ja.l_elbow_yaw();
    bodyAngles[5] = ja.l_elbow_roll();
    bodyAngles[6] = ja.l_hip_yaw_pitch();
    bodyAngles[7] = ja.l_hip_roll();
    bodyAngles[8] = ja.l_hip_pitch();
    bodyAngles[9] = ja.l_knee_pitch();
    bodyAngles[10] = ja.l_ankle_pitch();
    bodyAngles[11] = ja.l_ankle_roll();
    bodyAngles[12] = ja.r_hip_yaw_pitch();
    bodyAngles[13] = ja.r_hip_roll();
    bodyAngles[14] = ja.r_hip_pitch();
    bodyAngles[15] = ja.r_knee_pitch();
    bodyAngles[16] = ja.r_ankle_pitch();
    bodyAngles[17] = ja.r_ankle_roll();
    bodyAngles[18] = ja.r_shoulder_pitch();
    bodyAngles[19] = ja.r_shoulder_roll();
    bodyAngles[20] = ja.r_elbow_yaw();
    bodyAngles[21] = ja.r_elbow_roll();

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

    ufmatrix4* camera_calibration;
    // factor in camera calibration
    if (_isTopCam) {
        camera_calibration = CameraCalibrate::getTransforms(man::Camera::TOP);
    } else {
        camera_calibration = CameraCalibrate::getTransforms(man::Camera::BOTTOM);
    }
    for (int i = 0; i < CameraCalibrate::NUM_PARAMS; i++) {

        cameraToBodyTransform = prod(cameraToBodyTransform,
                                     camera_calibration[i]);
    }


      // The following basically assumes that we are always standing on our left
      // leg. THIS IS VERY BAD. We NEED to find a way to get support foot 
      // information to pose. Previously, sensors got this from the motion switch
      // board, but now we are trying to avoid that kind of dependency mess, so we need
      // a better way.

      // tl;dr THE FOLLOWING CODE IS BAAAAADDDDDD!!!!!!!!!!!!!!!!!!!
    supportLegToBodyTransform = calculateForwardTransform(LLEG_CHAIN,
							  lLegAngles);

    const ublas::vector<float>
            supportLegLocation(prod(supportLegToBodyTransform, origin));

    // // Now support leg to body is actually world to body. World is explained in
    // // the header.
    supportLegToBodyTransform(X_AXIS, W_AXIS) = 0.0f;
    supportLegToBodyTransform(Y_AXIS, W_AXIS) = 0.0f;
    supportLegToBodyTransform(Z_AXIS, W_AXIS) = 0.0f;

    // // **************************
    // // The code below is old but is kept around just in case the new code does
    // // not work. We used to get the body rotation using the leg transform, but
    // // now we use the accelerometers. The question is whether we should first
    // // apply the x rotation or the y one.
    // // **************************
    // // We need the inverse but we calculate the transpose because they are
    // // equivalent for orthogonal matrices and transpose is faster.
    //  ublas::matrix<float>bodyToWorldTransform=trans(supportLegToBodyTransform);
    // // **************************
    // // End old code
    // // **************************


    // At this time we trust inertial
     bodyInclinationX = inert.angle_x();
     bodyInclinationY = inert.angle_y();

    //cout<<"inertial.x "<<bodyInclinationX<<" y: "<<bodyInclinationY<<endl;

	 ublas::matrix<float> bodyToWorldTransform =
	 	 prod(CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
	 								   bodyInclinationY),
	 		  CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
	 								   bodyInclinationX));

    ublas::vector<float> torsoLocationInLegFrame = prod(bodyToWorldTransform,
                                                        supportLegLocation);
    // get the Z component of the location
    comHeight = -torsoLocationInLegFrame[Z];

    cameraToWorldFrame = prod(bodyToWorldTransform, cameraToBodyTransform);

    calcImageHorizonLine();
    cameraInWorldFrame.x = cameraToWorldFrame(X, 3);
    cameraInWorldFrame.y = cameraToWorldFrame(Y, 3);
    cameraInWorldFrame.z = cameraToWorldFrame(Z, 3);

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
    ublas::permutation_matrix<> P(3);
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
estimate NaoPose::pixEstimate(pixels pixelX, pixels pixelY, mms objectHeight) const {

    // Computed this constant and also checked it experimentally
    // For objects in 320x240-size images ONLY!
    float FOCAL_LENGTH = 290.f;
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

    float alpha = length(pixelInWorldFrame) / pixelInWorldFrame(Z);

    float distance3D = alpha * (cameraInWorldFrame.z + comHeight - objectHeight);
    float distance2D = sqrt(distance3D * distance3D - (cameraInWorldFrame.z + comHeight - objectHeight) *
                            (cameraInWorldFrame.z + comHeight - objectHeight));


    return makeEstimateFrom(pixelInWorldFrame, distance2D, objectHeight);
}

float NaoPose::getDistanceVariance(float distance) {
	return static_cast<float>(0.6499*pow(M_E, 0.0248 * distance));
}

float NaoPose::getBearingVariance(float distance) {
	return max<float>(-0.00002f * distance + 0.0115f, 0);
}

estimate NaoPose::estimateWithKnownDistance(pixels x, pixels y, cms objectHeight, cms groundDist) const {

    if (groundDist <= 0.0) {
        return NULL_ESTIMATE;
    }

    //Do a kinematics estimate to get the bearing
    estimate initialEstimate = this->pixEstimate(x, y, objectHeight);

    estimate newEstimate = initialEstimate;

    newEstimate.dist = groundDist;
    newEstimate.x = groundDist * cos(initialEstimate.bearing);
    newEstimate.y = groundDist * sin(initialEstimate.bearing);

    //TODO: we could probably determine a better elevation for the object as well from
    //knowing the distance

    newEstimate.distance_variance = getDistanceVariance(groundDist);
    newEstimate.bearing_variance = getBearingVariance(groundDist);

    return newEstimate;
}

estimate NaoPose::makeEstimateFrom(ufvector4 pixelInCameraWorldFrame,
                                   mms groundDistance, mms objectHeight) const {

    // The bird's eye view angle between the X axis that originates from the camera and the
    // line that goes through the camera and the pixel
    // You can think of this as the object's bearing, but measured from the camera rather
    // than the CoM
    float beta = NBMath::safe_atan2(pixelInCameraWorldFrame(Y), pixelInCameraWorldFrame(X));

    estimate est;

    // Compute the component distances from the camera, and then add the camera offsets
    // to translate them to the CoM origin
    float distX = groundDistance * cos(beta) + cameraInWorldFrame.x;
    float distY = groundDistance * sin(beta) + cameraInWorldFrame.y;

    est.bearing = NBMath::safe_atan2(distY, distX);

    est.x = distX*MM_TO_CM;
    est.y = distY*MM_TO_CM;
    est.dist = sqrt(est.x * est.x + est.y * est.y);

    //TODO: figure out elevation
//    const float temp2 = -(focalPointInWorldFrame.z + comHeight - objectHeight) / distance3D;
//    if (temp2 <= 1.0)
//        est.elevation = NBMath::safe_asin(temp2);

    // SANITY CHECKS
    //If the target object plane is below the camera height,
    //then we need to make sure that the pixel in world frame is lower than
    //the focal point, or else, we will get odd results, since the point
    //of intersection with that plane will be behind us.
    if (objectHeight < comHeight + cameraInWorldFrame.z && pixelInCameraWorldFrame(Z) >= 0) {
        return NULL_ESTIMATE;
    }

    est.distance_variance = getDistanceVariance(est.dist);
    est.bearing_variance = getBearingVariance(est.dist);

    return est;
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
      if (isTopCam) fullTransform = prod(fullTransform, END_TRANSFORMS_TOP[id][i]);
      else fullTransform = prod(fullTransform, END_TRANSFORMS_BOTTOM[id][i]);
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

NBMath::ufvector3 intersection(ufvector3 a1, ufvector3 a2, ufvector3 b1, ufvector3 b2) {

    int X = 0;
    int Y = 1;

    //Line1
    float A1 = a2(Y) - a1(Y);
    float B1 = a2(X) - a1(X);
    float C1 = A1*a1(X) + B1*a1(Y);

    //Line2
    float A2 = b2(Y) - b1(Y);
    float B2 = b2(X) - b1(X);
    float C2 = A2 * b2(X) + B2 * b2(Y);

    float det = A1*B2 - A2*B1;
    if (det == 0)
    {
        //parallel
        return CoordFrame3D::vector3D(-1, -1);
    }
    else
    {
        float x = (B2*C1 - B1*C2)/det;
        float y = (A1 * C2 - A2 * C1) / det;
        return CoordFrame3D::vector3D(x,y);
    }
}

/**
 * getExpectedVisualLinesFromFieldPosition
 *
 * returns the expected visual lines from a known field position
 * x, y, robotAngle are considered from a bird's eye view perspective
 **/
std::vector<boost::shared_ptr<VisualLine> > NaoPose::getExpectedVisualLinesFromFieldPosition(
        float x, float y, float robotAngle) {

    std::vector<shared_ptr<VisualLine> > visualLines;
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

        ublas::vector <float> linePoint2 = CoordFrame3D::vector3D(
                (**i).getFieldX2(), (**i).getFieldY2());
        //get the line point in the robot coordinate system
        linePoint2 = prod(worldOriginToRobotOriginTranslation, linePoint2);
        linePoint2 = prod(worldToRobotRotation, linePoint2);

        ublas::vector <float> pixel1 = worldPointToPixel(linePoint1);
        ublas::vector <float> pixel2 = worldPointToPixel(linePoint2);

        float FOCAL_LENGTH = 290.0;

        // correct if one point is behind image plane
        if (pixel1(Z) < 0 && pixel2(Z) > 0) {
            pixel1 = ((FOCAL_LENGTH - pixel1(Z)) /
                    (pixel2(Z) - pixel1(Z)))*
                    (pixel1-pixel2) + pixel1;
        } else if (pixel2(Z) < 0 && pixel1(Z) > 0) {
            pixel2 = ((FOCAL_LENGTH - pixel2(Z)) /
                    (pixel1(Z) - pixel2(Z))) *
                    (pixel2-pixel1) + pixel2;
        }

        linePoint visualLinePoint1;
        visualLinePoint1.x = (int) pixel1(X);
        visualLinePoint1.y = (int) pixel1(Y);

        linePoint visualLinePoint2;
        visualLinePoint2.x = (int) pixel2(X);
        visualLinePoint2.y = (int) pixel2(Y);

        list<linePoint> visualLinePoints;
        if (pixel1(X) != 0 && pixel1(Y) != 0)
            visualLinePoints.push_back(visualLinePoint1);
        if (pixel2(X) != 0 && pixel2(Y) != 0)
            visualLinePoints.push_back(visualLinePoint2);
        if (visualLinePoints.size() > 0){
            visualLines.push_back(shared_ptr<VisualLine>(new VisualLine(visualLinePoints)));}
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
    pointVectorInWorldFrame(X) = pointVectorInWorldFrame(X) - cameraInWorldFrame.x;
    pointVectorInWorldFrame(Y) = pointVectorInWorldFrame(Y) - cameraInWorldFrame.y;
    pointVectorInWorldFrame(Z) = pointVectorInWorldFrame(Z) - cameraInWorldFrame.z;

    //now transform the point from camera frame to image frame
    ufmatrix4 cameraToWorldRotation = cameraToWorldFrame;
    cameraToWorldRotation(0, 3) = 0;
    cameraToWorldRotation(1, 3) = 0;
    cameraToWorldRotation(2, 3) = 0;
    pointVectorInWorldFrame = prod(trans(cameraToWorldRotation), pointVectorInWorldFrame);

    float FOCAL_LENGTH = 290.0f;

    //scale to image size
    float t = FOCAL_LENGTH / pointVectorInWorldFrame(X);
    float x = -(t * pointVectorInWorldFrame(Y)) + IMAGE_CENTER_X;
    float y = -(t * pointVectorInWorldFrame(Z)) + IMAGE_CENTER_Y;

    //if t is negatve, then object is behind, cannnot put that in image
//    if (t < 0) {x = 0;  y = 0;}

    return CoordFrame3D::vector3D(x, y);
}

std::vector<radians> NaoPose::headAnglesToRobotPoint(ublas::vector <float> point) {

    NBMath::ufvector3 dest_pixel = this->worldPointToPixel(point);
    float x = dest_pixel(X);
    float y = dest_pixel(Y);
    x -= IMAGE_CENTER_X;
    y -= IMAGE_CENTER_Y;
    static std::vector<radians> headAngles(NUM_JOINTS_CHAIN[HEAD_CHAIN]);
    headAngles[HEAD_YAW] = x*PIX_TO_RAD_X;
    headAngles[HEAD_PITCH] = y*PIX_TO_RAD_Y;
    cout << headAngles[HEAD_YAW]*TO_DEG << " " << headAngles[HEAD_PITCH]*TO_DEG << endl;
    return headAngles;
}

estimate NaoPose::estimateFromObjectSize(pixels pixelX, pixels pixelY, mms objectHeight, float pixelSize, mms realSize) const {

    pixels FOCAL_LENGTH = 290;

    if (pixelSize <= 0 || realSize <= 0)
        return NULL_ESTIMATE;

    // ratio gives a mm/pixel estimate for the object
    float ratio = realSize / pixelSize;

    ufvector4 pixelInCameraFrame =
                vector4D((float) FOCAL_LENGTH,
                         ((float)IMAGE_CENTER_X - (float)pixelX),
                         ((float)IMAGE_CENTER_Y - (float)pixelY));

    float pixelDistance = length(pixelInCameraFrame);

    // distance from the focal point (camera) to the pixel in the image
    // in camera coordinates
    mms distance3D = ratio * pixelDistance;

    mms robotHeight = cameraInWorldFrame.z + comHeight;
    mms deltaHeight = robotHeight - objectHeight;

//    std::cout << "com height" << comHeight <<
//            " distance3D " << distance3D << " robotHeight " << robotHeight << std::endl;

    //safe to assume that if the focal distance is smaller than the height, then the pixel size is too big
    //default back to full kinematics estimate
    if (distance3D < deltaHeight) {
        return pixEstimate(pixelX, pixelY, objectHeight);
    }

    mms distance2D = sqrt(distance3D * distance3D - deltaHeight * deltaHeight);

    ufmatrix4 cameraToWorldRotation = cameraToWorldFrame;
    cameraToWorldRotation(0, 3) = 0;
    cameraToWorldRotation(1, 3) = 0;
    cameraToWorldRotation(2, 3) = 0;

//    cout << "y-rotation " << NBMath::safe_asin(-cameraToWorldRotation(2, 0)) * 180 / M_PI << std::endl;

    //TODO: clean this up and comment
    ufvector4 pixelInWorldFrame = prod(cameraToWorldRotation, pixelInCameraFrame);

    return makeEstimateFrom(pixelInWorldFrame, distance2D, objectHeight);
}

