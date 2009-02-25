package TOOL.Image;

import java.util.List;
import java.util.Iterator;

import TOOL.TOOL;
import TOOL.Vision.TOOLVisionLink;
import TOOL.Data.Frame;

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
    TOOLVisionLink visionLink;

    public ProcessedImage(TOOLImage image, ColorTable cTable) {
        super(image.getWidth(), image.getHeight());
        visionLink = new TOOLVisionLink();

        this.thresholdImage(cTable, image);
    }

    /**
     * This method overrides the super class method.
     * It gets called on instantiation and habitually by other modules of the
     * TOOL.
     */
    public void thresholdImage(ColorTable table, TOOLImage image) {
        // Grab a constant reference to the current instance of the TOOL
        final TOOL tool = TOOL.instance;

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
            float[] joints = new float[22]; //default length for Nao
            Frame currentFrame = tool.getDataManager().activeFrame();
            if(currentFrame.hasJoints()){
                List<Float> list_joints = currentFrame.joints();
                joints = new float[list_joints.size()];
                int i = 0;
                for(Iterator itr = list_joints.iterator(); itr.hasNext(); ){
                joints[i++] = ((Float)itr.next()).floatValue();
                }
            }else{
                tool.CONSOLE.message("Warning: Processing image w/o " +
                                     "joint info");
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
                                                  joints,rawTable);
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
}