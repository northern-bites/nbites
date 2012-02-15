/**
 * \file  calibrate.cc
 * \brief Calculates position of the kinect
 *
 *  This ROS node calculates the position and orientation of 
 *  the kinect sensor in the field coordinate system. This information is 
 *  stored to file and read by the detection system
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/04/2011 02:18:43 PM piyushk $
 */

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <image_geometry/pinhole_camera_model.h>

#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>

#include <pcl/point_types.h>
#include <pcl_visualization/pcl_visualizer.h>
#include <pcl/common/transformation_from_correspondences.h>
#include <pcl/registration/transforms.h>
#include <pcl/features/normal_3d.h>
#include <pcl/filters/extract_indices.h>
#include <terminal_tools/parse.h>

#include <Eigen/Core>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cv_bridge/CvBridge.h>

#include <ground_truth/field_provider.h>

namespace {

  sensor_msgs::PointCloud2ConstPtr cloudPtr, oldCloudPtr;
  pcl::PointCloud<pcl::PointXYZRGB> cloud, transformedCloud;
  boost::mutex mCloud;
  pcl_visualization::PointCloudColorHandler<pcl::PointXYZRGB>::Ptr colorHandler;
  pcl_visualization::PointCloudGeometryHandler<pcl::PointXYZRGB>::Ptr geometryHandler;
  ground_truth::FieldProvider fieldProvider;

  IplImage* rgbImage = NULL;
  boost::mutex mImage;
  sensor_msgs::CvBridge bridge;
  image_geometry::PinholeCameraModel model;

  Eigen::Vector3f rayPt1, rayPt2;

  const int SELECTOR_IMAGE_WIDTH = 240;
  const int SELECTOR_IMAGE_HEIGHT = 180;

  IplImage * selectorImage;
  pcl::TransformationFromCorrespondences rigidBodyTransform;

  const int MAX_GROUND_POINTS = 5;
  Eigen::Vector3f groundPoints[MAX_GROUND_POINTS];
  int numGroundPoints = 0;
  int displayGroundPoints = 0;

  Eigen::Vector3f landmarkPoints[ground_truth::NUM_GROUND_PLANE_POINTS];
  bool landmarkAvailable[ground_truth::NUM_GROUND_PLANE_POINTS];
  int currentLandmark = 0;
  int displayLandmark = 0;
  bool newDisplayLandmark;

  bool transformationAvailable = false;
  Eigen::Affine3f transformMatrix;
  Eigen::Vector4f groundPlaneParameters;

  int queue_size = 1;
  std::string calibFile = "data/calib.txt";

  std::string status;
  bool stayAlive = true;

  enum State {
    COLLECT_GROUND_POINTS,
    GET_GROUND_POINT_INFO,
    TRANSITION_TO_LANDMARK_COLLECTION,
    COLLECT_LANDMARKS,
    GET_LANDMARK_INFO,
    TRANSFORMATION_CALCULATED,
  };

  State state = COLLECT_GROUND_POINTS;

}

/**
 * /brief Helper function for attaching a unique id to a string.
 * /return the string with the unique identifier
 */
inline std::string getUniqueName(const std::string &baseName, int uniqueId) {
  return baseName + boost::lexical_cast<std::string>(uniqueId);
}

/**
 * \brief  Helper function to attach static object to boost::shared_ptr. Used for efficiency 
 */
template <typename T>
void noDelete(T *ptr) {
}

/**
 * \brief  Simple function to display the point cloud in the visualizer
 */
void displayCloud(pcl_visualization::PCLVisualizer &visualizer, pcl::PointCloud<pcl::PointXYZRGB> &cloudToDisplay) {
  
  pcl::PointCloud<pcl::PointXYZRGB> displayCloud;

  /* This Filter code is currently there due to some failure for nan points while displaying */

  // Filter to remove NaN points
  pcl::PointIndices inliers;
  for (unsigned int i = 0; i < cloudToDisplay.points.size(); i++) {
    pcl::PointXYZRGB *pt = &cloud.points[i];
    if (pcl_isfinite(pt->x))
      inliers.indices.push_back(i);
  }
  pcl::ExtractIndices<pcl::PointXYZRGB> extract;
  pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr ptr(&cloudToDisplay, noDelete<pcl::PointCloud<pcl::PointXYZRGB> >);
  extract.setInputCloud(ptr);
  extract.setIndices(boost::make_shared<pcl::PointIndices>(inliers));
  extract.setNegative(false);
  extract.filter(displayCloud);

  /* Filter code Ends */

  visualizer.removePointCloud();
  colorHandler.reset (new pcl_visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> (displayCloud));
  geometryHandler.reset (new pcl_visualization::PointCloudGeometryHandlerXYZ<pcl::PointXYZRGB> (displayCloud));
  visualizer.addPointCloud<pcl::PointXYZRGB>(displayCloud, *colorHandler, *geometryHandler);
}

/**
 * \brief  Calculates the transformation (i.e. location of the kinect sensor) based on known positions of landmarks 
 */
void calculateTransformation() {
  for (int i = 0; i < ground_truth::NUM_GROUND_PLANE_POINTS; i++) {
    if (landmarkAvailable[i]) {
      rigidBodyTransform.add(landmarkPoints[i], fieldProvider.getGroundPoint(i), 1.0 / (landmarkPoints[i].norm() * landmarkPoints[i].norm()));
    }
  }
  transformMatrix = rigidBodyTransform.getTransformation();
  transformationAvailable = true;

  // Output to file
  std::ofstream fout(calibFile.c_str());
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      fout << transformMatrix(i,j) << " ";
    }
    fout << endl;
  }
  fout.close();

}

/**
 * \brief  Calculates the ground plane based on user entered points 
 */
void calculateGroundPlane() {
  pcl::PointCloud<pcl::PointXYZ> groundPlaneCloud;
  pcl::PointIndices inliers;
  pcl::NormalEstimation<pcl::PointXYZ,pcl::Normal> normalEstimator;

  for (int i = 0; i < MAX_GROUND_POINTS; i++) {
    pcl::PointXYZ point;
    point.x = groundPoints[i].x();
    point.y = groundPoints[i].y();
    point.z = groundPoints[i].z();
    groundPlaneCloud.points.push_back(point);
    inliers.indices.push_back(i);
  }

  float curvature;
  normalEstimator.computePointNormal(groundPlaneCloud, inliers.indices, groundPlaneParameters, curvature);
}

/**
 * \brief  Obtains a point by intersecting the ray entered by the user with the ground plane
 * \return The location of the point in the kinects frame of reference 
 */
Eigen::Vector3f getPointFromGroundPlane() {
  
  //Obtain a point and normal for the plane
  Eigen::Vector3f c(0,0,-groundPlaneParameters(3)/groundPlaneParameters(2));
  Eigen::Vector3f n(groundPlaneParameters(0), groundPlaneParameters(1), groundPlaneParameters(2));

  float t = (c - rayPt1).dot(n) / (rayPt2 - rayPt1).dot(n);
  
  Eigen::Vector3f point;  
  point = rayPt1 + t*(rayPt2 - rayPt1);

  return point;
}

/**
 * \brief  Displays the status on the visualizer window
 */
void displayStatus(const char* format, ...) {

  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  status = std::string(buffer);
  //ROS_INFO(buffer);
} 

/**
 * \brief   Collects information about the ray in the kinect's frame of reference based on pixel indicated by the user
 */
void collectRayInfo(int x, int y) {
  cv::Point2d origPt(x, y), rectPt;
  rectPt = model.rectifyPoint(origPt);
  cv::Point3d ray = model.projectPixelTo3dRay(rectPt);
  rayPt1 = Eigen::Vector3f(0,0,0);
  rayPt2 = Eigen::Vector3f(ray.x, ray.y, ray.z);
} 

/**
 * \brief   Helper function to calculate the distance of a point from a ray
 *
 * This is usefull in obtaining information about a point entered by the user from the point cloud itself
 */
float distanceLineFromPoint(Eigen::Vector3f ep1, Eigen::Vector3f ep2, Eigen::Vector3f point) {
  return ((point - ep1).cross(point - ep2)).norm() / (ep2 - ep1).norm();
} 

/**
 * \brief   Returns a point directly sampled from the pointcloud
 * \param   point A reference to the object through which the sampled value is returned 
 * \return  true if enough samples were obtained to get a good average, false otherwise
 */
bool getPointFromCloud(Eigen::Vector3f &point) {
  
  unsigned int count = 0;
  Eigen::Vector3f averagePt(0, 0, 0);

  for (unsigned int i = 0; i < cloud.points.size(); i++) {

    pcl::PointXYZRGB *pt = &cloud.points[i];

    // Failed Points
    if (!pcl_isfinite(pt->x))
      continue;

    // Calculate Distance for valid points
    Eigen::Vector3f pt2(pt->x, pt->y, pt->z); 
    float distance = distanceLineFromPoint(rayPt1, rayPt2, pt2);
    if (distance < 0.025) { // within 2.5 cm of the ray
      averagePt+= pt2;
      count++;
    }
  }
  averagePt /= count;

  point = averagePt;

  return count > 10;

}

/**
 * \brief   The mouse callback on the camera image being displayed by the Kinect.
 *
 * Mouse Events are used to collect information about the ground points and landmarks
 * being indicated by the user.
 */
void imageMouseCallback(int event, int x, int y, int flags, void* param) {

  switch(event) {

    case CV_EVENT_LBUTTONDOWN: {

      switch(state) {
        case COLLECT_GROUND_POINTS: {             // Obtain ground point (lclick)
          collectRayInfo(x, y);
          state = GET_GROUND_POINT_INFO;
          break;
        }
        case TRANSITION_TO_LANDMARK_COLLECTION: {  // 
          calculateGroundPlane();
          numGroundPoints = 0;
          state = COLLECT_LANDMARKS;
          fieldProvider.get2dField(selectorImage, currentLandmark);
          cvNamedWindow("Selector");
          cvMoveWindow("Selector", 10, 700);
          cvShowImage("Selector", selectorImage);
          displayStatus("Select Landmark (%i of %i) (LClick), Next(RClick), Prev(Ctrl+RClick)", currentLandmark+1, ground_truth::NUM_GROUND_PLANE_POINTS);
          break;
        }
        case COLLECT_LANDMARKS: {
          if (flags & CV_EVENT_FLAG_CTRLKEY) {    // Deselect Landmark (ctrl + lclick)
            landmarkAvailable[currentLandmark] = false;
            newDisplayLandmark = true;
          } else {                                // Obtain current landmark (lclick)
            collectRayInfo(x, y);
            state = GET_LANDMARK_INFO;
          }
          break;
        }
        case TRANSFORMATION_CALCULATED: {
          stayAlive = false;
          break;
        }
        default:
          break;
      }

      break; 
    }

    case CV_EVENT_RBUTTONDOWN: {

      switch(state) {
        case TRANSITION_TO_LANDMARK_COLLECTION:
        case COLLECT_GROUND_POINTS: {
          numGroundPoints--;
          if (numGroundPoints == 0) {
            displayStatus("Select Ground Point (%i of %i)", numGroundPoints+1, MAX_GROUND_POINTS);
          } else {
            displayStatus("Select Ground Point (%i of %i) (LClick), Deselect (RClick)", numGroundPoints+1, MAX_GROUND_POINTS);
          }
          state = COLLECT_GROUND_POINTS;
          break;
        }
        case COLLECT_LANDMARKS: {
          if (flags & CV_EVENT_FLAG_CTRLKEY) {    // Go back to previous landmark (ctrl + rclick)
            currentLandmark--;
            currentLandmark = (currentLandmark < 0) ? 0 : currentLandmark;
            fieldProvider.get2dField(selectorImage, currentLandmark);
            cvShowImage("Selector", selectorImage);
            displayStatus("Select Landmark (%i of %i) (LClick), Next(RClick), Prev(Ctrl+RClick)", currentLandmark+1, ground_truth::NUM_GROUND_PLANE_POINTS);
          } else {                                // Go to next landmark (rclick)
            currentLandmark++;
            if (currentLandmark == ground_truth::NUM_GROUND_PLANE_POINTS) {
              calculateTransformation();
              displayStatus("Transformation calculated and saved. Exit (LClick)");
              state = TRANSFORMATION_CALCULATED;
            } else {
              fieldProvider.get2dField(selectorImage, currentLandmark);
              cvShowImage("Selector", selectorImage);
              displayStatus("Select Landmark (%i of %i) (LClick), Next(RClick), Prev(Ctrl+RClick)", currentLandmark+1, ground_truth::NUM_GROUND_PLANE_POINTS);
            }
          }
          break;
        }
        default:
          break;
      }
    
      break; // outer
    }
  }
}

/**
 * \brief  Callback function for the image message being received from the kinect driver 
 */
void imageCallback(const sensor_msgs::ImageConstPtr& image,
    const sensor_msgs::CameraInfoConstPtr& camInfo) {
  ROS_DEBUG("Image received height %i, width %i", image->height, image->width);
  mImage.lock();
  rgbImage = bridge.imgMsgToCv(image, "bgr8");
  model.fromCameraInfo(camInfo);
  mImage.unlock();
}

/**
 * \brief   Callback function for the point cloud message received from the kinect driver
 */
void cloudCallback (const sensor_msgs::PointCloud2ConstPtr& cloudPtrMsg) {
  ROS_DEBUG("PointCloud with %d, %d data points (%s), stamp %f, and frame %s.", cloudPtrMsg->width, cloudPtrMsg->height, pcl::getFieldsList (*cloudPtrMsg).c_str (), cloudPtrMsg->header.stamp.toSec (), cloudPtrMsg->header.frame_id.c_str ()); 
  mCloud.lock();
  cloudPtr = cloudPtrMsg;
  mCloud.unlock();
}

int main (int argc, char** argv) {

  ros::init (argc, argv, "kinect_position_calibrator");
  ros::NodeHandle nh;

  // Get the queue size from the command line
  terminal_tools::parse_argument (argc, argv, "-qsize", queue_size);

  terminal_tools::parse_argument (argc, argv, "-calibFile", calibFile);
  ROS_INFO("Calib File: %s", calibFile.c_str());

  // Create a ROS subscriber for the point cloud
  ros::Subscriber subCloud = nh.subscribe ("input", queue_size, cloudCallback);

  // Subscribe to image using image transport
  image_transport::ImageTransport it(nh);
  image_transport::CameraSubscriber subImage = it.subscribeCamera("inputImage", 1, imageCallback);

  // Stuff to display the point cloud properly
  pcl_visualization::PCLVisualizer visualizer (argc, argv, "Online PointCloud2 Viewer");
  visualizer.addCoordinateSystem(); // Good for reference

  // Stuff to display the rgb image
  cvStartWindowThread();
  cvNamedWindow("ImageCam");
  cvMoveWindow("ImageCam", 0,0);
  cvSetMouseCallback( "ImageCam", imageMouseCallback);

  // Stuff to display the selector
  currentLandmark = 0;
  selectorImage = cvCreateImage(cvSize(SELECTOR_IMAGE_WIDTH, SELECTOR_IMAGE_HEIGHT), IPL_DEPTH_8U, 3);
  
  displayStatus("Select Ground Point (%i of %i) (LClick)", numGroundPoints+1, MAX_GROUND_POINTS);

  while (nh.ok() && stayAlive) {

    // Spin
    ros::spinOnce ();
    ros::Duration (0.001).sleep();
    visualizer.spinOnce (10);

    // If no cloud received yet, continue
    if (!cloudPtr)
      continue;

    if (cloudPtr == oldCloudPtr)
      continue;

    mCloud.lock ();
    pcl::fromROSMsg(*cloudPtr, cloud);

    switch (state) {

      case GET_GROUND_POINT_INFO: {
        bool pointAvailable = getPointFromCloud(groundPoints[numGroundPoints]);
        if (!pointAvailable) {
          displayStatus("Unable to get point data. Select Ground Point (%i of %i) (LClick)", numGroundPoints+1, MAX_GROUND_POINTS);
          state = COLLECT_GROUND_POINTS;
          break;
        }
        numGroundPoints++;
        if (numGroundPoints == MAX_GROUND_POINTS) {
          displayStatus("Proceed to Landmark Selection (LClick), Deselect (RClick)", currentLandmark+1, ground_truth::NUM_GROUND_PLANE_POINTS);
          state = TRANSITION_TO_LANDMARK_COLLECTION;
        } else {
          displayStatus("Select Ground Point (%i of %i) (LClick), Deselect (RClick)", numGroundPoints+1, MAX_GROUND_POINTS);
          state = COLLECT_GROUND_POINTS; 
        }
        break;
      }

      case GET_LANDMARK_INFO: {
        landmarkPoints[currentLandmark] = getPointFromGroundPlane();
        landmarkAvailable[currentLandmark] = true;
        state = COLLECT_LANDMARKS;
        displayStatus("Landmark Info obtained (%i of %i), Redo(LClick), Deselect(Ctrl+LClick), Next(RClick), Prev(Ctrl+RClick)", currentLandmark+1, ground_truth::NUM_GROUND_PLANE_POINTS);
        newDisplayLandmark = true;
        break;
      }

      default:
        break;
    }

    // Display point cloud
    if (transformationAvailable) {
      pcl::transformPointCloud(cloud, transformedCloud, transformMatrix);
      displayCloud(visualizer, transformedCloud);
    } else {
      displayCloud(visualizer, cloud);
    }

    // Display spheres during ground point selection
    if (displayGroundPoints != numGroundPoints) {
      // Add necessary spheres
      for (; displayGroundPoints < numGroundPoints; displayGroundPoints++) {
        pcl::PointXYZ point(groundPoints[displayGroundPoints].x(), groundPoints[displayGroundPoints].y(), groundPoints[displayGroundPoints].z());
        visualizer.addSphere(point, 0.05, 0,1,0, getUniqueName("ground", displayGroundPoints));
      }
      // Remove unnecessary spheres
      for (; displayGroundPoints > numGroundPoints; displayGroundPoints--) {
        visualizer.removeShape(getUniqueName("ground", displayGroundPoints - 1));
      }
    }

    // Display spheres during landmark selection
    if (displayLandmark != currentLandmark || newDisplayLandmark) {
      visualizer.removeShape(getUniqueName("landmark", displayLandmark));
      if (currentLandmark != ground_truth::NUM_GROUND_PLANE_POINTS)
        displayLandmark = currentLandmark;
    }
    if (displayLandmark == currentLandmark && landmarkAvailable[displayLandmark] && newDisplayLandmark) {
      pcl::PointXYZ point(landmarkPoints[displayLandmark].x(), landmarkPoints[displayLandmark].y(), landmarkPoints[displayLandmark].z());
      visualizer.addSphere(point, 0.05, 0,1,0, getUniqueName("landmark", displayLandmark));
      newDisplayLandmark = false;
    }

    // Use old pointer to prevent redundant display
    oldCloudPtr = cloudPtr;
    mCloud.unlock();

    mImage.lock();
    if (rgbImage) {
      cvShowImage("ImageCam", rgbImage);
    }
    mImage.unlock();

    visualizer.removeShape("status");
    visualizer.addText(status, 75, 0, "status");
  }

  return (0);
}
