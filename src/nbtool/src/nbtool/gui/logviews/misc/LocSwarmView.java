package nbtool.gui.logviews.misc;


import java.awt.*;
import java.awt.geom.*;
import javax.swing.*;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

import com.google.protobuf.Message;

import messages.RobotLocationOuterClass.*;
import messages.ParticleSwarmOuterClass.ParticleSwarm;
import messages.ParticleSwarmOuterClass.Particle;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;

public class LocSwarmView extends ViewParent {
	private static final long serialVersionUID = 1L;
	public static final Dimension fieldDimension = new Dimension(
								(int)FieldConstants.FIELD_WIDTH+
								(int)FieldConstants.GREEN_PAD_X,
								(int)FieldConstants.FIELD_HEIGHT+
								(int)FieldConstants.GREEN_PAD_X);
	private JScrollPane sp;
	private DrawPane dPane;

	public void setLog(Log newlog) {
		log = newlog;

		RobotLocation naoLoc;
		ParticleSwarm naoSwarm;

		float naoX;
		float naoY;

		try {
			naoLoc = RobotLocation.parseFrom(log.bytesForContentItem(0));
			naoX = naoLoc.getX();
			naoY = naoLoc.getY();
			System.out.println("X and Y: "+naoX+", "+naoY);
			dPane.naoPlayer.moveTo(naoX,naoY);

			naoSwarm = ParticleSwarm.parseFrom(log.bytesForContentItem(1));
			for(int i=0; i<naoSwarm.getParticleCount(); i++) {
				RobotLocation currentNao = naoSwarm.getParticle(i).getLoc();
				dPane.naoParticles[i].moveTo(currentNao.getX(),currentNao.getY());
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	public LocSwarmView() {
		super();

		dPane = new DrawPane();
		dPane.setPreferredSize(fieldDimension);

		sp = new JScrollPane(dPane);
		sp.setVisible(true);
		sp.setPreferredSize(new Dimension(800,800));

		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);

		this.add(sp);
	}

	protected void useSize(Dimension s) {
		dPane.setBounds(0,0,s.width,s.height);
		sp.setBounds(0,0,s.width,s.height);
	}

	private class DrawPane extends JPanel {
		public Field f;
		public NaoRobot naoPlayer;
		public NaoParticle naoParticles[] = new NaoParticle[300];

		public DrawPane() {
			super();
			f = new Field();
			naoPlayer = new NaoRobot();
			for(int i=0; i<naoParticles.length; i++) {
				naoParticles[i] = new NaoParticle();
			}
		}
		@Override
		public void paintComponent(Graphics g) {
			Graphics2D g2 = (Graphics2D) g;
			f.drawField(g2);
			naoPlayer.drawNao(g2);
			for(int i=0; i<naoParticles.length; i++) {
				naoParticles[i].drawParticle(g2);
			}
		}
	}
}

class Field {
	private static final long serialVersionUID = 1L;

	public static final Dimension fieldDimension = new Dimension((int)FieldConstants.FIELD_WIDTH,(int)FieldConstants.FIELD_HEIGHT);
	
    public static final int lineWidth = (int) FieldConstants.LINE_WIDTH;
    public static final int goalBoxWidth = 100;
    
    public static final Color fieldColor = new Color(0, 143, 56);
    
    public double playerLocX;
    public double playerLocY;
    
    public void drawField(Graphics2D g2) {
    	//drawing the green field
        g2.setColor(fieldColor);
        g2.fillRect(0, 0, fieldDimension.width+(int)FieldConstants.GREEN_PAD_X, fieldDimension.height+(int)FieldConstants.GREEN_PAD_X);
        //
        g2.setStroke(new BasicStroke(lineWidth));
        //draw center circle
        g2.setColor(Color.WHITE);
        g2.draw(new Ellipse2D.Float(FieldConstants.CENTER_FIELD_X-FieldConstants.CENTER_CIRCLE_RADIUS/2.0f, 
        							FieldConstants.CENTER_FIELD_Y-FieldConstants.CENTER_CIRCLE_RADIUS/2.0f, 
        							FieldConstants.CENTER_CIRCLE_RADIUS, FieldConstants.CENTER_CIRCLE_RADIUS));
        //center line
        g2.draw(new Line2D.Float(FieldConstants.MIDFIELD_X, FieldConstants.GREEN_PAD_Y, 
        						 FieldConstants.MIDFIELD_X, FieldConstants.FIELD_WHITE_TOP_SIDELINE_Y));
        //playing field borders
        g2.draw(new Rectangle2D.Float(FieldConstants.GREEN_PAD_X, FieldConstants.GREEN_PAD_Y, 
        							  FieldConstants.FIELD_WHITE_WIDTH, FieldConstants.FIELD_WHITE_HEIGHT));
        //left goalbox
        g2.draw(new Rectangle2D.Float(FieldConstants.GREEN_PAD_X,FieldConstants.FIELD_WHITE_HEIGHT*0.45f,
        							  FieldConstants.GOALBOX_DEPTH,FieldConstants.GOALBOX_WIDTH));
        //right goalbox
        g2.draw(new Rectangle2D.Float(FieldConstants.FIELD_GREEN_WIDTH-FieldConstants.GREEN_PAD_X-
        							  FieldConstants.GOALBOX_DEPTH, FieldConstants.FIELD_WHITE_HEIGHT*0.45f, 
        							  FieldConstants.GOALBOX_DEPTH,FieldConstants.GOALBOX_WIDTH));
        
        //left penalty mark
        g2.fillRect(FieldConstants.LEFT_PENALTY_MARK_X, fieldDimension.height/2, FieldConstants.PENALTY_MARK_WIDTH, FieldConstants.PENALTY_MARK_HEIGHT);
        g2.fillRect(FieldConstants.LEFT_PENALTY_MARK_X-FieldConstants.PENALTY_MARK_WIDTH, fieldDimension.height/2+FieldConstants.PENALTY_MARK_WIDTH, 
        		FieldConstants.PENALTY_MARK_HEIGHT, FieldConstants.PENALTY_MARK_WIDTH);
        //right penalty mark
        g2.fillRect(FieldConstants.RIGHT_PENALTY_MARK_X, fieldDimension.height/2, FieldConstants.PENALTY_MARK_WIDTH, FieldConstants.PENALTY_MARK_HEIGHT);
        g2.fillRect(FieldConstants.RIGHT_PENALTY_MARK_X-FieldConstants.PENALTY_MARK_WIDTH, fieldDimension.height/2+FieldConstants.PENALTY_MARK_WIDTH, 
        		FieldConstants.PENALTY_MARK_HEIGHT, FieldConstants.PENALTY_MARK_WIDTH);        
    }
}

abstract class FieldObject {
	protected float x,y,h,radius;
	protected Color naoColor;
	
	public FieldObject() {
		x = 0.0f;
		y = 0.0f;
		setRadiusAndColor();
	}

	public FieldObject(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
		setRadiusAndColor();
	}

	public FieldObject(float xCoord, float yCoord, float heading) {
		x = xCoord;
		y = yCoord;
		h = heading;
		setRadiusAndColor();
	}
	
	protected void FieldObject() {
		radius = 40.0f;
		naoColor = Color.CYAN;
	}

	protected void setRadiusAndColor() {
		radius = 40.0f;
		naoColor = Color.CYAN;
	}
	
	public void moveTo(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
	}
	
	public void drawNao(Graphics2D g2) {
		g2.setColor(naoColor);
		g2.fill(new Ellipse2D.Float(x-7, FieldConstants.FIELD_HEIGHT-y-7, 15.f, 15.f));
	}

	public void drawParticle(Graphics2D g2) {
		//want color based on particle weight
		g2.setColor(Color.red);
		g2.fill(new Ellipse2D.Float(x-1, FieldConstants.FIELD_HEIGHT-y-1, 2.f, 2.f));
	}
	
	public float getX() { return x; }
	public float getY() { return y; }
	public float getH() { return h; }
}

class NaoRobot extends FieldObject {
	private final static float naoRobotSize = 15.f;

	public NaoRobot() { super(); }

	NaoRobot(float xCoord, float yCoord, float heading) {
		super(xCoord, yCoord);
	}

	public void setRadiusAndColor() {
		radius = naoRobotSize;
		naoColor = Color.cyan;
	}

	public void moveTo(float xCoord, float yCoord) {
		super.moveTo(xCoord, yCoord);
	}
}

class NaoParticle extends FieldObject {
	private final static float particleSize = 2.f;

	public NaoParticle() { super(); }

	NaoParticle(float xCoord, float yCoord) {
		super(xCoord, yCoord);
	}

	public void setRadiusAndColor() {
		radius = particleSize;
		naoColor = Color.red;
	}

	public void moveTo(float xCoord, float yCoord) {
		super.moveTo(xCoord, yCoord);
	}
}

