
// This file is part of TOOL, a robotics interaction and development
// package created by the Northern Bites RoboCup team of Bowdoin College
// in Brunswick, Maine.
//
// TOOL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TOOL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TOOL.  If not, see <http://www.gnu.org/licenses/>.


/**
 * This class provides the link to cpp. To get the results of vision processing
 * from cpp, you can call visionLink.processImage(), which returns a
 * thresholded image.
 *
 * Future Work:
 * IMPORTANT: When the cpp sublayer has an error (i.e. segfault), Java cannot
 *            catch it.
 *
 * It is not clear what the right format for the img_data is.
 * Vision expects to get a pointer to a continuous 1D array of length w*h*2
 *
 * In the future, we'd like to get back much more information --
 * like field objects, etc
 *
 * This class purposefully doesnt rely directly on the TOOL class,
 * so that it is easier to generate the native headers. If desirable
 * some work on the make file would allow you to pass in an instance of the
 * tool, allowing you to write to the tool.CONSOLE
 *
 * @author Octavian Neamtu
 */

package TOOL.Vision;

//the object classes
import TOOL.Vision.Ball;
import TOOL.Vision.VisualFieldObject;
import TOOL.Vision.VisualLine;
import TOOL.Vision.LinePoint;
import TOOL.Vision.VisualCorner;
import TOOL.Vision.Horizon;

import TOOL.Vision.Vision;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.UnsatisfiedLinkError;
import java.util.Vector;
import java.util.Date;

import TOOL.Misc.Estimate;
public class TOOLVisionLink {
    //These are defined here for ease, but need to get read from somewhere else
    public static final  int DFLT_IMAGE_WIDTH = 640;
    public static final  int DFLT_IMAGE_HEIGHT = 480;

    public static final byte GREEN = Vision.GREEN;
    public static final byte BLUE = Vision.BLUE;
    public static final byte YELLOW = Vision.YELLOW;
    public static final byte ORANGE = Vision.ORANGE;
    public static final byte YELLOWWHITE = Vision.YELLOWWHITE;
    public static final byte BLUEGREEN = Vision.BLUEGREEN;
    public static final byte ORANGERED = Vision.ORANGERED;
    public static final byte ORANGEYELLOW = Vision.ORANGEYELLOW;
    public static final byte RED = Vision.RED;
    public static final byte WHITE = Vision.WHITE;
	public static final byte GREY = Vision.GREY;
    public static final byte NAVY = Vision.NAVY;

    public static final byte BIT_GREY = 0x00;
    public static final byte BIT_WHITE = 0x01;
    public static final byte BIT_GREEN = 0x02;
    public static final byte BIT_BLUE = 0x04;
    public static final byte BIT_YELLOW = 0x08;
    public static final byte BIT_ORANGE = 0x10;
    public static final byte BIT_RED = 0x20;
    public static final byte BIT_NAVY = 0x40;
    public static final int BIT_DRAWING = 0x80;
    public static final byte BIT_MASK = 0x7e;

    //image dimensions
    private int width;
    private int height;
    //objects
    private Ball ball;
    private VisualFieldObject bgrp, bglp, ygrp, yglp, bgBackstop, ygBackstop;
    private Vector<VisualFieldObject> visualFieldObjects;
    private Vector<VisualLine> visualLines;
    private Vector<VisualLine> expectedVisualLines;
    private Vector<LinePoint> unusedPoints;
    private Vector<VisualCorner> visualCorners;
    private Horizon poseHorizon;
    private int visionHorizon;
    //point helper arrays - helps with loading the point
    private int[] pointX, pointY, pointFWS;
    private double[] pointLineWidth;
    private int len, curEl;
    static private boolean visionLinkSuccessful;
    //processTime
    private int processTime;

    public TOOLVisionLink() {
        setImageDimensions(DFLT_IMAGE_WIDTH, DFLT_IMAGE_HEIGHT);
        ball = new Ball();
        /*
          bgrp = new VisualFieldObject(VisualFieldObject.BLUE_GOAL_RIGHT_POST);
          bglp = new VisualFieldObject(VisualFieldObject.BLUE_GOAL_LEFT_POST);
          ygrp = new VisualFieldObject(VisualFieldObject.YELLOW_GOAL_RIGHT_POST);
          yglp = new VisualFieldObject(VisualFieldObject.YELLOW_GOAL_LEFT_POST);
          //should add the backstops too if ever going to be used
          */
        visualFieldObjects = new Vector<VisualFieldObject>();
        visualLines = new Vector<VisualLine>(5, 5);
        expectedVisualLines = new Vector<VisualLine>(5, 5);
        visualCorners = new Vector<VisualCorner>(10);
    }

    /**
     * Method sets the size of image, etc that we are sending to cpp
     * Important: The cpp lower level will rejects images with the wrong
     * dimensions
     */
    public void setImageDimensions(int w, int h) {
        this.width = w; this.height = h;
    }

    //returns true if link is active
    public boolean isLinkActive() {
        return visionLinkSuccessful;
    }

    //gets as params the raw image, the joint data and the color table
    //returns a thresholded image
    //it also gets all the objects (ball etc) when the cpp
    //calls the java setballinfo
    public byte[][] processImage(byte[] img_data, int imageWidth,
                                 int imageHeight,
                                 float[] joint_data,
                                 float[] sensor_data,
                                 byte[] ct_data)
    {
        byte[][] threshResult = new byte[imageHeight][imageWidth];
        if( visionLinkSuccessful){
            try{
                //reset the field object vector so that it's ready for the
                //new stuff coming in
                visualFieldObjects.clear();
                visualLines.clear();

                //call the jni function
                cppProcessImage(img_data,joint_data,
                                sensor_data,ct_data,
                                threshResult);
                // update the thresholded array for display
                for (int i = 0; i < imageWidth; i++) {
                    for (int j = 0; j < imageHeight; j++) {
                        byte col = threshResult[j][i];
                        if (col == Vision.MAROON ||
							col == Vision.PINK ||
							col == Vision.CYAN ||
							col == Vision.MAGENTA ||
							col == Vision.PURPLE ||
							col == Vision.BLACK ||
							col == Vision.RED ||
							col == Vision.ORANGE) {
                        } else if ((col & BIT_ORANGE) > 0) {
                            if ((col & BIT_RED) > 0) {
                                col = ORANGERED;
                            }
                            else if ((col & BIT_YELLOW) > 0) {
                                col = ORANGEYELLOW;
                            } else {
                                col = ORANGE;
                            }
                        }
                        else if ((col & BIT_GREEN) > 0) {
                            if ((col & BIT_BLUE) > 0) {
                                col = BLUEGREEN;
                            } else {
                                col = GREEN;
                            }
                        }
                        else if ((col & BIT_BLUE) > 0) {
                            col = BLUE;
                        } else if ((col & BIT_NAVY) > 0) {
                            col = NAVY;
                        }
                        else if ((col & BIT_YELLOW) > 0) {
                            if ((col & BIT_WHITE) > 0) {
                                col = YELLOWWHITE;
                            } else {
                                col = YELLOW;
                            }
                        }
                        else if ((col & BIT_WHITE) > 0) {
                            col = WHITE;
                        }
                        else if ((col & BIT_RED) > 0) {
                            col = RED;
                        }
                        threshResult[j][i] = col;
                    }
                }
            }catch(Throwable e){
                System.err.println("Error in c++ side of the vision link. \n");
                e.printStackTrace();
            }
        }
        else {
            System.out.println("VisionLink inactive,"+
                               " so image processing failed");
        }
        return threshResult;

    }

    public Estimate pixEstimate(int pixelX, int pixelY, float objectHeight) {
        double[] estimateResult = new double[5];
        if (visionLinkSuccessful) {
            try {
                cppPixEstimate(pixelX, pixelY, objectHeight, estimateResult);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n"+
                                   "   pixEstimate failed.");
            }
        }
        else
            System.out.println("VisionLink inactive," +
                               " so pixEstimate does not work");

        return new Estimate(estimateResult[0], estimateResult[1],
                            estimateResult[2], estimateResult[3],
                            estimateResult[4]);
    }

    //getters for the objects
    public Ball getBall() { return ball; }
    public Vector<VisualFieldObject> getVisualFieldObjects() { return visualFieldObjects;}
    public Vector<VisualLine> getVisualLines() { return visualLines;}
    public Vector<VisualLine> getExpectedVisualLines() { return expectedVisualLines;}
    public Vector<LinePoint> getUnusedPoints() { return unusedPoints;}
    public Vector<VisualCorner> getVisualCorners() { return visualCorners;}
    public Horizon getPoseHorizon(){ return poseHorizon;}
    public int getVisionHorizon(){ return visionHorizon;}
    public int getProcessTime(){ return processTime;}
    //Native methods:
    native private void cppProcessImage(byte[] img_data, float[] joint_data,
                                        float[] sensors_data,
                                        byte[] table_data,
                                        byte[][] threshResult);

    native private void cppPixEstimate(int pixelX, int pixelY,
                                       float objectHeight,
                                       double[] estimateResult);
    native private void cppGetCameraCalibrate(float[] calibrateArray);

    native private void cppSetCameraCalibrate(float[] calibrateArray);
    native private void cppSetFieldLinesDebugVertEdgeDetect(boolean _bool);
    native private void cppSetFieldLinesDebugHorEdgeDetect(boolean _bool);
    native private void cppSetFieldLinesDebugSecondVertEdgeDetect(boolean _bool);
    native private void cppSetFieldLinesDebugCreateLines(boolean _bool);
    native private void cppSetFieldLinesDebugFitUnusedPoints(boolean _bool);
    native private void cppSetFieldLinesDebugJoinLines(boolean _bool);
    native private void cppSetFieldLinesDebugExtendLines(boolean _bool);
    native private void cppSetFieldLinesDebugIntersectLines(boolean _bool);
    native private void cppSetFieldLinesDebugIdentifyCorners(boolean _bool);
    native private void cppSetFieldLinesDebugCcScan(boolean _bool);
    native private void cppSetFieldLinesDebugRiskyCorners(boolean _bool);
    native private void cppSetFieldLinesDebugCornerAndObjectDistances(boolean _bool);
	native private void cppSetBallDebugBall(boolean _bool);
	native private void cppSetBallDebugBallDistance(boolean _bool);
	native private void cppSetCrossDebug(boolean _bool);
	native private void cppSetPostPrintDebug(boolean _bool);
	native private void cppSetPostDebug(boolean _bool);
	native private void cppSetPostLogicDebug(boolean _bool);
	native private void cppSetPostSanityDebug(boolean _bool);
	native private void cppSetPostCorrectDebug(boolean _bool);
	native private void cppSetFieldHorizonDebug(boolean _bool);
	native private void cppSetFieldEdgeDebug(boolean _bool);
    native private void cppSetShootingDebug(boolean _bool);
    native private void cppSetOpenFieldDebug(boolean _bool);
    native private void cppSetEdgeDetectionDebug(boolean _bool);
    native private void cppSetHoughTransformDebug(boolean _bool);
    native private void cppSetRobotDebug(boolean _bool);
    native private void cppSetDebugDangerousBall(boolean _bool);
    native private void cppSetVisualLinesDebug(boolean _bool);

    // Set edge detection threshold
    native private void cppSetEdgeThreshold(int _t);

    // Set hough line acceptance threshold
    native private void cppSetHoughAcceptThreshold(int _t);

    //Load the cpp library that implements the native methods
    static
    {
        try{
            System.loadLibrary("TOOLVisionLink");
            visionLinkSuccessful = true;
            System.out.println("TOOLVisionLink lib loaded successfuly");
        }catch(UnsatisfiedLinkError e){
            visionLinkSuccessful = false;
            System.err.print("Vision Link Failed to initialize: ");
            System.err.println(e.getMessage());
        }

    }

    //sets the ball variables
    public void setBallInfo(double width, double height, int x, int y,
                            int centerX, int centerY, double radius) {
        ball.setWidth(width);
        ball.setHeight(height);
        ball.setX(x);
        ball.setY(y);
        ball.setCenterX(centerX);
        ball.setCenterY(centerY);
        ball.setRadius(radius);
    }
    //set field object variables
    public void setFieldObjectInfo(int id, double width, double height,
                                   int ltx, int lty, int rtx, int rty,
                                   int lbx, int lby, int rbx, int rby) {

        VisualFieldObject fieldObject = new VisualFieldObject();
        //attach the data to the object
        fieldObject.setID(id);
        fieldObject.setWidth(width); fieldObject.setHeight(height);
        fieldObject.setLeftTopX(ltx); fieldObject.setLeftTopY(lty);
        fieldObject.setRightTopX(rtx); fieldObject.setRightTopY(rty);
        fieldObject.setLeftBottomX(lbx); fieldObject.setLeftBottomY(lby);
        fieldObject.setRightBottomX(rbx); fieldObject.setRightBottomY(rby);
        visualFieldObjects.add(fieldObject);//add the object to the vector
    }
    //prepare the visual line data for the tranfer of the points;
    public void prepPointBuffers(int len){
        this.len = len;
        curEl = 0;
        pointX = new int[len];
        pointY = new int[len];
        pointLineWidth = new double[len];
        pointFWS = new int[len];
    }
    //the point loading function should be replaced with a direct transfer;
    //too slow
    //set the data in visual lines
    public void setVisualLineInfo(int bx, int by, int ex, int ey){
        Vector<LinePoint> linePoints = new Vector<LinePoint>(len);
        for(int i = 0; i < len; i++)
            linePoints.add(new LinePoint(pointX[i], pointY[i],
                                         pointLineWidth[i], pointFWS[i]));
        //flush the buffers
        pointX = null;
        pointY = null;
        pointLineWidth = null;
        pointFWS = null;
        visualLines.add(new VisualLine(bx, by, ex, ey, linePoints));
    }
    //similar to setVisualLineInfo, the use is for getting expected lines (see camera calibration)
    public void setExpectedVisualLineInfo(int bx, int by, int ex, int ey){
        Vector<LinePoint> linePoints = new Vector<LinePoint>(len);
        for(int i = 0; i < len; i++)
            linePoints.add(new LinePoint(pointX[i], pointY[i],
                                         pointLineWidth[i], pointFWS[i]));
        //flush the buffers
        pointX = null;
        pointY = null;
        pointLineWidth = null;
        pointFWS = null;
        expectedVisualLines.add(new VisualLine(bx, by, ex, ey, linePoints));
    }
    //set the points
    public void setPointInfo(int x, int y, double lw, int fws){
        pointX[curEl] = x;
        pointY[curEl] = y;
        pointLineWidth[curEl] = lw;
        pointFWS[curEl] = fws;
        curEl++;
    }
    //set the unused points
    public void setUnusedPointsInfo(){
        unusedPoints = new Vector<LinePoint>(len);
        for(int i = 0; i < len; i++)
            unusedPoints.add(new LinePoint(pointX[i], pointY[i],
                                           pointLineWidth[i], pointFWS[i]));
        //flush the buffers
        pointX = null;
        pointY = null;
        pointLineWidth = null;
        pointFWS = null;
    }
    //set the corners
    public void setVisualCornersInfo(int x, int y,
                                     float distance, float bearing,
                                     int cornerShape){
        visualCorners.add(new VisualCorner(x, y, distance,
                                           bearing, cornerShape));
    }
    //set the pose + vision horizon
    public void setHorizonInfo(int lx, int ly, int rx, int ry, int visHor){
        poseHorizon = new Horizon(lx, ly, rx, ry);
        visionHorizon = visHor;
    }
    //set the processTime
    public void setProcessTime(int p) {
        processTime = p;
    }

    public void setEdgeThreshold(int t) {
        if (visionLinkSuccessful) {
            try{
                cppSetEdgeThreshold(t);
            } catch (Throwable e){
                System.err.println("Error in cpp sub system. \n "+
                                   "\tsetEdgeThreshold failed.");
            }
        }
    }

    public void setHoughAcceptThreshold(int t) {
        if (visionLinkSuccessful) {
            try{
                cppSetHoughAcceptThreshold(t);
            } catch (Throwable e){
                System.err.println("Error in cpp sub system. \n "+
                                   "\tsetHoughAcceptThreshold failed.");
            }
        }
    }

    public float[] getCameraCalibrate() {
        float[] calibrateArray = new float[9];
        if (visionLinkSuccessful) {
            try {
                cppGetCameraCalibrate(calibrateArray);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n"+
                                   "   getCameraCalibrate failed.");
            }
        }
        else
            System.out.println("VisionLink inactive," +
                               " so cameraCalibrate does not work");

        return calibrateArray;
    }

    public void setCameraCalibrate(float[] calibrateArray) {
        if (visionLinkSuccessful) {
            try {
                cppSetCameraCalibrate(calibrateArray);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n"+
                                   "   getCameraCalibrate failed.");
            }
        }
        else
            System.out.println("VisionLink inactive," +
                               " so cameraCalibrate does not work");
    }


    /**
     * FieldLines cpp setters
     */
    public void setFieldLinesDebugVertEdgeDetect(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugVertEdgeDetect(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugHorEdgeDetect(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugHorEdgeDetect(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugSecondVertEdgeDetect(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugSecondVertEdgeDetect(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugCreateLines(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugCreateLines(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugFitUnusedPoints(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugFitUnusedPoints(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugJoinLines(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugJoinLines(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugExtendLines(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugExtendLines(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugIntersectLines(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugIntersectLines(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");

            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setFieldLinesDebugIdentifyCorners(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugIdentifyCorners(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }

    public void setFieldLinesDebugCcScan(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugCcScan(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }

    public void setFieldLinesDebugRiskyCorners(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugRiskyCorners(_bool);
            } catch(Throwable e) {
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }

    public void setFieldLinesDebugCornerAndObjectDistances(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldLinesDebugCornerAndObjectDistances(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugBall(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetBallDebugBall(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugBallDistance(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetBallDebugBallDistance(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugCross(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetCrossDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugPostPrint(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetPostPrintDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugPost(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetPostDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugPostLogic(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetPostLogicDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugPostSanity(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetPostSanityDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugPostCorrect(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetPostCorrectDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugFieldHorizon(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldHorizonDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugFieldEdge(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetFieldEdgeDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugOpenField(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetOpenFieldDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugShooting(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetShootingDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }

    public void setDebugEdgeDetection(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetEdgeDetectionDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugHoughTransform(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetHoughTransformDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugRobot(boolean _bool) {
        if (visionLinkSuccessful) {
            try {
                cppSetRobotDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugDangerousBall(boolean _bool) {
        if (visionLinkSuccessful) {
            try {
                cppSetDebugDangerousBall(_bool);

            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
    public void setDebugVisualLines(boolean _bool){
        if (visionLinkSuccessful) {
            try {
                cppSetVisualLinesDebug(_bool);
            } catch(Throwable e){
                System.err.println("Error in cpp sub system. \n");
            }
        } else {
            System.out.println("Vision Link Inactive. Flag not set.");
        }
    }
}