package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.util.Vector;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JComboBox;

import nbtool.util.Logger;
import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.DebugImage;
import nbtool.images.Y8image;
import nbtool.images.EdgeImage;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.util.Utility;

public class DebugImageView extends ViewParent implements IOFirstResponder, ActionListener {

	static final int YIMAGE = 0;
	static final int WHITE_IMAGE = 1;
	static final int GREEN_IMAGE = 2;
	static final int ORANGE_IMAGE = 3;
	static final int SEGMENTED = 4;
	static final int EDGE_IMAGE = 5;
	static final int LINE_IMAGE = 6;
	static final int BALL_IMAGE = 7;
	static final int CENTER_CIRCLE = 8;
	static final int DRAWING = 9;
	static final int ORIGINAL = 10;

	String[] imageViews = { "Original", "Green", "Orange", "White", "Edge", "Ball" };
	JComboBox viewList;

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
    DebugImage debugImage;
	BufferedImage debugImageDisplay;
	BufferedImage displayImages[] = new BufferedImage[ORIGINAL+1];

	static int currentTop, currentBottom;

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

        displayImages[ORIGINAL] = Utility.biFromLog(newlog);
    }

    public void paintComponent(Graphics g) {
        if (debugImage != null) {
            g.drawImage(debugImageDisplay, 0, 0, displayw, displayh, null);
			g.drawImage(displayImages[currentBottom], 0, displayh + 5, displayw,
						displayh, null);
			viewList.setBounds(0, displayh * 2 + 10, displayw / 2, 25);
			//currentBottom = GREEN_IMAGE;
        }
    }

    public DebugImageView() {
        super();
        setLayout(null);
		viewList = new JComboBox(imageViews);
		viewList.setSelectedIndex(0);
		viewList.addActionListener(this);
        this.addMouseListener(new DistanceGetter());
		currentBottom = ORIGINAL;
		currentTop = ORIGINAL;
		add(viewList);
    }

	public void actionPerformed(ActionEvent e) {
		JComboBox cb = (JComboBox)e.getSource();
		String viewName = (String)cb.getSelectedItem();
		updateView(viewName);
	}

	public void updateView(String viewName) {
		System.out.println("Selected "+viewName);
		if (viewName == "Green") {
			currentBottom = GREEN_IMAGE;
		} else if (viewName == "White") {
			currentBottom = WHITE_IMAGE;
		} else if (viewName == "Orange") {
			currentBottom = ORANGE_IMAGE;
		} else if (viewName == "Edge") {
			currentBottom = EDGE_IMAGE;
		} else if (viewName == "Ball") {
			currentBottom = BALL_IMAGE;
		} else if (viewName == "Original") {
			currentBottom = ORIGINAL;
		} else {
			currentBottom = ORIGINAL;
		}
		repaint();
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
		if (out.length > GREEN_IMAGE) {
            Y8image green8 = new Y8image(width, height, out[GREEN_IMAGE].bytes);
            displayImages[GREEN_IMAGE] = green8.toBufferedImage();
        }

		if (out.length > WHITE_IMAGE) {
            Y8image white8 = new Y8image(width, height, out[WHITE_IMAGE].bytes);
            displayImages[WHITE_IMAGE] = white8.toBufferedImage();
        }

		if (out.length > ORANGE_IMAGE) {
            Y8image orange8 = new Y8image(width, height, out[ORANGE_IMAGE].bytes);
            displayImages[ORANGE_IMAGE] = orange8.toBufferedImage();
        }

		if (out.length > EDGE_IMAGE) {
			EdgeImage ei = new EdgeImage(width, height,  out[EDGE_IMAGE].bytes);
			displayImages[EDGE_IMAGE] = ei.toBufferedImage();
		}

        debugImage = new DebugImage(width, height, out[DRAWING].bytes,
									   displayImages[ORIGINAL]);
        debugImageDisplay = debugImage.toBufferedImage();


        repaint();

    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }
}
