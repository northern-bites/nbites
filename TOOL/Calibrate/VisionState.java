package TOOL.Calibrate;

import java.util.Vector;

import TOOL.TOOL;
import TOOL.Vision.Vision;
import TOOL.Vision.TOOLVisionLink;
import TOOL.Vision.Ball;
import TOOL.Vision.VisualFieldObject;
import TOOL.Image.TOOLImage;
import TOOL.Image.ImageOverlay;
import TOOL.Image.ProcessedImage;
import TOOL.Image.ColorTable;

/**
 * Class VisionState
 * 
 * it holds all the vision detection stuff - the thresholded image, the ball etc
 * it updates all the stuff when things are changed in calibrate (e.g. colorTable)
 * by calling the processImage function, and then getting all the objects from the visionLink
 * and drawing them
 *
 * @see Calibrate.java
 * @see TOOLVisionLink.java
 * 
 * @author modified Octavian Neamtu 2009
 */

public class VisionState {
    
    //constants
    public final static int BALL_BOX_THICKNESS = 2;
    public final static byte BALL_BOX_COLOR = Vision.RED;

    //images + colortable
    private TOOLImage rawImage;
    private ProcessedImage thresholdedImage;
    private ImageOverlay thresholdedOverlay;
    private ColorTable  colorTable;
  
    //objects
    private Vector<VisualFieldObject> objects;
    private Ball ball;

    //gets the image from the data frame, inits colortable
    public VisionState(Frame f, ColorTable c) {
  
	rawImage = f.image();
        colorTable = c;
	
	//init the objects
	objects = new Vector<VisualFieldObject>();

        if (rawImage != null && colorTable != null)  {
	    thresholdedImage = new ProcessedImage(rawImage, colorTable, this);
	    thresholdedOverlay = new ImageOverlay(thresholdedImage.getWidth(), 
						  thresholdedImage.getHeight());
	}
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
	    //we process the image; the visionLink updates itself with the new data from the bot
	    thresholdedImage.thresholdImage();
	    //get the ball from the link
	    ball = thresholdedImage.getVisionLink().getBall();
	    //draw the stuff onto the overlay
	    drawObjectBoxes();
	}
    }
    
    //drawObjectBoxes - draws the object onto the overlay
    public void drawObjectBoxes(){
	thresholdedOverlay.resetPixels();//reset the overlay
	//set ball box
	thresholdedOverlay.setRectOverlay(ball.getX(), ball.getY(),
					  (int) ball.getWidth(), (int) ball.getHeight(),
					  BALL_BOX_THICKNESS, BALL_BOX_COLOR);
    }

    //getters
    public TOOLImage getImage() { return rawImage;  }
    public ProcessedImage getThreshImage() { return thresholdedImage;  }
    public ImageOverlay getThreshOverlay() { return thresholdedOverlay; }
    public ColorTable getColorTable() { return colorTable;  }

    public Ball getBall() { return ball; }
    //setters
    public void setImage(TOOLImage i) { rawImage = i; }
    public void setThreshImage(ProcessedImage i) { thresholdedImage = i;  }
    public void setColorTable(ColorTable c) { colorTable = c; }

    public void addObject(FieldObject obj) {  objects.add(obj);  }
    public void removeObject(FieldObject obj) {   objects.remove(obj);  }
    public void setBall(Ball b) { ball = b;  }
}
