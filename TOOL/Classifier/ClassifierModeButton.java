package edu.bowdoin.robocup.TOOL.Classifier;

import javax.swing.JToggleButton;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import java.awt.image.BufferedImage;
import java.awt.Image;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.geom.GeneralPath;

import java.awt.Dimension;
/**
 * @author Nicholas Dunn
 * @date   05/16/08
 *
 * 
 */
public class ClassifierModeButton extends JToggleButton {
    public static final int ICON_SIZE = 32;
    public static final int LINE_WIDTH = ICON_SIZE / 10;
    public static final int PADDING = ICON_SIZE/8;

    public static final Color FIELD_COLOR = Color.GREEN.darker();


    private ClassifierModel.ClassifierMode mode;
    private int num;

    public ClassifierModeButton(String description, int num, Image img,
                                ClassifierModel.ClassifierMode mode) {
        this.num = num;
        setToolTipText(description);
        this.mode = mode;

        // If there is no image, just use the tool tip text as the button label
        if (img == null) {  

            setText(description); 
            return; 
        }
        drawNumberOnImage(img, num);
        setIcon(new ImageIcon(img));
        
    }

    
    // Draws the number in upper left hand corner of icon
    public void drawNumberOnImage(Image icon, int number) {
        Graphics2D g2d = (Graphics2D) icon.getGraphics();
        g2d.setColor(Color.BLACK);
        g2d.drawString("" + number, 0, 10);
    }



    public ClassifierModel.ClassifierMode getMode() {
        return mode;
    }


    public static Image getBYBeaconImage() {
        return getBeaconImage(Color.BLUE, Color.YELLOW);
    }

    public static Image getYBBeaconImage() {
        return getBeaconImage(Color.YELLOW, Color.BLUE);
    }

    public static Image getBeaconImage(Color topColor, Color bottomColor) {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(Color.WHITE);

        int BEACON_WIDTH = ICON_SIZE / 3;
        int LEFT_OFFSET = (ICON_SIZE - BEACON_WIDTH) / 2;

        // Draw bottom part of post
        g2d.fillRect(LEFT_OFFSET, ICON_SIZE/2, BEACON_WIDTH, ICON_SIZE/2);
        g2d.setColor(bottomColor);
        // Draw middle section
        g2d.fillRect(LEFT_OFFSET, ICON_SIZE/4, BEACON_WIDTH, ICON_SIZE/4);
        
        g2d.setColor(topColor);
        // draw top section
        g2d.fillRect(LEFT_OFFSET, 0, BEACON_WIDTH, ICON_SIZE/4);
        
        return b;
    }
    
    public static Image getArcImage(Color arcColor) {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, 
                                            BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);

        g2d.setColor(Color.WHITE);
        g2d.setStroke(new BasicStroke(LINE_WIDTH));
        // Draw an inverted v
        // Left side
        g2d.drawLine(0, ICON_SIZE/4, ICON_SIZE/2, 0);
        // right side
        g2d.drawLine(ICON_SIZE/2, 0, ICON_SIZE, ICON_SIZE/4);

        // Draw the arc
        g2d.setColor(arcColor);
        GeneralPath arc = new GeneralPath();
        arc.moveTo(0,ICON_SIZE/2);
        // draw a curve through first two args reaching the point specified
        // by last twoo args
        arc.quadTo(ICON_SIZE/2,3*ICON_SIZE/4,ICON_SIZE,ICON_SIZE/2);
        g2d.draw(arc);
        return b;
    }


    public static Image getBlueArcImage() {
        return getArcImage(Color.BLUE);
    }


    public static Image getYellowArcImage() {
        return getArcImage(Color.YELLOW);
    }

   
    
    public static Image getSelectionImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(Color.WHITE);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        g2d.setColor(Color.BLACK);

        // Draw a dashed rectangle
        g2d.setStroke(new BasicStroke(1.0F, BasicStroke.CAP_BUTT,
                                      BasicStroke.JOIN_MITER,10,
                                      new float[] {2}, 0));
        g2d.drawRect(ICON_SIZE/8, ICON_SIZE/8, (6 * ICON_SIZE / 8),
                     (6 * ICON_SIZE / 8));
                                           
        return b;
    }


    public static Image getLineImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(Color.WHITE);
        g2d.setStroke(new BasicStroke(LINE_WIDTH));
        g2d.drawLine(0,ICON_SIZE/3,ICON_SIZE,2*ICON_SIZE/3);
        
        return b;
    }


    public static Image getTCornerImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(Color.WHITE);
        g2d.setStroke(new BasicStroke(LINE_WIDTH));
        g2d.drawLine(0, ICON_SIZE/3, ICON_SIZE, ICON_SIZE/3);
        g2d.drawLine(ICON_SIZE/2, ICON_SIZE/3, ICON_SIZE/2, ICON_SIZE);

        
        return b;
    }

    public static Image getInnerLCornerImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(Color.WHITE);
        g2d.setStroke(new BasicStroke(LINE_WIDTH));
        // Draw an inverted V

        // Left half
        g2d.drawLine(0, ICON_SIZE, ICON_SIZE/2, 2 * ICON_SIZE/3);

        // Right half
        g2d.drawLine(ICON_SIZE/2, 2 * ICON_SIZE/3, ICON_SIZE, ICON_SIZE);
        
        
        return b;

    }

    public static Image getOuterLCornerImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(Color.WHITE);
        g2d.setStroke(new BasicStroke(LINE_WIDTH));

        // Horizontal portion
        g2d.drawLine(0, 2*ICON_SIZE/3, ICON_SIZE/2, 2*ICON_SIZE/3);
        // Right half
        g2d.drawLine(ICON_SIZE/2, 2 * ICON_SIZE/3, ICON_SIZE, 0);
        return b;

    }

    // Remember, left = left from the goalie's perspective (right to attacker)
    public static Image getLeftPostImage(Color color) {
        
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(color);
        
        g2d.fillOval(ICON_SIZE/4,3*ICON_SIZE/4,2*ICON_SIZE/3,ICON_SIZE/4);
        g2d.fillRect(ICON_SIZE/4,0,2*ICON_SIZE/3,3*ICON_SIZE/4+ICON_SIZE/8);
        
        // draw a portion of backstop extending to the left
        g2d.fillRect(0,ICON_SIZE/3,ICON_SIZE/4,3*ICON_SIZE/5);
        
        return b;
    }

    // Remember, right = right from goalie's perspective (left to attacker)
    public static Image getRightPostImage(Color color) {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(color);
        
        g2d.fillOval(0,
                     3*ICON_SIZE/4,2*ICON_SIZE/3,ICON_SIZE/4);
        g2d.fillRect(0,0,2*ICON_SIZE/3,3*ICON_SIZE/4+ICON_SIZE/8);
        
        // draw a portion of backstop extending to the left
        g2d.fillRect(2*ICON_SIZE/3,ICON_SIZE/3,ICON_SIZE/3,3*ICON_SIZE/5);
        
        return b;
        

    }


    public static Image getBackstopImage(Color color) {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);
        
        g2d.setColor(color);
        g2d.fillRect(0,ICON_SIZE/3,ICON_SIZE,3*ICON_SIZE/5);

        return b;
    }

   

    public static Image getBlueBackstopImage() {
        return getBackstopImage(Color.BLUE);
    }

    public static Image getBlueRightPostImage() {
        return getRightPostImage(Color.BLUE);
    }
    
    public static Image getBlueLeftPostImage() {
        return getLeftPostImage(Color.BLUE);
    }

    public static Image getYellowBackstopImage() {
        return getBackstopImage(Color.YELLOW);
    }

    public static Image getYellowRightPostImage() {
        return getRightPostImage(Color.YELLOW);
    }
    
    public static Image getYellowLeftPostImage() {
        return getLeftPostImage(Color.YELLOW);
    }

    

    public static Image getBallImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);

        g2d.setColor(Color.ORANGE);
        g2d.fillOval(ICON_SIZE/4, ICON_SIZE/4, ICON_SIZE/2, ICON_SIZE/2);

        return b;
    }
    

    public static Image getCenterCircleImage() {
        BufferedImage b = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = b.createGraphics();
        g2d.setColor(FIELD_COLOR);
        g2d.fillRect(0,0,ICON_SIZE,ICON_SIZE);

        g2d.setColor(Color.WHITE);
        g2d.setStroke(new BasicStroke(ICON_SIZE / 10));
        g2d.drawOval(ICON_SIZE/4, ICON_SIZE/4, ICON_SIZE/2, ICON_SIZE/2);
        g2d.drawLine(0,ICON_SIZE/3,ICON_SIZE,2*ICON_SIZE/3);

        return b;
    }


}
   