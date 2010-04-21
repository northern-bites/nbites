package TOOL.Data;

import java.awt.image.BufferedImage;
import java.awt.Color;
import java.awt.BasicStroke;
import java.awt.Graphics2D;
import java.awt.Dimension;



public class NaoField2009 extends Field {

    public NaoField2009() {}

    public final double getGreenPadX() { return 67.5; }
    public final double getGreenPadY() { return 67.5; }
    public final double getFieldWhiteWidth() { return 605.0; }
    public final double getFieldWhiteHeight() { return 405.0; }
    // Side length = 40, goal post width = 10
    public final double getGoalDepth() { return 50.0; }
    public final double getGoalSideLength() { return 40.0; }

    // Note: These goal width and goal box width values look wrong, as they do
    // not correspond with the pictures we have been provided of the field.
    // However, they are based off of the dimensions of the goal provided in
    // the NaoRules2008

    // The width between the two posts
    public final double getGoalWidth() { return 140.0; }
    public final double getGoalBoxWidth() { return 305.0; }
    public final double getGoalBoxDepth() { return 65.0; }
    public final double getCCRadius() { return 62.5; }
    public final double getGoalPostRadius() { return 5.0; }
    public final double getBallRadius() { return 4.0; }
    public final double getLineThickness() { return 5.0; }
    public final double getGoalCrossbarThickness() { return 2.5; }
}