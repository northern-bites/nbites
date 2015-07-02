package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.util.Vector;
import java.util.ArrayList;
import java.util.List;

import nbtool.util.Logger;
import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.DebugImage;
import nbtool.images.Y8image;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.util.Utility;

public class DebugImageView extends ViewParent implements IOFirstResponder {

    int width;
    int height;

    int displayw;
    int displayh;

    final int fieldw = 640;
    final int fieldh = 554;

    final int buffer = 5;

    double resize = 1;

    boolean click = false;
    boolean drag = false;

    // Click and release values
    int clickX1 = 0;
    int clickY1 = 0;
    int clickX2 = 0;
    int clickY2 = 0;

    // Field coordinate image upper left hand corder
    int fx0;
    int fy0;

    // Center of field cordinate system
    int fxc = displayw + buffer + fieldw/2;
    int fyc = fieldh;

    BufferedImage originalImage;
    BufferedImage debugImage;
	BufferedImage greenImage;

    @Override
    public void setLog(Log newlog) {
        CrossInstance ci = CrossIO.instanceByIndex(0);
        if (ci == null)
            return;
        CrossFunc func = ci.functionWithName("Vision");
        assert(func != null);

        CrossCall cc = new CrossCall(this, func, newlog);

        assert(ci.tryAddCall(cc));

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

        displayw = width*2;
        displayh = height*2;

        fx0 = displayw + buffer;
        fy0 = 0;

        fxc = displayw + buffer + fieldw/2;
        fyc = fieldh;

        originalImage = Utility.biFromLog(newlog);
    }

    public void paintComponent(Graphics g) {
        if (debugImage != null) {
            g.drawImage(originalImage, 0, 0, displayw, displayh, null);
            g.drawImage(debugImage, 0, displayh + buffer, displayw, displayh, null);
			g.drawImage(greenImage, displayw + 10, 0, displayw, displayh, null);
        }
    }

    public DebugImageView() {
        super();
        setLayout(null);
        this.addMouseListener(new DistanceGetter());
    }

    class DistanceGetter implements MouseListener {

      public void mouseClicked(MouseEvent e) {
        repaint();
      }

      public void mousePressed(MouseEvent e) {
      }

      public void mouseReleased(MouseEvent e) {
		  repaint();
      }

      public void mouseEntered(MouseEvent e) {}

      public void mouseExited(MouseEvent e) {}
    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
		System.out.println("IO received in Debug");
        DebugImage ei = new DebugImage(width, height, out[9].bytes, originalImage);
        debugImage = ei.toBufferedImage();

		if (out.length > 2) {
            Y8image green8 = new Y8image(width, height, out[2].bytes);
            this.greenImage = green8.toBufferedImage();
        }


        repaint();

    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }
}
