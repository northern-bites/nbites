package TOOL.WorldController;

import TOOL.TOOL;
import TOOL.TOOLException;
import TOOL.Net.RobotViewModule;
import java.util.Vector;
import java.net.*;

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
	private Vector<LocalizationPacket> locInfo;
	private Robot robot;
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

		robot = new Robot(0,0,0);

		try {
			while (true) {
				startTime = System.currentTimeMillis();
				if (!receiving){
					Thread.sleep(1000);
					continue;
				}

				PlayerInfo info = robotModule.getSelectedRobot().retrieveGCInfo();

				if (info == null){
					setReceiving(false);
					continue;
				}

				if (robot.getTeam() != info.team || robot.getNumber() != info.player ||
					robot.getColor() != info.color || robot == null) {
					robot = new Robot(info.team, info.player, info.color);
				}

				observedLandmarks =
					robotModule.getSelectedRobot().retrieveObjects();

				locInfo =
					robotModule.getSelectedRobot().retrieveLocalization();

				if (locInfo != null && observedLandmarks != null){
					robot.updateData(locInfo.get(0), locInfo.get(1));
					painter.updateRobot(robot);

					displayObservations();
					displayLocalization();
				}
				timeSpent = System.currentTimeMillis() - startTime;
				if (timeSpent < 80){
					Thread.sleep((int)(80 - timeSpent));
				}
			}
		} catch (InterruptedException e){
			setReceiving(false);
		}
	}

	public void displayObservations()
	{
		painter.reportEndFrame();
		ambiguousLandmarkCount = 0;
		debugViewer.removeLandmarks();
		for (Observation o : observedLandmarks) {
			// Paint observations
			int ID = o.getID();
			// Print observation info
			debugViewer.addLandmark(ID, o.getDistance(), o.getBearing());
			paintLandmark(ID);
		}
	}

	public void displayLocalization()
	{
		LocalizationPacket mine = locInfo.get(0);
		debugViewer.setMyLocEstimate(mine.getXEst(), mine.getYEst(), mine.getHeadingEst(),
									 mine.getXUncert(), mine.getYUncert(), mine.getHUncert());

		LocalizationPacket ball = locInfo.get(1);
		debugViewer.setBallLocEstimate(ball.getXEst(), ball.getYEst(),
									   ball.getXUncert(), ball.getYUncert(),
									   ball.getXVelocity(), ball.getYVelocity());

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
			System.out.println("Receiving TCP Information from robot");
		} else {
			painter.reportEndFrame();
		}
	}

	public boolean isReceiving()
	{
		return receiving;
	}

}