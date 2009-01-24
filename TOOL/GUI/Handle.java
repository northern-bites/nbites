package TOOL.GUI;

import java.awt.*;
import java.awt.geom.Rectangle2D;

public class Handle {
    
    private Point handleLoc;
    private Shape outline;
    private int handleSize = 2;

    public Handle(Point p) {
        handleLoc = p;
    }

    public boolean contains(Point p) {
        return getShape().contains(p);
    }
    
    public void moveTo(Point p) {
        handleLoc.setLocation(p);
    }

    
    public Shape getShape() {
        return new Rectangle2D.Double(handleLoc.getX() - handleSize / 2,
                                     handleLoc.getY() - handleSize / 2,
                                     handleSize,
                                     handleSize);

    }


}