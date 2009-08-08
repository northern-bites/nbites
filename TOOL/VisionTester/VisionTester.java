
package TOOL.VisionTester;

import java.awt.Component;
import java.util.Vector;
import java.util.HashMap;

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
		System.out.println("run!");
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
		System.out.println("numBalls is " + numBalls);
		System.out.println("done run!");
	}

	private void processFrame(Frame f) {
		System.out.println("frame #" + tool.getDataManager().activeFrameIndex());
		TOOLImage rawImage = f.image();
		ColorTable colorTable = tool.getColorTable();
		ProcessedImage thresholdedImage = new ProcessedImage(rawImage, colorTable);
		addToObjectCounts(thresholdedImage);
	}

	private void addToObjectCounts(ProcessedImage image) {
		TOOLVisionLink visionLink = image.getVisionLink();
		Ball ball = image.getVisionLink().getBall();
		System.out.println(ball.getRadius());
		System.out.println(ball.getHeight());
		System.out.println(ball.getWidth());
		System.out.println(ball.getX());
		if (ball.getRadius() > 0.0) {
			numBalls++;
		}

		Vector<VisualFieldObject> visFieldObjs = image.getVisionLink().getVisualFieldObjects();

		// for (VisualFieldObject obj : visFieldObjs) {
		// 	int id = obj.getID();

		// 	if (numFieldObjects.containsKey(id)){
		// 		numFieldObjects.put(id, 1);
		// 	} else {
		// 		int oldCount = numFieldObjects.get(id);
		// 		numFieldObjects.put(id, oldCount++);
		// 	}
		// }
	}
}