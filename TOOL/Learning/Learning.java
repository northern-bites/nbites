package TOOL.Learning;

import java.lang.Math;

import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.GridLayout;
import java.awt.Color;
import java.awt.image.*;
import java.awt.Cursor;
import java.awt.geom.*;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GradientPaint;
import java.awt.AlphaComposite;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelListener;
import java.awt.event.MouseWheelEvent;
import java.awt.Toolkit;
import java.awt.Point;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;


import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;


import TOOL.Data.DataListener;
import TOOL.Data.DataSet;
import TOOL.Data.Frame;
import TOOL.Data.ColorTableListener;
//import TOOL.Misc.Pair;
import TOOL.Misc.Estimate;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.Vector;

import javax.swing.JPanel;
import javax.swing.JSplitPane;

// Image stuff
import TOOL.Image.ColorTable;
import TOOL.Image.ImageSwatch;
import TOOL.Image.ImagePanel;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImage;
import TOOL.Image.ProcessedImage;
import TOOL.Image.PixelSelectionPanel;
import TOOL.Image.ImageMarkerPanel;
import TOOL.Image.CalibrationDrawingPanel;
import TOOL.Image.DrawingPanel;


import TOOL.Image.ImageOverlay;
import TOOL.Image.ImageOverlayAction;

import TOOL.Vision.Vision;

import TOOL.Data.Classification.Keys.Builder;
import TOOL.Data.Classification.KeyFrame.GoalType;
import TOOL.Data.Classification.KeyFrame.CrossType;
import TOOL.Data.Classification.Keys;
import TOOL.Data.Classification.KeyFrame;

import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import TOOL.GUI.IncrementalSliderParent;


import TOOL.TOOL;



/**
 * This is the learning class. It is responsible for managing and running
 * our classification system so we can machine learn vision.
 * @author modified Eric Chown, plus whoever wrote all the code I borrow from
 * other parts of the tool
 */

public class Learning implements DataListener, MouseListener,
                                  MouseMotionListener,
                                  PropertyChangeListener
{
	public static final int RIGHTPOST = 0;
	public static final int LEFTPOST = 1;
	public static final int UNKNOWNPOST = 2;
	public static final int BOTHPOSTS = 3;
	public static final int NOPOST = 4;

	public static final int YELLOWCROSS = 0;
	public static final int BLUECROSS = 1;
	public static final int BOTHCROSSES = 2;
	public static final int UNKNOWNCROSS = 3;
	public static final int NOCROSS = 4;


    protected PixelSelectionPanel selector;
	protected KeyPanel key;
	protected LearningPanel learnPanel;
    protected ImageOverlay overlay;
    protected TOOLImage rawImage;
    protected int imageID;
    protected ProcessedImage thresholdedImage;
    protected TOOL tool;
    protected ColorTable colorTable;
    protected VisionState visionState;

    protected JPanel main_panel;
    protected int imageHeight, imageWidth;

    private Frame currentFrame;
	private KeyFrame current;

	private int ind;

	protected Builder keys;
	protected KeyFrame.Builder newKey;

    private JSplitPane split_pane;
    private boolean split_changing;

    private Point start, end;

	private String keyName;

    public Learning(TOOL t){
        tool = t;
        colorTable = tool.getColorTable();

        //get all the image panels ready
        selector = new PixelSelectionPanel();
        selector.changeSettings(ImagePanel.SCALE_AUTO_BOTH);

		key = new KeyPanel(this);
        setupWindowsAndListeners();

		ind = 0;

    }


    public ColorTable getTable() {
        return colorTable;
    }

    /**
     * Sets up all the windows and panels; installs listener onto them.
     */
    private void setupWindowsAndListeners(){
        //data listeners
        tool.getDataManager().addDataListener(this);

        // create the main panel
        main_panel = new JPanel();
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        main_panel.setLayout(gridbag);

        // create the panel that will hold the images
        JPanel images_panel = new JPanel();
        images_panel.setLayout(new GridLayout(1, 1));
        c.weightx = 1;
        c.weighty = 5;
        c.fill = GridBagConstraints.BOTH;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = GridBagConstraints.RELATIVE;
        gridbag.setConstraints(images_panel, c);
        main_panel.add(images_panel);

        // within that panel, we have a split view of the two images
        split_pane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
        split_pane.setLeftComponent(selector);
        split_pane.setRightComponent(key);
        split_pane.setDividerLocation(.8); // initial divider location
        split_pane.setResizeWeight(.8); // how to distribute new space
        split_changing = false;
        split_pane.addPropertyChangeListener(this);
        images_panel.add(split_pane);

        // set up mouse listeners
        selector.addMouseListener(this);
        key.addMouseListener(this);
        selector.addMouseMotionListener(this);
        key.addMouseMotionListener(this);

        learnPanel = new LearningPanel(this);
        main_panel.add(learnPanel);


        main_panel.setFocusable(true);
        main_panel.requestFocusInWindow();

		learnPanel.fixButtons();
    }

    /** @return JPanel holding all of Calibrate stuff */
    public JPanel getContentPane() {
        return main_panel;
    }


    public void getLastImage() {
        tool.getDataManager().last();
        // fix the backward skipping button
        learnPanel.fixButtons();
    }

    public void getNextImage() {
        tool.getDataManager().next();
        // fix the forward skipping button
        learnPanel.fixButtons();
    }

    public void skipForward(int i) {
        tool.getDataManager().skip(i);
        // fix the forward skipping button
        learnPanel.fixButtons();
    }

    public void skipBackward(int i) {
        tool.getDataManager().revert(i);
        // fix the backward skipping button
        learnPanel.fixButtons();
    }

    public void setImage(int i) {
        tool.getDataManager().set(i);
    }


    public boolean canGoForward() {
        return tool.getDataManager().hasElementAfter();
    }

    public boolean canGoBackward() {
        return tool.getDataManager().hasElementBefore();
    }

    public TOOL getTool() {
        return tool;
    }

    /** @return true if we have a thresholded image, else false. */
    public Boolean hasImage() {
        return thresholdedImage != null;
    }


    ////////////////////////////////////////////////////////////
    // LISTENER METHODS
    ////////////////////////////////////////////////////////////

    // MouseListener methods
    public void mouseClicked(MouseEvent e) {}

    // When mouse enters, make sure the cursor is the rectangular swatch
    public void mouseEntered(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {    }

    public void mousePressed(MouseEvent e) {
        start = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {

    }

    // MouseMotionListener methods
    public void mouseDragged(MouseEvent e) {
        end = e.getPoint();
        mouseMoved(e);
    }


    // PropertyChangeListener method
    public void propertyChange(PropertyChangeEvent e) {
        if (e.getPropertyName().equals(JSplitPane.DIVIDER_LOCATION_PROPERTY)
            && !split_changing) {
            split_changing = true;
            split_pane.setDividerLocation(.5);
            split_changing = false;
        }

        else if (e.getPropertyName().equals(ImagePanel.X_SCALE_CHANGE)) {
        }
    }

    public void mouseMoved(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();
    }

    //mouseWheelListener Methods
    public void mouseWheelMoved(MouseWheelEvent e){

    }

    //dataListener Methods
    public void notifyDataSet(DataSet s, Frame f) {
		boolean keyExists = true;
		keys = Keys.newBuilder();
		keyName = s.path()+"KEY.KEY";
		// See if the key exists.
		try {
			FileInputStream input = new FileInputStream(keyName);
			keys.mergeFrom(input);
			input.close();
		} catch (FileNotFoundException e) {
			keyExists = false;
			System.out.println(keyName + ": not found.  Creating a new file.");
		} catch (java.io.IOException e) {
			System.out.println("Problems with key file");
		}
		if (!keyExists) {
			for (int i = 0; i < s.size(); i++) {
				// make a new key for the file and add it
				KeyFrame next =
					KeyFrame.newBuilder()
					.setHumanChecked(false)
					.setBall(false)
					.setBlueGoal(GoalType.NO_POST)
					.setYellowGoal(GoalType.NO_POST)
					.setCross(CrossType.NO_CROSS)
					.setRedRobots(0)
					.setBlueRobots(0)
					.build();
				keys.addFrame(next);
			}
		}

        notifyFrame(f);
    }
    //to do: clean up this code - Octavian
    public void notifyFrame(Frame f) {
        currentFrame = f;
        if (!f.hasImage())
            return;
        //if visionState is null, initialize, else just load the frame
		if (visionState == null)
			visionState = new VisionState(f, tool.getColorTable());
		else
			visionState.newFrame(f, tool.getColorTable());

        thresholdedImage = visionState.getThreshImage();//sync the thresholded images
        rawImage = visionState.getImage();
        imageID = rawImage.hashCode();

        colorTable = visionState.getColorTable();
		/*
		  if (drawThreshColors) {
		  thresholdedImage.thresholdImage(colorTable, rawImage);
		  }
		*/

        // Since we now handle different sized frames, it's possible to
        // switch between modes, changing the image's size without updating
        // the overlay.  This will catch that
        if(overlay == null || overlay.getWidth() != rawImage.getWidth()) {
            overlay = new ImageOverlay(rawImage.getWidth(),rawImage.getHeight());
        }
        imageHeight = rawImage.getHeight();
        imageWidth = rawImage.getWidth();

        overlay.generateNewEdgeImage(rawImage);
        selector.updateImage(rawImage);
		visionState.update();
		visionState.updateObjects();

		// retrieve the frame information
		ind = f.index();
		current = keys.getFrame(ind);
		// setup the buttons on the key panel to reflect the contents of the file
		key.setHumanStatus(current.getHumanChecked());
		if (current.getHumanChecked()) {
			System.out.println("Getting old data");
			key.setBallStatus(current.getBall());
			key.setBlueGoalStatus(current.getBlueGoal());
			key.setYellowGoalStatus(current.getYellowGoal());
			System.out.println("Old Cross: "+current.getCross());
			key.setCrossStatus(current.getCross());
			key.setRedRobotStatus(current.getRedRobots());
			key.setBlueRobotStatus(current.getBlueRobots());
		} else {
			// set up based upon vision data
			key.setBallStatus(getBall());
			key.setBlueGoalStatus(getBlueGoal());
			key.setYellowGoalStatus(getYellowGoal());
			key.setCrossStatus(getCross());
			key.setRedRobotStatus(getRedRobots());
			key.setBlueRobotStatus(getBlueRobots());
		}
		// write out the vision data
		key.setBall(getBallString());
		key.setBlueGoal(getBlueGoalString());
		key.setYellowGoal(getYellowGoalString());
		key.setCross(getCrossString());
		key.setRedRobot(getRedRobotString());
		key.setBlueRobot(getBlueRobotString());
		learnPanel.setOverlays();
		// set up the builder in case we decide to edit
		newKey =
			KeyFrame.newBuilder()
			.setHumanChecked(current.getHumanChecked())
			.setBall(current.getBall())
			.setBlueGoal(current.getBlueGoal())
			.setYellowGoal(current.getYellowGoal())
			.setCross(current.getCross())
			.setRedRobots(current.getRedRobots())
			.setBlueRobots(current.getBlueRobots());

		selector.setOverlayImage(visionState.getThreshOverlay());
        selector.repaint();

        // They loaded something so make sure our buttons reflect the
        // active state; e.g. that our undo stack and redo stack are
        // empty.
        learnPanel.fixButtons();
        // 0 based indexing.
        learnPanel.setText("Image " + (f.index()) + " of " +
                               (f.dataSet().size() - 1) +
			       " -  processed in " + visionState.getProcessTime() +
			       " micro secs");
    }


    public ImageOverlay getEdgeOverlay() {
		return overlay;
    }
    public VisionState getVisionState() {
		return visionState;
    }

	public void setBall(boolean hasBall) {
		if (newKey != null) {
			newKey.setBall(hasBall);
		}
	}

	public void writeData() {
		// Write the new address book back to disk.
		try {
			FileOutputStream output = new FileOutputStream(keyName);
			keys.build().writeTo(output);
			output.close();
			newKey = null;
		} catch (java.io.IOException e) {
			System.out.println("Problems with key file");
		}
	}

	public void setHuman(boolean hasHuman) {
		if (newKey != null) {
			newKey.setHumanChecked(hasHuman);
			keys.setFrame(ind , newKey);
			newKey = null;
		}
	}

	public void setCross(CrossType which) {
		System.out.println("Setting Cross "+which);
		if (newKey != null)
			newKey.setCross(which);
	}

	public void setYellowGoal(GoalType which) {
		if (newKey != null)
			newKey.setYellowGoal(which);
	}

	public void setBlueGoal(GoalType which) {
		if (newKey != null)
			newKey.setBlueGoal(which);
	}

	public void setRedRobot(int howMany) {
		if (newKey != null)
			newKey.setRedRobots(howMany);
	}

	public void setBlueRobot(int howMany) {
		if (newKey != null)
			newKey.setBlueRobots(howMany);
	}

	public boolean getBall() {
		if (visionState == null) return false;
		return visionState.getBallVision();
	}

	public GoalType getBlueGoal() {
		if (visionState == null) return GoalType.NO_POST;
		return visionState.getBlueGoalVision();
	}

	public GoalType getYellowGoal() {
		if (visionState == null) return GoalType.NO_POST;
		return visionState.getYellowGoalVision();
	}

	public CrossType getCross() {
		if (visionState == null) return CrossType.NO_CROSS;
		return visionState.getCrossVision();
	}

	public int getRedRobots() {
		if (visionState == null) return 0;
		return visionState.getRedRobotsVision();
	}

	public int getBlueRobots() {
		if (visionState == null) return 0;
		return visionState.getBlueRobotsVision();
	}

	public String getHuman() {
		//return visionState.getHumanString();
		return "No";
	}

	public String getBallString() {
		if (visionState == null) return "No Frame Loaded";
		return visionState.getBallString();
	}

	public String getCrossString() {
		if (visionState == null) return "No Frame Loaded";
		return visionState.getCrossString();
	}

	public String getRedRobotString() {
		if (visionState == null) return "No Frame Loaded";
		return visionState.getRedRobotString();
	}

	public String getBlueRobotString() {
		if (visionState == null) return "No Frame Loaded";
		return visionState.getBlueRobotString();
	}

	public String getBlueGoalString() {
		if (visionState == null) return "No Frame Loaded";
		return visionState.getBlueGoalString();
	}

	public String getYellowGoalString() {
		if (visionState == null) return "No Frame Loaded";
		return visionState.getYellowGoalString();
	}

}
