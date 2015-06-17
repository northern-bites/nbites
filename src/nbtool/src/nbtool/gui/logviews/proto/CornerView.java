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
import java.lang.Exception;

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
	private static final int BALL_SIZE = 18;
	private static final int SPACE = 20;

	private Map<String, Object> filteredBall;
	private Map<String, Object> visionBall;
	private Map<String, Object> naiveBall;
	private Boolean ball_on;

	public static Boolean shouldLoadInParallel() {return false;}

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

		// if (1 > 0) return;

		if (!ball_on || visionBall.get("on").toString().equals("false")) {return;}
		int ballX = (robotX - OFFSET*((Float)filteredBall.get("rel_y")).intValue());
		int ballY = (robotY - OFFSET*((Float)filteredBall.get("rel_x")).intValue());

		int interceptX = (robotX - OFFSET*((Float)naiveBall.get("yintercept")).intValue());
		int interceptY = robotY;

		g.setColor(Color.black);
		g.drawString("Velocity: " + naiveBall.get("velocity"), ballX + 10, ballY - 10);
		g.setColor(Color.red);
		g.fillOval(ballX - (BALL_SIZE/2), ballY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

		g.setColor(Color.pink);
		g.fillOval(interceptX - (BALL_SIZE/2), interceptY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

		g.setColor(Color.black);
		g.drawString("yintercept: " + naiveBall.get("yintercept"), 160, height + SPACE);
		g.drawString(naiveBall.get("yintercept").toString(), interceptX - 10, interceptY + 30);
		g.drawString("velocity: " + naiveBall.get("velocity"), 160, height + 40);
		g.drawString("rel_x: " + filteredBall.get("rel_x"), 10, height + SPACE);
		g.drawString("rel_y: " + filteredBall.get("rel_y"), 10, height + 2*SPACE);

		ArrayList<Float> nbX = (ArrayList<Float>)naiveBall.get("position_x");
		ArrayList<Float> nbY = (ArrayList<Float>)naiveBall.get("position_y");

		if (nbX == null || nbY == null) { return; }

		for (int i = 0; i < nbX.size(); i+=2) {
			int nBallX = (robotX - OFFSET*((Float) nbY.get(i)).intValue());
			int nBallY = (robotY - OFFSET*((Float) nbX.get(i)).intValue());
			float a = .4f * ((float)i / (float)nbX.size());

			g.setColor(new Color(0.f,0.f,.9f,a));
			g.drawOval(nBallX - (BALL_SIZE/2), nBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);
			g.fillOval(nBallX - (BALL_SIZE/2), nBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

		}

		g.setColor(Color.black);
		g.drawString("stationary: " + naiveBall.get("stationary"), 10, height + 70);
		g.drawString("x_vel: " + naiveBall.get("x_vel"), 10, height + 90);
		g.drawString("y_vel: " + naiveBall.get("y_vel"), 10, height + 110);
		g.drawString("distance: " + filteredBall.get("distance"), 10, height + 150);
		g.drawString("bearing: " + filteredBall.get("bearing"), 10, height + 170);
		g.drawString("bearing_deg: " + filteredBall.get("bearing_deg"), 10, height + 190);

		ArrayList<Float> dest_buf_x = (ArrayList<Float>)naiveBall.get("buf_x");
		ArrayList<Float> dest_buf_y = (ArrayList<Float>)naiveBall.get("buf_y");
		if (dest_buf_x == null || dest_buf_y == null) return;

		int lineStartX = ballX;
		int lineStartY = ballY;
		int lineEndX = robotX - OFFSET*(((Float) dest_buf_y.get(4)).intValue());
		int lineEndY = robotY - OFFSET*(((Float) dest_buf_x.get(4)).intValue());

		lineEndX = (lineEndX > width) ? width - 10 : lineEndX;
		lineEndY = (lineEndY > height) ? height - 10 : lineEndY;

		for (int i = 0; i < dest_buf_x.size(); i++) {
			int dBallX = (robotX - OFFSET*((Float) dest_buf_y.get(i)).intValue());
			int dBallY = (robotY - OFFSET*((Float) dest_buf_x.get(i)).intValue());

			if (dBallX > width || dBallY > height) break;

			float a = .81f * ((float)i / (float)dest_buf_x.size());

			g.setColor(new Color(0.9f,0.8f,0.0f,(.9f - a)));
			g.drawOval(dBallX - (BALL_SIZE/2), dBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);
			g.fillOval(dBallX - (BALL_SIZE/2), dBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);
			g.setColor(Color.black);
			// g.drawString("x: " + dest_buf_x.get(i) + " y: " + dest_buf_y.get(i), dBallX, dBallY);
		}

		g.setColor(Color.black);
		g.drawLine(lineStartX, lineStartY, lineEndX, lineEndY);
		g.drawLine(lineStartX, lineStartY-1, lineEndX, lineEndY-1);

		if (naiveBall.get("stationary").toString().equals("true")) g.setColor(Color.darkGray);
		else g.setColor(Color.red);
		g.fillOval(ballX - (BALL_SIZE/2), ballY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

	}

	private void clear() {
		filteredBall.clear();
		visionBall.clear();
		naiveBall.clear();
	}

	public void setLog(Log newlog) {
		clear();
		try {
			String t = (String) newlog.primaryType();
			ball_on = false;
			if (!t.equals("MULTIBALL")) return;

			ball_on = true;

			int nb_length = Integer.parseInt(newlog.tree().find("contents").get(1).find("nb_length").get(1).value());
			byte[] nb = Arrays.copyOfRange(newlog.bytes,0,nb_length);
			byte[] fb = Arrays.copyOfRange(newlog.bytes,nb_length,newlog.bytes.length);

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

			Map<FieldDescriptor, Object> nbFields = nbMsg.getAllFields();
			for (Map.Entry<FieldDescriptor, Object> entry : nbFields.entrySet()) {
				if (entry.getKey().getName().equals("position")) {
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
				} else if (entry.getKey().getName().equals("dest_buffer")) {
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

		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("FOUNDEXCEPTION");
		}
	}

	protected void useSize(Dimension s) {


	}


}
