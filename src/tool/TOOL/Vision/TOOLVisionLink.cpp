/**
 * This is the cpp end of the JNI connection between the tool and the cpp vision
 * algorithm. See the method processFrame() implemented below.
 *
 * @author Johannes Strom
 * @author Mark McGranagan
 * @author Octavian Neamtu
 *
 * @date November 2008
 * @modified 2010
 */

#include <jni.h>

#include <iostream>
#include <vector>
#include <stdint.h>

#include <boost/shared_ptr.hpp>

//#include "TOOL_Vision_TOOLVisionLink.h"
#include "Vision.h"
#include "NaoPose.h"
#include "Sensors.h"
#include "SensorDef.h"       // for NUM_SENSORS
#include "Kinematics.h"      // for NUM_JOINTS
#include "Structs.h"         // for estimate struct
#include "VisualFieldObject.h"
#include "VisualLine.h"
#include "Common.h"
#include "ImageAcquisition.h"
#include "ColorParams.h"
#include "Speech.h"

using namespace std;
using namespace boost;

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
static shared_ptr<Sensors> sensors(new Sensors(
                                       shared_ptr<Speech>(new Speech())));
static shared_ptr<NaoPose> pose(new NaoPose(sensors));
static Vision vision(pose);

JNIEXPORT void JNICALL Java_TOOL_Vision_TOOLVisionLink_cppProcessImage
(JNIEnv * env, jobject jobj, jbyteArray jimg, jfloatArray jjoints,
        jfloatArray jsensors, jbyteArray jtable, jobjectArray thresh_target) {

    //Size checking -- we expect the sizes of the arrays to match
    //Base these on the size cpp expects for the image
    unsigned int tlenw =
    env->GetArrayLength((jbyteArray)
            env->GetObjectArrayElement(thresh_target,0));
    unsigned int numSensorsInFrame = env->GetArrayLength(jsensors);
    //If one of the dimensions is wrong, we exit
    if(env->GetArrayLength(jimg) !=
       640 * 480 * 2) { // Image size
        cout << "Error: the image had the wrong byte size" << endl;
        cout << "Image byte size should be " << IMAGE_BYTE_SIZE << endl;
        cout << "Detected byte size of " << env->GetArrayLength(jimg)
        << endl;
        return;
    }
    if (env->GetArrayLength(jjoints) != Kinematics::NUM_JOINTS) {
        cout << "Error: the joint array had incorrect dimensions" << endl;
        return;
    }
    // I (George) have disabled this check because we now have a variable
    // number of sensors depending on which version of the frame format
    // is loaded.
    /*
     if (numSensorsInFrame != NUM_SENSORS) {
     cout << "Warning: This frame must be old because the number of "
     "sensors stored in it is\n"
     "wrong. The missing values will be initialized to 0."
     << endl;
     }
     */
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

    // Set the joints data - Note: set visionBodyAngles not bodyAngles
    float * joints = env->GetFloatArrayElements(jjoints,0);
    vector<float> joints_vector(&joints[0],&joints[Kinematics::NUM_JOINTS]);
    env->ReleaseFloatArrayElements(jjoints,joints,0);
    sensors->setVisionBodyAngles(joints_vector);

    // Set the sensor data
    float * sensors_array = env->GetFloatArrayElements(jsensors,0);
    vector<float> sensors_vector(&sensors_array[0],
            &sensors_array[numSensorsInFrame]);
    env->ReleaseFloatArrayElements(jsensors,sensors_array,0);

    // If there are missing sensors values in the frame, the vector will be
    // shorter. We add 0s to it until it is of size NUM_SENSORS.
    for (unsigned int i = 0; i < NUM_SENSORS - numSensorsInFrame; ++i)
    sensors_vector.push_back(0.0f);

    sensors->setLeftFootFSR(sensors_vector[0], sensors_vector[1],
            sensors_vector[2], sensors_vector[3]);
    sensors->setRightFootFSR(sensors_vector[4], sensors_vector[5],
                sensors_vector[6], sensors_vector[7]);
    sensors->setLeftFootBumper(sensors_vector[8], sensors_vector[9]);
    sensors->setLeftFootBumper(sensors_vector[10], sensors_vector[11]);
    sensors->setInertial(sensors_vector[12], sensors_vector[13], sensors_vector[14],
            sensors_vector[15], sensors_vector[16], sensors_vector[17],
            sensors_vector[18]);
    sensors->setUltraSound(sensors_vector[19], sensors_vector[20]);
    sensors->setSupportFoot(sensors_vector[21]);


    // Clear the debug image on which the vision algorithm can draw
    vision.thresh->initDebugImage();

    jbyte *buf_img = env->GetByteArrayElements( jimg, 0);
    byte * img = (byte *)buf_img; //convert it to a reg. byte array

    uint16_t *newImg = reinterpret_cast<uint16_t*>(new uint8_t[320*240*7]);

    ColorParams  cp(0,0,0,256,256,256,128,128,128);

    //timing the vision process
    long long startTime = thread_micro_time();

    // Shrink (by averaging) the image, and do color segmentation
    ImageAcquisition::acquire_image_fast(table, cp, img, newImg);

    //PROCESS VISION!!
    vision.notifyImage(newImg);

    long long processTime = thread_micro_time() - startTime;

    vision.drawBoxes();
    env->ReleaseByteArrayElements( jimg, buf_img, 0);

    //copy results from vision thresholded to the array passed in from java
    //we access to each row in the java array, and copy in from cpp thresholded
    //we may in the future want to experiment with malloc, for increased speed
    for(int i = 0; i < IMAGE_HEIGHT; i++) {
        jbyteArray row_target=
        (jbyteArray) env->GetObjectArrayElement(thresh_target,i);
        jbyte* row = env->GetByteArrayElements(row_target,0);

        for(int j = 0; j < IMAGE_WIDTH; j++) {
            row[j]= vision.thresh->getThresholded(i,j);
#ifdef OFFLINE
			// problem: the thresh image has non-bitwise colors
            if (vision.thresh->debugImage[i][j] != GREY) {
                row[j]= vision.thresh->debugImage[i][j];
            }
#endif

        }
        env->ReleaseByteArrayElements(row_target, row, 0);
    }

    delete newImg;
    env->ReleaseByteArrayElements( jtable, buf_table, 0);

    //get the id for the java class, so we can get method IDs
    jclass javaClass = env->GetObjectClass(jobj);

    //push the processTime
    jmethodID setProcessTime = env->GetMethodID(javaClass, "setProcessTime", "(I)V");
    env->CallVoidMethod(jobj, setProcessTime, (int) processTime);

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
    VisualCrossbar * cb;
    VisualCross *cross = NULL;
    VisualRobot *robot = NULL;
    int k = 0;
    while(k != -1) {
        //loop through all the objects we want to pass
        int id = 0;
        switch(k) {
        case 0: obj = vision.bgrp; k++; cb = NULL; cross = NULL; robot = NULL; break;
        case 1: obj = vision.bglp; k++; cb = NULL; cross = NULL; robot = NULL; break;
        case 2: obj = vision.ygrp; k++; cb = NULL; cross = NULL; robot = NULL; break;
        case 3: obj = vision.yglp; k++; cb = NULL; cross = NULL; robot = NULL; break;
        case 4: cb = vision.ygCrossbar; k++; obj = NULL; cross = NULL; robot = NULL; break;
        case 5: cb = vision.bgCrossbar; k++; obj = NULL; cross = NULL; robot = NULL; break;
        case 6: cross = vision.cross; k++; cb = NULL; obj = NULL; robot = NULL; break;
        case 7: robot = vision.navy1; k++; id = 49; obj = NULL; cb = NULL; cross = NULL; break;
        case 8: robot = vision.navy2; k++; id = 49; obj = NULL; cb = NULL; cross = NULL; break;
        case 9: robot = vision.navy3; k++; id = 49; obj = NULL; cb = NULL; cross = NULL; break;
        case 10: robot = vision.red1; k++; id = 50; obj = NULL; cb = NULL; cross = NULL; break;
        case 11: robot = vision.red2; k++; id = 50; obj = NULL; cb = NULL; cross = NULL; break;
        case 12: robot = vision.red3; k++; id = 50; obj = NULL; cb = NULL; cross = NULL; break;
        default: k = -1; obj = NULL; cb = NULL; break;
        }
        if (obj != NULL) {
            id = (int) obj->getID();
            if (obj->getPossibilities()->size() > 1) {
                if (id == BLUE_GOAL_LEFT_POST ||
                        id == BLUE_GOAL_RIGHT_POST ||
                        id == BLUE_GOAL_POST) {
                    id = BLUE_GOAL_POST;

                } else {
                    id = YELLOW_GOAL_POST;
                }
            }

            env->CallVoidMethod(jobj, setFieldObjectInfo,
                    id,
                    obj->getWidth(), obj->getHeight(),
                    obj->getLeftTopX(), obj->getLeftTopY(),
                    obj->getRightTopX(), obj->getRightTopY(),
                    obj->getLeftBottomX(), obj->getLeftBottomY(),
                    obj->getRightBottomX(), obj->getRightBottomY());
        } else if (cb != NULL) {
            env->CallVoidMethod(jobj, setFieldObjectInfo,
                    47,
                    cb->getWidth(), cb->getHeight(),
                    cb->getLeftTopX(), cb->getLeftTopY(),
                    cb->getRightTopX(), cb->getRightTopY(),
                    cb->getLeftBottomX(), cb->getLeftBottomY(),
                    cb->getRightBottomX(), cb->getRightBottomY());
        } else if (robot != NULL) {
            env->CallVoidMethod(jobj, setFieldObjectInfo,
                    id,
                    robot->getWidth(), robot->getHeight(),
                    robot->getLeftTopX(), robot->getLeftTopY(),
                    robot->getRightTopX(), robot->getRightTopY(),
                    robot->getLeftBottomX(), robot->getLeftBottomY(),
                    robot->getRightBottomX(), robot->getRightBottomY());
        } else if (cross != NULL) {
            env->CallVoidMethod(jobj, setFieldObjectInfo,
                    (int)cross->getID(),
                    cross->getWidth(), cross->getHeight(),
                    cross->getLeftTopX(), cross->getLeftTopY(),
                    cross->getRightTopX(), cross->getRightTopY(),
                    cross->getLeftBottomX(), cross->getLeftBottomY(),
                    cross->getRightBottomX(), cross->getRightBottomY());
        }
    }

    //get the methodIDs for the visual line setter methods from java
    jmethodID setVisualLineInfo = env->GetMethodID(javaClass, "setVisualLineInfo",
            "(IIII)V");
    jmethodID setExpectedVisualLineInfo = env->GetMethodID(javaClass, "setExpectedVisualLineInfo",
            "(IIII)V");
    jmethodID prepPointBuffers = env->GetMethodID(javaClass, "prepPointBuffers",
            "(I)V");
    jmethodID setPointInfo = env->GetMethodID(javaClass, "setPointInfo",
            "(IIDI)V");
    jmethodID setUnusedPointsInfo = env->GetMethodID(javaClass, "setUnusedPointsInfo",
            "()V");
    jmethodID setVisualCornersInfo = env->GetMethodID(javaClass, "setVisualCornersInfo",
            "(IIFFI)V");
//#define CALIBRATE_CAMERA
#ifdef CALIBRATE_CAMERA
    //push stuff from expected lines
    const vector<VisualLine> expectedLines =
    pose->getExpectedVisualLinesFromFieldPosition(CENTER_FIELD_X, CENTER_FIELD_Y, 0);//GREEN_PAD_X, FIELD_HEIGHT/2, 0);//
            for (vector<VisualLine>::const_iterator i = expectedLines.begin();
                    i!= expectedLines.end(); i++) {
                env->CallVoidMethod(jobj, prepPointBuffers,
                        i->getPoints().size());
                for(vector<linePoint>::const_iterator j = i->getPoints().begin();
                        j != i->getPoints().end(); j++) {
                    env->CallVoidMethod(jobj, setPointInfo,
                            j->x, j->y,
                            j->lineWidth, j->foundWithScan);
                }
                env->CallVoidMethod(jobj, setExpectedVisualLineInfo,
                        i->getStartpoint().x, i->getStartpoint().y,
                        i->getEndpoint().x, i->getEndpoint().y);
            }
#endif

            //push data from the lines object
            const vector< shared_ptr<VisualLine> > *lines = vision.fieldLines->getLines();
            for (vector< shared_ptr<VisualLine> >::const_iterator i = lines->begin();
                    i!= lines->end(); i++) {
                env->CallVoidMethod(jobj, prepPointBuffers,
                                    (*i)->getPoints().size());

                const vector<linePoint> points = (*i)->getPoints();
                for(vector<linePoint>::const_iterator j = points.begin();
                        j != points.end(); j++) {

                    env->CallVoidMethod(jobj, setPointInfo,
                            j->x, j->y,
                            j->lineWidth, j->foundWithScan);
                }

                env->CallVoidMethod(jobj, setVisualLineInfo,
                        (*i)->getStartpoint().x, (*i)->getStartpoint().y,
                        (*i)->getEndpoint().x, (*i)->getEndpoint().y);
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
                    i->getX(), i->getY(),
                    i->getDistance(), i->getBearing(),
                    i->getShape());
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

        JNIEXPORT void JNICALL Java_TOOL_Vision_TOOLVisionLink_cppPixEstimate
        (JNIEnv * env, jobject jobj, jint pixelX, jint pixelY,
                jfloat objectHeight, jdoubleArray estimateResult) {
            // make sure the array for the estimate is big enough. There
            // should be room for five things in there. (subject to change)
            if (env->GetArrayLength(estimateResult) !=
                    sizeof(estimate)/sizeof(float)) {
                cout << "Error: the estimateResult array had incorrect "
                "dimensions" << endl;
                return;
            }

            //load the table
            jdouble * buf_estimate =
            env->GetDoubleArrayElements( estimateResult, 0);
            double * estimate_array =
            (double *)buf_estimate;
            //vision.thresh->initTableFromBuffer(table);
            estimate est = pose->pixEstimate(static_cast<int>(pixelX),
                    static_cast<int>(pixelY),
                    static_cast<float>(objectHeight));

            estimate_array[0] = est.dist;
            estimate_array[1] = est.elevation;
            estimate_array[2] = est.bearing;
            estimate_array[3] = est.x;
            estimate_array[4] = est.y;

            env->ReleaseDoubleArrayElements( estimateResult, buf_estimate, 0);
        }

        JNIEXPORT void JNICALL Java_TOOL_Vision_TOOLVisionLink_cppGetCameraCalibrate
        (JNIEnv * env, jobject jobj, jfloatArray cameraCalibrate) {

            jfloat * cam_calibrate =
            env->GetFloatArrayElements( cameraCalibrate, 0);
            float * cam_calib = (float *) cam_calibrate;
            for (int i = 0; i < 9; i++)
            cam_calib[i] = CameraCalibrate::Params[i];
            env->ReleaseFloatArrayElements( cameraCalibrate, cam_calibrate, 0);
        }

        JNIEXPORT void JNICALL Java_TOOL_Vision_TOOLVisionLink_cppSetCameraCalibrate
        (JNIEnv * env, jobject jobj, jfloatArray cameraCalibrate) {

            jfloat * cam_calibrate =
            env->GetFloatArrayElements( cameraCalibrate, 0);
            float * cam_calib = (float *) cam_calibrate;
            CameraCalibrate::UpdateWithParams(cam_calib);
            env->ReleaseFloatArrayElements( cameraCalibrate, cam_calibrate, 0);
        }

    JNIEXPORT void JNICALL
    Java_TOOL_Vision_TOOLVisionLink_cppSetEdgeThreshold
        (JNIEnv * env, jobject jobj, jint thresh){
        vision.linesDetector.setEdgeThreshold(thresh);
    }

    JNIEXPORT void JNICALL
    Java_TOOL_Vision_TOOLVisionLink_cppSetHoughAcceptThreshold
        (JNIEnv * env, jobject jobj, jint thresh){
        vision.linesDetector.setHoughAcceptThreshold(thresh);
    }

	/**
	 * FieldLines debug flag functions
	 */
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugVertEdgeDetect
	(JNIEnv * env, jobject jobj, jboolean debugVertEdgeDetect){
		vision.fieldLines->setDebugVertEdgeDetect(debugVertEdgeDetect);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugHorEdgeDetect
	(JNIEnv * env, jobject jobj, jboolean debugHorEdgeDetect){
		vision.fieldLines->setDebugHorEdgeDetect(debugHorEdgeDetect);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugSecondVertEdgeDetect
	(JNIEnv * env, jobject jobj, jboolean debugSecondVertEdgeDetect){
		vision.fieldLines->setDebugSecondVertEdgeDetect(debugSecondVertEdgeDetect);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugCreateLines
	(JNIEnv * env, jobject jobj, jboolean debugCreateLines){
		vision.fieldLines->setDebugCreateLines(debugCreateLines);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugJoinLines
	(JNIEnv * env, jobject jobj, jboolean debugJoinLines){
		vision.fieldLines->setDebugJoinLines(debugJoinLines);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugIntersectLines
	(JNIEnv * env, jobject jobj, jboolean debugIntersectLines){
		vision.fieldLines->setDebugIntersectLines(debugIntersectLines);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugExtendLines
	(JNIEnv * env, jobject jobj, jboolean debugExtendLines){
		vision.fieldLines->setDebugExtendLines(debugExtendLines);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugIdentifyCorners
	(JNIEnv * env, jobject jobj, jboolean debugIdentifyCorners){
		vision.fieldLines->setDebugIdentifyCorners(debugIdentifyCorners);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetDebugDangerousBall
	(JNIEnv * env, jobject jobj, jboolean debugDangerousBall){
		vision.thresh->context->setDebugDangerousBall(debugDangerousBall);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugCcScan
	(JNIEnv * env, jobject jobj, jboolean debugCcScan){
		vision.fieldLines->setDebugCcScan(debugCcScan);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugRiskyCorners
	(JNIEnv * env, jobject jobj, jboolean debugRiskyCorners){
		vision.fieldLines->setDebugRiskyCorners(debugRiskyCorners);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugCornerAndObjectDistances
	(JNIEnv * env, jobject jobj, jboolean debugCornerAndObjectDistances){
		vision.fieldLines->setDebugCornerAndObjectDistances(debugCornerAndObjectDistances);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldLinesDebugFitUnusedPoints
	(JNIEnv * env, jobject jobj, jboolean debugFitUnusedPoints){
		vision.fieldLines->setDebugFitUnusedPoints(debugFitUnusedPoints);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetBallDebugBall
	(JNIEnv * env, jobject jobj, jboolean debugBall){
		vision.thresh->orange->setDebugBall(debugBall);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetBallDebugBallDistance
	(JNIEnv * env, jobject jobj, jboolean debugBallDistance){
		vision.thresh->orange->setDebugBallDistance(debugBallDistance);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetCrossDebug
	(JNIEnv * env, jobject jobj, jboolean debugCross){
		vision.thresh->cross->setCrossDebug(debugCross);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetPostPrintDebug
	(JNIEnv * env, jobject jobj, jboolean debugPostPrint){
		vision.thresh->yellow->setPrintObjs(debugPostPrint);
		vision.thresh->blue->setPrintObjs(debugPostPrint);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetPostDebug
	(JNIEnv * env, jobject jobj, jboolean debugPost){
		vision.thresh->yellow->setPostDebug(debugPost);
		vision.thresh->blue->setPostDebug(debugPost);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetPostLogicDebug
	(JNIEnv * env, jobject jobj, jboolean debugLogic){
		vision.thresh->yellow->setPostLogic(debugLogic);
		vision.thresh->blue->setPostLogic(debugLogic);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetPostSanityDebug
	(JNIEnv * env, jobject jobj, jboolean debugSanity){
		vision.thresh->yellow->setSanity(debugSanity);
		vision.thresh->blue->setSanity(debugSanity);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetPostCorrectDebug
	(JNIEnv * env, jobject jobj, jboolean debugCorrect){
		vision.thresh->yellow->setCorrect(debugCorrect);
		vision.thresh->blue->setCorrect(debugCorrect);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldHorizonDebug
	(JNIEnv * env, jobject jobj, jboolean debugHorizon){
		vision.thresh->field->setDebugHorizon(debugHorizon);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetFieldEdgeDebug
	(JNIEnv * env, jobject jobj, jboolean debugEdge){
		vision.thresh->field->setDebugFieldEdge(debugEdge);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetShootingDebug
	(JNIEnv * env, jobject jobj, jboolean debugShot){
		vision.thresh->setDebugShooting(debugShot);
	}
	JNIEXPORT void JNICALL
	Java_TOOL_Vision_TOOLVisionLink_cppSetOpenFieldDebug
	(JNIEnv * env, jobject jobj, jboolean debugField){
		vision.thresh->setDebugOpenField(debugField);
	}

    JNIEXPORT void JNICALL
    Java_TOOL_Vision_TOOLVisionLink_cppSetEdgeDetectionDebug
    (JNIEnv * env, jobject jobj, jboolean debugEdgeDetection){
        vision.thresh->setDebugEdgeDetection(debugEdgeDetection);
    }

    JNIEXPORT void JNICALL
    Java_TOOL_Vision_TOOLVisionLink_cppSetHoughTransformDebug
    (JNIEnv * env, jobject jobj, jboolean debugHoughTransform){
        vision.thresh->setDebugHoughTransform(debugHoughTransform);
    }
    JNIEXPORT void JNICALL
    Java_TOOL_Vision_TOOLVisionLink_cppSetRobotDebug
    (JNIEnv * env, jobject jobj, jboolean debugRobot) {
        vision.thresh->setDebugRobots(debugRobot);
    }

#ifdef __cplusplus
    }
#endif
