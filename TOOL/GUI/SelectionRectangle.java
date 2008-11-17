package edu.bowdoin.robocup.TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;
import java.lang.Math;

public class SelectionRectangle extends Rectangle {
    
    public SelectionRectangle() {
        Stroke dashedLine = new BasicStroke(1.0F, BasicStroke.CAP_BUTT,
                                            BasicStroke.JOIN_MITER, 10,
                                            new float[] {2}, 0);
        setStroke(dashedLine);
    }

    public void setLineWidth(float f) {
        setStroke(new BasicStroke(f, BasicStroke.CAP_BUTT,
                                  BasicStroke.JOIN_MITER, 10,
                                  new float[] {(int) (2 * f)}, 0));
    }


}
