package nbtool.gui.logviews.fieldLocation;

import javax.swing.JPanel;
import javax.swing.JScrollPane;

import java.awt.*;
import java.awt.geom.*;

public class Field {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	//dimensions
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