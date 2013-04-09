
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


/**
 * NaoPose: The class that is responsible for calculating the pose of the robot
 * in order that the vision system can tell where the objects it sees are located
 * with respect to the center of the body.
 *
 * To understand this file, please review the modified Denavit Hartenberg
 * convention on robotics coordinate frame transformations, as well as the
 * following documentation:
 *  * Nao Camera documentation
 *  * NaoWare documentation
 *  * HONG NUBots Enhancements to vision 2005 (for Horizon)
 *  * Wiki line-plane intersection:  en.wikipedia.org/wiki/Line-plane_intersection
 *
 * The following coordinate frames will be important:
 *  * The camera frame. origin at the focal point, aligned with the head.
 *  * The camera world frame. origin at the focal point, aligned with the ground
 *  * The body frame. origin at the center of mass (com), aligned with the torso
 *  * The world frame. orgin at the center of mass, aligned with the ground
 *  * The horizon frame. origin at the focal point, aligned with the ground
 *
 * The following methods are central to the functioning of NaoPose:
 *  * tranform()     - must be called each frame. setups transformation matrices
 *                     for the legs and the camera, which enables calculation of
 *                     body height and horizon
 *
 *  * pixEstimate()  - returns an estimate to a given x,y pixel, representing an
 *                     object at a certain height from the ground. Takes untis of
 *                     CM, and returns in CM. See also bodyEstimate
 *
 *  * bodyEstimate() - returns an estimate for a given x,y pixel, and a distance
 *                     calculated from vision by blob size. See also pixEstimate
 *
 *
 *  * Performance Profile: mDH for 3 chains takes 70% time in tranform(), and
 *                         horizon caculation takes the other 30%. pixEstimate()
 *                         and bodyEstimate() take two orders of magnitude less
 *                         time than transform()
 *  * Optimization: Eventually we'll calculate the world frame rotation using
 *                  gyros, so we won't need to use the support leg's orientation
 *                  in space to figure this out. That will save quite a bit of
 *                  computation.
 *
 * @date July 2008
 * @author George Slavov
 * @author Johannes Strom
 *
 * @date June 2010
 * @modified
 * @author Octavian Neamtu
 *
 * TODO:
 *   - Need to fix the world frame, since it currently relies on the rotation of
 *     the foot also about the Z axis, which means when we are turning, for
 *     example, we will report distances and more imporantly bearings relative
 *     to the foot instead of the body. This has been hacked for now, in
 *     bodyEstimate.
 *
 *   - While testing this code on the robot, we saw a significant, though
 *     infrequent, jump in the estimate for the horizon line. The cause could be
 *     one of bad joint information from the motion engine, a singular matrix
 *     inversion we do somewhere in the code, or something completely different.
 *     Needs to be looked at.
 *
 *   - While testing this code on the robot, we have noticed severe
 *     discrepancies between distances reported from pixEstimate, and their
 *     actual values. While the model seems to work well in general, there
 *     needs to be a serious evaluation of these defects, since these
 *     inaccuracies will have serious consequences for localization.
 *
 */

#ifndef _NaoPose_h_DEFINED
#define _NaoPose_h_DEFINED

#include <math.h>
#include <vector>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/fwd.hpp>             // for matrix_column
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include <boost/numeric/ublas/io.hpp>              // for cout

#include "Common.h"               // For ROUND
#include "VisionDef.h"           // For camera parameters
#include "Kinematics.h"         // For physical body parameters
#include "CameraCalibrate.h"   //for camera calibraton, go figure
#include "VisualLine.h" //for visual lines, helps in calibration

#include "Structs.h"

#include "JointAngles.pb.h"
#include "InertialState.pb.h"


namespace man {
namespace vision {
/**
 *  *****************   Various constants follow   **********************
 */


// CONSTANTS

struct mDHParam{
    float xRot; //xRot
    float xTrans; //xTrans
    float zRot; //zRot
    float zTrans; //zTrans

};

class NaoPose {
 protected: // Constants
    static const float IMAGE_WIDTH_MM;
    static const float IMAGE_HEIGHT_MM;
    static const float FOCAL_LENGTH_MM;
    static const float MM_TO_PIX_X;
    static const float MM_TO_PIX_Y;
    static const float PIX_X_TO_MM;
    static const float PIX_Y_TO_MM;
    static const float IMAGE_CENTER_X;
    static const float IMAGE_CENTER_Y;

    static const float PIX_TO_RAD_X;
    static const float PIX_TO_RAD_Y;

    static const estimate NULL_ESTIMATE;

    static const float INFTY;
    // Screen edge coordinates in the camera coordinate frame.
    static const boost::numeric::ublas::vector <float> topLeft, bottomLeft, topRight, bottomRight;

    // We use this to access the x,y,z components of vectors
    enum Cardinal {
        X = 0,
        Y = 1,
        Z = 2
    };

 public:
    NaoPose();
    ~NaoPose() { }

    /********** Core Methods **********/
	void angles(const messages::JointAngles& ja, const messages::InertialState& inert);
    void transform (bool _isTopCam, const messages::JointAngles& ja, 
		    const messages::InertialState& inert);

    /**
     * Takes a pixel and the height orthogonal from the ground
     * of the point that the pixel represents in real world
     *
     * @params
     * pixelX, pixelY - the coordinates of the pixel, where 0,0
     * is the top left corner of the image (the standard way images are
     * mapped)
     *
     * objectHeight - the height of the point represented by the pixel
     * in !!MM!!
     *
     * @returns
     * an estimate of the distance to the point (and the bearing)
     * all estimates are in CM
     */
    estimate pixEstimate(pixels pixelX, pixels pixelY, mms objectHeight) const;


    /**
     * Returns an estimate to a pixel in the image which represents the bottom
     * of an object of known real-world size
     *
     * Note: because of the noise in estimates, objectHeight can usually considered
     * to be 0 if small enough
     *
     * Note: this assumes the image and the real object are in parallel planes
     * (usually true of posts and the ball, which is what we should use this for)
     */
    estimate estimateFromObjectSize(pixels pixelX, pixels pixelY, mms objectHeight,
                                    float pixelSize, mms realSize) const;


    /**
     * Returns an estimate to a pixel in the image to which we know the ground (2D world) distance
     * (a.k.a the distance from the robot's CoM projection on the ground to that pixel)
     * and the height of that pixel from the ground
     */
    estimate estimateWithKnownDistance(pixels x, pixels y, mms objectHeight, cms groundDist) const;

    /**
     * Takes a pixel in the camera world frame and the known ground (world frame) distance
     * to that pixel and extracts the bearing and the distance components;
     *
     * Computes the variance for the distance and the bearing
     *
     * Used internally by NaoPose
     */
    estimate makeEstimateFrom(ufvector4 pixelInCameraWorldFrame,
                              mms groundDistance,
                              mms objectHeight) const;

    /********** Getters **********/
    const int getHorizonY(const int x) const;
    const int getHorizonX(const int y) const;
    const point <int> getLeftHorizon() const { return horizonLeft; }
    const point <int> getRightHorizon() const { return horizonRight; }
    const int getLeftHorizonY() const { return horizonLeft.y; }
    const int getRightHorizonY() const { return horizonRight.y; }
    const float getHorizonSlope() const { return horizonSlope; }
    const float getPerpenSlope() const { return perpenHorizonSlope; }
    const float getHeadYaw() {
/* MUST FIX */return 1;//sensors->getVisionAngle(Kinematics::HEAD_YAW);
    }
    const float getDistanceBetweenTwoObjects(estimate e1, estimate e2);
    std::vector<boost::shared_ptr<VisualLine> > getExpectedVisualLinesFromFieldPosition(float x, float y, float robotAngle);
    const boost::numeric::ublas::vector <float> worldPointToPixel(boost::numeric::ublas::vector <float> point);
    std::vector<angle::radians> headAnglesToRobotPoint(boost::numeric::ublas::vector <float> point);

    const float getHeadPitch() {
/* MUST FIX */return 1;//sensors->getVisionAngle(Kinematics::HEAD_PITCH);
    }
    const float getBodyCenterHeight() const { return comHeight; }
    const float getFocalPointInWorldFrameZ() const { return cameraInWorldFrame.z;}
    const float getFocalPointInWorldFrameX() const { return cameraInWorldFrame.x;}
    const float getFocalPointInWorldFrameY() const { return cameraInWorldFrame.y;}

 protected: // helper methods
    const boost::numeric::ublas::matrix <float>
        calculateForwardTransform(const Kinematics::ChainID id,
                                  const std::vector <float> &angles);
    static const boost::numeric::ublas::vector <float>
        calcFocalPointInBodyFrame();

    void calcImageHorizonLine();
    // This method solves a system of linear equations and return a 3-d vector
    // in homogeneous coordinates representing the point of intersection
    static boost::numeric::ublas::vector <float>
        intersectLineWithXYPlane(const std::vector<
                                 boost::numeric::ublas::vector <float> > &aLine);
    // In homogeneous coordinates, get the length of a n-dimensional vector.
    static const float
        getHomLength(const boost::numeric::ublas::vector <float> &vec);

    // takes in two sides of a triangle, returns hypotenuse
    static const float getHypotenuse(const float x, const float y) {
        return std::sqrt(x*x + y*y);
    }

    // Variance as a function of distance
    // computed by Yoni Ackerman (c) 2011
    // to update - see the one file that Yoni added
    // to robobiteS
    static float getDistanceVariance(float distance);
    static float getBearingVariance(float distance);

 protected: // members
    float bodyInclinationX;
    float bodyInclinationY;
    point <int> horizonLeft, horizonRight;
    float horizonSlope,perpenHorizonSlope;
    point3 <float> cameraInWorldFrame;
    boost::numeric::ublas::matrix<float> supportLegToBodyTransform;
    float comHeight; // center of mass height in mm
    // In this array we hold the matrix transformation which takes us from the
    // originn to the end of each chain of the body.
    boost::numeric::ublas::matrix <float>
        forwardTransforms[Kinematics::NUM_CHAINS];
    // World frame is defined as the coordinate frame centered at the center of
    // mass and parallel to the ground plane.
    boost::numeric::ublas::matrix <float> cameraToWorldFrame;
    // Current hack for better beraing est
    boost::numeric::ublas::matrix <float> cameraToBodyTransform;

    bool isTopCam; //true = using the top camera
};

}
}

#endif
