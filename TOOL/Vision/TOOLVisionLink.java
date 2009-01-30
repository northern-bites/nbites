
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
import TOOL.Data.Ball;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.UnsatisfiedLinkError;

public class TOOLVisionLink {
    //These are defined here for ease, but need to get read from somewhere else
    public static final  int DFLT_IMAGE_WIDTH = 640;
    public static final  int DFLT_IMAGE_HEIGHT = 480;
    //image dimensions
    private int width;
    private int height;
    //objects
    private Ball ball;

    static private boolean visionLinkSuccessful;

    public TOOLVisionLink() {
        setImageDimensions(DFLT_IMAGE_WIDTH, DFLT_IMAGE_HEIGHT);
	ball = new Ball();
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
    public byte[][] processImage(byte[] img_data, float[] joint_data,
                                 byte[] ct_data)
    {
        byte[][] threshResult = new byte[height][width];
        if( visionLinkSuccessful){
            try{
                cppProcessImage(img_data,joint_data,ct_data,
                                threshResult);
            }catch(Throwable e){
                System.err.println("Error in cpp sub system. \n"+
                                   "   Processing failed.");
            }
        }
        else
            System.out.println("VisionLink inactive,"+
                               " so image processing failed");

        return threshResult;

    }

    //a simple getter for the ball 
    public Ball getBall() { return ball;  }

    //Native methods:
    native private void cppProcessImage(byte[] img_data, float[] joint_data,
                                        byte[] table_data,
                                        byte[][] threshResult);

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
    public void setBallInfo(double width, double height, int x, int y) {
	ball.setWidth(width);
	ball.setHeight(height);
	ball.setX(x);
	ball.setY(y);
    }
}