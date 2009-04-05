
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
 */

package TOOL.Vision;

//the object classes
import TOOL.Vision.Ball;
import TOOL.Vision.VisualFieldObject;
import TOOL.Vision.VisualLine;
import TOOL.Vision.LinePoint;
import TOOL.Vision.VisualCorner;
import TOOL.Vision.Horizon;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.UnsatisfiedLinkError;
import java.util.Vector;

import TOOL.Misc.Estimate;
public class TOOLVisionLink {
    //These are defined here for ease, but need to get read from somewhere else
    public static final  int DFLT_IMAGE_WIDTH = 640;
    public static final  int DFLT_IMAGE_HEIGHT = 480;
    //image dimensions
    private int width;
    private int height;
    //objects
    private Ball ball;
    private VisualFieldObject bgrp, bglp, ygrp, yglp, bgBackstop, ygBackstop;
    private Vector<VisualFieldObject> visualFieldObjects;
    private Vector<VisualLine> visualLines;
    private Vector<LinePoint> unusedPoints;
    private Vector<VisualCorner> visualCorners;
    private Horizon poseHorizon;
    private int visionHorizon;
    //point helper arrays - helps with loading the point
    private int[] pointX, pointY, pointFWS;
    private double[] pointLineWidth;
    private int len, curEl;
    static private boolean visionLinkSuccessful;

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
    //it also gets all the objects (ball etc) when the cpp calls the java setballinfo
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
    public Vector<LinePoint> getUnusedPoints() { return unusedPoints;}
    public Vector<VisualCorner> getVisualCorners() { return visualCorners;}
    public Horizon getPoseHorizon(){ return poseHorizon;}
    public int getVisionHorizon(){ return visionHorizon;}
    //Native methods:
    native private void cppProcessImage(byte[] img_data, float[] joint_data,
                                        float[] sensors_data,
                                        byte[] table_data,
                                        byte[][] threshResult);

    native private void cppPixEstimate(int pixelX, int pixelY,
                                       float objectHeight,
                                       double[] estimateResult);

    //Load the cpp library that implements the native methods
    static
    {
        try{
            System.loadLibrary("TOOLVisionLink");
            visionLinkSuccessful = true;
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
        /*useless unles someone uses each object in particular
          switch (id) {
          case VisualFieldObject.BLUE_GOAL_RIGHT_POST: fieldObject = bgrp; break;
          case VisualFieldObject.BLUE_GOAL_LEFT_POST: fieldObject = bglp; break;
          case VisualFieldObject.YELLOW_GOAL_RIGHT_POST: fieldObject = ygrp; break;
          case VisualFieldObject.YELLOW_GOAL_LEFT_POST: fieldObject = yglp; break;
          default: fieldObject = new VisualFieldObject(); break;
          }
        */
        //attach the data to the object
        System.out.println(width);
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
    public void setVisualCornersInfo(int x, int y){
        visualCorners.add(new VisualCorner(x,y));
    }
    //set the pose + vision horizon
    public void setHorizonInfo(int lx, int ly, int rx, int ry, int visHor){
        poseHorizon = new Horizon(lx, ly, rx, ry);
        visionHorizon = visHor;
    }
}