package nbtool.gui.logviews.images;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.image.BufferedImage;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;

import nbtool.util.Utility;
import nbtool.images.Y8image;

public class BallView extends ViewParent implements IOFirstResponder {
    Log in;
    BufferedImage original;
    BufferedImage orange;
    Log balls;

    @Override
    public void setLog(Log newlog)
    {
        CrossInstance cross = CrossIO.instanceByIndex(0);
        if (cross == null) return;

        CrossFunc func = cross.functionWithName("Vision");
        assert(func != null);

        CrossCall call = new CrossCall(this, func, newlog);
        assert(cross.tryAddCall(call));

        in = newlog;
        original = Utility.biFromLog(newlog);
    }

    @Override
    public void paintComponent(Graphics g) {
        if(orange == null) return;
        g.drawImage(original, 0, 0, original.getWidth(), original.getHeight(), null);
        drawBlobs();
        g.drawImage(orange, 0, original.getHeight(), orange.getWidth(), orange.getHeight(), null);
    }

    public void drawBlobs()
    {
        if (orange == null) return;
        Graphics2D graph = orange.createGraphics();
        graph.setColor(Color.RED);
        String b = "blob";

        for (int i=0; ; i++)
        {
            SExpr tree = balls.tree();
            SExpr bl = tree.find(b+i);
            if (!bl.exists()) { 
                break;
            }

            SExpr blob = bl.get(1);
            drawBlob(graph, blob);
        }


        graph.setColor(Color.GREEN);

        b = "ball";

        for(int i=0; ;i++)
        {
            SExpr tree = balls.tree();
            SExpr ball = tree.find(b+i);
            if (!ball.exists()){
                break;
            }
            SExpr blob = ball.get(1).find("blob").get(1);
            double diam = ball.get(1). find("expectedDiam").get(1).valueAsDouble();
            SExpr loc = blob.find("center").get(1);

            int x = (int) Math.round(loc.get(0).valueAsDouble());
            int y = (int) Math.round(loc.get(1).valueAsDouble());

            graph.draw(new Ellipse2D.Double(x - diam/2, y - diam/2, diam, diam));
        }
    }

    private void drawBlob(Graphics2D g, SExpr blob)
    {
        SExpr loc = blob.find("center").get(1);

        int x = (int) Math.round(loc.get(0).valueAsDouble());
        int y = (int) Math.round(loc.get(1).valueAsDouble());

        double len1 = blob.find("len1").get(1).valueAsDouble();
        double len2 = blob.find("len2").get(1).valueAsDouble();
        double ang1 = blob.find("ang1").get(1).valueAsDouble();
        double ang2 = blob.find("ang2").get(1).valueAsDouble();

        int firstXOff = (int)Math.round(len1 * Math.cos(ang1));
        int firstYOff = (int)Math.round(len1 * Math.sin(ang1));
        int secondXOff = (int)Math.round(len2 * Math.cos(ang2));
        int secondYOff = (int)Math.round(len2 * Math.sin(ang2));

        g.drawLine(x - firstXOff, y - firstYOff,
                       x + firstXOff, y + firstYOff);
        g.drawLine(x - secondXOff, y - secondYOff,
                       x + secondXOff, y + secondYOff);
        Ellipse2D.Double ellipse = new Ellipse2D.Double(x-len1, y-len2, len1*2, len2*2);
        Shape rotated = (AffineTransform.getRotateInstance(ang1, x, y).createTransformedShape(ellipse));
        g.draw(rotated);
    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out)
    {
        SExpr otree = out[3].tree();
        Y8image o = new Y8image(otree.find("width").get(1).valueAsInt(),
                                otree.find("height").get(1).valueAsInt(),
                                out[3].bytes);
        orange = new BufferedImage(o.width, o.height, BufferedImage.TYPE_3BYTE_BGR);
        Graphics2D g = orange.createGraphics();
        g.drawImage(o.toBufferedImage(), 0, 0, null);
        //drawBlobs();
        balls = out[7];
    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }

}