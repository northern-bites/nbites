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
	private static final int BALL_RADIUS = 17;

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
		int ballX = (robotY - OFFSET*((Float)filteredBall.get("rel_y")).intValue());
		int ballY = (robotX - OFFSET*((Float)filteredBall.get("rel_x")).intValue());

		g.setColor(Color.red);
		g.drawOval(ballX - 5, ballY - 5, BALL_RADIUS, BALL_RADIUS);
		g.fillOval(ballX - 5, ballY - 5, BALL_RADIUS, BALL_RADIUS);


		g.setColor(Color.black);
		g.drawString("rel_x: " + filteredBall.get("rel_x"), 10, height + 20);
		g.drawString("rel_y: " + filteredBall.get("rel_y"), 10, height + 40);

		ArrayList<Float> nbX = (ArrayList<Float>)naiveBall.get("position_x");
		ArrayList<Float> nbY = (ArrayList<Float>)naiveBall.get("position_y");
		if (nbX == null || nbY.isEmpty()) { return; }
		int nBallX = (robotY - OFFSET*((Float) nbY.get(0)).intValue());
		int nBallY = (robotX - OFFSET*((Float) nbX.get(0)).intValue());

		g.setColor(Color.blue);
		g.drawOval(nBallX - 5, nBallY - 5, BALL_RADIUS, BALL_RADIUS);
		g.fillOval(nBallX - 5, nBallY - 5, BALL_RADIUS, BALL_RADIUS);


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
		Map<FieldDescriptor, Object> nbFields = nbMsg.getAllFields();
		for (Map.Entry<FieldDescriptor, Object> entry : nbFields.entrySet()) {
			if (entry.getKey().getName().equals("position") && entry.getKey().isRepeated()) {
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
			}
			else { naiveBall.put(entry.getKey().getName(), entry.getValue()); }
		}


		System.out.println(naiveBall.toString());
		repaint();
	}

	protected void useSize(Dimension s) {


	}


}
