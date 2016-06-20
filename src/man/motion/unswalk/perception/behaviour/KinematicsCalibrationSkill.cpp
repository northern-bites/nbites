#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "perception/behaviour/KinematicsCalibrationSkill.hpp"
#include "types/RRCoord.hpp"
#include "blackboard/Blackboard.hpp"
#include "types/ActionCommand.hpp"
#include "perception/kinematics/UNSWKinematics.hpp"
#include "perception/kinematics/Pose.hpp"
#include "utils/Logger.hpp"
#include "FADBAD++/fadiff.h"

fadbad::F<float> fadbadAtan2(fadbad::F<float> y, fadbad::F<float> x) {
   fadbad::F<float> PI = 3.1415926535;
   if (x == 0) {
      if (y > 0) {
         return PI / 2;
      } else if (y < 0) {
         return -PI / 2;
      } else {
         return 0;
      }
   } else if (x > 0) {
      return fadbad::atan(y / x);
   } else if (y >= 0 && x < 0) {
      return fadbad::atan(y / x) + PI;
   } else if (y < 0 && x < 0) {
      return fadbad::atan(y / x) - PI;
   } else {
      return 0;
   }
}

KinematicsCalibrationSkill::KinematicsCalibrationSkill(Blackboard *bb) : Adapter(bb) {
   points.push_back(std::make_pair(-3020 + 1200, -2025));
   points.push_back(std::make_pair(1200, -2025));
   points.push_back(std::make_pair(1200, 0));
   points.push_back(std::make_pair(1200, 2025));
   points.push_back(std::make_pair(-3020 + 1200, 2025));
   currentWaypoint = 0;
   beenAtFrameFor = 0;
   takenReading = false;
   resetGradients();
   alpha = MAX_ALPHA;
   isTop = false;
}

KinematicsCalibrationSkill::~KinematicsCalibrationSkill() {}

/* Input
      - Joint Angles
      - Offsets
      - Perceived Location of Object in the Image

   Output
      - Euclidian distance between Perceived and Actual Position

   Processing Steps
      - Set up FADBAD variables for each of the offsets
      - Feed 3 Inputs into Kinematics (May have to change some of the
        the interfaces
      - Deterime Euclidian distance
*/

fadbad::F<float> KinematicsCalibrationSkill::objectiveFunction(
   Parameters<fadbad::F<float> > &parameters) {
   UNSWKinematics kinematics;
   kinematics.sensorValues = readFrom(motion, sensors);
   kinematics.parameters = parameters.cast<float>();

   kinematics.updateDHChain();
   Pose pose = kinematics.getPose();


   /* Image to RR */
   Point cc_p = readFrom(vision, balls[0].imageCoords);
   std::pair<uint16_t, uint16_t>  cc(cc_p.x(), cc_p.y());

   // calculate vector to pixel in camera space
   boost::numeric::ublas::matrix<fadbad::F<float> > lOrigin, lOrigin2;
   fadbad::F<float> imgCols = IMAGE_COLS;
   fadbad::F<float> imgRows = IMAGE_ROWS;
   fadbad::F<float> pixelSize = TOP_PIXEL_SIZE; // TODO: properly if used
   fadbad::F<float> x = cc.first;
   fadbad::F<float> y = cc.second;
   lOrigin2 = vec4<fadbad::F<float> >(
      (((imgCols) / 2.0 - x) *  pixelSize),
      (((imgRows) / 2.0 - y) * pixelSize),
      0,
      1);

   lOrigin = prod(
      pose.getC2wTransform(),
      lOrigin2);

   boost::numeric::ublas::matrix<fadbad::F<float> > toPixel, toPixel2;
   toPixel2 = vec4<fadbad::F<float> >(0, 0, FOCAL_LENGTH, 1);
   toPixel = prod(
      pose.getC2wTransform(),
      toPixel2);

   boost::numeric::ublas::matrix<fadbad::F<float> > cdir(toPixel - lOrigin);

   fadbad::F<float> lambda = (35 - lOrigin(2, 0)) / (1.0 * cdir(2, 0));
   boost::numeric::ublas::matrix<fadbad::F<float> > intercept;
   intercept = vec4<fadbad::F<float> >(lOrigin(0, 0) + lambda * cdir(0, 0),
                                       lOrigin(1, 0) + lambda * cdir(1, 0),
                                       35,
                                       1);
   fadbad::F<float> distance, heading;
   distance = fadbad::sqrt(fadbad::pow(intercept(0, 0), 2) +
                           fadbad::pow(intercept(1, 0), 2));
   heading = fadbadAtan2(intercept(1, 0), intercept(0, 0));
   fadbad::F<float> px, py;
   px = fadbad::cos(heading) * distance;
   py = fadbad::sin(heading) * distance;

   // find which point is closest to the one we see
   float tx, ty;
   tx = px.x();
   ty = py.x();

   int minIndex = 0;
   float rx, ry;
   rx = points[0].first;
   ry = points[0].second;
   float bDistance = sqrt(pow(rx - tx, 2) + pow(ry - ty, 2));
   for (unsigned int i = 1; i < points.size(); i++) {
      rx = points[i].first;
      ry = points[i].second;
      float cDistance = sqrt(pow(rx - tx, 2) + pow(ry - ty, 2));
      if (cDistance < bDistance) {
         bDistance = cDistance;
         minIndex = i;
      }
   }

   fadbad::F<float> bx, by;
   bx = points[minIndex].first;
   by = points[minIndex].second;
   return fadbad::sqrt(fadbad::pow(bx - px, 2) +
                       fadbad::pow(by - py, 2));
}

void KinematicsCalibrationSkill::updateGradient() {
   // read in the parameters
   Parameters<fadbad::F<float> > parameters;
   parameters.cameraYawBottom = readFrom(kinematics,
                                         parameters.cameraYawBottom);
   parameters.cameraPitchBottom = readFrom(kinematics,
                                           parameters.cameraPitchBottom);
   parameters.cameraRollBottom = readFrom(kinematics,
                                          parameters.cameraRollBottom);
   parameters.cameraYawTop = readFrom(kinematics,
                                      parameters.cameraYawTop);
   parameters.cameraPitchTop = readFrom(kinematics,
                                        parameters.cameraPitchTop);
   parameters.cameraRollTop = readFrom(kinematics,
                                       parameters.cameraRollTop);
   parameters.bodyPitch = readFrom(kinematics, parameters.bodyPitch);
   lastParams = parameters;

   // set them to be derived
   parameters.cameraYawBottom.diff(0, 7);
   parameters.cameraPitchBottom.diff(1, 7);
   parameters.cameraRollBottom.diff(2, 7);

   parameters.cameraYawTop.diff(3, 7);
   parameters.cameraPitchTop.diff(4, 7);
   parameters.cameraRollTop.diff(5, 7);

   parameters.bodyPitch.diff(6, 7);

   // calculate objective function
   fadbad::F<float> f = objectiveFunction(parameters);

   value += f.x();
   n += 1;
   // store gradients
   gradients.cameraYawBottom += f.d(0);
   gradients.cameraPitchBottom += f.d(1);
   gradients.cameraRollBottom += f.d(2);
   gradients.cameraYawTop += f.d(3);
   gradients.cameraPitchTop += f.d(4);
   gradients.cameraRollTop += f.d(5);

   gradients.bodyPitch += f.d(6);
}

BehaviourRequest KinematicsCalibrationSkill::execute() {
   BehaviourRequest request;
   if (readFrom(vision, balls).size() > 0 &&
       beenAtFrameFor > STABALIZE_FRAMES) {
      updateGradient();
      takenReading = true;
   }

   // we have gathered point.size number of samples. update and get new batch
   if (true && currentWaypoint == points.size() - 1 &&
       takenReading == true) {
      Parameters<fadbad::F<float> > parameters;
      parameters.cameraYawBottom = readFrom(kinematics,
                                            parameters.cameraYawBottom);
      parameters.cameraPitchBottom = readFrom(kinematics,
                                              parameters.cameraPitchBottom);
      parameters.cameraRollBottom = readFrom(kinematics,
                                             parameters.cameraRollBottom);
      parameters.cameraYawTop = readFrom(kinematics,
                                         parameters.cameraYawTop);
      parameters.cameraPitchTop = readFrom(kinematics,
                                           parameters.cameraPitchTop);
      parameters.cameraRollTop = readFrom(kinematics,
                                          parameters.cameraRollTop);
      parameters.bodyPitch = readFrom(kinematics, parameters.bodyPitch);

      // optimization algorithm goes here...
      gradientDescent(parameters, gradients);

      // write values back for next iteration
      writeTo(kinematics, parameters.cameraYawBottom,
              parameters.cameraYawBottom.x());
      writeTo(kinematics, parameters.cameraPitchBottom,
              parameters.cameraPitchBottom.x());
      writeTo(kinematics, parameters.cameraRollBottom,
              parameters.cameraRollBottom.x());
      writeTo(kinematics, parameters.cameraYawTop,
              parameters.cameraYawTop.x());
      writeTo(kinematics, parameters.cameraPitchTop,
              parameters.cameraPitchTop.x());
      writeTo(kinematics, parameters.cameraRollTop,
              parameters.cameraRollTop.x());

      writeTo(kinematics, parameters.bodyPitch, parameters.bodyPitch.x());
      resetGradients();
      isTop = !isTop;
   }

   std::pair<float, float> cwp = points[currentWaypoint];
   float angle = atan2(cwp.second, cwp.first);
   if (abs(readFrom(motion, sensors).joints.angles[Joints::HeadYaw] - angle) < .01) {
      beenAtFrameFor++;
   }

   if (takenReading == true) {
      beenAtFrameFor = 0;
      currentWaypoint = (currentWaypoint + 1) % points.size();
      takenReading = false;
   }

   // move head to correct spot
   if (isTop) {
      request.actions.head = ActionCommand::Head(angle,
                                                 DEG2RAD(5), false, 0.5f, 0.5f);
   } else {
      request.actions.head = ActionCommand::Head(angle,
                                                 DEG2RAD(-25), false, 0.5f, 0.5f);
   }
   request.actions.body = ActionCommand::Body::STAND;

   return request;
}

void KinematicsCalibrationSkill::resetGradients() {
   n = 0;
   value = 0;
   gradients.cameraYawBottom = 0;
   gradients.cameraPitchBottom = 0;
   gradients.cameraRollBottom = 0;
   gradients.cameraYawTop = 0;
   gradients.cameraPitchTop = 0;
   gradients.cameraRollTop = 0;

   gradients.bodyPitch = 0;
}


bool KinematicsCalibrationSkill::gradientDescent(
   Parameters<fadbad::F<float> > &parameters,
   Parameters<fadbad::F<float> > gradients) {
   parameters.cameraYawBottom -= alpha * gradients.cameraYawBottom / n;
   parameters.cameraPitchBottom -= alpha * gradients.cameraPitchBottom / n;
   parameters.cameraRollBottom -= alpha * gradients.cameraRollBottom / n;
   parameters.cameraYawTop -= alpha * gradients.cameraYawTop / n;
   parameters.cameraPitchTop -= alpha * gradients.cameraPitchTop / n;
   parameters.cameraRollTop -= alpha * gradients.cameraRollTop / n;
   parameters.bodyPitch -= alpha * gradients.bodyPitch / n;
   alpha -= 0.0005;
   if (alpha < MIN_ALPHA) {
      alpha = MIN_ALPHA;
   }

   //static int t = 0;
   std::cout << printParams(parameters) << std::endl;
   std::cout << "Gradients: ";
   std::cout << gradients.cameraYawBottom.x() << " " <<
   gradients.cameraPitchBottom.x() << " " <<
   gradients.cameraRollBottom.x() << " " <<
   gradients.bodyPitch.x() << std::endl;
   std::cout << "Value: " << value / n << std::endl;
   std::cout << "Alpha: " << alpha << std::endl;
   std::cout << std::endl;

   return false;
}

std::string KinematicsCalibrationSkill::printParams(
   Parameters<fadbad::F<float> > &parameters) {
   std::stringstream s;
   std::vector<std::pair<std::string, float> > plist;
   plist.push_back(std::make_pair(
                      "cameraYawBottom", parameters.cameraYawBottom.x()));
   plist.push_back(std::make_pair(
                      "cameraPitchBottom", parameters.cameraPitchBottom.x()));
   plist.push_back(std::make_pair(
                      "cameraRollBottom", parameters.cameraRollBottom.x()));
   plist.push_back(std::make_pair(
                      "cameraYawTop", parameters.cameraYawTop.x()));
   plist.push_back(std::make_pair(
                      "cameraPitchTop", parameters.cameraPitchTop.x()));
   plist.push_back(std::make_pair(
                      "cameraRollTop", parameters.cameraRollTop.x()));


   plist.push_back(std::make_pair(
                      "bodyPitch", parameters.bodyPitch.x()));

   for (unsigned int i = 0; i < plist.size(); i++) {
      s << plist[i].first << "=" << plist[i].second << std::endl;
   }
   return s.str();
}

std::string KinematicsCalibrationSkill::printParams(
   Parameters<float> &parameters) {
   std::stringstream s;
   std::vector<std::pair<std::string, float> > plist;
   plist.push_back(std::make_pair(
                      "cameraYawBottom", parameters.cameraYawBottom));
   plist.push_back(std::make_pair(
                      "cameraPitchBottom", parameters.cameraPitchBottom));
   plist.push_back(std::make_pair(
                      "cameraRollBottom", parameters.cameraRollBottom));
   plist.push_back(std::make_pair(
                      "cameraYawTop", parameters.cameraYawTop));
   plist.push_back(std::make_pair(
                      "cameraPitchTop", parameters.cameraPitchTop));
   plist.push_back(std::make_pair(
                      "cameraRollTop", parameters.cameraRollTop));

   plist.push_back(std::make_pair(
                      "bodyPitch", parameters.bodyPitch));
   for (unsigned int i = 0; i < plist.size(); i++) {
      s << plist[i].first << "=" << plist[i].second << std::endl;
   }
   return s.str();
}

