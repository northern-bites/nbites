
package TOOL.VisionTester;

import java.awt.Component;
import java.util.Vector;
import java.util.HashMap;
import java.util.Map;

import TOOL.TOOL;
import TOOL.TOOLModule;

import TOOL.Vision.*;
import TOOL.Data.DataSet;
import TOOL.Data.Frame;
import TOOL.Data.DataListener;
import TOOL.Data.DataManager;
import TOOL.Image.TOOLImage;

import TOOL.Image.ColorTable;
import TOOL.Image.ProcessedImage;

public class VisionTester implements DataListener
{
	private boolean isTesting = false;

	protected TOOL tool;
	private VisionTesterPanel mainPanel;

	private boolean notRunning = true;

	private int numBalls;
	private HashMap<Integer, Integer> numFieldObjects;
    private Vector<VisualLine> numVisualLines;
    private Vector<VisualCorner> numVisualCorners;

	public VisionTester(TOOL t){
        tool = t;
		mainPanel = new VisionTesterPanel(this);
		tool.getDataManager().addDataListener(this);
		numFieldObjects = new HashMap<Integer, Integer>();
	}

	public void notifyDataSet(DataSet s, Frame f) {
		notifyFrame(f);
	}

	public void notifyFrame(Frame f) {
		mainPanel.fixButtons();
    }

	public Component getContentPane(){
		return mainPanel;
	}

	public boolean canRunCount() {
		return tool.getDataManager().activeSet() != null && notRunning;
	}

	public void runSetObjCount(){
		notRunning = false;
		mainPanel.fixButtons();

		DataManager dm = tool.getDataManager();

		dm.set(0);				// Start with first frame
		while (dm.hasElementAfter() ) { // hits all but last frame
			processFrame( dm.activeFrame() );
			dm.next();
		}
		processFrame( dm.activeFrame() ); // Last frame doesn't have an elem after

		notRunning = true;
		mainPanel.fixButtons();
		printCounts();
		resetCounts();
	}

	private void processFrame(Frame f) {
		TOOLImage rawImage = f.image();
		ColorTable colorTable = tool.getColorTable();
		ProcessedImage thresholdedImage = new ProcessedImage(rawImage, colorTable);
		thresholdedImage.thresholdImage(rawImage, colorTable);
		addToCounts(thresholdedImage);
	}

	private void addToCounts(ProcessedImage image) {
		updateBallCount(image);
		updateFieldObjectCount(image);
	}
	private void updateBallCount(ProcessedImage image) {
		Ball ball = image.getVisionLink().getBall();
		if (ball.getRadius() > 0.0) {
			numBalls++;
		}
	}

	private void updateFieldObjectCount(ProcessedImage image) {
		TOOLVisionLink vl = image.getVisionLink();
		Vector<VisualFieldObject> visFieldObjs = vl.getVisualFieldObjects();
		for (VisualFieldObject obj : visFieldObjs) {
			updateFieldObjectAtID(obj);
		}
	}

	private void updateFieldObjectAtID(VisualFieldObject obj) {
		if (obj.getWidth() <= 0 ) {
			return;
		}

		int id = obj.getID();
		if (numFieldObjects.containsKey(id)) {
			numFieldObjects.put(id, numFieldObjects.get(id) + 1);
		} else {
			numFieldObjects.put(id, 1);
		}
	}

	private void printCounts() {
		printBallCounts();
		printVisualFieldObjectCounts();
	}
	private void printBallCounts() {
		System.out.println("Saw " + numBalls + " balls");
	}
	private void printVisualFieldObjectCounts() {
		for ( Map.Entry<Integer, Integer> entry : numFieldObjects.entrySet() ) {
			String objectName = visualFieldObjectStringFromID( entry.getKey() );
			System.out.println("Saw " + entry.getValue() + " of " + objectName );
		}
	}

	private String visualFieldObjectStringFromID(int id) {
		switch (id) {
		case VisualFieldObject.BLUE_GOAL_LEFT_POST:
			return "Blue Goal Left Post";
		case VisualFieldObject.YELLOW_GOAL_LEFT_POST:
			return "Yellow Goal Left Post";
		case VisualFieldObject.BLUE_GOAL_RIGHT_POST:
			return "Blue Goal Right Post";
		case VisualFieldObject.YELLOW_GOAL_RIGHT_POST:
			return "Yellow Goal Right Post";
		case VisualFieldObject.BLUE_GOAL_POST:
			return "Blue Goal Post";
		case VisualFieldObject.YELLOW_GOAL_POST:
			return "Yellow Goal Post";
		case VisualFieldObject.BLUE_GOAL_BACKSTOP:
			return "Blue Goal Backstop";
		case VisualFieldObject.YELLOW_GOAL_BACKSTOP:
			return "Yellow Goal Backstop";
		default:
			return "Unknown Object";
		}
	}

	private void resetCounts() {
		numBalls = 0;
		numFieldObjects.clear();
	}
}