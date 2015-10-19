package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import java.awt.image.BufferedImage;
import nbtool.images.Y8image;
import nbtool.images.Y8ThreshImage;

import nbtool.data.Log;
import nbtool.data.SExpr;

import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;

import nbtool.util.Utility;
import java.util.Vector;

public class RobotView extends ViewParent
     implements IOFirstResponder {

    Log in;
    BufferedImage original;
    BufferedImage whiteImage;

    int width;
    int height;

    // JComboBox displayList;

    public RobotView() {
        super();
        setLayout(null);
    }

    public void paintComponent(Graphics g) {
        // super.paintComponent(g);

        if (original != null) {
            // int wd = original.getWidth()/2;
            // int ht = original.getHeight()/2;
            g.drawImage(original, 0, 0, width, height, null);

            // if (white != null) {
            //     g.drawImage(white, wd+10, 0, wd, ht, null);
            // }
        }

        if (whiteImage != null) {
            System.out.println("Am i trying? idk!");
            g.drawImage(whiteImage, width+10, 0, width, height, null);
        }

    }

    @Override
    public void setLog(Log newlog) {
        Vector<SExpr> vec = newlog.tree().recursiveFind("width");
        if (vec.size() > 0) {
            SExpr w = vec.get(vec.size()-1);
            width =  w.get(1).valueAsInt() / 2;
        } else {
            System.out.printf("COULD NOT READ WIDTH FROM LOG DESC\n");
            width = 320;
        }

        vec = newlog.tree().recursiveFind("height");
        if (vec.size() > 0) {
            SExpr h = vec.get(vec.size()-1);
            height = h.get(1).valueAsInt() / 2;
        } else {
            System.out.printf("COULD NOT READ HEIGHT FROM LOG DESC\n");
            height = 240;
        }

        CrossInstance ci = CrossIO.instanceByIndex(0);
        if (ci == null) { return; }

        CrossFunc func = ci.functionWithName("Vision");
        assert(func != null);

        CrossCall cc = new CrossCall(this, func, newlog);
        assert(ci.tryAddCall(cc));

        in = newlog;
        this.original = Utility.biFromLog(newlog);
        // repaint();
    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
        // Y8image white8 = new Y8image(640, 480, out[1].bytes);
        // white = new BufferedImage(640, 480, BufferedImage.TYPE_3BYTE_BGR);

        // Graphics2D g = white.createGraphics();
        // g.drawImage(white8.toBufferedImage(), 0, 0, null);

        // SExpr otree = out[1].tree();
        // Y8image o = new Y8image(otree.find("width").get(1).valueAsInt(),
        //                         otree.find("height").get(1).valueAsInt(),
        //                         out[1].bytes);
        // white = new BufferedImage(o.width, o.height, BufferedImage.TYPE_3BYTE_BGR);
        // Graphics2D g = white.createGraphics();
        // g.drawImage(o.toBufferedImage(), 0, 0, null);

        // repaint();

        // SExpr otree = out[3].tree();
        // Y8image o = new Y8image(otree.find("width").get(1).valueAsInt(),
        //                         otree.find("height").get(1).valueAsInt(),
        //                         out[3].bytes);
        // white = new BufferedImage(o.width, o.height, BufferedImage.TYPE_3BYTE_BGR);
        // Graphics2D g = white.createGraphics();
        // g.drawImage(o.toBufferedImage(), 480, 640, null);

        if (out.length > 1) {
            System.out.println("Making this image");
            Y8image white8 = new Y8image(width, height, out[1].bytes);
            this.whiteImage = white8.toBufferedImage();
        }
        repaint();

    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }
}
