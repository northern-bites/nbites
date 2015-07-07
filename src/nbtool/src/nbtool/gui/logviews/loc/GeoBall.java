package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

public class GeoBall {
    int x,y;
    double confidence;

    public GeoBall() {
        x = 0;
        y = 0;
    }

    public GeoBall(int bx, int by, double b_con) {
        x = bx;
        y = by;
        confidence = b_con;
    }

    public void draw(Graphics2D g2, boolean shouldFlip) {
        Font font = new Font("Sans_Serif", Font.PLAIN, 30);
        g2.setColor(Color.orange);
        g2.fillOval((int)x - 10, (int)(FieldConstants.FIELD_HEIGHT - y - 10), 20, 20);
        font = new Font("Sans_Serif", Font.PLAIN, 18);
        g2.setFont(font);
        g2.drawString(Double.toString(confidence),x,FieldConstants.FIELD_HEIGHT-y+60);
    }
}
