package TOOL.Calibrate;

import java.util.Vector;

import TOOL.TOOL;
import TOOL.Vision.TOOLVisionLink;
import TOOL.Image.TOOLImage;
import TOOL.Image.ProcessedImage;
import TOOL.Image.ColorTable;
import TOOL.Data.*;

public class VisionState {
    //images + colortable
    private TOOLImage rawImage;
    private ProcessedImage thresholdedImage;
    private ColorTable  colorTable;
  
    //objects
    private Vector<FieldObject> objects;
    private Ball ball;

    //gets the image from the data frame, inits colortable
    public VisionState(Frame f, ColorTable c) {
  
	rawImage = f.image();
        colorTable = c;
	
	//init the objects
	objects = new Vector<FieldObject>();
        ball = new Ball();

        if (rawImage != null && colorTable != null)
            thresholdedImage = new ProcessedImage(rawImage, colorTable, this);
    }

    //this is useless
    public VisionState() {
        rawImage = null;
        thresholdedImage = null;
	colorTable = null;
	
        objects = new Vector<FieldObject>();
        ball = null;
    }


    //this is also useless
    public VisionState(TOOLImage newRawImage, 
		       ProcessedImage newThreshImage, 
		       ColorTable newColorTable){
	rawImage = newRawImage;
        thresholdedImage = newThreshImage;
	colorTable = newColorTable;
	
        objects = new Vector<FieldObject>();
        ball = null;
    }


    //This updates the whole processed stuff - the thresholded image, the field objects and the ball
    public void update(ProcessedImage thresholdedImage) {
	if (thresholdedImage != null)  {
	    thresholdedImage.thresholdImage();
	    ball = thresholdedImage.getVisionLink().getBall();
	    //debug
	    System.out.println(ball.getWidth());
	}
    }

    //getters
    public TOOLImage getImage() { return rawImage;  }
    public ProcessedImage getThreshImage() {  return thresholdedImage;  }
    public ColorTable getColorTable() { return colorTable;  }
    //setters
    public void setImage(TOOLImage i) { rawImage = i; }
    public void setThreshImage(ProcessedImage i) { thresholdedImage = i;  }
    public void setColorTable(ColorTable c) { colorTable = c; }

    public void addObject(FieldObject obj) {
        objects.add(obj);
    }

    public void removeObject(FieldObject obj) {
        objects.remove(obj);
    }

    public void setBall(Ball b) {
        ball = b;
    }

    public Ball getBall() {
        return ball;
    }

    public void printWidth() {
        TOOL.CONSOLE.message("Image width: " + rawImage.getWidth());
    }
}
