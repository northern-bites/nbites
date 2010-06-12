package TOOL.Learning;

import java.util.Vector;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import java.awt.GridLayout;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;


import TOOL.TOOL;
import TOOL.Vision.*;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImageOverlay;
import TOOL.Image.ProcessedImage;
import TOOL.Image.ColorTable;
import TOOL.Data.Frame;
import TOOL.Calibrate.ColorTableUpdate;

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

	public final static byte YELLOW_CROSS_COLOR = Vision.YELLOW;
	public final static byte BLUE_CROSS_COLOR = Vision.BLUE;

	public final static int ORANGE = 2;
	public final static int WHITE = 0;
	public final static int YELLOW = 3;
	public final static int BLUE = 4;
	public final static int RED = 5;
	public final static int NAVY = 6;
	public final static int GREEN = 1;

	public int frms = 0;
    //images + colortable
	private Frame frame;
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
	private int seeLCorners, seeTCorners, seeCcCorners;
    private Vector<VisualFieldObject> visualFieldObjects;
    private Vector<VisualCorner> visualCorners;
	private int tableSize = 128;
	private int stats[][][][] = new int[tableSize][tableSize][tableSize][7];

    //gets the image from the data frame, inits colortable
    public VisionState(Frame f, ColorTable c) {
		frame = f;
        rawImage = f.image();
        colorTable = c;
        //init the objects
        if (rawImage != null && colorTable != null)  {
            thresholdedImage = new ProcessedImage(frame, colorTable);
            thresholdedOverlay = new ThresholdedImageOverlay(thresholdedImage.getWidth(),
                                                             thresholdedImage.getHeight());
        }
		seeBall = false;
		seeBlue = GoalType.NO_POST;
		seeYellow = GoalType.NO_POST;
		seeCross = CrossType.NO_CROSS;
		seeRedRobots = 0;
		seeBlueRobots = 0;
		seeLCorners = 0;
		seeTCorners = 0;
		seeCcCorners = 0;

		createDebugWindow();
    }

	/** We are going to collect stats on the pixels we see in a bunch of frames.  For any
		given pixel value we'll collect how it correlates to the various objects we are
		concerned with.
	 */
	public void initStats() {
		for (int i = 0; i < tableSize; i++) {
			for (int j = 0; j < tableSize; j++) {
				for (int k = 0; k < tableSize; k++) {
					for (int l = 0; l < 7; l++) {
						stats[i][j][k][l] = 0;
					}
				}
			}
		}
	}

	public void reviseStats(boolean orange, boolean yellow, boolean blue, boolean white,
							boolean red, boolean navy) {
		// we need to find out where any errors are
		//update(true);
		updateObjects();
		int MISSED = 1, CORRECT = 0, FALSEPOS = 2;
		int org = CORRECT, yell = CORRECT, blew = CORRECT;
		if (orange != seeBall) {
			if (orange)
				org = FALSEPOS;
			else
				org = MISSED;
		}
		if (seeYellow != GoalType.NO_POST) {
			if (yellow)
				yell = CORRECT;
			else
				yell = MISSED;
		} else {
			if (yellow)
				yell = FALSEPOS;
			else
				yell = CORRECT;
		}
		if (seeBlue != GoalType.NO_POST) {
			if (blue)
				blew = CORRECT;
			else
				blew = MISSED;
		} else {
			if (blue)
				blew = FALSEPOS;
			else
				blew = CORRECT;
		}
		// sometimes we don't need to process the frame
		if (blew + yell + org == CORRECT) return;
		frms++;
		if (frms % 50 == 0) System.out.println("Processed "+frms+" frames");
		// now go through the image and process accordingly
        int Y_SHIFT =colorTable.getYShift();
        int CB_SHIFT =colorTable.getCBShift();
        int CR_SHIFT =colorTable.getCRShift();

		int h = rawImage.getHeight();
		int w = rawImage.getWidth();
		Horizon poseHorizon = thresholdedImage.getVisionLink().getPoseHorizon();
		int horizon = 100;
		if (poseHorizon != null)
			horizon = Math.max(0, poseHorizon.getLeftY());
		for (int i = 0; i < w; i++) {
			for (int j = horizon; j < h; j++) {
				int[] current = rawImage.getPixel(i,j);
				current[0] = current[0] >> Y_SHIFT;
				current[1] = current[1] >> CB_SHIFT;
				current[2] = current[2] >> CR_SHIFT;
				if (org == MISSED) {
					//stats[current[0]][current[1]][current[2]][ORANGE]++;
				} else if (org == FALSEPOS) {
					stats[current[0]][current[1]][current[2]][ORANGE]--;
				}
				if (yell == MISSED) {
					//stats[current[0]][current[1]][current[2]][YELLOW]++;
				} else if (yell == FALSEPOS) {
					stats[current[0]][current[1]][current[2]][YELLOW]--;
				}
				if (blew == MISSED) {
					//stats[current[0]][current[1]][current[2]][BLUE]++;
				} else if (blew == FALSEPOS) {
					stats[current[0]][current[1]][current[2]][BLUE]--;
				}
			}
		}
	}

	public int learnGreenWhite() {
        int Y_SHIFT =colorTable.getYShift();
        int CB_SHIFT =colorTable.getCBShift();
        int CR_SHIFT =colorTable.getCRShift();

		int h = rawImage.getHeight();
		int w = rawImage.getWidth();
		int yes = 0;
		int start = 0;
		boolean sawStuff = false;
		byte colors = Vision.BLACK, previousColor = Vision.BLACK;
		Horizon poseHorizon = thresholdedImage.getVisionLink().getPoseHorizon();
		int horizon = 100;
		if (poseHorizon != null) {
			horizon = Math.max(0, poseHorizon.getLeftY());
		}
		for (int j = 0; j < w; j++) {
			for (int i = h - 5; i > horizon + 20; i--) {
				if (i == h - 50) {
					if (start == 0) {
						start++;
						previousColor = Vision.YELLOW;
					}
				}
				if (Math.abs(rawImage.getComponent(j, i, 0) - rawImage.getComponent(j, i - 1, 0)) > 30) {
					if (yes > 5) {
						//if (start == 0) {
						//	colors = Vision.BLACK;
						//} else
						// transition from darker to lighter suggests possibility of green to white
						if (rawImage.getComponent(j, i, 0) > rawImage.getComponent(j, i - 1, 0)) {
							colors = Vision.YELLOW;
							if (previousColor == Vision.ORANGE && start > 0) {
								for (int k = 1; k < yes; k++) {
									int[] current = rawImage.getPixel(j, i + k);
									current[0] = current[0] >> Y_SHIFT;
									current[1] = current[1] >> CB_SHIFT;
									current[2] = current[2] >> CR_SHIFT;
									stats[current[0]][current[1]][current[2]][GREEN]--;
									stats[current[0]][current[1]][current[2]][WHITE]++;
								}
								sawStuff = true;
							}
						} else {
							// probably white to green
							colors = Vision.ORANGE;
							if (previousColor == Vision.YELLOW && start > 0) {
								for (int k = 1; k < yes; k++) {
									int[] current = rawImage.getPixel(j, i +k);
									current[0] = current[0] >> Y_SHIFT;
									current[1] = current[1] >> CB_SHIFT;
									current[2] = current[2] >> CR_SHIFT;
									stats[current[0]][current[1]][current[2]][GREEN]++;
									stats[current[0]][current[1]][current[2]][WHITE]--;
								}
								sawStuff = true;
							}
						}
						//thresholdedOverlay.drawCross(j, i, (byte)3, (byte)1, colors);
						previousColor = colors;
					} else {
						previousColor = Vision.BLACK;
					}
					yes = 0;
					start++;
				} else {
					yes++;
				}
			}
			yes = 0;
			start = 0;
			previousColor = Vision.BLACK;
		}
		// we don't like green over the horizon
		for (int i = 0; i < horizon - 10; i++) {
			for (int j = 0; j < w; j+= 5) {
					int[] current = rawImage.getPixel(j,i);
					current[0] = current[0] >> Y_SHIFT;
					current[1] = current[1] >> CB_SHIFT;
					current[2] = current[2] >> CR_SHIFT;
					stats[current[0]][current[1]][current[2]][GREEN]--;
			}
		}
		if (sawStuff) return 1;
		return 0;
	}

	/* Pretty much the same as the previous method, except this time we've already updated
	   the green and now we're filling in holes as it were.
	 */
	public int moreLearnGreenWhite(int yMin, int yMax, int uMin, int uMax, int vMin,
									int vMax) {
		// remember to re-initialize the stats
        int Y_SHIFT =colorTable.getYShift();
        int CB_SHIFT =colorTable.getCBShift();
        int CR_SHIFT =colorTable.getCRShift();

		int h = rawImage.getHeight();
		int w = rawImage.getWidth();
		int yes = 0;
		int start = 0;
		byte colors = Vision.BLACK, previousColor = Vision.BLACK;
		byte cur = Vision.GREEN, previous = Vision.GREEN;
		int evidence = 0;
		int horizon = thresholdedImage.getVisionLink().getVisionHorizon();
		int updated = 0;
		int run, badones;
		int startrun = 0;
		int lastone = 0;
		int total = 0;
		for (int j = 0; j < w; j++) {
			previous = Vision.GREEN;
			run = 0;
			total = 0;
			badones = 0;
			for (int i = h - 5; i > horizon + 20; i--) {
				// get the color of the new pixel
				int[] current = rawImage.getPixel(j, i);
				current[0] = current[0] >> Y_SHIFT;
				current[1] = current[1] >> CB_SHIFT;
				current[2] = current[2] >> CR_SHIFT;
				cur = colorTable.getRawColor(current);
				if (cur == previous) {
					if (run == 0)
						startrun = i;
					run++;
					total++;
					lastone = i;
				} else {
					if (cur == Vision.GREY) {
						badones++;
						total++;
						if (badones > 4) {
							run = 0;
							total = 0;
						}
					} else {
						if (previous == Vision.GREEN && run > 8) {
							// we had a run of GREEN
							for (int k = lastone; k < startrun; k++) {
								current = rawImage.getPixel(j, k);
								current[0] = current[0] >> Y_SHIFT;
								current[1] = current[1] >> CB_SHIFT;
								current[2] = current[2] >> CR_SHIFT;
								if (current[0] > yMin - 10 && current[0] < yMin + 8) {
									stats[current[0]][current[1]][current[2]][GREEN]++;
									stats[current[0]][current[1]][current[2]][WHITE]--;
								}
							}
						} else if (previous == Vision.WHITE && run > 8) {
							for (int k = lastone; k < startrun; k++) {
								current = rawImage.getPixel(j, k);
								current[0] = current[0] >> Y_SHIFT;
								current[1] = current[1] >> CB_SHIFT;
								current[2] = current[2] >> CR_SHIFT;
								stats[current[0]][current[1]][current[2]][GREEN]--;
								stats[current[0]][current[1]][current[2]][WHITE]++;
							}
						} else {
							run = 0;
							startrun = i;
						}
					}
				}
			}
		}
		// we don't like green over the horizon
		for (int i = 0; i < horizon - 10; i++) {
			for (int j = 0; j < w; j+= 5) {
					int[] current = rawImage.getPixel(j,i);
					current[0] = current[0] >> Y_SHIFT;
					current[1] = current[1] >> CB_SHIFT;
					current[2] = current[2] >> CR_SHIFT;
					stats[current[0]][current[1]][current[2]][GREEN]--;
			}
		}
		return updated;
	}

	public boolean between(int x, int y, int z) {
		return x <= z && y >= z;
	}

	public void updateGreenWhite(int frames) {
		int pixie[] = new int[3];
		byte cur = Vision.GREY;
		System.out.println("Doing green and white with "+frames);
		int count = 0;
		int changed = 0;
		int start = 0;
		for (int i = 0; i < tableSize; i++) {
			for (int j = 0; j < tableSize; j++) {
				for (int k = 0; k < tableSize; k++) {
					//if (!revision) {
						pixie[0] = i; pixie[1] = j; pixie[2] = k;
						for (int l = start; l < 2; l++) {
							int need = 5;
							if (l == GREEN) {
								if (frames > 0) {
									need = frames / 20;
									if (stats[i][j][k][l] > 5 && stats[i][j][k][l] <= need)
										count++;
								} else {
									need = 200;
								}
							}
							if (stats[i][j][k][l] > need && colorTable.getRawColor(pixie) == Vision.GREY) {
								byte col = Vision.GREY;
								switch (l) {
								case WHITE:
									col = Vision.WHITE; break;
								case GREEN:
									changed++;
									col = Vision.GREEN; break;
								}
								colorTable.setRawColor(pixie, col);
								l = 12;
							}
						}
				}
			}
		}
		System.out.println("Ignored "+count+" potential green pixels while changing "+changed);
		// now let's improve the colors a bit
		colorTable.loseIslands(Vision.WHITE);
		colorTable.loseIslands(Vision.GREEN);
		byte col = Vision.GREEN;
		for (int i = 0; i < 2; i++) {
			switch (i) {
			case WHITE:
				col = Vision.WHITE; break;
			case GREEN:
				col = Vision.GREEN; break;
			}
			ColorTableUpdate up;
			do {
				up = colorTable.fillHoles(col);
			} while (up.getSize() > 0);
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
		int yes = 0;
		int start = 0;
		byte colors = Vision.BLACK, previousColor = Vision.BLACK;
		Horizon poseHorizon = thresholdedImage.getVisionLink().getPoseHorizon();
		int horizon = 100;
		if (poseHorizon != null) {
			horizon = Math.max(0, poseHorizon.getLeftY());
		}
		int max[]  = new int[w];
		for (int i = 0; i < w; i++) {
			max[i] = h;
		}
		// determine the top green edge for the field
		for (int i = 0; i < w; i++) {
			for (int j = h - 1; j > horizon; j--) {
				int[] pix = rawImage.getPixel(i,j);
				pix[0] = pix[0] >> Y_SHIFT;
				pix[1] = pix[1] >> CB_SHIFT;
				pix[2] = pix[2] >> CR_SHIFT;
				byte col = colorTable.getRawColor(pix);
				if (col == Vision.GREEN) {
					max[i] = j;
				}
			}
		}

		for (int i = 0; i < w; i++) {
			for (int j = horizon; j < h; j++) {
				int[] current = rawImage.getPixel(i,j);
				current[0] = current[0] >> Y_SHIFT;
				current[1] = current[1] >> CB_SHIFT;
				current[2] = current[2] >> CR_SHIFT;
				if (orange) {
					stats[current[0]][current[1]][current[2]][ORANGE]++;
				} else {
					stats[current[0]][current[1]][current[2]][ORANGE]-=2;
				}
				if (yellow && j < max[i]) {
					stats[current[0]][current[1]][current[2]][YELLOW]++;
				} else  {
					stats[current[0]][current[1]][current[2]][YELLOW]--;
				}
				if (blue) {
					stats[current[0]][current[1]][current[2]][BLUE]++;
				} else {
					stats[current[0]][current[1]][current[2]][BLUE]--;
				}
				if (red) {
					stats[current[0]][current[1]][current[2]][RED]++;
				} else {
					stats[current[0]][current[1]][current[2]][RED]--;
				}
				if (navy) {
					stats[current[0]][current[1]][current[2]][NAVY]++;
				} else {
					stats[current[0]][current[1]][current[2]][NAVY]--;
				}
			}
		}
	}

	public void printStats(int frames, int balls, int yposts, int bposts, int crosses,
						   int rrobots, int brobots, boolean revision) {
		int pixie[] = new int[3];
		int avg[][] = new int[3][7];
		int tots[] = new int[7];
		byte cur = Vision.GREY;
		int need = 0;
		for (int i = 0; i < 7; i++) {
			avg[0][i] = 0;
			avg[1][i] = 0;
			avg[2][i] = 0;
			tots[0] = 0;
		}
		for (int i = 0; i < tableSize; i++) {
			for (int j = 0; j < tableSize; j++) {
				for (int k = 0; k < tableSize; k++) {
					//if (!revision) {
						pixie[0] = i; pixie[1] = j; pixie[2] = k;
						//colorTable.setRawColor(pixie, Vision.GREY);
						if (colorTable.getRawColor(pixie) == Vision.GREY) {
							for (int l = 2; l < 7; l++) {
								switch (l) {
									case ORANGE:
										need = balls / 20; break;
									case YELLOW:
										need = yposts / 20; break;
									case BLUE:
										need = bposts / 20; break;
									case NAVY:
										need = brobots / 10; break;
									case RED:
										need = rrobots / 10; break;
								}
								if (stats[i][j][k][l] > need) {
									avg[0][l] += i; avg[1][l] += j; avg[2][l] += k;
									tots[l]++;
									byte col = Vision.GREY;
									switch (l) {
									case ORANGE:
										col = Vision.ORANGE; break;
									case YELLOW:
										col = Vision.YELLOW; break;
									case BLUE:
										col = Vision.BLUE; break;
									case NAVY:
										col = Vision.NAVY; break;
									case RED:
										col = Vision.RED; break;
									}
									colorTable.setRawColor(pixie, col);
									l = 12;
								}
							}
						}
				}
			}
		}
		// now let's improve the colors a bit
		colorTable.loseIslands(Vision.ORANGE);
		colorTable.loseIslands(Vision.BLUE);
		colorTable.loseIslands(Vision.YELLOW);
		byte col = Vision.GREEN;
		for (int i = 2; i < 7; i++) {
			switch (i) {
			case ORANGE:
				col = Vision.ORANGE; break;
			case YELLOW:
				col = Vision.YELLOW; break;
			case BLUE:
				col = Vision.BLUE; break;
			case NAVY:
				col = Vision.NAVY; break;
			case RED:
				col = Vision.RED; break;
			}
			ColorTableUpdate up;
			do {
				up = colorTable.fillHoles(col);
			} while (up.getSize() > 0);
		}
		/*for (int l = 0; l < 7; l++) {
			avg[0][l] = avg[0][l] / tots[l];
			avg[1][l] = avg[1][l] / tots[l];
			avg[2][l] = avg[2][l] / tots[l];
			pixie[0] = avg[0][l];
			pixie[1] = avg[1][l];
			pixie[2] = avg[2][l];
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
			System.out.println("Color "+l+" "+pixie[0]+" "+pixie[1]+" "+pixie[2]);
			colorTable.setRawColor(pixie, col);
			}*/

	}

    public void newFrame(Frame f, ColorTable c) {
		frame = f;
		rawImage = f.image();
        colorTable = c;
        //init the objects
        if (rawImage != null && colorTable != null)  {
            thresholdedImage = new ProcessedImage(frame, colorTable);
			if (thresholdedOverlay == null)
				thresholdedOverlay = new ThresholdedImageOverlay(thresholdedImage.getWidth(),
																 thresholdedImage.getHeight());
			else
				thresholdedOverlay.resetPixels();
		} else {
			System.out.println("Frame didn't load image "+f.index());
		}
    }

    //This updates the whole processed stuff
    //- the thresholded image, the field objects and the ball
    public void update(boolean silent, Frame frame) {
	//if the thresholdedImage is not null, process it again
        if (thresholdedImage != null)  {
            //we process the image; the visionLink updates itself with the new data from the bot
            thresholdedImage.thresholdImage(frame, colorTable);
            //get the ball from the link
            ball = thresholdedImage.getVisionLink().getBall();
            visualFieldObjects = thresholdedImage.getVisionLink().getVisualFieldObjects();
            visualCorners = thresholdedImage.getVisionLink().getVisualCorners();
            //draw the stuff onto the overlay
			if (!silent)
				drawObjectBoxes();
        }
		//else the thresholdedImage is null, so initialize it
		else {
			thresholdedImage = new ProcessedImage(frame, colorTable);
			update(silent, frame);
		}
    }

	public void updateObjects() {
		if (ball == null) {
			seeBall = false;
			System.out.println("Ball is null");
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
        //set corners
        VisualCorner corner;
		int other = 0;
		seeLCorners = 0;
		seeTCorners = 0;
		seeCcCorners = 0;
        for (int i = 0; i < visualCorners.size(); i++) {
            corner = visualCorners.elementAt(i);
			VisualCorner.shape corn = corner.getShape();
			if (corn == VisualCorner.shape.INNER_L || corn == VisualCorner.shape.OUTER_L) {
				seeLCorners++;
			} else if (corn == VisualCorner.shape.T) {
				seeTCorners++;
			} else if (corn == VisualCorner.shape.CIRCLE ){
				seeCcCorners++;
			} else {
				// current covers UNKNOWN and CIRCLE it is plausible that we'll want to process these
				other++;
			}
        }
	}

    //drawObjectBoxes - draws the object onto the overlay
    public void drawObjectBoxes(){
        thresholdedOverlay.resetPixels();//reset the overlay
		int h = rawImage.getHeight();
		int w = rawImage.getWidth();
		int yes = 0;
		int start = 0;
		byte colors = Vision.BLACK, previousColor = Vision.BLACK;
		Horizon poseHorizon = thresholdedImage.getVisionLink().getPoseHorizon();
		int horizon = Math.max(0, poseHorizon.getLeftY());
		for (int j = 0; j < w; j = j + 10) {
			for (int i = h - 5; i > horizon + 20; i--) {
				if (i == h - 50) {
					if (start == 0) {
						start++;
						previousColor = Vision.YELLOW;
					}
				}
				if (Math.abs(rawImage.getComponent(j, i, 0) - rawImage.getComponent(j, i - 1, 0)) > 30) {
					if (yes > 5) {
						//if (start == 0) {
						//	colors = Vision.BLACK;
						//} else
						// transition from darker to lighter suggests possibility of green to white
						if (rawImage.getComponent(j, i, 0) > rawImage.getComponent(j, i - 1, 0)) {
							colors = Vision.YELLOW;
							if (previousColor == Vision.ORANGE && start > 0) {
								thresholdedOverlay.drawLine(j, i, j, i + yes, (byte)1, Vision.WHITE);
							}
						} else {
							// probably white to green
							colors = Vision.ORANGE;
							if (previousColor == Vision.YELLOW && start > 0) {
								thresholdedOverlay.drawLine(j, i, j, i + yes, (byte)1, Vision.GREEN);
							}
						}
						thresholdedOverlay.drawCross(j, i, (byte)3, (byte)1, colors);
						previousColor = colors;
					} else {
						previousColor = Vision.BLACK;
					}
					yes = 0;
					start++;
				} else {
					yes++;
				}
			}
			yes = 0;
			start = 0;
			previousColor = Vision.BLACK;
		}
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
		frame = f;
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
	public int getLCornersVision() {return seeLCorners;}
	public int getTCornersVision() {return seeTCorners;}
	public int getCcCornersVision() {return seeCcCorners;}

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

	public String getLCornerString() {
		return ""+seeLCorners;
	}
	public String getTCornerString() {
		return ""+seeTCorners;
	}
	public String getCcCornerString() {
		return ""+seeCcCorners;
	}

    //setters
    public void setImage(TOOLImage i) { rawImage = i; }
    public void setThreshImage(ProcessedImage i) { thresholdedImage = i;  }
    public void setColorTable(ColorTable c) { colorTable = c; }
    public void setBall(Ball b) { ball = b;  }


	/**
	 * Constructs the window with checkboxes to toggle on and off vision debugging flags.
	 */
	private void createDebugWindow()
	{
		JFrame debugWindow = new JFrame();
		JPanel buttonPanel = new JPanel();

		final JCheckBox fieldLinesDebugVertEdgeDetectBox = new JCheckBox(" Debug VertEdgeDetect");
		fieldLinesDebugVertEdgeDetectBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugVertEdgeDetect(fieldLinesDebugVertEdgeDetectBox.isSelected());
				}
			});

		final JCheckBox fieldLinesDebugHorEdgeDetectBox = new JCheckBox(" Debug Hor Edge DetectBox");
		fieldLinesDebugHorEdgeDetectBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugHorEdgeDetect(fieldLinesDebugHorEdgeDetectBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugSecondVertEdgeDetectBox = new JCheckBox(" Debug Second Vert EdgeDetectBox");
		fieldLinesDebugSecondVertEdgeDetectBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugSecondVertEdgeDetect(fieldLinesDebugSecondVertEdgeDetectBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugCreateLinesBox = new JCheckBox(" Debug Create Lines Box");
		fieldLinesDebugCreateLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugCreateLines(fieldLinesDebugCreateLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugFitUnusedPointsBox = new JCheckBox(" Debug Fit Unused PointsBox");
		fieldLinesDebugFitUnusedPointsBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugFitUnusedPoints(fieldLinesDebugFitUnusedPointsBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugJoinLinesBox = new JCheckBox(" Debug Join Lines Box");
		fieldLinesDebugJoinLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugJoinLines(fieldLinesDebugJoinLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugExtendLinesBox = new JCheckBox(" Debug Extend Lines Box");
		fieldLinesDebugExtendLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugExtendLines(fieldLinesDebugExtendLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugIntersectLinesBox = new JCheckBox(" Debug Intersect Lines Box");
		fieldLinesDebugIntersectLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugIntersectLines(fieldLinesDebugIntersectLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugIdentifyCornersBox = new JCheckBox(" Debug Identify Corners Box");
		fieldLinesDebugIdentifyCornersBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugIdentifyCorners(fieldLinesDebugIdentifyCornersBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugCcScanBox = new JCheckBox(" Debug Cc Scan Box");
		fieldLinesDebugCcScanBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugCcScan(fieldLinesDebugCcScanBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugRiskyCornersBox = new JCheckBox(" Debug Risky Corners Box");
		fieldLinesDebugRiskyCornersBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugRiskyCorners(fieldLinesDebugRiskyCornersBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugCornerAndObjectDistancesBox = new JCheckBox(" Debug Corner And ObjectDistancesBox");
		fieldLinesDebugCornerAndObjectDistancesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugCornerAndObjectDistances(fieldLinesDebugCornerAndObjectDistancesBox.isSelected());}
			});

		buttonPanel.add(new JLabel("\tField Line Flags"));
		buttonPanel.add(fieldLinesDebugVertEdgeDetectBox);
		buttonPanel.add(fieldLinesDebugHorEdgeDetectBox);
		buttonPanel.add(fieldLinesDebugCornerAndObjectDistancesBox);
		buttonPanel.add(fieldLinesDebugSecondVertEdgeDetectBox);
		buttonPanel.add(fieldLinesDebugCreateLinesBox);
		buttonPanel.add(fieldLinesDebugFitUnusedPointsBox);
		buttonPanel.add(fieldLinesDebugJoinLinesBox);
		buttonPanel.add(fieldLinesDebugExtendLinesBox);
		buttonPanel.add(fieldLinesDebugIntersectLinesBox);
		buttonPanel.add(fieldLinesDebugIdentifyCornersBox);
		buttonPanel.add(fieldLinesDebugCcScanBox);
		buttonPanel.add(fieldLinesDebugRiskyCornersBox);

		buttonPanel.setLayout(new GridLayout(13,1));

		debugWindow.add(buttonPanel);
		debugWindow.setSize(400,400);
		debugWindow.setVisible(true);
	}
}
