package nbtool.gui.logviews.proto;

import java.awt.Dimension;
import java.awt.Color;
import java.awt.Graphics;
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
	private Map<String, Object> filteredBall;
	private Map<String, Object> visionBall;

	public static Boolean shouldLoadInParallel() {return true;}

	public ProtoBallView() {
		super();
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



	}

	public void setLog(Log newlog) {
		String t = (String) newlog.primaryType();
		Class<? extends com.google.protobuf.GeneratedMessage> lClass = Utility.protobufClassFromType(t);
		Logger.logf(Logger.INFO, "ProtoBufView: using class %s for type %s.\n", lClass.getName(), t);
		com.google.protobuf.Message msg = Utility.protobufInstanceForClassWithData(lClass, newlog.bytes);

		if (!t.equals("proto-FilteredBall")) { return; }

		Map<FieldDescriptor, Object> fields = msg.getAllFields();
		for (Map.Entry<FieldDescriptor, Object> entry : fields.entrySet()) {
			if (entry.getKey().getName().equals("vis")) {
				parseVisList((com.google.protobuf.Message)entry.getValue());
			} else
			{ filteredBall.put(entry.getKey().getName(), entry.getValue()); }
		}

		repaint();


	}

	private void parseVisList(com.google.protobuf.Message visMsg) {
		System.out.println("IN PARSE VIS");

		Map<FieldDescriptor, Object> fields = visMsg.getAllFields();
		for (Map.Entry<FieldDescriptor, Object> entry : fields.entrySet()) {
			visionBall.put(entry.getKey().getName(), entry.getValue());
		}

	}

	protected void useSize(Dimension s) {


	}


}
