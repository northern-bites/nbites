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

import nbtool.data.log._Log;
import nbtool.util.Logger;
import nbtool.util.Utility;
import messages.FilteredBall;
import messages.NaiveBall;
// import messages.Vision.*;
import messages.NaiveBall.Position;

public final class ProtoBallView extends nbtool.gui.logviews.misc.ViewParent {
	private static final int OFFSET = 2;
	private static final int BALL_SIZE = 18;
	private static final int SPACE = 20;

	private FilteredBall filteredBall;
	private NaiveBall naiveBall;

	public static Boolean shouldLoadInParallel() {return false;}

	public ProtoBallView() {
		super();
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

		if (!filteredBall.getVis().getOn()) {return;}
		int ballX = (robotX - OFFSET*((Float)filteredBall.getRelY()).intValue());
		int ballY = (robotY - OFFSET*((Float)filteredBall.getRelX()).intValue());

		int interceptX = (robotX - OFFSET*((Float)naiveBall.getYintercept()).intValue());
		int interceptY = robotY;

		g.setColor(Color.black);
		g.drawString("Velocity: " + naiveBall.getVelocity(), ballX + 10, ballY - 10);
		g.setColor(Color.red);
		g.fillOval(ballX - (BALL_SIZE/2), ballY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

		g.setColor(Color.pink);
		g.fillOval(interceptX - (BALL_SIZE/2), interceptY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

		g.setColor(Color.black);
		g.drawString("yintercept: " + naiveBall.getYintercept(), 160, height + SPACE);
		// g.drawString(naiveBall.getYintercept(), interceptX - 10, interceptY + 30);
		g.drawString("velocity: " + naiveBall.getVelocity(), 160, height + 40);
		g.drawString("rel_x: " + filteredBall.getRelX(), 10, height + SPACE);
		g.drawString("rel_y: " + filteredBall.getRelY(), 10, height + 2*SPACE);

		ArrayList<Position> nbPosition = (ArrayList<Position>)naiveBall.getPositionList();

		if (naiveBall.getPositionCount() < 1) { return; }

		for (int i = 0; i < naiveBall.getPositionCount(); i+=2) {
			int nBallX = (robotX - OFFSET*((Float) nbPosition.get(i).getX()).intValue());
			int nBallY = (robotY - OFFSET*((Float) nbPosition.get(i).getY()).intValue());
			float a = .4f * ((float)i / (float)nbPosition.size());

			g.setColor(new Color(0.f,0.f,.9f,a));
			g.drawOval(nBallX - (BALL_SIZE/2), nBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);
			g.fillOval(nBallX - (BALL_SIZE/2), nBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

		}

		g.setColor(Color.black);
		g.drawString("stationary: " + naiveBall.getStationary(), 10, height + 70);
		g.drawString("x_vel: " + naiveBall.getXVel(), 10, height + 90);
		g.drawString("y_vel: " + naiveBall.getYVel(), 10, height + 110);
		g.drawString("distance: " + filteredBall.getDistance(), 10, height + 150);
		g.drawString("bearing: " + filteredBall.getBearing(), 10, height + 170);
		g.drawString("bearing_deg: " + filteredBall.getBearingDeg(), 10, height + 190);

		ArrayList<Position> nbDest = (ArrayList<Position>)naiveBall.getDestBufferList();
		if (naiveBall.getDestBufferCount() < 1) return;

		int lineStartX = ballX;
		int lineStartY = ballY;
		int lineEndX = robotX - OFFSET*(((Float) nbDest.get(4).getY()).intValue());
		int lineEndY = robotY - OFFSET*(((Float) nbDest.get(4).getX()).intValue());

		lineEndX = (lineEndX > width) ? width - 10 : lineEndX;
		lineEndY = (lineEndY > height) ? height - 10 : lineEndY;

		for (int i = 0; i < nbDest.size(); i++) {
			int dBallX = (robotX - OFFSET*((Float) nbDest.get(i).getY()).intValue());
			int dBallY = (robotY - OFFSET*((Float) nbDest.get(i).getX()).intValue());

			if (dBallX > width || dBallY > height) break;

			float a = .81f * ((float)i / (float)nbDest.size());

			g.setColor(new Color(0.9f,0.8f,0.0f,(.9f - a)));
			g.drawOval(dBallX - (BALL_SIZE/2), dBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);
			g.fillOval(dBallX - (BALL_SIZE/2), dBallY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);
			g.setColor(Color.black);
			// g.drawString("x: " + dest_buf_x.get(i) + " y: " + dest_buf_y.get(i), dBallX, dBallY);
		}

		g.setColor(Color.black);
		g.drawLine(lineStartX, lineStartY, lineEndX, lineEndY);
		g.drawLine(lineStartX, lineStartY-1, lineEndX, lineEndY-1);

		if (naiveBall.getStationary()) g.setColor(Color.darkGray);
		else g.setColor(Color.red);
		g.fillOval(ballX - (BALL_SIZE/2), ballY - (BALL_SIZE/2), BALL_SIZE, BALL_SIZE);

	}

	// private void clear() {
	// 	filteredBall.clear();
	// 	visionBall.clear();
	// 	naiveBall.clear();
	// }

	public void setLog(_Log newlog) {
		// clear();
		String t = (String) newlog.primaryType();
		if (!t.equals("MULTIBALL")) return;

		int nb_length = Integer.parseInt(newlog.tree().find("contents").get(1).find("nb_length").get(1).value());
		byte[] nb = Arrays.copyOfRange(newlog.bytes,0,nb_length);
		byte[] fb = Arrays.copyOfRange(newlog.bytes,nb_length,newlog.bytes.length);

		try {
			FilteredBall fballMsg = FilteredBall.parseFrom(fb);
			NaiveBall nballMsg = NaiveBall.parseFrom(nb);

		} catch (Exception e) {
			e.printStackTrace();
		}


	}


}
