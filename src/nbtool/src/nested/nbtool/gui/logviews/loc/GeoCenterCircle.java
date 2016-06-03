package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

import nbtool.gui.field.FieldConstants;

public class GeoCenterCircle {
    float x,y;
    float prob;

    public GeoCenterCircle() {
        x = 0;
        y = 0;
    }

    public GeoCenterCircle(float cx, float cy, float c_prob) {
        x = cx;
        y = cy;
        prob = c_prob;
    }

    public void draw(Graphics2D g2, boolean shouldFlip) {
        Font font = new Font("Sans_Serif", Font.PLAIN, 30);
        g2.setColor(Color.white);
        g2.setStroke(new BasicStroke(3));

        g2.drawOval((int)x - 15, (int)(FieldConstants.FIELD_HEIGHT - y) -15, 30, 30);
        font = new Font("Sans_Serif", Font.PLAIN, 18);
        g2.setFont(font);
        g2.drawString(Float.toString(prob),x,FieldConstants.FIELD_HEIGHT-y+60);
    }
}
