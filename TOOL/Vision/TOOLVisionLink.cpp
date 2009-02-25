/**
 * This is the cpp end of the JNI connection between the tool and the cpp vision
 * algorithm. See the method processFrame() implemented below.
 *
 * @author Johannes Strom
 * @author Mark McGranagan
 *
 * @date November 2008
**/

#include <jni.h>

#include <iostream>
#include <vector>
#include <string>

#include "TOOL_Vision_TOOLVisionLink.h"
#include "Vision.h"
#include "NaoPose.h"
#include "Sensors.h"
#include "VisualFieldObject.h"
#include "VisualLine.h"
using namespace std;

static long long
micro_time (void)
{
#ifdef __GNUC__
    // Needed for microseconds which we convert to milliseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000000 + tv.tv_usec;
#else
    return 0L;
#endif
}

/**
 *
 * This is the central cpp method called by the Java TOOL to run vision
 * results. It takes in the raw image data, as well as the joint angles
 * and single byte array representation of the color table.
 * (possibly later it will also need to take other sensor values.)
 *
 * Due to the difficulty of JNI, we currently also require that the thresholded
 * array which Java wants filled be passed in as well. This removes the need
 * for us to construct a java byte[][] from cpp.
 *
 * This method explicitly returns nothing. The results of vision computation
 * are sent back to Java via some setter methods. Right now we only send back
 * the thresholded array after vision is done with it. In the future,
 * we should make a method to translate a cpp field object into a
 * Data/FieldObject.java compatible field object.
 *
 * KNOWN INEFFICIENCIES:In the future, we'd like to be able
 * to store a static file-level pointer to the vision class. A method which
 * instantiates vision  be called in the TOOLVisionLink.java  contructor.
 * This instance of vision will also need to be destroyed somehow when
 * we are done with this VisionLink
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

     //Instantiate the vision stuff
     static Sensors sensors = Sensors();
     static NaoPose pose = NaoPose(&sensors);
     static Profiler profiler = Profiler(&micro_time);
     static Vision vision = Vision(&pose,&profiler);
  
    JNIEXPORT void JNICALL Java_TOOL_Vision_TOOLVisionLink_cppProcessImage
     (JNIEnv * env, jobject jobj, jbyteArray jimg, jfloatArray jjoints,
      jbyteArray jtable, jobjectArray thresh_target)  {
      //Size checking -- we expect the sizes of the arrays to match
      //Base these on the size cpp expects for the image
      unsigned int tlenw =
	env->GetArrayLength((jbyteArray)
			    env->GetObjectArrayElement(thresh_target,0));
      //If one of the dimensions is wrong, we exit
      if(env->GetArrayLength(jimg) != IMAGE_BYTE_SIZE) {
	cout << "Error: the image had the wrong byte size" << endl;
	return;
      }
      if (env->GetArrayLength(jjoints) != 22) {
	cout << "Error: the joint array had incorrect dimensions" << endl;
	return ;
      }
      if (env->GetArrayLength(jtable) != YMAX*UMAX*VMAX) {
	cout << "Error: the color table had incorrect dimensions" << endl;
	return;
      }
      if (env->GetArrayLength(thresh_target) != IMAGE_HEIGHT ||
	tlenw != IMAGE_WIDTH) {
	cout << "Error: the thresh_target had incorrect dimensions" << endl;
	return;
      }
      
      //load the table
      jbyte *buf_table = env->GetByteArrayElements( jtable, 0);
      byte * table = (byte *)buf_table; //convert it to a reg. byte array
      vision.thresh->initTableFromBuffer(table);
      env->ReleaseByteArrayElements( jtable, buf_table, 0);
      
      //Set the Senors data - Note: set visionBodyAngles not bodyAngles
      float * joints = env->GetFloatArrayElements(jjoints,0);
      vector<float> joints_vector = vector<float>(&joints[0],&joints[22]);
      env->ReleaseFloatArrayElements(jjoints,joints,0);
      sensors.setVisionBodyAngles(joints_vector);
      
      // Clear the debug image on which the vision algorithm can draw
      vision.thresh->initDebugImage();
      
      //get pointer access to the java image array
      jbyte *buf_img = env->GetByteArrayElements( jimg, 0);
      byte * img = (byte *)buf_img; //convert it to a reg. byte array
      //PROCESS VISION!!
      vision.notifyImage(img);
      //vision.drawBoxes();
      env->ReleaseByteArrayElements( jimg, buf_img, 0);

      //copy results from vision thresholded to the array passed in from java
      //we access to each row in the java array, and copy in from cpp thresholded
      //we may in the future want to experiment with malloc, for increased speed
      for(int i = 0; i < IMAGE_HEIGHT; i++){
	jbyteArray row_target=
	  (jbyteArray) env->GetObjectArrayElement(thresh_target,i);
	jbyte* row = env->GetByteArrayElements(row_target,0);
	
	for(int j = 0; j < IMAGE_WIDTH; j++){
	  row[j]= vision.thresh->thresholded[i][j];
	}
	env->ReleaseByteArrayElements(row_target, row, 0);
      }
      
      //get the id for the java class, so we can get method IDs
      jclass javaClass = env->GetObjectClass(jobj);
      
      //push the ball
      jmethodID setBallInfo = env->GetMethodID(javaClass, "setBallInfo", "(DDIIIID)V");
      env->CallVoidMethod(jobj, setBallInfo, 
			  vision.ball->getWidth(), vision.ball->getHeight(), 
			  vision.ball->getX(), vision.ball->getY(),
			  vision.ball->getCenterX(), vision.ball->getCenterY(),
			  vision.ball->getRadius());
      
      //get the method ID for the field object setter
      jmethodID setFieldObjectInfo = env->GetMethodID(javaClass, "setFieldObjectInfo", 
						      "(IDDIIIIIIII)V");
      
      //push each field object
      VisualFieldObject *obj;
      int k = 0;
      while(k != -1) {
	//loop through all the objects we want to pass
	switch(k){
	case 0: obj = vision.bgrp; k++; break;
	case 1: obj = vision.bglp; k++; break;
	case 2: obj = vision.ygrp; k++; break;
	case 3: obj = vision.yglp; k++; break;
	case 4: obj = vision.ygBackstop; k++; break;
	case 5: obj = vision.bgBackstop; k++; break;
	default: k = -1; obj = NULL; break;
	}
	if (obj!=NULL)
	cout<<obj->getWidth();
	if (obj != NULL) {
	  env->CallVoidMethod(jobj, setFieldObjectInfo,
			      (int) obj->getID(),
			      obj->getWidth(), obj->getHeight(),
			      obj->getLeftTopX(), obj->getLeftTopY(),
			      obj->getRightTopX(), obj->getRightTopY(),
			      obj->getLeftBottomX(), obj->getLeftBottomY(),
			      obj->getRightBottomX(), obj->getRightBottomY());
	}
      }
      //get the methodIDs for the visual line setter methods from java
      jmethodID setVisualLineInfo = env->GetMethodID(javaClass, "setVisualLineInfo",
						     "(IIII)V");
      jmethodID prepPointBuffers = env->GetMethodID(javaClass, "prepPointBuffers", 
							 "(I)V");
      jmethodID setPointInfo = env->GetMethodID(javaClass, "setPointInfo", 
						"(IIDI)V");
      jmethodID setUnusedPointsInfo = env->GetMethodID(javaClass, "setUnusedPointsInfo",
						   "()V");
      jmethodID setVisualCornersInfo = env->GetMethodID(javaClass, "setVisualCornersInfo",
						       "(II)V");
      //push data from the lines object
      const vector<VisualLine> *lines = vision.fieldLines->getLines();
      for (vector<VisualLine>::const_iterator i = lines->begin();
	   i!= lines->end(); i++) {
	env->CallVoidMethod(jobj, prepPointBuffers,
			    i->points.size());
	for(vector<linePoint>::const_iterator j = i->points.begin();
	    j != i->points.end(); j++) {
	  env->CallVoidMethod(jobj, setPointInfo,
			      j->x, j->y,
			      j->lineWidth, j->foundWithScan);
	}
	env->CallVoidMethod(jobj, setVisualLineInfo,
			    i->start.x, i->start.y,
			    i->end.x, i->end.y);
      }
      //push data from unusedPoints
      const list <linePoint> *unusedPoints = vision.fieldLines->getUnusedPoints();
      env->CallVoidMethod(jobj, prepPointBuffers, unusedPoints->size());
      for (list <linePoint>::const_iterator i = unusedPoints->begin(); 
	   i != unusedPoints->end(); i++)
	env->CallVoidMethod(jobj, setPointInfo,
			    i->x, i->y,
			    i->lineWidth, i->foundWithScan);
      env->CallVoidMethod(jobj, setUnusedPointsInfo);
      //push data from visualCorners
      const list <VisualCorner>* corners = vision.fieldLines->getCorners();
      for (list <VisualCorner>::const_iterator i = corners->begin();
	   i != corners->end(); i++)
	env->CallVoidMethod(jobj, setVisualCornersInfo,
			    i->getX(), i->getY());
      //horizon line
      jmethodID setHorizonInfo = env->GetMethodID(javaClass, "setHorizonInfo", 
						  "(IIIII)V");
      env->CallVoidMethod(jobj, setHorizonInfo, 
			  vision.pose->getLeftHorizon().x,
			  vision.pose->getLeftHorizon().y,
			  vision.pose->getRightHorizon().x,
			  vision.pose->getRightHorizon().y,
			  vision.thresh->getVisionHorizon());
      return;
    }

  
#ifdef __cplusplus
}
#endif

