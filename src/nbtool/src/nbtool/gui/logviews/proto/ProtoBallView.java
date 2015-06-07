package nbtool.gui.logviews.proto;

import java.awt.Dimension;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.geom.*;
import java.awt.Rectangle;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.util.Map;
import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.lang.Float;
import java.lang.Integer;

import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;

import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Message;

import nbtool.data.Log;
import nbtool.util.Logger;
import nbtool.util.Utility;

public final class ProtoBallView extends nbtool.gui.logviews.misc.ViewParent {
	private static final long serialVersionUID = -541524730464912737L;
	private static final int OFFSET = 2;
	private static final int BALL_RADIUS = 18;

	private Map<String, Object> filteredBall;
	private Map<String, Object> visionBall;
	private Map<String, Object> naiveBall;
	private Boolean ball_on;

	public static Boolean shouldLoadInParallel() {return true;}

	public ProtoBallView() {
		super();
		ball_on = false;
		filteredBall = new HashMap();
		visionBall = new HashMap();
		naiveBall = new HashMap();
	}

	public void paintComponent(Graphics g){
		int height = (int) (this.getHeight() * .75);
		int width = this.getWidth();

		g.setColor(new Color(0, 139, 0));
		g.drawRect(0,0,width, height);
		g.fillRect(0,0,width, height);

		int robotX = width/2;
		int robotY = height/2;

		g.setColor(Color.black);
		g.drawLine(robotX-10,robotY-10,robotX+10,robotY+10);
		g.drawLine(robotX-10,robotY+10,robotX+10,robotY-10);

		if (!ball_on) {return;}
		int ballX = (robotX - OFFSET*((Float)filteredBall.get("rel_y")).intValue());
		int ballY = (robotY - OFFSET*((Float)filteredBall.get("rel_x")).intValue());
		int intX = (robotX - OFFSET*((Float)naiveBall.get("yintercept")).intValue());
		int intY = robotY;

		g.setColor(Color.red);
		g.drawOval(ballX - (BALL_RADIUS/2), ballY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);
		g.fillOval(ballX - (BALL_RADIUS/2), ballY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);
		g.setColor(Color.pink);
		g.fillOval(intX - (BALL_RADIUS/2), intY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);

		g.setColor(Color.black);
		g.drawString("yintercept: " + naiveBall.get("yintercept"), 160, height + 20);
		g.drawString("velocity: " + naiveBall.get("velocity"), 160, height + 40);
		g.drawString("rel_x: " + filteredBall.get("rel_x"), 10, height + 20);
		g.drawString("rel_y: " + filteredBall.get("rel_y"), 10, height + 40);

		ArrayList<Float> nbX = (ArrayList<Float>)naiveBall.get("position_x");
		ArrayList<Float> nbY = (ArrayList<Float>)naiveBall.get("position_y");

		if (nbX == null || nbY == null) { return; }

		for (int i = 0; i < nbX.size(); i++) {
			int nBallX = (robotX - OFFSET*((Float) nbY.get(i)).intValue());
			int nBallY = (robotY - OFFSET*((Float) nbX.get(i)).intValue());

			g.setColor(new Color(0.f,0.f,.9f,(float)(.2 + .2*(i/nbX.size()))));
			g.drawOval(nBallX - (BALL_RADIUS/2), nBallY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);
			g.fillOval(nBallX - (BALL_RADIUS/2), nBallY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);

		}

		int lineStartX = robotX - OFFSET*((Float) nbY.get(1)).intValue();
		int lineStartY = robotY - OFFSET*((Float) nbX.get(1)).intValue();
		int lineEndX = robotX - OFFSET*(((Float) nbY.get(0)).intValue());
		int lineEndY = robotY - OFFSET*(((Float) nbX.get(0)).intValue());

		g.setColor(Color.black);
		g.drawString("nb_x_start: " + nbX.get(1), 10, height + 60);
		g.drawString("nb_y_start: " + nbY.get(1), 10, height + 80);
		g.setColor(Color.black);
		g.drawString("nb_x_end: " + nbX.get(0), 10, height + 100);
		g.drawString("nb_y_end: " + nbY.get(0), 10, height + 120);

		g.setColor(Color.black);
		g.drawLine(lineStartX, lineStartY, lineEndX, lineEndY);
		g.drawLine(lineStartX, lineStartY-1, lineEndX, lineEndY-1);

		ArrayList<Float> bufX = (ArrayList<Float>)naiveBall.get("buf_x");
		ArrayList<Float> bufY = (ArrayList<Float>)naiveBall.get("buf_y");
		if (bufX == null || bufY == null) return;

		for (int i = 0; i < bufX.size(); i++) {
			int nBallX = (robotX - OFFSET*((Float) bufY.get(i)).intValue());
			int nBallY = (robotY - OFFSET*((Float) bufX.get(i)).intValue());

			g.setColor(new Color(0.9f,0.6f,0.0f,.8f));
			g.drawOval(nBallX - (BALL_RADIUS/2), nBallY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);
			g.fillOval(nBallX - (BALL_RADIUS/2), nBallY - (BALL_RADIUS/2), BALL_RADIUS, BALL_RADIUS);

		}

	}

	private void clear() {
		filteredBall.clear();
		visionBall.clear();
		naiveBall.clear();
	}

	public void setLog(Log newlog) {
		clear();
		String t = (String) newlog.primaryType();
		ball_on = false;
		if (!t.equals("MULTIBALL")) return;
		ball_on = true;

		int nb_length = Integer.parseInt(newlog.tree().find("contents").get(1).find("nb_length").get(1).value());
		byte[] nb = Arrays.copyOfRange(newlog.bytes,0,nb_length);
		byte[] fb = Arrays.copyOfRange(newlog.bytes,nb_length,newlog.bytes.length);

		System.out.println("OMG");
		Class<? extends com.google.protobuf.GeneratedMessage> nbClass = Utility.protobufClassFromType("proto-NaiveBall");
		Class<? extends com.google.protobuf.GeneratedMessage> fbClass = Utility.protobufClassFromType("proto-FilteredBall");
		com.google.protobuf.Message nbMsg = Utility.protobufInstanceForClassWithData(nbClass, nb);
		com.google.protobuf.Message fbMsg = Utility.protobufInstanceForClassWithData(fbClass, fb);

		Map<FieldDescriptor, Object> fbFields = fbMsg.getAllFields();
		for (Map.Entry<FieldDescriptor, Object> entry : fbFields.entrySet()) {
			if (entry.getKey().getName().equals("vis")) {
				Map<FieldDescriptor, Object> visFields = ((com.google.protobuf.Message)entry.getValue()).getAllFields();
				for (Map.Entry<FieldDescriptor, Object> visEntry : visFields.entrySet()) {
					visionBall.put(visEntry.getKey().getName(), visEntry.getValue());
				}
			} else {
				filteredBall.put(entry.getKey().getName(), entry.getValue());
			}
		}
		System.out.println("OMGg");
		Map<FieldDescriptor, Object> nbFields = nbMsg.getAllFields();
		for (Map.Entry<FieldDescriptor, Object> entry : nbFields.entrySet()) {
			if (entry.getKey().getName().equals("position")) {
				System.out.println("FOUND POSITION");
				ArrayList<Float> position_x = new ArrayList();
				ArrayList<Float> position_y = new ArrayList();
				List<Object> vals = (List<Object>) entry.getValue();
				int i = 0;
				for (Object v : vals) {
					Map<FieldDescriptor, Object> posFields = ((com.google.protobuf.Message)v).getAllFields();
					for (Map.Entry<FieldDescriptor, Object> posEntry : posFields.entrySet()) {
						if (posEntry.getKey().getName().equals("x")) {position_x.add((float)posEntry.getValue());}
						else {position_y.add((float)posEntry.getValue());}
					}
				}
				naiveBall.put("position_x", position_x);
				naiveBall.put("position_y", position_y);
			} else if (entry.getKey().getName().equals("sec_buffer")) {
				ArrayList<Float> buf_x = new ArrayList();
				ArrayList<Float> buf_y = new ArrayList();
				List<Object> vals = (List<Object>) entry.getValue();
				int i = 0;
				for (Object v : vals) {
					Map<FieldDescriptor, Object> posFields = ((com.google.protobuf.Message)v).getAllFields();
					for (Map.Entry<FieldDescriptor, Object> posEntry : posFields.entrySet()) {
						if (posEntry.getKey().getName().equals("x")) {buf_x.add((float)posEntry.getValue());}
						else {buf_y.add((float)posEntry.getValue());}
					}
				}
				naiveBall.put("buf_x", buf_x);
				naiveBall.put("buf_y", buf_y);
			}
			else { naiveBall.put(entry.getKey().getName(), entry.getValue()); }
		}

		System.out.println("OMGw");

		// System.out.println(naiveBall.toString());
		repaint();
	}

	protected void useSize(Dimension s) {


	}


}
