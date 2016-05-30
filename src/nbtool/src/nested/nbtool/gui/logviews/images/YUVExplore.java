package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import javax.swing.event.*;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import java.awt.event.ActionEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.util.Vector;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JComboBox;
import javax.swing.JCheckBox;
import javax.swing.JSlider;
import javax.swing.JPanel;
import java.awt.GridLayout;

import nbtool.util.Logger;
import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.DebugImage;
import nbtool.images.Y8image;
import nbtool.images.EdgeImage;
import nbtool.images.Y8ThreshImage;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.util.Utility;

public class YUVExplore extends ViewParent
	implements IOFirstResponder, ActionListener, ChangeListener, MouseMotionListener {

	// Values according to nbcross/vision_defs.cpp - must be kept in sync
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
	static final int THRESH = 10;
	static final int LEARN = 11;
	static final int Y = 12;
	static final int U = 13;
	static final int V = 14;
	static final int EDGEPLUS = 15;
	static final int ORIGINAL = 16;

	static final int DEFAULT_WIDTH = 320;
	static final int DEFAULT_HEIGHT = 240;

	static final int BUFFER = 5;
	static final int STARTSIZE = 3;
	static final int FIELDW = 640;
	static final int FIELDH = 554;

	// Images that we can view in this view using the combo box
	String[] imageViews = { "Green", "Orange", "White", "Edge",
							"Thresh", "Y", "U", "V", "EdgePlus" };
	JComboBox viewList;

	JSlider edgeThreshold;
	ChangeListener sliderListener;
	static int thresh = 12;

	// Dimensions of the image that we are working with
    int width;
    int height;

	// Dimensions as we want to display them
    int displayw;
    int displayh;

    BufferedImage originalImage;            // what the robot saw
	BufferedImage displayImages[] = new BufferedImage[ORIGINAL+1]; // our images
	Y8ThreshImage greenCheck;
	Y8image green8;

	private String label = null;

	Log currentLog;

	static int currentBottom;  // track current selection
	static boolean firstLoad = true;
	boolean newLogLoaded = true;

    public YUVExplore() {
        super();
        setLayout(null);
		// set up combo box to select views
		viewList = new JComboBox(imageViews);
		viewList.setSelectedIndex(0);
		viewList.addActionListener(this);

		// set up slider
		edgeThreshold = new JSlider(JSlider.HORIZONTAL, 0, 60, thresh);
		edgeThreshold.addChangeListener(this);
		edgeThreshold.setMajorTickSpacing(10);
		edgeThreshold.setMinorTickSpacing(1);
		edgeThreshold.setPaintTicks(true);
		edgeThreshold.setPaintLabels(true);

		add(viewList);
		add(edgeThreshold);
        this.addMouseListener(new DistanceGetter());
		this.addMouseMotionListener(this);

		// default image to display - save across instances
		if (firstLoad) {

			firstLoad = false;
			currentBottom = ORIGINAL;
		} else {
			System.out.println("Reloading");
			newLogLoaded = true;
		}
    }

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
            width = DEFAULT_WIDTH;
        }

        vec = newlog.tree().recursiveFind("height");
        if (vec.size() > 0) {
            SExpr h = vec.get(vec.size()-1);
            height = h.get(1).valueAsInt() / 2;
        } else {
            System.out.printf("COULD NOT READ HEIGHT FROM LOG DESC\n");
            height = DEFAULT_HEIGHT;
        }

        displayw = 640; //width*2;
        displayh = 480; //height*2;

        displayImages[ORIGINAL] = Utility.biFromLog(newlog);
		currentLog = newlog;
    }

    public void paintComponent(Graphics g) {
		final int BOX_HEIGHT = 25;
		super.paintComponent(g);

        if (currentLog != null) {
            g.drawImage(displayImages[ORIGINAL], 0, 0, displayw, displayh, null);
			if (currentBottom == Y || currentBottom == U || currentBottom == V) {
				drawEdge(g);
			} else if (currentBottom == EDGEPLUS) {
				drawEdgePlus(g);
			}
			if (currentBottom < LEARN) {
				g.drawImage(displayImages[currentBottom], 0, displayh + 25, displayw / 2,
							displayh / 2, null);
			} else if (currentBottom == LEARN) {
				findGreen(g);
			} else {
				displayYUV(g);
			}
			viewList.setBounds(displayw / 2 + 10, displayh  + 10, displayw / 2, BOX_HEIGHT);
			if (label != null) {
				g.setColor(Color.BLACK);
				g.drawString(label, 10, displayh + 20);
			}

			edgeThreshold.setBounds(displayw / 2, displayh + 15 + BOX_HEIGHT, 500, BOX_HEIGHT+20);
			edgeThreshold.repaint();
        }
    }


	/* Called when our display conditions have changed, but we still want to
	 * run on the current log.
	 */

	public void rerunLog() {
		System.out.println("Rerunning log");
        CrossInstance ci = CrossIO.instanceByIndex(0);
        if (ci == null)
            return;
        CrossFunc func = ci.functionWithName("Vision");
        assert(func != null);

        CrossCall cc = new CrossCall(this, func, currentLog);

        assert(ci.tryAddCall(cc));
	}

	/* Currently only called by the JComboBox, if we start adding more actions
	 * then we will need to update this accordingly.
	 */
	public void actionPerformed(ActionEvent e) {
		JComboBox cb = (JComboBox)e.getSource();
		String viewName = (String)cb.getSelectedItem();
		updateView(viewName);
	}

	/* Updates the image displayed on the bottom according to the user's
	 * selection.
	 */
	public void updateView(String viewName) {
		if (viewName == "Green") {
			currentBottom = GREEN_IMAGE;
		} else if (viewName == "White") {
			currentBottom = WHITE_IMAGE;
		} else if (viewName == "Orange") {
			currentBottom = ORANGE_IMAGE;
		} else if (viewName == "Edge") {
			currentBottom = EDGE_IMAGE;
		} else if (viewName == "Original") {
			currentBottom = ORIGINAL;
		} else if (viewName == "Thresh") {
			currentBottom = THRESH;
		} else if (viewName == "Learn") {
			currentBottom = LEARN;
		} else if (viewName == "Y") {
			currentBottom = Y;
		} else if (viewName == "U") {
			currentBottom = U;
		} else if (viewName == "V") {
			currentBottom = V;
		} else if (viewName == "EdgePlus") {
			currentBottom = EDGEPLUS;
		} else {
			currentBottom = ORIGINAL;
		}
		repaint();
	}

	public void stateChanged(ChangeEvent e) {
		JSlider source = (JSlider)e.getSource();
		if (!source.getValueIsAdjusting()) {
			thresh = (int)source.getValue();
			System.out.println("New value is "+thresh);
			greenCheck.setThresh(thresh);
			displayImages[THRESH] = greenCheck.toBufferedImage();
			if (currentBottom == THRESH) {
				repaint();
			}
		}
		//parent.adjustParams();
		//parent.repaint();
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
	public void mouseDragged(MouseEvent e) {}

	@Override
	public void mouseMoved(MouseEvent e) {
		if (currentLog == null) {
			return;
		}

		int col = e.getX();
		int row = e.getY();

		if (col < 0 || row < 0 || col >= displayw || row >= displayh) {
			return;
		}

		boolean first = (col & 1) == 0;
		int cbase = (col & ~1);
		int i = (row * displayw * 2) + (cbase * 2);

		int y = currentLog.data()[first ? i : i + 2] & 0xff;
		int u = currentLog.data()[i + 1] & 0xff;
		int v = currentLog.data()[i + 3] & 0xff;
		label = String.format("(%d,%d): y=%d u=%d v=%d", col, row, y, u, v);
		repaint();
	}

	/* Experimental code to see if we can generate green on the fly.
	 * For now it cheats by using the green image. In the future it
	 * should try and figure it out from scratch.
	 */

	public void findGreen(Graphics g) {
		int max = 0;
		int maxY = 0;
		int maxU = 0;
		int maxV = 0;
		for (int col = 0; col < width; col++) {
			for (int row = 0; row < height; row++) {
				int gr = (green8.data[row * width + col]) & 0xFF;
				if (gr > max) {
					boolean first = (col & 1) == 0;
					int cbase = (col & ~1);
					int i = (row * displayw * 2) + (cbase * 2);

					maxY = currentLog.data()[first ? i : i + 2] & 0xff;
					maxU = currentLog.data()[i + 1] & 0xff;
					maxV = currentLog.data()[i + 3] & 0xff;
				}
			}
		}
		for (int col = 0; col < width * 2; col++) {
			for (int row = 0; row < height * 2; row++) {
				boolean first = (col & 1) == 0;
				int cbase = (col & ~1);
				int i = (row * width * 2 * 2) + (cbase * 2);

				int y = currentLog.data()[first ? i : i + 2] & 0xff;
				int u = currentLog.data()[i + 1] & 0xff;
				int v = currentLog.data()[i + 3] & 0xff;
				if (Math.abs(y - maxY) < 15 && Math.abs(u - maxU) < 10 &&
					Math.abs(v - maxV) < 10) {
					g.setColor(Color.GREEN);
				} else {
					g.setColor(Color.BLACK);
				}
				g.fillRect(col/2, row/2+displayh+30, 1, 1);
			}
		}
	}

	/* Draws edges on the original image in the Y, U, or V channel.
	 */

	public void drawEdge(Graphics g) {
		g.setColor(Color.MAGENTA);
		for (int col = 2; col < width * 2 - 4; col++) {
			for (int row = 2; row < height * 2 - 4; row++) {
				boolean first = (col & 1) == 0;
				int cbase = (col & ~1);
				int i = (row * width * 2 * 2) + (cbase * 2);

				int y = currentLog.data()[first ? i : i + 2] & 0xff;
				int u = currentLog.data()[i + 1] & 0xff;
				int v = currentLog.data()[i + 3] & 0xff;
				int color = y;
				if (currentBottom == U) {
					color = u;
				} else if (currentBottom == V) {
					color = v;
				}
				for (int k = -2; k < 3; k++) {
					for (int j = -2; j < 3; j++) {
						if (!(k == 0 && j == 0)) {
							cbase = ((col+j) & ~1);
							i = ((row+k)*width*2*2) + (cbase*2);
							int y1 = currentLog.data()[first ? i : i + 2] & 0xff;
							int u1 = currentLog.data()[i + 1] & 0xff;
							int v1 = currentLog.data()[i + 3] & 0xff;
							int color1 = y1;
							if (currentBottom == U) {
								color1 = u1;
							} else if (currentBottom == V) {
								color1 = v1;
							}
							if (Math.abs(color1 - color) > thresh) {
								g.fillRect(col, row, 1, 1);
							}
						}
					}
				}
			}
		}
	}

	/* Right now this is a crude version of an edge image that takes
	 * Y, U and V into account simultaneously. Just looks for jumps
	 * in any dimension in the neighborhood of each pixel. Doesn't
	 * figure the gradient or anything like that. Like I said, crude.
	 */

	public void drawEdgePlus(Graphics g) {
		g.setColor(Color.BLUE);
		for (int col = 2; col < width * 2 - 4; col++) {
			for (int row = 2; row < height * 2 - 4; row++) {
				boolean first = (col & 1) == 0;
				int cbase = (col & ~1);
				int i = (row * width * 2 * 2) + (cbase * 2);

				int y = currentLog.data()[first ? i : i + 2] & 0xff;
				int u = currentLog.data()[i + 1] & 0xff;
				int v = currentLog.data()[i + 3] & 0xff;
				for (int k = 0; k < 3; k++) {
					for (int j = 0; j < 3; j++) {
						if (!(k == 0 && j == 0)) {
							cbase = ((col+j) & ~1);
							i = ((row+k)*width*2*2) + (cbase*2);
							int y1 = currentLog.data()[first ? i : i + 2] & 0xff;
							int u1 = currentLog.data()[i + 1] & 0xff;
							int v1 = currentLog.data()[i + 3] & 0xff;
							if (Math.abs(y - y1) > thresh ||
								Math.abs(u - u1) > thresh ||
								Math.abs(v - v1) > thresh) {
								g.fillRect(col, row, 1, 1);
							}
						}
					}
				}
			}
		}
	}

	/* Draw a Y, U, or V image depending on what is selected.
	 * @param g     the current drawing context
	 */

	public void displayYUV(Graphics g) {
		// loop through the original image grabbing the YUV
		for (int col = 0; col < width * 2; col++) {
			for (int row = 0; row < height * 2; row++) {
				// magic courtesy of Phil 
				boolean first = (col & 1) == 0;
				int cbase = (col & ~1);
				int i = (row * width * 2 * 2) + (cbase * 2);

				int y = currentLog.data()[first ? i : i + 2] & 0xff;
				int u = currentLog.data()[i + 1] & 0xff;
				int v = currentLog.data()[i + 3] & 0xff;
				int color = y;
				if (currentBottom == U) {
					color = u;
				} else if (currentBottom == V) {
					color = v;
				}
				Color display = new Color(color, color, color);
				g.setColor(display);
				g.fillRect(col/2, row/2+displayh+30, 1, 1);
			}
		}
	}

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
		System.out.println("IO received in Debug");
		//yuv = out[0].bytes;
		if (out.length > GREEN_IMAGE) {
            green8 = new Y8image(width, height, out[GREEN_IMAGE].bytes);
            displayImages[GREEN_IMAGE] = green8.toBufferedImage();
			greenCheck = new Y8ThreshImage(width, height, out[GREEN_IMAGE].bytes);
			greenCheck.setThresh(thresh);
			displayImages[THRESH] = greenCheck.toBufferedImage();
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

		if (newLogLoaded) {
			newLogLoaded = false;
		}

        repaint();

    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }
}
