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
import java.util.ArrayList;
import java.util.HashMap;
import java.lang.Float;

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
	private Map<String, Object> filteredBall;
	private Map<String, Object> visionBall;
	private Boolean ball_on;

	public static Boolean shouldLoadInParallel() {return true;}

	public ProtoBallView() {
		super();
		ball_on = false;
		filteredBall = new HashMap();
		visionBall = new HashMap();
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
		g.drawOval(ballX - 5, ballY - 5, 17, 17);
		g.fillOval(ballX - 5, ballY - 5, 17, 17);

		g.setColor(Color.black);
		g.drawString("rel_x: " + filteredBall.get("rel_x"), 10, height + 20);
		g.drawString("rel_y: " + filteredBall.get("rel_y"), 10, height + 40);

	}

	public void setLog(Log newlog) {
		String t = (String) newlog.primaryType();
		Class<? extends com.google.protobuf.GeneratedMessage> lClass = Utility.protobufClassFromType(t);
		Logger.logf(Logger.INFO, "ProtoBufView: using class %s for type %s.\n", lClass.getName(), t);
		com.google.protobuf.Message msg = Utility.protobufInstanceForClassWithData(lClass, newlog.bytes);
		ball_on = false;
		if (!t.equals("proto-FilteredBall")) { return; }
		ball_on = true;
		Map<FieldDescriptor, Object> fields = msg.getAllFields();
		for (Map.Entry<FieldDescriptor, Object> entry : fields.entrySet()) {
			if (entry.getKey().getName().equals("vis")) {
				Map<FieldDescriptor, Object> visFields = ((com.google.protobuf.Message)entry.getValue()).getAllFields();
				for (Map.Entry<FieldDescriptor, Object> visEntry : visFields.entrySet()) {
					visionBall.put(entry.getKey().getName(), visEntry.getValue());
				}
			} else {
				filteredBall.put(entry.getKey().getName(), entry.getValue());
			}
		}

		repaint();


	}

	protected void useSize(Dimension s) {


	}


}
