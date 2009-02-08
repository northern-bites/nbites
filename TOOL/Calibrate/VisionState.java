package TOOL.Calibrate;

import java.util.Vector;

import TOOL.TOOL;
import TOOL.Vision.Vision;
import TOOL.Vision.TOOLVisionLink;
import TOOL.Vision.Ball;
import TOOL.Vision.VisualFieldObject;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImageOverlay;
import TOOL.Image.ProcessedImage;
import TOOL.Image.ColorTable;
import TOOL.Data.Frame;

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
    public final static int GOAL_POST_BOX_THICKNESS = 2;
    public final static byte GOAL_RIGHT_POST_BOX_COLOR = Vision.ORANGEYELLOW;
    public final static byte GOAL_LEFT_POST_BOX_COLOR = Vision.ORANGERED;
    public final static byte GOAL_POST_BOX_COLOR = Vision.PINK;

    //images + colortable
    private TOOLImage rawImage;
    private ProcessedImage thresholdedImage;
    private ThresholdedImageOverlay thresholdedOverlay;
    private ColorTable  colorTable;
  
    //objects - these are just pointers to the objects in the visionLink
    private Ball ball;
    private Vector<VisualFieldObject> visualFieldObjects;

    //gets the image from the data frame, inits colortable
    public VisionState(Frame f, ColorTable c) {
	rawImage = f.image();
        colorTable = c;
	//init the objects
        if (rawImage != null && colorTable != null)  {
	    thresholdedImage = new ProcessedImage(rawImage, colorTable);
	    thresholdedOverlay = new ThresholdedImageOverlay(thresholdedImage.getWidth(), 
							     thresholdedImage.getHeight());
	}
    }

    //This updates the whole processed stuff 
    //- the thresholded image, the field objects and the ball
    public void update() {
	if (thresholdedImage != null)  {
	    //we process the image; the visionLink updates itself with the new data from the bot
	    thresholdedImage.thresholdImage();
	    //get the ball from the link
	    ball = thresholdedImage.getVisionLink().getBall();
	    visualFieldObjects = thresholdedImage.getVisionLink().getVisualFieldObjects();
	    //draw the stuff onto the overlay
	    drawObjectBoxes();
	}
    }
    
    //drawObjectBoxes - draws the object onto the overlay
    public void drawObjectBoxes(){
	thresholdedOverlay.resetPixels();//reset the overlay
	//set ball box
	if (ball.getRadius() > 0)
	    thresholdedOverlay.drawCircle(ball.getCenterX(), ball.getCenterY(),
					  (int) ball.getRadius(),
					  BALL_BOX_THICKNESS, BALL_BOX_COLOR);
	//set VisualFieldObjects
	VisualFieldObject obj;
	//loop through the objects
    	for (int i = 0; i < visualFieldObjects.size(); i++) {
	    obj = visualFieldObjects.elementAt(i);
	    //choose a color
	    byte color;
	    switch(obj.getID()) {
	    case VisualFieldObject.BLUE_GOAL_LEFT_POST:
	    case VisualFieldObject.YELLOW_GOAL_LEFT_POST: 
		color = GOAL_LEFT_POST_BOX_COLOR; break;
	    case VisualFieldObject.BLUE_GOAL_RIGHT_POST:
	    case VisualFieldObject.YELLOW_GOAL_RIGHT_POST: 
		color = GOAL_RIGHT_POST_BOX_COLOR; break;
	    case VisualFieldObject.BLUE_GOAL_POST:
	    case VisualFieldObject.YELLOW_GOAL_POST: 
		color = GOAL_POST_BOX_COLOR; break;
	    default: color = Vision.BLACK; break;
	    }
	    //draw the box
	    if (obj.getWidth() > 0)
		thresholdedOverlay.drawPolygon(obj.getLeftTopX(), obj.getRightTopX(),
					       obj.getRightBottomX(), obj.getLeftBottomX(),
					       obj.getLeftTopY(), obj.getRightTopY(),
					       obj.getRightBottomY(), obj.getLeftBottomY(),
					       GOAL_POST_BOX_THICKNESS, color);
	}
    }
    
    //load frame - loads data from a frame - we're interested in the raw image
    public void loadFrame(Frame f) {
	rawImage = f.image();
    }
	
    //getters
    public TOOLImage getImage() { return rawImage;  }
    public ProcessedImage getThreshImage() { return thresholdedImage;  }
    public ThresholdedImageOverlay getThreshOverlay() { return thresholdedOverlay; }
    public ColorTable getColorTable() { return colorTable;  }
    public Ball getBall() { return ball; }

    //setters
    public void setImage(TOOLImage i) { rawImage = i; }
    public void setThreshImage(ProcessedImage i) { thresholdedImage = i;  }
    public void setColorTable(ColorTable c) { colorTable = c; }
    public void setBall(Ball b) { ball = b;  }
}
