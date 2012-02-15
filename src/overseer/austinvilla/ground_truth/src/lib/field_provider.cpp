/**
 * \file  field_provider.cpp
 * \brief Provides definitions for the FieldProvider header 
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/10/2011 11:55:27 AM piyushk $
 */

#include <pcl/point_types.h>
#include <math.h>

#include <ground_truth/field_provider.h>

#define PCL_WHITE 1.0, 1.0, 1.0
#define PCL_BLUE 0.0, 0.0, 1.0
#define PCL_GREEN 0.0, 1.0, 0.0
#define PCL_YELLOW 1.0, 1.0, 0.0 

#define CV_WHITE cvScalar(255, 255, 255)
#define CV_BLUE cvScalar(255, 0, 0)
#define CV_YELLOW cvScalar(0, 255, 255)
#define CV_GREEN cvScalar(0, 255, 0)
#define CV_RED cvScalar(0, 0, 255)
#define CV_BLACK cvScalar(0,0,0)

namespace ground_truth {

  /**
   * \brief   Constructor with field center coordinates
   */
  FieldProvider::FieldProvider(float x, float y, float z) {

    centerField = Eigen::Vector3f(x, y, z);
  
    groundPoints[YELLOW_BASE_TOP] = Eigen::Vector3f(FIELD_X / 2, -FIELD_Y / 2, 0);
    groundPoints[YELLOW_BASE_PENALTY_TOP] = Eigen::Vector3f(FIELD_X / 2, -PENALTY_Y / 2, 0);
    groundPoints[YELLOW_BASE_PENALTY_BOTTOM] = Eigen::Vector3f(FIELD_X / 2, PENALTY_Y / 2, 0);
    groundPoints[YELLOW_BASE_BOTTOM] = Eigen::Vector3f(FIELD_X / 2, FIELD_Y / 2, 0);
    groundPoints[YELLOW_PENALTY_TOP] = Eigen::Vector3f(FIELD_X / 2 - PENALTY_X, -PENALTY_Y / 2, 0);
    groundPoints[YELLOW_PENALTY_BOTTOM] = Eigen::Vector3f(FIELD_X / 2 - PENALTY_X, PENALTY_Y / 2, 0);
    groundPoints[YELLOW_PENALTY_CROSS] = Eigen::Vector3f(PENALTY_CROSS_X, 0, 0);

    groundPoints[MID_TOP] = Eigen::Vector3f(0, -FIELD_Y / 2, 0);
    groundPoints[MID_BOTTOM] = Eigen::Vector3f(0, FIELD_Y / 2, 0);

    groundPoints[BLUE_BASE_TOP] = Eigen::Vector3f(-FIELD_X / 2, -FIELD_Y / 2, 0);
    groundPoints[BLUE_BASE_PENALTY_TOP] = Eigen::Vector3f(-FIELD_X / 2, -PENALTY_Y / 2, 0);
    groundPoints[BLUE_BASE_PENALTY_BOTTOM] = Eigen::Vector3f(-FIELD_X / 2, PENALTY_Y / 2, 0);
    groundPoints[BLUE_BASE_BOTTOM] = Eigen::Vector3f(-FIELD_X / 2, FIELD_Y / 2, 0);
    groundPoints[BLUE_PENALTY_TOP] = Eigen::Vector3f(-FIELD_X / 2 + PENALTY_X, -PENALTY_Y / 2, 0);
    groundPoints[BLUE_PENALTY_BOTTOM] = Eigen::Vector3f(-FIELD_X / 2 + PENALTY_X, PENALTY_Y / 2, 0);
    groundPoints[BLUE_PENALTY_CROSS] = Eigen::Vector3f(-PENALTY_CROSS_X, 0, 0);

    groundPoints[YELLOW_GOALPOST_TOP] = Eigen::Vector3f(FIELD_X / 2, -GOAL_Y / 2, 0);
    groundPoints[YELLOW_GOALPOST_BOTTOM] = Eigen::Vector3f(FIELD_X / 2, GOAL_Y / 2, 0);

    groundPoints[BLUE_GOALPOST_TOP] = Eigen::Vector3f(-FIELD_X / 2, -GOAL_Y / 2, 0);
    groundPoints[BLUE_GOALPOST_BOTTOM] = Eigen::Vector3f(-FIELD_X / 2, GOAL_Y / 2, 0);

    groundPoints[MID_CIRCLE_TOP] = Eigen::Vector3f(0, -CIRCLE_RADIUS, 0);
    groundPoints[MID_CIRCLE_BOTTOM] = Eigen::Vector3f(0, CIRCLE_RADIUS, 0);

    highPoints[YELLOW_GOALPOST_TOP_HIGH] = Eigen::Vector3f(FIELD_X / 2, -GOAL_Y / 2, GOAL_HEIGHT);
    highPoints[YELLOW_GOALPOST_BOTTOM_HIGH] = Eigen::Vector3f(FIELD_X / 2, GOAL_Y / 2, GOAL_HEIGHT);
    highPoints[BLUE_GOALPOST_TOP_HIGH] = Eigen::Vector3f(-FIELD_X / 2, -GOAL_Y / 2, GOAL_HEIGHT);
    highPoints[BLUE_GOALPOST_BOTTOM_HIGH] = Eigen::Vector3f(-FIELD_X / 2, GOAL_Y / 2, GOAL_HEIGHT);

    // Translate by centerField
    for (int i = 0; i < NUM_GROUND_PLANE_POINTS; i++) {
      groundPoints[i] += centerField;
    }

  }

  /* 2D Functions */

  /**
   * \brief   Draws a line on an IplImage from 3D points using the appropriate scale
   */
  void FieldProvider::draw2dLine(IplImage* image, const Eigen::Vector3f &ep1, const Eigen::Vector3f &ep2, const CvScalar &color, int width) {
    cv::Point2d ep2d1, ep2d2;
    convertCoordinates(ep2d1, image->height, image->width, ep1);
    convertCoordinates(ep2d2, image->height, image->width, ep2);
    cvLine(image, ep2d1, ep2d2, color, width);
  }

  /**
   * \brief   Draws a dot (small circle) on an IplImage from 3D points using the appropriate scale
   */
  void FieldProvider::draw2dCenterCircle(IplImage* image, const Eigen::Vector3f &centerPt, const Eigen::Vector3f &circlePt, const CvScalar &color, int width) {
    cv::Point2d center, circle;
    convertCoordinates(center, image->height, image->width, centerPt);
    convertCoordinates(circle, image->height, image->width, circlePt);
    float radius = sqrtf(powf(center.x - circle.x, 2) + pow(center.y - circle.y, 2));
    cvCircle(image, center, radius, color, width);
  }

  /**
   * \brief   Draws the center circle on an IplImage from 3D points using the appropriate scale
   */
  void FieldProvider::draw2dCircle(IplImage * image, const Eigen::Vector3f &pt, int radius, const CvScalar &color, int width) {
    cv::Point2d pt2d;
    convertCoordinates(pt2d, image->height, image->width, pt);
    cvCircle(image, pt2d, radius, color, width);
  }

  /**
   * \brief   Scales the points from 3D locations to the correct pixel on an IplImage
   */
  void FieldProvider::convertCoordinates(cv::Point2d &pos2d, int height, int width, const Eigen::Vector3f &pos3d) {
    float xMul = width / GRASS_X;
    float yMul = height / GRASS_Y;
    pos2d.x = xMul * -(pos3d.x() - centerField.x()) + width / 2;
    pos2d.y = yMul * (pos3d.y() - centerField.y()) + height / 2;
  }

  /**
   * \brief   Draws out a 2D field to scale on an OpenCV IplImage
   */
  void FieldProvider::get2dField(IplImage* image, int highlightPoint) {

    cvZero(image);

    for (int i = 0; i < image->width; i++) {
      for (int j = 0; j < image->height; j++) {
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+1] = 64;
      }
    }

    draw2dLine(image, groundPoints[YELLOW_BASE_TOP], groundPoints[YELLOW_BASE_BOTTOM], CV_WHITE, 4);
    draw2dLine(image, groundPoints[YELLOW_BASE_PENALTY_TOP], groundPoints[YELLOW_PENALTY_TOP], CV_WHITE, 4);
    draw2dLine(image, groundPoints[YELLOW_BASE_PENALTY_BOTTOM], groundPoints[YELLOW_PENALTY_BOTTOM], CV_WHITE, 4);
    draw2dLine(image, groundPoints[YELLOW_PENALTY_TOP], groundPoints[YELLOW_PENALTY_BOTTOM], CV_WHITE, 4);
    draw2dCircle(image, groundPoints[YELLOW_PENALTY_CROSS], 3, CV_WHITE, -1);

    draw2dLine(image, groundPoints[BLUE_BASE_TOP], groundPoints[BLUE_BASE_BOTTOM], CV_WHITE, 4);
    draw2dLine(image, groundPoints[BLUE_BASE_PENALTY_TOP], groundPoints[BLUE_PENALTY_TOP], CV_WHITE, 4);
    draw2dLine(image, groundPoints[BLUE_BASE_PENALTY_BOTTOM], groundPoints[BLUE_PENALTY_BOTTOM], CV_WHITE, 4);
    draw2dLine(image, groundPoints[BLUE_PENALTY_TOP], groundPoints[BLUE_PENALTY_BOTTOM], CV_WHITE, 4);
    draw2dCircle(image, groundPoints[BLUE_PENALTY_CROSS], 3, CV_WHITE, -1);
    
    draw2dLine(image, groundPoints[BLUE_BASE_TOP], groundPoints[YELLOW_BASE_TOP], CV_WHITE, 4);
    draw2dLine(image, groundPoints[BLUE_BASE_BOTTOM], groundPoints[YELLOW_BASE_BOTTOM], CV_WHITE, 4);

    draw2dLine(image, groundPoints[MID_TOP], groundPoints[MID_BOTTOM], CV_WHITE, 4);

    draw2dCenterCircle(image, centerField, groundPoints[MID_CIRCLE_TOP], CV_WHITE, 4);

    draw2dLine(image, groundPoints[YELLOW_GOALPOST_TOP], groundPoints[YELLOW_GOALPOST_BOTTOM], CV_YELLOW, 8);
    draw2dLine(image, groundPoints[BLUE_GOALPOST_TOP], groundPoints[BLUE_GOALPOST_BOTTOM], CV_BLUE, 8);

    if (highlightPoint >= 0 && highlightPoint < NUM_GROUND_PLANE_POINTS) {
      draw2dCircle(image, groundPoints[highlightPoint], 5, CV_BLACK, -1);
    }

    /*
    // Used for drawing robot locations during experiments
    for (int x=-1; x>=-5; x-=2) {
      for (int y=-3; y <=3; y+=2) {
        Eigen::Vector3f point(x/2.0, y/2.0, 0); 
        draw2dCircle(image, point, 5, CV_BLACK, -1);
      }
    }
    */

    /*
    // Used for marking landmarks on image
    for (int i = 0; i < NUM_GROUND_PLANE_POINTS; i++) {
      draw2dCircle(image, groundPoints[i], 5, CV_BLACK, -1);
    }
    */

    /*
    // Used for drawing (approximately) the region of interest
    for (int i = 0; i < 22; i++) {
      for (int j = 50; j < 130; j++) {
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+1] = 0;
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+0] = 0;
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+2] = 0;
      }
    }

    for (int i = image->width-1; i >= image->width - 22; i--) {
      for (int j = 50; j < 130; j++) {
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+1] = 0;
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+0] = 0;
        ((uchar*)(image->imageData + image->widthStep*j))[i*3+2] = 0;
      }
    }
    */

  }

  /* 3D Functions */

  /**
   * \brief  Draws a line in 3D space
   */
  void FieldProvider::draw3dLine(pcl_visualization::PCLVisualizer &visualizer, const Eigen::Vector3f &ep1, const Eigen::Vector3f &ep2, double r, double g, double b, const std::string &name) {
    pcl::PointXYZ ep3d1(ep1.x(), ep1.y(), ep1.z());
    pcl::PointXYZ ep3d2(ep2.x(), ep2.y(), ep2.z());
    visualizer.addLine<pcl::PointXYZ, pcl::PointXYZ>(ep3d1, ep3d2, r, g, b, "__"+name+"__");
    visualizer.addSphere<pcl::PointXYZ>(ep3d1, 0.02, r, g, b, "__"+name+"_pt1"+"__");
    visualizer.addSphere<pcl::PointXYZ>(ep3d2, 0.02, r, g, b, "__"+name+"_pt2"+"__");
  }

  /**
   * \brief  Draws the center circle in 3d space
   */
  void FieldProvider::draw3dCenterCircle(pcl_visualization::PCLVisualizer &visualizer, const Eigen::Vector3f &centerPt, const Eigen::Vector3f &circlePt, double r, double g, double b, const std::string &name) {
    if (centerPt.z() == 0) { // Only supported for z = 0
      pcl::ModelCoefficients circleModel;
      circleModel.values.push_back(centerPt.x());
      circleModel.values.push_back(centerPt.y());
      circleModel.values.push_back((circlePt - centerPt).norm());
      visualizer.addCircle(circleModel, "__"+name+"__");
    }
  }

  /**
   * \brief   Draws out a 3d field to scale in a PCLVisualizer Window
   */
  void FieldProvider::get3dField(pcl_visualization::PCLVisualizer &visualizer) {
    
    draw3dLine(visualizer, groundPoints[YELLOW_BASE_TOP], groundPoints[YELLOW_BASE_BOTTOM], PCL_WHITE, "yellow_base");
    draw3dLine(visualizer, groundPoints[YELLOW_BASE_PENALTY_TOP], groundPoints[YELLOW_PENALTY_TOP], PCL_WHITE, "yellow_penalty_top");
    draw3dLine(visualizer, groundPoints[YELLOW_BASE_PENALTY_BOTTOM], groundPoints[YELLOW_PENALTY_BOTTOM], PCL_WHITE, "yellow_penalty_bottom");
    draw3dLine(visualizer, groundPoints[YELLOW_PENALTY_TOP], groundPoints[YELLOW_PENALTY_BOTTOM], PCL_WHITE, "yellow_penalty");

    pcl::PointXYZ yellowCross(groundPoints[YELLOW_PENALTY_CROSS].x(), groundPoints[YELLOW_PENALTY_CROSS].y(), groundPoints[YELLOW_PENALTY_CROSS].z());
    visualizer.addSphere<pcl::PointXYZ>(yellowCross, 0.02, PCL_WHITE, "yellowCross");

    draw3dLine(visualizer, groundPoints[BLUE_BASE_TOP], groundPoints[BLUE_BASE_BOTTOM], PCL_WHITE, "blue_base");
    draw3dLine(visualizer, groundPoints[BLUE_BASE_PENALTY_TOP], groundPoints[BLUE_PENALTY_TOP], PCL_WHITE, "blue_penalty_top");
    draw3dLine(visualizer, groundPoints[BLUE_BASE_PENALTY_BOTTOM], groundPoints[BLUE_PENALTY_BOTTOM], PCL_WHITE, "blue_penalty_bottom");
    draw3dLine(visualizer, groundPoints[BLUE_PENALTY_TOP], groundPoints[BLUE_PENALTY_BOTTOM], PCL_WHITE, "blue_penalty");

    pcl::PointXYZ blueCross(groundPoints[BLUE_PENALTY_CROSS].x(), groundPoints[BLUE_PENALTY_CROSS].y(), groundPoints[BLUE_PENALTY_CROSS].z());
    visualizer.addSphere<pcl::PointXYZ>(blueCross, 0.02, PCL_WHITE, "blueCross");
    
    draw3dLine(visualizer, groundPoints[BLUE_BASE_TOP], groundPoints[YELLOW_BASE_TOP], PCL_WHITE, "top_long");
    draw3dLine(visualizer, groundPoints[BLUE_BASE_BOTTOM], groundPoints[YELLOW_BASE_BOTTOM], PCL_WHITE, "bottom_long");

    draw3dLine(visualizer, groundPoints[MID_TOP], groundPoints[MID_BOTTOM], PCL_WHITE, "mid_short");

    draw3dCenterCircle(visualizer, centerField, groundPoints[MID_CIRCLE_TOP], PCL_WHITE, "center_circle");

    draw3dLine(visualizer, highPoints[YELLOW_GOALPOST_TOP_HIGH], highPoints[YELLOW_GOALPOST_BOTTOM_HIGH], PCL_YELLOW, "yellow_goal_high");
    draw3dLine(visualizer, highPoints[BLUE_GOALPOST_TOP_HIGH], highPoints[BLUE_GOALPOST_BOTTOM_HIGH], PCL_BLUE, "blue_goal_high");
    draw3dLine(visualizer, highPoints[YELLOW_GOALPOST_TOP_HIGH], groundPoints[YELLOW_GOALPOST_TOP], PCL_YELLOW, "yellow_goal_top");
    draw3dLine(visualizer, highPoints[BLUE_GOALPOST_TOP_HIGH], groundPoints[BLUE_GOALPOST_TOP], PCL_BLUE, "blue_goal_top");
    draw3dLine(visualizer, highPoints[YELLOW_GOALPOST_BOTTOM_HIGH], groundPoints[YELLOW_GOALPOST_BOTTOM], PCL_YELLOW, "yellow_goal_bottom");
    draw3dLine(visualizer, highPoints[BLUE_GOALPOST_BOTTOM_HIGH], groundPoints[BLUE_GOALPOST_BOTTOM], PCL_BLUE, "blue_goal_bottom");

  }

}
