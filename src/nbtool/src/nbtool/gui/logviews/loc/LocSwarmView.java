package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

import com.google.protobuf.Message;

import messages.RobotLocationOuterClass.*;
import messages.ParticleSwarmOuterClass.ParticleSwarm;
import messages.ParticleSwarmOuterClass.Particle;
import messages.Vision.*;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;

public class LocSwarmView extends ViewParent implements ActionListener {

    private int numLines;

	@Override
	public void setLog(Log newlog) {
		log = newlog;
		naoParticles = initParticles(naoParticles);
		naoLines = initLines(naoLines);

		RobotLocation naoLoc;
		ParticleSwarm naoSwarm;
		FieldLines naoFieldLines;

		float naoX, naoY, naoH;

		try {
			naoLoc = RobotLocation.parseFrom(log.bytesForContentItem(0));
			naoX = naoLoc.getX();
			naoY = naoLoc.getY();
			naoH = naoLoc.getH();
			naoPlayer.moveTo(naoX,naoY, naoH);

			naoSwarm = ParticleSwarm.parseFrom(log.bytesForContentItem(1));
			for(int i=0; i<naoSwarm.getParticleCount(); i++) {
				RobotLocation currentNaoSwarm = naoSwarm.getParticle(i).getLoc();
				pWeight = naoSwarm.getParticle(i).getWeight();
				naoParticles[i].moveTo(currentNaoSwarm.getX(),currentNaoSwarm.getY());
			}

			naoFieldLines = FieldLines.parseFrom(log.bytesForContentItem(2));
            numLines = naoFieldLines.getLineCount();
			for(int i=0; i<naoFieldLines.getLineCount(); i++) {
				FieldLine curFieldLine = naoFieldLines.getLine(i);
				naoLines[i].r = curFieldLine.getInner().getR();
				naoLines[i].t = curFieldLine.getInner().getT();
				naoLines[i].end0 = curFieldLine.getInner().getEp0();
				naoLines[i].end1 = curFieldLine.getInner().getEp1();
				naoLines[i].houghIndex = 0.0;
				naoLines[i].fieldIndex = 0.0;
				naoLines[i].id = curFieldLine.getId();
				naoLines[i].prob = curFieldLine.getProb();
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	public LocSwarmView() {
		super();
		if(shouldFlip) {
			flip = new JButton("unflip");
		} else { flip = new JButton("flip"); }
		flip.addActionListener(this);
		flip.setPreferredSize(new Dimension(70,25));
		flip.setBounds((int)FieldConstants.FIELD_WIDTH/2,10,70,25);
		this.add(flip);
	}

	protected void useSize(Dimension s) {
		sp.setBounds(0,0,s.width,s.height);
	}

	public void actionPerformed(ActionEvent e) {
		if(e.getSource() == flip) {
			if(shouldFlip == true) {
				shouldFlip = false;
				flip.setText("flip");
				repaint();
			} else { 
				shouldFlip = true;
				flip.setText("unflip");
				repaint();
			}
		}
	}

	Field f = new Field();
	NaoRobot naoPlayer = new NaoRobot();
	NaoParticle naoParticles[] = new NaoParticle[300];
	GeoLine naoLines[] = new GeoLine[100];
	private JButton flip;
	private JScrollPane sp;
	public static float pWeight;
	public static boolean shouldFlip = false;
	public static final Dimension fieldDimension = new Dimension(
								(int)FieldConstants.FIELD_WIDTH,
								(int)FieldConstants.FIELD_HEIGHT);

	@Override
	public void paintComponent(Graphics g) {
		Graphics2D g2 = (Graphics2D) g;
		f.drawField(g2,shouldFlip);
		naoPlayer.drawNao(g2,shouldFlip);
		for(int i=0; i<naoParticles.length; i++) {
			naoParticles[i].drawParticle(g2,pWeight,shouldFlip);
		}
		for(int i=0; i<numLines; i++) {
			naoLines[i].draw(g2);
		}
	}

	public NaoParticle[] initParticles(NaoParticle[] p) {
		for(int i=0; i<p.length; i++) {
			p[i] = new NaoParticle();
		}
		return p;
	}
	public GeoLine[] initLines(GeoLine[] g) {
		for(int i=0; i<g.length; i++) {
			g[i] = new GeoLine();
		}
		return g;
	}
}
