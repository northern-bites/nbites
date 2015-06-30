package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

public class Field extends JPanel {
	private static final long serialVersionUID = 1L;

	public static final Dimension fieldDimension = new Dimension((int)FieldConstants.FIELD_WIDTH,
        (int)FieldConstants.FIELD_HEIGHT);
	
    public static final int lineWidth = (int) FieldConstants.LINE_WIDTH;
    public static final int goalBoxWidth = 100;
    
    public static final Color fieldColor = new Color(0, 143, 56);

    public Field() {
        this.setMinimumSize(fieldDimension);
        this.setPreferredSize(fieldDimension);
        this.setMaximumSize(fieldDimension);
    }
    
    public void drawField(Graphics2D g2, boolean shouldFlip) {
    	//drawing the green field
        g2.setColor(fieldColor);
        g2.fillRect(0, 0, fieldDimension.width, fieldDimension.height); //greens
        g2.setStroke(new BasicStroke(lineWidth));

        g2.setColor(Color.WHITE);
        //center circle
        g2.draw(new Ellipse2D.Float(FieldConstants.CENTER_FIELD_X-FieldConstants.CENTER_CIRCLE_RADIUS/2.0f, 
        							FieldConstants.CENTER_FIELD_Y-FieldConstants.CENTER_CIRCLE_RADIUS/2.0f, 
        							FieldConstants.CENTER_CIRCLE_RADIUS, FieldConstants.CENTER_CIRCLE_RADIUS));
        //center line
        g2.setColor(Color.gray);
        g2.draw(new Line2D.Float(FieldConstants.MIDFIELD_X, FieldConstants.GREEN_PAD_Y, 
        						 FieldConstants.MIDFIELD_X, FieldConstants.FIELD_WHITE_TOP_SIDELINE_Y));
        //left goalbox
        if(shouldFlip) { g2.setColor(Color.orange); } else { g2.setColor(Color.magenta);}
        g2.draw(new Rectangle2D.Float(FieldConstants.GREEN_PAD_X,FieldConstants.FIELD_WHITE_HEIGHT*0.45f,
        							  FieldConstants.GOALBOX_DEPTH,FieldConstants.GOALBOX_WIDTH));
        //right goalbox
        if(shouldFlip) { g2.setColor(Color.magenta); } else { g2.setColor(Color.orange);}
        g2.draw(new Rectangle2D.Float(FieldConstants.FIELD_GREEN_WIDTH-FieldConstants.GREEN_PAD_X-
        							  FieldConstants.GOALBOX_DEPTH, FieldConstants.FIELD_WHITE_HEIGHT*0.45f, 
        							  FieldConstants.GOALBOX_DEPTH,FieldConstants.GOALBOX_WIDTH));

        //our end line
        if(shouldFlip) { g2.setColor(Color.red); } else { g2.setColor(Color.blue);}
        g2.draw(new Line2D.Float(FieldConstants.GREEN_PAD_X,FieldConstants.GREEN_PAD_Y,
            FieldConstants.GREEN_PAD_X, FieldConstants.FIELD_GREEN_HEIGHT-FieldConstants.GREEN_PAD_Y));

        //their end line
        if(shouldFlip) { g2.setColor(Color.blue); } else { g2.setColor(Color.red);}
        g2.draw(new Line2D.Float(FieldConstants.FIELD_WHITE_WIDTH+FieldConstants.GREEN_PAD_X, 
            FieldConstants.GREEN_PAD_Y,FieldConstants.FIELD_WHITE_WIDTH+FieldConstants.GREEN_PAD_X,
            FieldConstants.FIELD_GREEN_HEIGHT-FieldConstants.GREEN_PAD_Y));

        //left side line (top)
        if(shouldFlip) { g2.setColor(Color.cyan); } else { g2.setColor(Color.pink);}
        g2.draw(new Line2D.Float(FieldConstants.GREEN_PAD_X,FieldConstants.GREEN_PAD_Y,
            FieldConstants.FIELD_GREEN_WIDTH-FieldConstants.GREEN_PAD_X,FieldConstants.GREEN_PAD_Y));

        //right side line (bottom)
        if(shouldFlip) { g2.setColor(Color.pink); } else { g2.setColor(Color.cyan);}
        g2.draw(new Line2D.Float(FieldConstants.GREEN_PAD_X,
            FieldConstants.FIELD_GREEN_HEIGHT-FieldConstants.GREEN_PAD_Y,
            FieldConstants.FIELD_GREEN_WIDTH-FieldConstants.GREEN_PAD_X,
            FieldConstants.FIELD_GREEN_HEIGHT-FieldConstants.GREEN_PAD_Y));

        //left penalty mark
        g2.setColor(Color.white);
        g2.fillRect(FieldConstants.LEFT_PENALTY_MARK_X, fieldDimension.height/2, 
            FieldConstants.PENALTY_MARK_WIDTH, FieldConstants.PENALTY_MARK_HEIGHT);
        g2.fillRect(FieldConstants.LEFT_PENALTY_MARK_X-FieldConstants.PENALTY_MARK_WIDTH, 
                fieldDimension.height/2+FieldConstants.PENALTY_MARK_WIDTH, 
        		FieldConstants.PENALTY_MARK_HEIGHT, FieldConstants.PENALTY_MARK_WIDTH);
        //right penalty mark
        g2.fillRect(FieldConstants.RIGHT_PENALTY_MARK_X, fieldDimension.height/2, 
            FieldConstants.PENALTY_MARK_WIDTH, FieldConstants.PENALTY_MARK_HEIGHT);
        g2.fillRect(FieldConstants.RIGHT_PENALTY_MARK_X-FieldConstants.PENALTY_MARK_WIDTH, 
                fieldDimension.height/2+FieldConstants.PENALTY_MARK_WIDTH, 
        		FieldConstants.PENALTY_MARK_HEIGHT, FieldConstants.PENALTY_MARK_WIDTH);        
    }
}
