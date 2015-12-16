#include "Pose.hpp"

#include <perception/vision/VisionDefs.hpp>
#include <utils/matrix_helpers.hpp>

Pose::Pose() {
   for (int i = 0; i < EXCLUSION_RESOLUTION; i++) {
      topExclusionArray[i] = TOP_IMAGE_ROWS;
      botExclusionArray[i] = TOP_IMAGE_ROWS+BOT_IMAGE_ROWS;
   }

   origin = vec4<float>(0, 0, 0, 1);
   zunit = vec4<float>(0, 0, 0, 1);
   topToFocus = vec4<float>(0, 0, FOCAL_LENGTH, 1);
   botToFocus = vec4<float>(0, 0, FOCAL_LENGTH, 1);

   topCameraToWorldTransform  = boost::numeric::ublas::identity_matrix<float>(4);
   botCameraToWorldTransform  = boost::numeric::ublas::identity_matrix<float>(4);
   topWorldToCameraTransform  = boost::numeric::ublas::identity_matrix<float>(4);
   botWorldToCameraTransform  = boost::numeric::ublas::identity_matrix<float>(4);
   topWorldToCameraTransformT = boost::numeric::ublas::identity_matrix<float>(4);
   botWorldToCameraTransformT = boost::numeric::ublas::identity_matrix<float>(4);
   neckToWorldTransform = boost::numeric::ublas::identity_matrix<float>(4);
   worldToNeckTransform = boost::numeric::ublas::identity_matrix<float>(4);

   horizon = std::pair<int, int>(0, 0);
}

Pose::Pose(boost::numeric::ublas::matrix<float> topCameraToWorldTransform,
           boost::numeric::ublas::matrix<float> botCameraToWorldTransform,
           boost::numeric::ublas::matrix<float> neckToWorldTransform,
           std::pair<int, int> horizon)
   : topWorldToCameraTransform(4, 4), botWorldToCameraTransform(4, 4), worldToNeckTransform(4, 4)
{
   this->topCameraToWorldTransform = topCameraToWorldTransform;
   this->botCameraToWorldTransform = botCameraToWorldTransform;
   this->neckToWorldTransform = neckToWorldTransform;
   this->horizon = horizon;

   makeConstants();
}

XYZ_Coord Pose::robotRelativeToNeckCoord(RRCoord coord, int h) const {
   Point cartesian = coord.toCartesian();

   boost::numeric::ublas::matrix<float> p = vec4<float>(cartesian[0], cartesian[1], h, 1);
   boost::numeric::ublas::matrix<float> neckP = prod(worldToNeckTransform, p);
   return XYZ_Coord(neckP(0, 0), neckP(1, 0), neckP(2, 0));
}

RRCoord Pose::imageToRobotRelative(Point p, int h) const
{
   Point rr_p = imageToRobotXY(p, h);

   RRCoord r;
   r.distance() = hypotf(rr_p.y(), rr_p.x());
   r.heading()  = atan2f(rr_p.y(), rr_p.x());

   return r;
}

RRCoord Pose::imageToRobotRelative(int x, int y, int h) const
{
   return imageToRobotRelative(Point(x, y), h);
}

Point Pose::imageToRobotXY(const Point &image, int h) const
{
   // determine which camera
   bool top = true;
   Point i = image;
   if (image.y() >= TOP_IMAGE_ROWS) {
      top = false;
      i.y() -= TOP_IMAGE_ROWS;
   }

   // Variations depending on which image we are looking at
   const int COLS = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   const int ROWS = (top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;
   const float PIXEL = (top) ? TOP_PIXEL_SIZE : BOT_PIXEL_SIZE;

   // calculate vector to pixel in camera space
   boost::numeric::ublas::matrix<float> lOrigin, lOrigin2;
   lOrigin2 = vec4<float>(
      (((COLS) / 2.0 - i.x()) * PIXEL),
      (((ROWS) / 2.0 - i.y()) * PIXEL),
      0,
      1);

   boost::numeric::ublas::matrix<float> cdir;

   if (top) {
      lOrigin = prod(
         topCameraToWorldTransform,
         lOrigin2);
      cdir = (topToFocus - lOrigin);
   } else {
      lOrigin = prod(
         botCameraToWorldTransform,
         lOrigin2);
      cdir = (botToFocus - lOrigin);
   }


   float lambda = (h - lOrigin(2, 0)) / (1.0 * cdir(2, 0));

   boost::numeric::ublas::matrix<float> intercept;
   intercept = vec4<float>(lOrigin(0, 0) + lambda * cdir(0, 0),
                       lOrigin(1, 0) + lambda * cdir(1, 0),
                       h,
                       1);

   return Point(intercept(0, 0), intercept(1, 0));
}


/* Sean - I think this tries bottom camera first,
 * then tries top camera if that doesn't work.
 * 99.9% sure its right, it works after testing */
Point Pose::robotToImageXY(Point robot, int h) const
{
   boost::numeric::ublas::matrix<float> p = vec4<float>(robot.x(), robot.y(), h, 1);

   boost::numeric::ublas::matrix<float> pixel = prod(botWorldToCameraTransformT,p);

   pixel(0, 0) /= ABS(pixel(3, 0));
   pixel(1, 0) /= ABS(pixel(3, 0));

   pixel(0, 0) = (pixel(0, 0) * (BOT_IMAGE_COLS / 2))+(BOT_IMAGE_COLS / 2);
   pixel(1, 0) = (pixel(1, 0) * (BOT_IMAGE_COLS / 2))+(BOT_IMAGE_ROWS / 2);

   if (pixel(1 , 0) < 0) {
      pixel = prod(topWorldToCameraTransformT,p);

      pixel(0, 0) /= ABS(pixel(3, 0));
      pixel(1, 0) /= ABS(pixel(3, 0));

      pixel(0, 0) = (pixel(0, 0) * (TOP_IMAGE_COLS / 2)) + (TOP_IMAGE_COLS / 2);
      pixel(1, 0) = (pixel(1, 0) * (TOP_IMAGE_COLS / 2)) + (TOP_IMAGE_ROWS / 2);
   } else {
      pixel(1, 0) += TOP_IMAGE_ROWS;
   }

   return Point(pixel(0, 0), pixel(1, 0));
}

std::pair<int, int> Pose::getHorizon() const {
   return horizon;
}

const int16_t *Pose::getTopExclusionArray() const {
   return topExclusionArray;
}

int16_t *Pose::getTopExclusionArray() {
   return topExclusionArray;
}

const int16_t *Pose::getBotExclusionArray() const {
   return botExclusionArray;
}

int16_t *Pose::getBotExclusionArray() {
   return botExclusionArray;
}

const boost::numeric::ublas::matrix<float>
      Pose::getC2wTransform(bool top) const {
   boost::numeric::ublas::matrix<float> c(4, 4);
   if (top) {
      c(0, 0) = topCameraToWorldTransform(0, 0);
      c(1, 0) = topCameraToWorldTransform(1, 0);
      c(2, 0) = topCameraToWorldTransform(2, 0);
      c(3, 0) = topCameraToWorldTransform(3, 0);
      c(0, 1) = topCameraToWorldTransform(0, 1);
      c(1, 1) = topCameraToWorldTransform(1, 1);
      c(2, 1) = topCameraToWorldTransform(2, 1);
      c(3, 1) = topCameraToWorldTransform(3, 1);
      c(0, 2) = topCameraToWorldTransform(0, 2);
      c(1, 2) = topCameraToWorldTransform(1, 2);
      c(2, 2) = topCameraToWorldTransform(2, 2);
      c(3, 2) = topCameraToWorldTransform(3, 2);
      c(0, 3) = topCameraToWorldTransform(0, 3);
      c(1, 3) = topCameraToWorldTransform(1, 3);
      c(2, 3) = topCameraToWorldTransform(2, 3);
      c(3, 3) = topCameraToWorldTransform(3, 3);
   } else {
      c(0, 0) = botCameraToWorldTransform(0, 0);
      c(1, 0) = botCameraToWorldTransform(1, 0);
      c(2, 0) = botCameraToWorldTransform(2, 0);
      c(3, 0) = botCameraToWorldTransform(3, 0);
      c(0, 1) = botCameraToWorldTransform(0, 1);
      c(1, 1) = botCameraToWorldTransform(1, 1);
      c(2, 1) = botCameraToWorldTransform(2, 1);
      c(3, 1) = botCameraToWorldTransform(3, 1);
      c(0, 2) = botCameraToWorldTransform(0, 2);
      c(1, 2) = botCameraToWorldTransform(1, 2);
      c(2, 2) = botCameraToWorldTransform(2, 2);
      c(3, 2) = botCameraToWorldTransform(3, 2);
      c(0, 3) = botCameraToWorldTransform(0, 3);
      c(1, 3) = botCameraToWorldTransform(1, 3);
      c(2, 3) = botCameraToWorldTransform(2, 3);
      c(3, 3) = botCameraToWorldTransform(3, 3);
   } 
   return c;
}

void Pose::makeConstants()
{
   boost::numeric::ublas::matrix<float> projection(4, 4);
   projection(0, 0) = 1;
   projection(0, 1) = 0;
   projection(0, 2) = 0;
   projection(0, 3) = 0;

   projection(1, 0) = 0;
   projection(1, 1) = 1;
   projection(1, 2) = 0;
   projection(1, 3) = 0;

   projection(2, 0) = 0;
   projection(2, 1) = 0;
   projection(2, 2) = 1;
   projection(2, 3) = 0;

   projection(3, 0) = 0;
   projection(3, 1) = 0;
   projection(3, 2) = tan(CAMERA_FOV_W / 2);
   projection(3, 3) = 0;

   invertMatrix(topCameraToWorldTransform, topWorldToCameraTransform);
   topWorldToCameraTransformT = prod(projection, topWorldToCameraTransform);
   invertMatrix(botCameraToWorldTransform, botWorldToCameraTransform);
   botWorldToCameraTransformT = prod(projection, botWorldToCameraTransform);
   
   invertMatrix(neckToWorldTransform, worldToNeckTransform);

   origin = vec4<float>(0, 0, 0, 1);
   zunit = vec4<float>(0, 0, 0, 1);
   topCOrigin = prod(topCameraToWorldTransform, origin);
   botCOrigin = prod(botCameraToWorldTransform, origin);
   topToFocus = prod(topCameraToWorldTransform, vec4<float>(0, 0,FOCAL_LENGTH, 1));
   botToFocus = prod(botCameraToWorldTransform, vec4<float>(0, 0,FOCAL_LENGTH, 1));
}

