/**
 * This class provides the link to cpp. To get the results of vision processing
 * from cpp, you can call visionLink.processImage(), which returns a 
 * thresholded image.
 *
 *
 * Future Work:
 * IMPORTANT: We absolutely need to try, catch all the calls to the cpp
 * subsystem. Otherwise when cpp fails, we crash the tool.
 * 
 * It is not clear what the right format for the img_data is.
 * Vision expects to get a pointer to a continuous 1D array of length w*h*2
 * Java 
 *
 * In the future, we'd like to get back much more information -- 
 * like field objects, etc
 */

import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;


class TOOLVisionLink {
    //These are defined here for ease, but need to get read from somewhere else
    public static final  int DFLT_IMAGE_WIDTH = 640;
    public static final  int DFLT_IMAGE_HEIGHT = 480;

    int width;
    int height;

    //private byte thresholded[][];

    public TOOLVisionLink() {
        width = DFLT_IMAGE_WIDTH;
        height = DFLT_IMAGE_HEIGHT;

        //thresholded = new byte[height][width];
    }

    public byte[][] processImage(byte[] img_data, float[] joint_data,
                                 String colorTablePath )
    {
        byte[][] threshResult = new byte[height][width];
        cppProcessImage(img_data,joint_data,colorTablePath,threshResult);
        return threshResult;
            
    }
    native private void cppProcessImage(byte[] img_data, float[] joint_data,
                                        String colorTablePath,
                                        byte[][] threshResult);

    /**
     * Method sets the size of image, etc that we are sending to cpp
     * Important: The cpp lower level will rejects images with the wrong 
     * dimensions
     */
    public void setImageDimensions(int w, int h)
    {
        width = w; height = h;
    }

    //Load the cpp library that implements the native methods
    static { System.loadLibrary("TOOLVisionLink"); }

   /**
     * Test method to load a frame file from disk
     */
    public static byte[] getFrameDataFromFile(String framePath, int size)
    {
        FileInputStream fis = null;
        //System.out.println("Loading Frame from File: " + framePath + "... ");

        byte[] buf  = new byte[size];

        try {
            fis = new FileInputStream(framePath);
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            System.out.println("frane load failed.");
            return buf;
        }

        try {
            fis.read(buf);
        } catch (IOException e) {
            System.err.println(e.getMessage());
            System.out.println("table load failed.");
            return buf;
        }

        return buf;

    }


    public static void main(String args[])
    {
        TOOLVisionLink vLink = new TOOLVisionLink();

        String colorTablePath = "table.mtb";

        //Load the first 2 frames in this directory
        for(int i = 0; i < 2; i++){
            String framePath = ""+i+".NFRM";
            byte[] img = vLink.getFrameDataFromFile(framePath,640*480*2);

            float[] joints = new float[22];
            byte[][] thresh = vLink.processImage(img,joints,colorTablePath);

            /* int greenCounter = 0;
            for(int k = 0; k < 480; k++){

                for(int j = 0; j < 640; j++){
                    if (thresh[k][j] == 2){
                        greenCounter++;
                    }
                }
            }
            System.out.println("Count of green "+greenCounter);*/
        }
    }



}