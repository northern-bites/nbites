package edu.bowdoin.robocup.TOOL.Image;


import java.awt.AlphaComposite;
import java.awt.Graphics;
import java.awt.Graphics2D;
import edu.bowdoin.robocup.TOOL.ColorEdit.ColorEdit;
import java.awt.image.BufferedImage;
import java.awt.event.*;

public class ColorTablePanel extends DrawingPanel {
    
    protected ColorEdit colorEdit;
    protected BufferedImage colorTableOverlay;

   

    public ColorTablePanel(ColorEdit edit) {
        super();
        colorEdit = edit;
        filled = true;
        drawMode = DrawingMode.RECT;
        drawing = (colorEdit.getView() != ColorEdit.View.COLORVIEW) &&
            (colorEdit.getEditing());
    }

    public void paint(Graphics g) {
        super.paint(g);

        Graphics2D g2d = (Graphics2D)g;
        g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 
                                                    1.0F));

        if (colorTableOverlay != null) {
            g2d.drawImage(colorTableOverlay, imageOp, 0, 0);
        }
        g2d.dispose();

        
    }
    
    public void setColorTableOverlay(BufferedImage f) {
        colorTableOverlay = f;
    }

    public void mousePressed(MouseEvent e) {
        drawing = (colorEdit.getView() != ColorEdit.View.COLORVIEW) &&
            colorEdit.getEditing();
        x1 = (int) e.getX();
        y1 = (int) e.getY();

    }
    public void mouseReleased(MouseEvent e) {
        x2 = (int) e.getX();
        y2 = (int) e.getY();
        drawing = false;
    }


}