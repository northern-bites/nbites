/**
 * \file  field_provider.h
 * \brief This header defines the dimensions of the field, as well as 
 * declares the helper functions to draw out the field in 2D and 3D
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/10/2011 11:07:07 AM piyushk $
 */

#ifndef FIELD_PROVIDER_HWF1NX72
#define FIELD_PROVIDER_HWF1NX72

#include <Eigen/Core>
#include <opencv/cv.h>
#include <pcl_visualization/pcl_visualizer.h>

namespace ground_truth {

  /* constants describing the field */

  const float FIELD_Y = 3.950;            ///< width of the field
  const float FIELD_X = 5.950;            ///< length of the field

  const float GRASS_Y = 4.725;            ///< width of the grass
  const float GRASS_X = 6.725;            ///< length of the grass 

  const float PENALTY_Y = 2.150;          ///< distance of penalty box along width 
  const float PENALTY_X =  0.550;         ///< distance of penalty box along length
  const float CIRCLE_RADIUS =  0.650;     ///< center circle radius

  const float PENALTY_CROSS_X = 1.200;    ///< distance of penalty cross from field center

  const float GOAL_HEIGHT = 0.8;          ///< height of top goal bar
  const float GOAL_Y = 1.5;               ///< distance between goal posts

  /* Different points of interest on the field */

  /* Landmarks - points on the ground plane that are easily identifiable */

  enum GroundPoints {
    YELLOW_BASE_TOP = 0,
    YELLOW_BASE_PENALTY_TOP = 1,
    YELLOW_GOALPOST_TOP = 2,
    YELLOW_GOALPOST_BOTTOM = 3,
    YELLOW_BASE_PENALTY_BOTTOM = 4,
    YELLOW_BASE_BOTTOM = 5,
    YELLOW_PENALTY_TOP = 6,
    YELLOW_PENALTY_BOTTOM = 7,
    YELLOW_PENALTY_CROSS = 8,
    MID_TOP = 9,
    MID_CIRCLE_TOP = 10,
    MID_CIRCLE_BOTTOM = 11,
    MID_BOTTOM = 12,
    BLUE_BASE_TOP = 13,
    BLUE_BASE_PENALTY_TOP = 14,
    BLUE_GOALPOST_TOP = 15,
    BLUE_GOALPOST_BOTTOM = 16,
    BLUE_BASE_PENALTY_BOTTOM = 17,
    BLUE_BASE_BOTTOM = 18,
    BLUE_PENALTY_TOP = 19,
    BLUE_PENALTY_BOTTOM = 20,
    BLUE_PENALTY_CROSS = 21,
    NUM_GROUND_PLANE_POINTS = 22
  };

  /* High points - top corners of goals */

  enum HighPoints {
    YELLOW_GOALPOST_TOP_HIGH = 0,
    YELLOW_GOALPOST_BOTTOM_HIGH = 1,
    BLUE_GOALPOST_TOP_HIGH = 2,
    BLUE_GOALPOST_BOTTOM_HIGH = 3,
    NUM_HIGH_POINTS = 4
  };

  /**
   * \class FieldProvider
   * \brief Provides locations of key field landmarks and helper functions to draw the field out in 2D and 3D 
   */
  class FieldProvider {

    private:

      Eigen::Vector3f centerField;                            ///< Coordinates for the field center
      Eigen::Vector3f groundPoints[NUM_GROUND_PLANE_POINTS];  ///< True locations of landmarks on the ground plane
      Eigen::Vector3f highPoints[NUM_HIGH_POINTS];            ///< True locations of the top points in a goal (for visualization)

      /* 2D Helper Functions */
      
      /**
       * \brief   Draws a line on an IplImage from 3D points using the appropriate scale
       */
      void draw2dLine(IplImage* image, const Eigen::Vector3f &ep1, const Eigen::Vector3f &ep2, const CvScalar &color, int width);

      /**
       * \brief   Draws a dot (small circle) on an IplImage from 3D points using the appropriate scale
       */
      void draw2dCircle(IplImage * image, const Eigen::Vector3f &pt, int radius, const CvScalar &color, int width);

      /**
       * \brief   Draws the center circle on an IplImage from 3D points using the appropriate scale
       */
      void draw2dCenterCircle(IplImage *image, const Eigen::Vector3f &centerPt, const Eigen::Vector3f &circlePt, const CvScalar &color, int width);

      /**
       * \brief   Scales the points from 3D locations to the correct pixel on an IplImage
       */
      void convertCoordinates(cv::Point2d &pos2d, int height, int width, const Eigen::Vector3f &pos3d);

      /* 3D Helper Functions */
      
      /**
       * \brief  Draws a line in 3D space
       *
       * This function also draw out 2 small spheres at the end of the line
       */
      void draw3dLine(pcl_visualization::PCLVisualizer &visualizer, const Eigen::Vector3f &ep1, const Eigen::Vector3f &ep2, double r, double g, double b, const std::string &name);

      /**
       * \brief  Draws the center circle in 3d space
       *
       * This method currently uses the PCLVisualizer API for drawing circles.
       * Currently a wireframe of the center circle in the form of a Hexagon
       * is drawn
       *
       */
      void draw3dCenterCircle(pcl_visualization::PCLVisualizer &visualizer, const Eigen::Vector3f &centerPt, const Eigen::Vector3f &circlePt, double r, double g, double b, const std::string &name);

    public: 

      /**
       * \brief   Constructor with field center coordinates
       *
       * This constructor always assumes the xy plane to be the ground 
       */
      FieldProvider (float x = 0.0, float y = 0.0, float z = 0.0);
      

      /**
       * \brief   Draws out a 2D field to scale on an OpenCV IplImage
       * \param   image The image on which the field is to be drawn
       * \param   highlightPoint Indicates which landmark is to be highlighted(default is none)
       */
      void get2dField(IplImage* image, int highlightPoint = -1);

      /**
       * \brief   Draws out a 3d field to scale in a PCLVisualizer Window
       * \param   visualizer the PCLVisualizer window object 
       */
      void get3dField(pcl_visualization::PCLVisualizer &visualizer);

      /**
       * \brief   Returns the true location of a landmark
       * \param   index Identifier for the landmark
       * \return  The 3D location of the landmark
       */
      inline Eigen::Vector3f getGroundPoint(int index) {
        return groundPoints[index];
      }

  };
}

#endif /* end of include guard: FIELD_PROVIDER_HWF1NX72 */
