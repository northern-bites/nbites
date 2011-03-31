package TOOL.Image;
import TOOL.WorldController.WorldControllerPainter;
import TOOL.Data.Field;
import java.awt.Graphics;
import java.awt.Graphics2D;

public class WorldControllerViewer extends ImagePanel {

    protected WorldControllerPainter painter;
    protected Field field;

    public WorldControllerViewer(Field field) {
        super();
        this.field = field;
        setImage(field.getFieldImage());
        painter = new WorldControllerPainter(field, this);
    }

    public WorldControllerPainter getPainter() { return painter; }

    public void paint(Graphics g) {
        super.paint(g);
        Graphics2D g2 = (Graphics2D) g;
        // Scale the graphics context to match the scale of the buffered image;
        // otherwise the field graphics will not line up
        g2.scale(xImageScale, yImageScale);
        painter.paintInfoOnField(g2);
    }

    public void setField(Field f) {
        field = f;
        setImage(field.getFieldImage());
        painter.setField(f);
        repaint();
    }
}