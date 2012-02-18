/**
 * \file  detect.cc
 * \brief Performs the ground truth detection
 *
 * This ROS node detects the ground truth locations of the ball and the robots
 * on the field. It uses the collected calibration info as well as the color
 * table
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/10/2011 12:48:11 PM piyushk $
 *
 * @modified Octavian Neamtu, Northern Bites
 */

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <image_geometry/pinhole_camera_model.h>

#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>

#include <pcl/point_types.h>
#include <pcl_visualization/pcl_visualizer.h>
#include <pcl/registration/transforms.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/segmentation/extract_clusters.h>
#include <terminal_tools/parse.h>

#include <Eigen/Core>

#include <color_table/common.h>
#include <ground_truth/field_provider.h>

//NBites point struct
#include "Structs.h"
#include "OverseerServer.h"

/* Display modes */
#define FULL 1
#define RELEVANT 2

using namespace color_table;

namespace {
  sensor_msgs::PointCloud2ConstPtr cloudPtr, oldCloudPtr;
  boost::mutex mCloud;
  pcl_visualization::PointCloudColorHandler<pcl::PointXYZRGB>::Ptr colorHandler;
  pcl_visualization::PointCloudGeometryHandler<pcl::PointXYZRGB>::Ptr geometryHandler;

  std::string calibFile;
  std::string colorTableFile;
  std::string logFile;
  Eigen::Affine3f transformMatrix;

  int qSize;
  std::string cloudTopic;
  int mode;

  unsigned int numRobotsDisplayed = 0;
  unsigned int numBallsDisplayed = 0;

  ColorTable colorTable;
} 

/**
 * \brief  Loads color table from file into array 
 */
void loadColorTable() {
  FILE* f = fopen(colorTableFile.c_str(), "rb");
  size_t size = fread(colorTable, 128*128*128, 1, f);
  size = size; // remove warning
  fclose(f);
}

/**
 * \brief  Helper function to return time in seconds with micro-second precision 
 */
double getSystemTime() {
  // set time
  struct timezone tz;
  timeval timeT;
  gettimeofday(&timeT, &tz);
  return timeT.tv_sec + (timeT.tv_usec / 1000000.0);
}

/**
 * /brief Helper function for attaching a unique id to a string.
 * /return the string with the unique identifier
 */
inline std::string getUniqueName(const std::string &baseName, int uniqueId) {
  return baseName + boost::lexical_cast<std::string>(uniqueId);
}

/**
 * \brief  Function to detect the ball given the transformed point cloud from the kinect
 * \param cloudIn The transformed point cloud from the Kinect
 * \param ballPositions The detected ball positions (can be more than 1)
 * \param cloudOut The clusters contributing to the detected balls
 */
void detectBall(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudIn, std::vector<pcl::PointXYZ> &ballPositions, pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudOut) {

  pcl::PointIndices inliers;

  ballPositions.clear();

  for (unsigned int i = 0; i < cloudIn->points.size(); i++) {
    pcl::PointXYZRGB *pt = &cloudIn->points[i];
    int rgb = *reinterpret_cast<int*>(&pt->rgb);
    int r = ((rgb >> 16) & 0xff);
    int g = ((rgb >> 8) & 0xff);
    int b = (rgb & 0xff);
    if (colorTable[r/2][g/2][b/2] == ORANGE && fabs(pt->x) < 3.5 && fabs(pt->y) < 2.25 && fabs(pt->z) < 0.15 ) {
      inliers.indices.push_back(i);
    }
  }

  pcl::ExtractIndices<pcl::PointXYZRGB> extract;
  extract.setInputCloud(cloudIn);
  extract.setIndices(boost::make_shared<pcl::PointIndices>(inliers));
  extract.setNegative(false);
  extract.filter(*cloudOut);

  if (cloudOut->points.size() == 0)
    return;

  pcl::EuclideanClusterExtraction<pcl::PointXYZRGB> cluster;
  cluster.setClusterTolerance(0.1);
  cluster.setMinClusterSize(5);
  cluster.setInputCloud(cloudOut);
  std::vector<pcl::PointIndices> clusters;
  cluster.extract(clusters);

  for (unsigned int i = 0; i < clusters.size(); i++) {
    pcl::PointIndices clusterIndex = clusters[i];

    pcl::PointXYZ point(0,0,0);
    for (unsigned int j = 0; j < clusterIndex.indices.size(); j++) {
      point.x += cloudOut->points[clusterIndex.indices[j]].x;
      point.y += cloudOut->points[clusterIndex.indices[j]].y;
    }
    point.z = 0;
    point.x /= clusterIndex.indices.size();
    point.y /= clusterIndex.indices.size();
    ballPositions.push_back(point);
  }

}

/**
 * \brief  Function to detect robots given the transformed point cloud from the kinect
 * \param cloudIn The transformed point cloud from the Kinect
 * \param ballPositions The robot positions
 * \param cloudOut The clusters contributing to the detected robots
 */
void detectRobots(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudIn, std::vector<pcl::PointXYZ> &robotPositions, pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudOut) {

  pcl::PointIndices inliers;

  robotPositions.clear();

  for (unsigned int i = 0; i < cloudIn->points.size(); i++) {
    pcl::PointXYZRGB *pt = &cloudIn->points[i];
    if (pt->z > 0.25 && pt->y < 2 && pt->y > -2 && pt->x < 2.75 && pt->x > -2.75) {
      inliers.indices.push_back(i);
    }
  }

  pcl::ExtractIndices<pcl::PointXYZRGB> extract;
  extract.setInputCloud(cloudIn);
  extract.setIndices(boost::make_shared<pcl::PointIndices>(inliers));
  extract.setNegative(false);
  extract.filter(*cloudOut);

  if (cloudOut->points.size() == 0)
    return;

  pcl::EuclideanClusterExtraction<pcl::PointXYZRGB> cluster;
  cluster.setClusterTolerance(0.1);
  cluster.setMinClusterSize(200);
  cluster.setInputCloud(cloudOut);
  std::vector<pcl::PointIndices> clusters;
  cluster.extract(clusters);

  for (unsigned int i = 0; i < clusters.size(); i++) {
    pcl::PointIndices clusterIndex = clusters[i];

    pcl::PointXYZ point(0,0,0);
    bool highPoint = false;
    for (unsigned int j = 0; j < clusterIndex.indices.size(); j++) {
      if (cloudOut->points[clusterIndex.indices[j]].z > 0.7) {
        highPoint = true;
      }
      point.x += cloudOut->points[clusterIndex.indices[j]].x;
      point.y += cloudOut->points[clusterIndex.indices[j]].y;
    }
    if (highPoint)
      continue;
    point.z = 0;
    point.x /= clusterIndex.indices.size();
    point.y /= clusterIndex.indices.size();
    robotPositions.push_back(point);
  }

}

/**
 * \brief   Callback function for the point cloud message received from the kinect driver
 */
void cloudCallback (const sensor_msgs::PointCloud2ConstPtr& cloudPtrFromMsg) {
  mCloud.lock();
  cloudPtr = cloudPtrFromMsg;
  mCloud.unlock();
}

/**
 * \brief   Helper function to get parameters from the command line, or a ROS parameter server
 */
void getParameters(ros::NodeHandle &nh, int argc, char ** argv) {

  qSize = 1;
  cloudTopic = "input";
  calibFile = "data/calib.txt";
  colorTableFile = "data/default.col";
  mode = 1;

  terminal_tools::parse_argument (argc, argv, "-qsize", qSize);
  terminal_tools::parse_argument (argc, argv, "-calibFile", calibFile);
  terminal_tools::parse_argument (argc, argv, "-logFile", logFile);
  terminal_tools::parse_argument (argc, argv, "-colorTableFile", colorTableFile);
  terminal_tools::parse_argument (argc, argv, "-mode", mode);

  ROS_INFO("Calib File: %s", calibFile.c_str());
  ROS_INFO("Log File: %s", logFile.c_str());
  ROS_INFO("ColorTable File: %s", colorTableFile.c_str());
}

int main (int argc, char** argv) {

  ros::init (argc, argv, "pointcloud_online_viewer");
  ros::NodeHandle nh;

  getParameters(nh, argc, argv);

  // Create a ROS subscriber for the point cloud
  ros::Subscriber subCloud = nh.subscribe (cloudTopic, qSize, cloudCallback);

  // Get transformation that needs to be applied to each input cloud to get the cloud in the correct reference frame
  std::ifstream fin(calibFile.c_str());
  if (!fin) {
    ROS_ERROR("Unable to open calibration file!!");
    return -1;
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      fin >> transformMatrix(i,j);
    }
  }
  fin.close();

  pcl_visualization::PCLVisualizer visualizer(argc, argv, "PointCloud");
  visualizer.addCoordinateSystem(); // Good for reference
  ground_truth::FieldProvider field;
  field.get3dField(visualizer);

  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudSwap(new pcl::PointCloud<pcl::PointXYZRGB>);
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudDisplay;
  
  loadColorTable();

  std::ofstream log(logFile.c_str());

  std::vector<pcl::PointXYZ> ballPositions;
  point<float> ballFieldPosition;
  std::vector<pcl::PointXYZ> robotPositions;
  std::vector<point<float> > robotFieldPositions;
  nbites::overseer::OverseerServer overseerServer(&ballFieldPosition, &robotFieldPositions);

  while (nh.ok ()) {

    // Spin
    ros::spinOnce ();
    ros::Duration (0.001).sleep ();
    visualizer.spinOnce(10);

    // If no cloud received yet, continue
    if (!cloudPtr)
      continue;

    if (cloudPtr == oldCloudPtr)
      continue;

    // Convert to PointCloud<T>
    mCloud.lock ();

    pcl::fromROSMsg (*cloudPtr, *cloud);

    // Apply transformation to get the correct reference frame
    pcl::transformPointCloud(*cloud, *cloudSwap, transformMatrix);

    // Apply filter to extract only those points which are on the field

    if (mode == FULL) {

      pcl::PointIndices inliers;
      for (unsigned int i = 0; i < cloudSwap->points.size(); i++) {
        pcl::PointXYZRGB *pt = &cloudSwap->points[i];
        if (pcl_isfinite(pt->x)) {
          inliers.indices.push_back(i);
        }
      }
      pcl::ExtractIndices<pcl::PointXYZRGB> extract;
      extract.setInputCloud(cloudSwap);
      extract.setIndices(boost::make_shared<pcl::PointIndices>(inliers));
      extract.setNegative(false);
      extract.filter(*cloud);
      
      cloudDisplay.reset(new pcl::PointCloud<pcl::PointXYZRGB>);
      *cloudDisplay = *cloud;
    } else {

      cloudDisplay.reset(new pcl::PointCloud<pcl::PointXYZRGB>);
      // Ball

      detectBall(cloudSwap, ballPositions, cloud);
      for (unsigned int i = 0; i < numBallsDisplayed; i++) {
        visualizer.removeShape(getUniqueName("ball", i));
      }
      for (unsigned int i = 0; i < ballPositions.size(); i++) {
        visualizer.addSphere(ballPositions[i], 0.05, 1.0, 0.4, 0.0, getUniqueName("ball", i));
      }
      numBallsDisplayed = ballPositions.size();
      if (numBallsDisplayed > 0) {
          ballFieldPosition.x = ballPositions[0].x;
          ballFieldPosition.y = ballPositions[0].y;
      }


      /* 
      //Sample log file code
      log << std::fixed << getSystemTime() << ",";
      if (ballPositions.size() == 0) {
        log << "-7," << "-7,";
      } else if (ballPositions.size() == 1) {
        log << ballPositions[0].x << "," << ballPositions[0].y << ",";
      }
      */

      // Robots
      detectRobots(cloudSwap, robotPositions, cloud);
      *cloudDisplay = *cloud;                           // Display the cloud
      for (unsigned int i = 0; i < numRobotsDisplayed; i++) {
        visualizer.removeShape(getUniqueName("robot", i));
      }
      for (unsigned int i = 0; i < robotPositions.size(); i++) {
        visualizer.addSphere(robotPositions[i], 0.1, 1.0, 1.0, 1.0, getUniqueName("robot", i));
      }
      numRobotsDisplayed = robotPositions.size();

      robotFieldPositions.clear();
      for (int i = 0; i < numRobotsDisplayed; i++) {
          robotFieldPositions.push_back(
                  point<float>(robotFieldPositions[i].x, robotFieldPositions[i].y));
      }

      overseerServer.postData();

      /*
      //Sample log file code
      if (robotPositions.size() == 0) {
        log << "-7," << "-7,";
      } else if (robotPositions.size() == 1) {
        log << robotPositions[0].x << "," << robotPositions[0].y << ",";
      }
      log << std::endl;
      */

    }

    visualizer.removePointCloud();
    colorHandler.reset (new pcl_visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> (*cloudDisplay));
    geometryHandler.reset (new pcl_visualization::PointCloudGeometryHandlerXYZ<pcl::PointXYZRGB> (*cloudDisplay));
    visualizer.addPointCloud<pcl::PointXYZRGB>(*cloudDisplay, *colorHandler, *geometryHandler);

    oldCloudPtr = cloudPtr;

    mCloud.unlock ();
  }
  log.close();
  return (0);
}
