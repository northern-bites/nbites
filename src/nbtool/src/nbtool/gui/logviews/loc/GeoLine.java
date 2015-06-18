package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import javax.swing.*;

public class GeoLine {
	double r,t,end0,end1,houghIndex,fieldIndex;
	double ux, uy;

	public GeoLine() {
		r = 0;
		t = 0;
		end0 = 0;
		end1 = 0;
		houghIndex = 0;
		fieldIndex = 0;
		ux = Math.cos(0);
		uy = Math.sin(0);
	}

	public GeoLine(double line_r, double line_t, double line_end0, 
			double line_end1, double line_houghInd, double line_fieldInd) {
		r = line_r;
		t = line_t;
		end0 = line_end0;
		end1 = line_end1;
		houghIndex = line_houghInd;
		fieldIndex = line_fieldInd;
		ux = Math.cos(t);
		uy = Math.sin(t);
	}

	public void draw(Graphics2D g2, BufferedImage origImg) {
		int x,y;
		float lineWidth = 5.0f;

		g2.setStroke(new BasicStroke(lineWidth/2));

		if(fieldIndex == -1) {
			g2.setColor(Color.blue);
		} else { g2.setColor(Color.red); }

		double x0 = 2*r * Math.cos(t) + origImg.getWidth() / 2;
        double y0 = -2*r * Math.sin(t) + origImg.getHeight() / 2;
        int x1 = (int) Math.round(x0 + 2*end0 * Math.sin(t));
        int y1 = (int) Math.round(y0 + 2*end0 * Math.cos(t));
        int x2 = (int) Math.round(x0 + 2*end1 * Math.sin(t));
        int y2 = (int) Math.round(y0 + 2*end1 * Math.cos(t));

        double xstring = (x1 + x2) / 2;
        double ystring = (y1 + y2) / 2;

        double scale = 0;
        if (r > 0) {
        	scale = 10;
        } else { scale = 3; }
        
        xstring += scale*Math.cos(t);
        ystring += scale*Math.sin(t);

        g2.drawLine(x1,y1,x2,y2);
        g2.drawString(Integer.toString((int) houghIndex) + "/" 
        				+ Integer.toString((int) fieldIndex), 
                        (int) xstring, (int) ystring);
	}

	public void draw(Graphics2D g2) {
		int x,y;
		float lineWidth = 5.0f;

		g2.setStroke(new BasicStroke(lineWidth/2));

		double x0 = 2*r*Math.cos(t);
		double y0 = -2*r*Math.sin(t);

		int x1 = (int) Math.round(x0 + 2*end0 * Math.sin(t));
		int y1 = (int) Math.round(y0 + 2*end0 * Math.cos(t));
        int x2 = (int) Math.round(x0 + 2*end1 * Math.sin(t));
        int y2 = (int) Math.round(y0 + 2*end1 * Math.cos(t));

        double scale = 0;
        if (r > 0) {
        	scale = 10;
        } else { scale = 3; }

        g2.drawLine(x1,y1,x2,y2);
	}
	/*
	public void endPoints(Double[] epoints) {
		double x0 = r*ux;
		double y0 = r*uy;
		epoints[0] = x0+end0*uy;
		epoints[1] = y0-end0*ux;
		epoints[2] = x0+end1*uy;
		epoints[3] = y0-end1*ux;
	}

	public void translateRotate(double xTrans, double yTrans, double rotation) {
		Double ep[] = new Double[4];
		endPoints(ep);

		Double ep1t[] = new Double[2];
		Double ep2t[] = new Double[2];

		translateRotate(ep[0],ep[1],xTrans,yTrans,rotation,ep1t);
		translateRotate(ep[2],ep[3],xTrans,yTrans,rotation,ep2t);


	} */
}