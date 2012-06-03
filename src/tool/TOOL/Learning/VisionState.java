package TOOL.Learning;

import java.util.Vector;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import java.awt.GridLayout;
import javax.swing.BoxLayout;

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

    //gets the image from the data frame, inits colortable
    public VisionState(Frame f, ColorTable c) {
		frame = f;
        rawImage = f.image();
        colorTable = c;
        //init the objects
        if (rawImage != null && colorTable != null)  {
            thresholdedImage = new ProcessedImage(frame, colorTable);
            thresholdedOverlay =
                new ThresholdedImageOverlay(thresholdedImage.getWidth(),
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
	}

	public void reviseStats(boolean orange, boolean yellow, boolean blue, boolean white,
							boolean red, boolean navy) {
	}

	public int learnGreenWhite() {
		return 0;
	}

	/* Pretty much the same as the previous method, except this time we've already updated
	   the green and now we're filling in holes as it were.
	 */
	public int moreLearnGreenWhite(int yMin, int yMax, int uMin, int uMax, int vMin,
									int vMax) {
		return 0;
	}

	public boolean between(int x, int y, int z) {
		return x <= z && y >= z;
	}

	public void updateGreenWhite(int frames) {
	}

	/** Go through the current
	 */
	public void updateStats(boolean orange, boolean yellow, boolean blue, boolean white,
							boolean red, boolean navy) {
	}

	public void printStats(int frames, int balls, int yposts, int bposts, int crosses,
						   int rrobots, int brobots, boolean revision) {
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
		JPanel ballPanel = new JPanel();

		final JCheckBox fieldLinesDebugVertEdgeDetectBox = new JCheckBox(" Debug VertEdgeDetect");
		fieldLinesDebugVertEdgeDetectBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugVertEdgeDetect(fieldLinesDebugVertEdgeDetectBox.isSelected());
				}
			});

		final JCheckBox fieldLinesDebugHorEdgeDetectBox = new JCheckBox(" Debug Hor Edge Detect");
		fieldLinesDebugHorEdgeDetectBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugHorEdgeDetect(fieldLinesDebugHorEdgeDetectBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugSecondVertEdgeDetectBox = new JCheckBox(" Debug Second Vert EdgeDetect");
		fieldLinesDebugSecondVertEdgeDetectBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugSecondVertEdgeDetect(fieldLinesDebugSecondVertEdgeDetectBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugCreateLinesBox = new JCheckBox(" Debug Create Lines ");
		fieldLinesDebugCreateLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugCreateLines(fieldLinesDebugCreateLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugFitUnusedPointsBox = new JCheckBox(" Debug Fit Unused Points");
		fieldLinesDebugFitUnusedPointsBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugFitUnusedPoints(fieldLinesDebugFitUnusedPointsBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugJoinLinesBox = new JCheckBox(" Debug Join Lines ");
		fieldLinesDebugJoinLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugJoinLines(fieldLinesDebugJoinLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugExtendLinesBox = new JCheckBox(" Debug Extend Lines ");
		fieldLinesDebugExtendLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugExtendLines(fieldLinesDebugExtendLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugIntersectLinesBox = new JCheckBox(" Debug Intersect Lines ");
		fieldLinesDebugIntersectLinesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugIntersectLines(fieldLinesDebugIntersectLinesBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugIdentifyCornersBox = new JCheckBox(" Debug Identify Corners ");
		fieldLinesDebugIdentifyCornersBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugIdentifyCorners(fieldLinesDebugIdentifyCornersBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugCcScanBox = new JCheckBox(" Debug Cc Scan ");
		fieldLinesDebugCcScanBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugCcScan(fieldLinesDebugCcScanBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugRiskyCornersBox = new JCheckBox(" Debug Risky Corners ");
		fieldLinesDebugRiskyCornersBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugRiskyCorners(fieldLinesDebugRiskyCornersBox.isSelected());}
			});
		final JCheckBox fieldLinesDebugCornerAndObjectDistancesBox = new JCheckBox(" Debug Corner And ObjectDistances");
		fieldLinesDebugCornerAndObjectDistancesBox.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setFieldLinesDebugCornerAndObjectDistances(fieldLinesDebugCornerAndObjectDistancesBox.isSelected());}
			});
		final JCheckBox ballDebugBallBox = new JCheckBox(" Debug Balls");
		ballDebugBallBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugBall(ballDebugBallBox.isSelected());}
			});
		final JCheckBox ballDebugBallDistanceBox = new JCheckBox(" Debug Ball Distance");
		ballDebugBallDistanceBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugBallDistance(ballDebugBallDistanceBox.isSelected());}
			});
		final JCheckBox crossDebugBox = new JCheckBox(" Debug Cross");
		crossDebugBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugCross(crossDebugBox.isSelected());}
			});
		final JCheckBox postPrintBox = new JCheckBox(" Debug Posts Print");
		postPrintBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugPostPrint(postPrintBox.isSelected());}
			});
		final JCheckBox postDebugBox = new JCheckBox(" Debug Posts");
		postDebugBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugPost(postDebugBox.isSelected());}
			});
		final JCheckBox postLogicBox = new JCheckBox(" Debug Posts Logic");
		postLogicBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugPostLogic(postLogicBox.isSelected());}
			});
		final JCheckBox postSanityBox = new JCheckBox(" Debug Posts Sanity");
		postSanityBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugPostSanity(postSanityBox.isSelected());}
			});
		final JCheckBox postCorrectBox = new JCheckBox(" Debug Posts Correct");
		postCorrectBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugPostCorrect(postCorrectBox.isSelected());}
			});
		final JCheckBox fieldHorizonBox = new JCheckBox(" Debug Field Horizon");
		fieldHorizonBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugFieldHorizon(fieldHorizonBox.isSelected());}
			});
		final JCheckBox fieldEdgeBox = new JCheckBox(" Debug Field Edge");
		fieldEdgeBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugFieldEdge(fieldEdgeBox.isSelected());}
			});
		final JCheckBox openFieldBox = new JCheckBox(" Debug Open Field");
		openFieldBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugOpenField(openFieldBox.isSelected());}
			});
		final JCheckBox shootingBox = new JCheckBox(" Debug Shooting");
		shootingBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugShooting(shootingBox.isSelected());}
			});
		final JCheckBox edgeDetectionBox = new
            JCheckBox(" Debug Edge Detection");
		edgeDetectionBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
					thresholdedImage.getVisionLink().
						setDebugEdgeDetection(edgeDetectionBox.isSelected());}
			});

        final JCheckBox houghTransformBox = new
            JCheckBox(" Debug Hough Transform");
        houghTransformBox.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    thresholdedImage.getVisionLink().
                        setDebugHoughTransform(houghTransformBox.isSelected());}
            });
        final JCheckBox robotBox = new
            JCheckBox(" Debug Robots");
        robotBox.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    thresholdedImage.getVisionLink().
                        setDebugRobot(robotBox.isSelected());}
            });
        final JCheckBox dangerBox = new
            JCheckBox(" Debug Dangerous Ball");
        dangerBox.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    thresholdedImage.getVisionLink().
                        setDebugDangerousBall(dangerBox.isSelected());}
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
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.Y_AXIS));
		ballPanel.add(new JLabel("\tVision Flags"));
		ballPanel.add(ballDebugBallBox);
		ballPanel.add(ballDebugBallDistanceBox);
		ballPanel.add(crossDebugBox);
		ballPanel.add(postPrintBox);
		ballPanel.add(postDebugBox);
		ballPanel.add(postLogicBox);
		ballPanel.add(postSanityBox);
		ballPanel.add(postCorrectBox);
		ballPanel.add(fieldHorizonBox);
		ballPanel.add(fieldEdgeBox);
        ballPanel.add(openFieldBox);
        ballPanel.add(shootingBox);
        ballPanel.add(edgeDetectionBox);
        ballPanel.add(houghTransformBox);
        ballPanel.add(robotBox);
		ballPanel.add(dangerBox);
		ballPanel.setLayout(new BoxLayout(ballPanel, BoxLayout.Y_AXIS));

		JPanel debugPanel = new JPanel();
		debugPanel.setLayout(new GridLayout(1,1));
		debugPanel.add(buttonPanel);
		debugPanel.add(ballPanel);
		debugWindow.add(debugPanel);
		debugWindow.setSize(500, 500);
		debugWindow.setVisible(true);

        // Disable all the debugging information by default
        thresholdedImage.getVisionLink().setFieldLinesDebugVertEdgeDetect(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugHorEdgeDetect(false);

        thresholdedImage.getVisionLink().
            setFieldLinesDebugSecondVertEdgeDetect(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugCreateLines(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugFitUnusedPoints(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugJoinLines(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugExtendLines(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugIntersectLines(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugIdentifyCorners(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugCcScan(false);

        thresholdedImage.getVisionLink().setFieldLinesDebugRiskyCorners(false);

        thresholdedImage.getVisionLink().
            setFieldLinesDebugCornerAndObjectDistances(false);

		thresholdedImage.getVisionLink().setDebugBall(false);
		thresholdedImage.getVisionLink().setDebugBallDistance(false);
		thresholdedImage.getVisionLink().setDebugCross(false);
		//thresholdedImage.getVisionLink().setDebugPostPrint(false);
		//thresholdedImage.getVisionLink().setDebugPost(false);
		/*System.out.println("more3");
		thresholdedImage.getVisionLink().setDebugPostLogic(false);
		thresholdedImage.getVisionLink().setDebugPostSanity(false);
		System.out.println("more4");
		thresholdedImage.getVisionLink().setDebugPostCorrect(false);
		thresholdedImage.getVisionLink().setDebugFieldHorizon(false);
		System.out.println("more5");
		thresholdedImage.getVisionLink().setDebugFieldEdge(false);
		thresholdedImage.getVisionLink().setDebugEdgeDetection(false);
		System.out.println("more6");
		thresholdedImage.getVisionLink().setDebugHoughTransform(false);
		thresholdedImage.getVisionLink().setDebugRobot(false);
		System.out.println("more7");
		thresholdedImage.getVisionLink().setDebugDangerousBall(false);
		*/
		System.out.println("going out");
	}
}
