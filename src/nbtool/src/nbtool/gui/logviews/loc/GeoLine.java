package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import javax.swing.*;

public class GeoLine {
	double r,t,end0,end1,houghIndex,fieldIndex,id,prob;

	public GeoLine() {
		r = 0;
		t = 0;
		end0 = 0;
		end1 = 0;
		houghIndex = 0;
		fieldIndex = 0;
		id = 0;
		prob = 0;
	}

	public GeoLine(double line_r, double line_t, double line_end0, 
			double line_end1, double line_houghInd, double line_fieldInd) {
		r = line_r;
		t = line_t;
		end0 = line_end0;
		end1 = line_end1;
		houghIndex = line_houghInd;
		fieldIndex = line_fieldInd;
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
		float lineWidth = 5.0f;
		g2.setStroke(new BasicStroke(lineWidth/2));

		double x0 = r*Math.cos(t);
		double y0 = r*Math.sin(t);

		int x1 = (int) Math.round(x0 + end0 * Math.sin(t));
		int y1 = (int) Math.round(y0 - end0 * Math.cos(t));
        int x2 = (int) Math.round(x0 + end1 * Math.sin(t));
        int y2 = (int) Math.round(y0 - end1 * Math.cos(t));

        double xstring = (x1 + x2) / 2;
        double ystring = (y1 + y2) / 2;

        double scale = 0;
        if (r > 0) {
        	scale = 10;
        } else { scale = 3; }
        
        xstring += scale*Math.cos(t);
        ystring += scale*Math.sin(t);

        switch ((int)id) {
			case 0: g2.setColor(Color.black); //not matched
					break;
			case 1: g2.setColor(Color.blue); //our end line
					break;
			case 2: g2.setColor(Color.red); //their end line
					break;
			case 3: g2.setColor(Color.gray); //our mid line
					break;
			case 4: g2.setColor(Color.gray); //their mid line
					break;
			case 5: g2.setColor(Color.magenta); //our goalbox
					break;
			case 6: g2.setColor(Color.orange); //their goalbox
					break;
			case 7: g2.setColor(Color.cyan); //right side line (bottom)
					break;
			case 8: g2.setColor(Color.pink); //left side line (top)
					break;
		}

        g2.drawLine(x1,(int)FieldConstants.FIELD_HEIGHT-y1,x2,(int)FieldConstants.FIELD_HEIGHT-y2);
        g2.drawString(Integer.toString((int) id) + "/" + Double.toString(prob), 
                        (int) xstring, (int) (FieldConstants.FIELD_HEIGHT-ystring));
	}
}
