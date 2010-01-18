package TOOL.WorldController;

import TOOL.TOOL;
import TOOL.TOOLException;
import TOOL.Net.RobotViewModule;
import java.util.Vector;
/**
 * Handles streaming landmark observation data from the robot.
 *
 * @author Jack Morrison (with heavy code reuse from LogHandler.java and other TOOL parts)
 */
public class TCPStreamHandler extends Thread {

	private boolean receiving;
	private RobotViewModule robotModule;
	private DebugViewer debugViewer;
	private WorldControllerPainter painter;
	private Vector<Observation> observedLandmarks;
	private int ambiguousLandmarkCount;

	public TCPStreamHandler(RobotViewModule robot_mod, DebugViewer _debug,
							WorldControllerPainter _painter)
	{
		super();
		receiving = false;
		start();
		robotModule = robot_mod;
		debugViewer = _debug;
		painter = _painter;
	}

	public void run()
	{
		float startTime = 0.0f;
		float timeSpent = 0.0f;
		try {
			while (true) {
				startTime = System.currentTimeMillis();
				if (!receiving){
					Thread.sleep(1000);
					continue;
				}
				observedLandmarks =
					robotModule.getSelectedRobot().retrieveObjects();

				displayObservations();
				timeSpent = System.currentTimeMillis() - startTime;
				if (timeSpent < robotModule.FRAME_LENGTH_MILLIS){
					Thread.sleep((int)(robotModule.FRAME_LENGTH_MILLIS - timeSpent));
				}
			}
		} catch (InterruptedException e){
			setReceiving(false);
		}
			//} catch (TOOLException e) {}
	}

	public void displayObservations()
	{
		ambiguousLandmarkCount = 0;
		debugViewer.removeLandmarks();
		for (Observation o : observedLandmarks) {
			// Paint observations
			int ID = o.getID();
			// Print observation info
			debugViewer.addLandmark(ID, o.getDistance(), o.getBearing());
			paintLandmark(ID);
		}
		painter.reportEndFrame();
	}

	private void paintLandmark(int ID)
	{
		if (debugViewer.isDistinctLandmarkID(ID)) {
			painter.sawLandmark((float)debugViewer.objectIDMap.get(ID).x,
								(float)debugViewer.objectIDMap.get(ID).y,
								0);
		} else if (ID != debugViewer.BALL_ID) { // We have an ambiguous landmark
			// get the list of possible landmarks
			++ambiguousLandmarkCount;
			for (int pos_id : debugViewer.getPossibleIDs(ID)) {
				painter.sawLandmark((float)
									debugViewer.objectIDMap.get(pos_id).x,
									(float)
									debugViewer.objectIDMap.get(pos_id).y,
									ambiguousLandmarkCount);
			}
		}
	}

	public void setReceiving(boolean toReceive)
	{
		if (receiving == toReceive)
			return;

		receiving = toReceive;
		if (receiving) {
			debugViewer.setVisible(true);
			System.out.println("Receiving TCP Information from robot");
		}

	}

}