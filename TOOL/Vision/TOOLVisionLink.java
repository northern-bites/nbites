
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

import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.UnsatisfiedLinkError;

import TOOL.Misc.Estimate;

public class TOOLVisionLink {
    static private boolean visionLinkSuccessful;

    public TOOLVisionLink() {
    }

    public boolean isLinkActive()
    {
        return visionLinkSuccessful;
    }
    public byte[][] processImage(byte[] img_data, int imageWidth,
                                 int imageHeight,
                                 float[] joint_data,
                                 float[] sensor_data,
                                 byte[] ct_data)
    {
        byte[][] threshResult = new byte[imageHeight][imageWidth];
        if( visionLinkSuccessful){
            try{
                cppProcessImage(img_data,joint_data,
                                sensor_data,ct_data,
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


}