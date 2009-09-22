package TOOL.Learning;

import java.util.Vector;

import TOOL.TOOL;
import TOOL.Vision.*;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImageOverlay;
import TOOL.Image.ProcessedImage;
import TOOL.Image.ColorTable;
import TOOL.Data.Frame;

import TOOL.Data.Classification.KeyFrame.GoalType;
import TOOL.Data.Classification.KeyFrame.CrossType;


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
 * @author modified Octavian Neamtu 2009 - stolen for learning by Eric Chown 2009
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

	public final static int ORANGE = 0;
	public final static int WHITE = 5;
	public final static int YELLOW = 1;
	public final static int BLUE = 2;
	public final static int RED = 4;
	public final static int NAVY = 3;
	public final static int GREEN = 6;

    //images + colortable
    private TOOLImage rawImage;
    private ProcessedImage thresholdedImage;
    private ThresholdedImageOverlay thresholdedOverlay;
    private ColorTable  colorTable;

    //objects - these are just pointers to the objects in the visionLink
    private Ball ball;
	private boolean seeBall;
	private CrossType seeCross;
	private GoalType seeBlue, seeYellow;
	private int seeRedRobots, seeBlueRobots;
    private Vector<VisualFieldObject> visualFieldObjects;
	private int tableSize = 128;
	private int stats[][][][] = new int[tableSize][tableSize][tableSize][12];

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
		seeBall = false;
		seeBlue = GoalType.NO_POST;
		seeYellow = GoalType.NO_POST;
		seeCross = CrossType.NO_CROSS;
		seeRedRobots = 0;
		seeBlueRobots = 0;
    }

	/** We are going to collect stats on the pixels we see in a bunch of frames.  For any
		given pixel value we'll collect how it correlates to the various objects we are
		concerned with.
	 */
	public void initStats() {
		for (int i = 0; i < tableSize; i++) {
			for (int j = 0; j < tableSize; j++) {
				for (int k = 0; k < tableSize; k++) {
					for (int l = 0; l < 12; l++) {
						stats[i][j][k][l] = 0;
					}
				}
			}
		}
	}

	/** Go through the current
	 */
	public void updateStats(boolean orange, boolean yellow, boolean blue, boolean white,
							boolean red, boolean navy) {
        int Y_SHIFT =colorTable.getYShift();
        int CB_SHIFT =colorTable.getCBShift();
        int CR_SHIFT =colorTable.getCRShift();

		int h = rawImage.getHeight();
		int w = rawImage.getWidth();
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				int[] current = rawImage.getPixel(i,j);
				current[0] = current[0] >> Y_SHIFT;
				current[1] = current[1] >> CB_SHIFT;
				current[2] = current[2] >> CR_SHIFT;
				if (orange) {
					stats[current[0]][current[1]][current[2]][ORANGE]++;
				} else {
					stats[current[0]][current[1]][current[2]][ORANGE]-=10;
				}
				if (yellow) {
					stats[current[0]][current[1]][current[2]][YELLOW]++;
				} else {
					stats[current[0]][current[1]][current[2]][YELLOW]-=10;
				}
				if (blue) {
					stats[current[0]][current[1]][current[2]][BLUE]++;
				} else {
					stats[current[0]][current[1]][current[2]][BLUE]-=10;
				}
				if (white) {
					stats[current[0]][current[1]][current[2]][WHITE]+=10;
				} else {
					stats[current[0]][current[1]][current[2]][WHITE]-=5;
				}
				if (red) {
					stats[current[0]][current[1]][current[2]][RED]++;
 				} else {
					stats[current[0]][current[1]][current[2]][RED]-=10;
				}
				if (navy) {
					stats[current[0]][current[1]][current[2]][NAVY]++;
				} else {
					stats[current[0]][current[1]][current[2]][NAVY]-=10;
				}
				stats[current[0]][current[1]][current[2]][GREEN]++;
			}
		}
	}

	public void printStats() {
		int pixie[] = new int[3];
		for (int i = 0; i < tableSize; i++) {
			for (int j = 0; j < tableSize; j++) {
				for (int k = 0; k < tableSize; k++) {
					for (int l = 0; l < 12; l++) {
						if (stats[i][j][k][l] > 10) {
							pixie[0] = i; pixie[1] = j; pixie[2] = k;
							byte col = Vision.GREY;
							switch (l) {
							case ORANGE:
								col = Vision.ORANGE; break;
							case WHITE:
								col = Vision.WHITE; break;
							case YELLOW:
								col = Vision.YELLOW; break;
							case BLUE:
								col = Vision.BLUE; break;
							case NAVY:
								col = Vision.NAVY; break;
							case RED:
								col = Vision.RED; break;
							case GREEN:
								col = Vision.GREEN; break;
							}
							colorTable.setRawColor(pixie, col);
							l = 12;
						}
					}
				}
			}
		}

	}

    public void newFrame(Frame f, ColorTable c) {
		rawImage = f.image();
        colorTable = c;
        //init the objects
        if (rawImage != null && colorTable != null)  {
            thresholdedImage = new ProcessedImage(rawImage, colorTable);
			if (thresholdedOverlay == null)
				thresholdedOverlay = new ThresholdedImageOverlay(thresholdedImage.getWidth(),
																 thresholdedImage.getHeight());
			else
				thresholdedOverlay.resetPixels();
		}
    }

    //This updates the whole processed stuff
    //- the thresholded image, the field objects and the ball
    public void update(boolean silent) {
	//if the thresholdedImage is not null, process it again
        if (thresholdedImage != null)  {
            //we process the image; the visionLink updates itself with the new data from the bot
            thresholdedImage.thresholdImage(rawImage, colorTable);
            //get the ball from the link
            ball = thresholdedImage.getVisionLink().getBall();
            visualFieldObjects = thresholdedImage.getVisionLink().getVisualFieldObjects();
            //draw the stuff onto the overlay
			if (!silent)
				drawObjectBoxes();
        }
		//else the thresholdedImage is null, so initialize it
		else {
			thresholdedImage = new ProcessedImage(rawImage, colorTable);
			update(silent);
		}
    }

	public void updateObjects() {
		if (ball == null) {
			seeBall = false;
		}
		else if (ball.getRadius() > 0) {
			seeBall = true;
		}
		else {
			seeBall = false;
		}
		seeYellow = GoalType.NO_POST;
		seeBlue = GoalType.NO_POST;
		seeCross = CrossType.NO_CROSS;
		seeRedRobots = 0;
		seeBlueRobots = 0;
		VisualFieldObject obj;
    	for (int i = 0; i < visualFieldObjects.size(); i++) {
            obj = visualFieldObjects.elementAt(i);
            if (obj.getWidth() > 0){
                //choose a color
                byte color;
                switch(obj.getID()) {
                case VisualFieldObject.BLUE_GOAL_LEFT_POST:
					if (seeBlue == GoalType.RIGHT)
						seeBlue = GoalType.BOTH;
					else
						seeBlue = GoalType.LEFT;
					break;
                case VisualFieldObject.YELLOW_GOAL_LEFT_POST:
					if (seeYellow == GoalType.RIGHT)
						seeYellow = GoalType.BOTH;
					else
						seeYellow = GoalType.LEFT;
					break;

                case VisualFieldObject.BLUE_GOAL_RIGHT_POST:
					if (seeBlue == GoalType.LEFT)
						seeBlue = GoalType.BOTH;
					else
						seeBlue = GoalType.RIGHT;
					break;
                case VisualFieldObject.YELLOW_GOAL_RIGHT_POST:
					if (seeYellow == GoalType.LEFT)
						seeYellow = GoalType.BOTH;
					else
						seeYellow = GoalType.RIGHT;
					break;
                case VisualFieldObject.BLUE_GOAL_POST:
					seeBlue = GoalType.UNSURE;
					break;
                case VisualFieldObject.YELLOW_GOAL_POST:
					seeYellow = GoalType.UNSURE;
					break;
					// Note:  We currently have only abstract crosses in Nao code
				case VisualFieldObject.ABSTRACT_CROSS:
					seeCross = CrossType.UNKNOWN;
					break;
				case VisualFieldObject.BLUE_CROSS:
					if (seeCross == CrossType.YELLOW)
						seeCross = CrossType.DOUBLE_CROSS;
					else
						seeCross = CrossType.BLUE;
					break;
				case VisualFieldObject.YELLOW_CROSS:
					if (seeCross == CrossType.BLUE)
						seeCross = CrossType.DOUBLE_CROSS;
					else
						seeCross = CrossType.YELLOW;
					break;
                default: break;
                }
            }
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
                default: color = Vision.BLACK;
					break;
                }
                //draw the box
                thresholdedOverlay.drawPolygon(obj.getLeftTopX(), obj.getRightTopX(),
                                               obj.getRightBottomX(), obj.getLeftBottomX(),
                                               obj.getLeftTopY(), obj.getRightTopY(),
                                               obj.getRightBottomY(), obj.getLeftBottomY(),
                                               VISUAL_OBJECT_THICKNESS, color);
            }
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
    public int getProcessTime() { return thresholdedImage.getVisionLink().getProcessTime();}
	public String getBallString() {
		if (seeBall)
			return "Yes";
		return "No";
	}

	public boolean getBallVision() {return seeBall;}
	public int getRedRobotsVision() {return seeRedRobots;}
	public int getBlueRobotsVision() {return seeBlueRobots;}
	public GoalType getBlueGoalVision() {return seeBlue;}
	public GoalType getYellowGoalVision() {return seeYellow;}
	public CrossType getCrossVision() {return seeCross;}

	public String getBlueGoalString() {
		switch (seeBlue) {
		case NO_POST: return "None";
		case LEFT: return "Left";
		case RIGHT: return "Right";
		case UNSURE: return "Unknown";
		case BOTH: return "Both";
		}
		return "Error";
	}
	public String getYellowGoalString() {
		switch (seeYellow) {
		case NO_POST: return "None";
		case LEFT: return "Left";
		case RIGHT: return "Right";
		case UNSURE: return "Unsure";
		case BOTH: return "Both";
		}
		return "Error";
	}
	public String getCrossString() {
		switch (seeCross) {
		case NO_CROSS: return "None";
		case BLUE: return "Blue";
		case YELLOW: return "Yellow";
		case UNKNOWN: return "Unknown";
		case DOUBLE_CROSS: return "Both";
		}
		return "Error";
	}
	public String getRedRobotString() {
		return ""+seeRedRobots;
	}
	public String getBlueRobotString() {
		return ""+seeBlueRobots;
	}

    //setters
    public void setImage(TOOLImage i) { rawImage = i; }
    public void setThreshImage(ProcessedImage i) { thresholdedImage = i;  }
    public void setColorTable(ColorTable c) { colorTable = c; }
    public void setBall(Ball b) { ball = b;  }
}
