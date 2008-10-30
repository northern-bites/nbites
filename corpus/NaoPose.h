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
 *  * The camera frame. origin at the focal point, alligned with the head.
 *  * The body frame. origin at the center of mass (com), alligned with the torso.
 *  * The world frame. orgin at the center of mass, alligned with the ground
 *  * The horizon frame. origin at the focal point, alligned with the ground
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

#include <cmath>
#include <vector>
#include <algorithm>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/fwd.hpp>             // for matrix_column
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include <boost/numeric/ublas/io.hpp>              // for cout
using namespace boost::numeric;

#include "Common.h"             // For ROUND
#include "VisionDef.h"          // For camera parameters
#include "Pose.h"               // Base class accepted by anyone with a particular
                                // contract
#include "Kinematics.h"         // For physical body parameters

using namespace Kinematics;

#include "Sensors.h"
#include "Structs.h"

class NaoPose : public Pose {
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

  static const float PIX_TO_DEG_X;
  static const float PIX_TO_DEG_Y;

  static const estimate NULL_ESTIMATE;

  static const float INFTY;
  // Screen edge coordinates in the camera coordinate frame.
  static const ublas::vector <float> topLeft, bottomLeft, topRight, bottomRight;

  // We use this to access the x,y,z components of vectors
  enum Cardinal {
    X = 0,
    Y = 1,
    Z = 2
  };

 public:
  NaoPose(Sensors *s);
  ~NaoPose() { }

  /********** Core Methods **********/
  virtual void transform ();

  virtual const estimate pixEstimate(const int pixelX, const int pixelY,
				     const float objectHeight);
  virtual const estimate bodyEstimate(const int x, const int y, const float dist);

  /********** Getters **********/
  virtual const int getHorizonY(const int x) const;
  virtual const int getHorizonX(const int y) const;
  virtual const point <int> getLeftHorizon() const { return horizonLeft; }
  virtual const point <int> getRightHorizon() const { return horizonRight; }
  virtual const int getLeftHorizonY() const { return horizonLeft.y; }
  virtual const int getRightHorizonY() const { return horizonRight.y; }
  virtual const float getHorizonSlope() const { return horizonSlope; }
  virtual const float getPerpenSlope() const { return perpenHorizonSlope; }

 protected: // helper methods
  static const ublas::matrix <float>
    calculateForwardTransform(const ChainID id,
			      const std::vector <float> &angles);
  static const ublas::vector <float> calcFocalPointInBodyFrame();

  void calcImageHorizonLine();
  // This method solves a system of linear equations and return a 3-d vector
  // in homogeneous coordinates representing the point of intersection
  static ublas::vector <float>
    intersectLineWithXYPlane(const std::vector<ublas::vector <float> > &aLine);
  // In homogeneous coordinates, get the length of a n-dimensional vector.
  static const float getHomLength(const ublas::vector <float> &vec);
  // takes in two sides of a triangle, returns hypotenuse
  
  static const double getHypotenuse(const float x, const float y) {
    return sqrt(pow(x,2)+pow(y,2));
  }

  //returns an 'estimate' object for a homogeneous vector pointing to an
  //object in the world frame
  static estimate getEstimate(ublas::vector <float> objInWorldFrame);

 protected: // members
  Sensors *sensors;
  point <int> horizonLeft, horizonRight;
  float horizonSlope,perpenHorizonSlope;;
  point3 <float> focalPointInWorldFrame;
  float comHeight; // center of mass height in mm
  // In this array we hold the matrix transformation which takes us from the
  // originn to the end of each chain of the body.
  ublas::matrix <float> forwardTransforms[NUM_CHAINS];
  // World frame is defined as the coordinate frame centered at the center of
  // mass and parallel to the ground plane.
  ublas::matrix <float> cameraToWorldFrame;
  // Current hack for better beraing est
  ublas::matrix <float> cameraToBodyTransform;
};

#endif
