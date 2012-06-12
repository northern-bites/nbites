package TOOL.Calibrate;

import java.util.Vector;

import TOOL.TOOL;
import TOOL.Vision.*;
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
    public final static byte BALL_BOX_THICKNESS = 2;
    public final static byte BALL_BOX_COLOR = Vision.RED;

    public final static byte VISUAL_OBJECT_THICKNESS = 2;
    public final static byte GOAL_RIGHT_POST_BOX_COLOR = Vision.MAGENTA;
    public final static byte GOAL_LEFT_POST_BOX_COLOR = Vision.RED;
    public final static byte GOAL_POST_BOX_COLOR = Vision.BLACK;

    public final static byte BLUE_GOAL_BACKSTOP_COLOR = Vision.WHITE;
    public final static byte YELLOW_GOAL_BACKSTOP_COLOR = Vision.WHITE;

    public final static byte VISUAL_LINE_THICKNESS = 2;
    public final static byte VISUAL_LINE_COLOR = Vision.BLUE;
    
    public final static byte EXPECTED_VISUAL_LINE_THICKNESS = 2;
    public final static byte EXPECTED_VISUAL_LINE_COLOR = Vision.RED;

    public final static byte POINT_CROSS_SIZE = 4;
    public final static byte POINT_CROSS_THICKNESS = 2;

    public final static byte POINT_HORIZONTAL_COLOR = Vision.YELLOW;
    public final static byte POINT_VERTICAL_COLOR = Vision.RED;

    public final static byte CORNER_POINT_COLOR = Vision.ORANGE;

    public final static byte POSE_HORIZON_THICKNESS = 2;
    public final static byte POSE_HORIZON_COLOR = Vision.BLUE;
    public final static byte VISION_HORIZON_COLOR = Vision.MAGENTA;

	public final static byte YELLOW_CROSS_COLOR = Vision.YELLOW;
	public final static byte BLUE_CROSS_COLOR = Vision.BLUE;


    //images + colortable
    private TOOLImage rawImage;
    private ProcessedImage thresholdedImage;
    private ThresholdedImageOverlay thresholdedOverlay;
    private ColorTable  colorTable;
    private Frame f;

    //objects - these are just pointers to the objects in the visionLink
    private Ball ball;
    private Vector<VisualFieldObject> visualFieldObjects;
    private Vector<VisualLine> visualLines;
    private Vector<VisualLine> expectedVisualLines;
    private Vector<VisualCorner> visualCorners;
    private Horizon poseHorizon;
    private int visionHorizon;
    //a boolean that says if we should draw the thresh colors
    private boolean drawThreshColors = true;

    //gets the image from the data frame, inits colortable
    public VisionState(Frame f, ColorTable c) {
    	this.f = f;
        rawImage = f.image();
        colorTable = c;
        //init the objects
        if (rawImage != null && colorTable != null)  {
            thresholdedImage = new ProcessedImage(f, colorTable);
            thresholdedOverlay = new ThresholdedImageOverlay(thresholdedImage.getWidth(),
                                                             thresholdedImage.getHeight());
        }
    }

    public void newFrame(Frame f, ColorTable c) {
    	this.f = f;
    	rawImage = f.image();
        colorTable = c;
        //init the objects
        if (rawImage != null && colorTable != null)  {
            thresholdedImage = new ProcessedImage(f, colorTable);
            thresholdedOverlay =
                new ThresholdedImageOverlay(thresholdedImage.getWidth(),
                                            thresholdedImage.getHeight());
        }
    }

    //This updates the whole processed stuff
    //- the thresholded image, the field objects and the ball
    public void update() {
        //if the thresholdedImage is not null, process it again
        if (thresholdedImage != null)  {
            //we process the image; the visionLink updates itself with
            //the new data from the bot
            thresholdedImage.thresholdImage(f, colorTable);
            if (!drawThreshColors) thresholdedImage.clearColoring();
            //get the ball from the link
            ball = thresholdedImage.getVisionLink().getBall();
            visualFieldObjects =
                thresholdedImage.getVisionLink().getVisualFieldObjects();
            visualLines = thresholdedImage.getVisionLink().getVisualLines();
            expectedVisualLines =
                thresholdedImage.getVisionLink().getExpectedVisualLines();
            visualCorners = thresholdedImage.getVisionLink().getVisualCorners();
            poseHorizon = thresholdedImage.getVisionLink().getPoseHorizon();
            visionHorizon = thresholdedImage.getVisionLink().getVisionHorizon();

            //draw the stuff onto the overlay
            drawObjectBoxes();
        }
        //else the thresholdedImage is null, so initialize it
        else {
            thresholdedImage = new ProcessedImage(f, colorTable);
            update();
        }
    }

    //drawObjectBoxes - draws the object onto the overlay
    public void drawObjectBoxes(){
        thresholdedOverlay.resetPixels();//reset the overlay
        //set the ball circle
        if (ball.getRadius() > 0)
            thresholdedOverlay.drawCircle(ball.getCenterX(), ball.getCenterY(),
                                          (int) ball.getRadius(),
                                          BALL_BOX_THICKNESS, BALL_BOX_COLOR);
        //set visual field objects
        VisualFieldObject obj;
        //loop through the objects
    	for (int i = 0; i < visualFieldObjects.size(); i++) {
            obj = visualFieldObjects.elementAt(i);
            if (obj.getWidth() > 0){
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
                case VisualFieldObject.BLUE_GOAL_BACKSTOP:
                    color = BLUE_GOAL_BACKSTOP_COLOR; break;
                case VisualFieldObject.YELLOW_GOAL_BACKSTOP:
                    color = YELLOW_GOAL_BACKSTOP_COLOR; break;
                case VisualFieldObject.YELLOW_CROSS:
                    color = YELLOW_CROSS_COLOR;
                    break;
                case VisualFieldObject.BLUE_CROSS:
                    color = BLUE_CROSS_COLOR;
                    break;
                case VisualFieldObject.NAVY_ROBOT:
                    color = Vision.YELLOW;
                    break;
                case VisualFieldObject.RED_ROBOT:
                    color = Vision.BLUE;
                    break;
                default: color = Vision.BLACK; break;
                }
                //draw the box
                thresholdedOverlay.drawPolygon(obj.getLeftTopX(), obj.getRightTopX(),
                                               obj.getRightBottomX(), obj.getLeftBottomX(),
                                               obj.getLeftTopY(), obj.getRightTopY(),
                                               obj.getRightBottomY(), obj.getLeftBottomY(),
                                               VISUAL_OBJECT_THICKNESS, color);
            }
        }
        //set field lines
        VisualLine line;
        for (int i = 0; i < visualLines.size(); i++) {
            line = visualLines.elementAt(i);
            thresholdedOverlay.drawLine(line.getBeginX(), line.getBeginY(),
                                        line.getEndX(), line.getEndY(),
                                        VISUAL_LINE_THICKNESS, VISUAL_LINE_COLOR);

            Vector<LinePoint> points;
            points = line.getLinePoints();
            LinePoint pt;
            byte color;
            for (int j = 0; j < points.size(); j++) {
                pt = points.elementAt(j);
                switch (pt.foundWithScan()) {
                case LinePoint.HORIZONTAL : color = POINT_HORIZONTAL_COLOR; break;
                case LinePoint.VERTICAL : color = POINT_VERTICAL_COLOR; break;
                default : color = Vision.PINK;//this should never be the case
                }
                thresholdedOverlay.drawCross(pt.getX(), pt.getY(),
                                             POINT_CROSS_SIZE, POINT_CROSS_THICKNESS,
                                             color);
            }
        }
        //expected lines
        if (expectedVisualLines != null){
            for (int i = 0; i < expectedVisualLines.size(); i++) {
                line = expectedVisualLines.elementAt(i);
                thresholdedOverlay.drawLine(line.getBeginX(), line.getBeginY(),
                        line.getEndX(), line.getEndY(),
                        EXPECTED_VISUAL_LINE_THICKNESS, EXPECTED_VISUAL_LINE_COLOR);

                /*Vector<LinePoint> points;
            points = line.getLinePoints();
            LinePoint pt;
            byte color;
            for (int j = 0; j < points.size(); j++) {
                pt = points.elementAt(j);
                switch (pt.foundWithScan()) {
                case LinePoint.HORIZONTAL : color = POINT_HORIZONTAL_COLOR; break;
                case LinePoint.VERTICAL : color = POINT_VERTICAL_COLOR; break;
                default : color = Vision.PINK;//this should never be the case
                }
                thresholdedOverlay.drawCross(pt.getX(), pt.getY(),
                                             POINT_CROSS_SIZE, POINT_CROSS_THICKNESS,
                                             color);
            }*/
            }
        }

        //set corners
        VisualCorner corner;
        for (int i = 0; i < visualCorners.size(); i++) {
            corner = visualCorners.elementAt(i);
            thresholdedOverlay.drawCross(corner.getX(), corner.getY(),
                                         POINT_CROSS_SIZE, POINT_CROSS_THICKNESS,
                                         CORNER_POINT_COLOR);
        }
        //set pose&vision horizon
        thresholdedOverlay.drawLine(poseHorizon.getLeftX(), poseHorizon.getLeftY(),
                                    poseHorizon.getRightX(), poseHorizon.getRightY(),
                                    POSE_HORIZON_THICKNESS, POSE_HORIZON_COLOR);
        thresholdedOverlay.drawCross(thresholdedOverlay.getWidth()/2, visionHorizon,
                                     POINT_CROSS_SIZE, POINT_CROSS_THICKNESS,
                                     VISION_HORIZON_COLOR);
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
    public int getProcessTime() { return thresholdedImage.getVisionLink().getProcessTime();}
    public boolean getDrawThreshColors(){ return drawThreshColors;}

    //setters
    public void setImage(TOOLImage i) { rawImage = i; }
    public void setThreshImage(ProcessedImage i) { thresholdedImage = i;  }
    public void setColorTable(ColorTable c) { colorTable = c; }
    public void setBall(Ball b) { ball = b;  }
    public void setDrawThreshColors(boolean dr){ drawThreshColors = dr;}
    public void setEdgeThreshold(int thresh){
        thresholdedImage.getVisionLink().setEdgeThreshold(thresh);
    }
    public void setHoughAcceptThresh(int thresh){
        thresholdedImage.getVisionLink().setHoughAcceptThreshold(thresh);
    }
}
