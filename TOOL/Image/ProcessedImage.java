package TOOL.Image;

import java.util.List;
import java.util.Iterator;

import TOOL.TOOL;
import TOOL.Vision.TOOLVisionLink;
import TOOL.Data.Frame;
import TOOL.Data.RobotDef;

import TOOL.Misc.Estimate;
import TOOL.Calibrate.VisionState;


/**
 * @author George Slavov
 * @date December 22, 2008
 * @see ThresholdedImage
 * This image will replace the old ThresholdedImage. It will actually call our
 * object recognition code to do all the processing including thresholding.
 *
 * This class will extend the thresholded image class since it provides very
 * similar functionality. In the event that TOOL Vision Link did not get loaded
 * correctly, all the calls to this image will fall back to thresholding via
 * java methods in ThresholdedImage.
 */

public class ProcessedImage extends ThresholdedImage {
    private TOOLVisionLink visionLink;//the link

    public ProcessedImage(Frame currentFrame, ColorTable cTable) {
        super(currentFrame.image().getWidth(), currentFrame.image().getHeight());

        visionLink = new TOOLVisionLink();
    }

    /**
     * This method overrides the super class method.
     * It gets called on instantiation and habitually by other modules of the
     * TOOL.
     * NOTE: This method assumes we are working with a NAO image. If someone
     *       cares to fix it...be my guest. Sincerely, George.
     */
    public void thresholdImage(Frame currentFrame, ColorTable table) {
        // Grab a constant reference to the current instance of the TOOL
        final TOOL tool = TOOL.instance;
        TOOLImage image = currentFrame.image();
        if(table == null)
            return;
        // Isn't it REALLY lame that these get initialized for the first time
        // in this method?!
        colorTable = table;
        baseImage = image;

        if(visionLink == null || !visionLink.isLinkActive())
            // just fall back to good ol' thresholding
            super.thresholdImage(table, image);
        else {
            //Get the joints from the frame if it exists
        	
            
            float[] joints = new float[RobotDef.NAO_DEF.numJoints()];
            if(currentFrame != null && currentFrame.hasJoints()){
                List<Float> list_joints = currentFrame.joints();
                joints = new float[list_joints.size()];
                int i = 0;
                //for(Iterator itr = list_joints.iterator(); itr.hasNext(); ){
                for (Float value : list_joints) {
                    joints[i++] = value;
                }
            }else{
                tool.CONSOLE.message("Warning: Processing image w/o " +
                                     "joint info");
            }

            float[] sensors = new float[RobotDef.NAO_DEF.numSensors()];
            if (currentFrame != null && currentFrame.hasSensors()) {
                List<Float> list_sensors = currentFrame.sensors();
                sensors = new float[list_sensors.size()];
                int i = 0;
                for (Float value : list_sensors) {
                    sensors[i++] = value;
                }
            }
            else {
                tool.CONSOLE.message("Warning: Processing image w/o " +
                                     "sensor info");
            }
            
            // Convert the TOOLImage to the one-dimensional format that the C++
            // side is used to.
            byte[] rawImage = new byte[baseImage.rawImageSize()];
            baseImage.writeByteArray(rawImage);

            // Convert the color table to the one-dimensional format that the
            // C++ side is used to.
            int ct_size =  colorTable.getYDimension()*
                colorTable.getUDimension()*colorTable.getVDimension();
            byte[] rawTable = new byte[ct_size];
            colorTable.writeByteArray(rawTable);

            // This variable is declared in the superclass and used by its
            // getters.
	    thresholded = visionLink.processImage(rawImage,
						  image.getWidth(),
						  image.getHeight(),
						  joints, sensors,
						  rawTable);
	}
    }

    /**
     * Overriding the ThresholdImage.thresholdImage() method, so that the
     * ProcessedImage.thresholdImage(colorTable, baseImage) is called rather
     * than the ThresholdImage one.
     **/
    public void thresholdImage() {
        if (colorTable == null) {
            TOOL.CONSOLE.error("ProcessedImage.thresholdImage() called on" +
                               " an image without associated color table");
        } else if(baseImage == null) {
            TOOL.CONSOLE.error("ProcessedImage.thresholdImage() called on" +
                               " an image without associated base image");
        } else
            thresholdImage(colorTable, baseImage);
    }

    /**
     * Overriding the ThresholdImage.pixEstimate() method, so that we use
     * the vision link and ask C++ to give us a pix estimate.
     **/
    public Estimate pixEstimate(int pixelX, int pixelY,
                                float objectHeight) throws RuntimeException {
        if(visionLink == null || !visionLink.isLinkActive())
            // Vision link is inactive. Pretend we are a thresholded image.
            // Thresholded image throws an exception when asked to do pixEst.
            super.pixEstimate(pixelX, pixelY, objectHeight);
	
        return visionLink.pixEstimate(pixelX, pixelY,
                                      objectHeight);
    }
    //getLink - returns the visionLink
    public TOOLVisionLink getVisionLink(){ return visionLink; }
    public void clearColoring(){ thresholded = new byte[getHeight()][getWidth()];}
    
}