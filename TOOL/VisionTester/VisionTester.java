
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
		printObjectCounts();
	}

	private void processFrame(Frame f) {
		System.out.println("frame #" + tool.getDataManager().activeFrameIndex());
		TOOLImage rawImage = f.image();
		ColorTable colorTable = tool.getColorTable();
		ProcessedImage thresholdedImage = new ProcessedImage(rawImage, colorTable);
		thresholdedImage.thresholdImage(rawImage, colorTable);
		addToObjectCounts(thresholdedImage);
	}

	private void addToObjectCounts(ProcessedImage image) {
		TOOLVisionLink visionLink = image.getVisionLink();
		Ball ball = image.getVisionLink().getBall();
		if (ball.getRadius() > 0.0) {
			numBalls++;
		}

		Vector<VisualFieldObject> visFieldObjs = image.getVisionLink().getVisualFieldObjects();
		for (VisualFieldObject obj : visFieldObjs) {
			int id = obj.getID();

			if (!numFieldObjects.containsKey(id)){
				numFieldObjects.put(id, 1);
			} else {
				numFieldObjects.put(id, numFieldObjects.get(id) + 1);
			}
		}
	}

	private void printObjectCounts() {
		System.out.println("Saw " + numBalls + " balls");
		for ( Map.Entry<Integer, Integer> entry : numFieldObjects.entrySet() ) {
			String objectName;
			switch ( entry.getKey() ){
			case VisualFieldObject.BLUE_GOAL_LEFT_POST:
				objectName = "Blue Goal Left Post"; break;
			case VisualFieldObject.YELLOW_GOAL_LEFT_POST:
				objectName = "Yellow Goal Left Post"; break;
			case VisualFieldObject.BLUE_GOAL_RIGHT_POST:
				objectName = "Blue Goal Right Post"; break;
			case VisualFieldObject.YELLOW_GOAL_RIGHT_POST:
				objectName = "Yellow Goal Right Post"; break;
			case VisualFieldObject.BLUE_GOAL_POST:
				objectName = "Blue Goal Post"; break;
			case VisualFieldObject.YELLOW_GOAL_POST:
				objectName = "Yellow Goal Post"; break;
			case VisualFieldObject.BLUE_GOAL_BACKSTOP:
				objectName = "Blue Goal Backstop"; break;
			case VisualFieldObject.YELLOW_GOAL_BACKSTOP:
				objectName = "Yellow Goal Backstop"; break;
			default:
				objectName = "Unknown Object";
			}
			System.out.println("Saw " + entry.getValue() + " of ID " + objectName );
		}
	}
}