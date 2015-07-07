package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
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
import javax.swing.JPanel;
import java.awt.GridLayout;

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

public class DebugImageView extends ViewParent
	implements IOFirstResponder, ActionListener {

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
	static final int ORIGINAL = 10;

	static final int DEFAULT_WIDTH = 320;
	static final int DEFAULT_HEIGHT = 240;

	// Images that we can view in this view using the combo box
	String[] imageViews = { "Original", "Green", "Orange", "White", "Edge",
							"Ball" };
	JComboBox viewList;
	JPanel checkBoxPanel;
	JCheckBox showCameraHorizon;
	JCheckBox showFieldHorizon;
	JCheckBox debugHorizon;
	JCheckBox debugFieldEdge;
	CheckBoxListener checkListener = null;

	static final int NUMBER_OF_PARAMS = 4; // update as new params are added
	int displayParams[] = new int[NUMBER_OF_PARAMS];

	// Dimensions of the image that we are working with
    int width;
    int height;

	// Dimensions as we want to display them
    int displayw;
    int displayh;

    BufferedImage originalImage;            // what the robot saw
    DebugImage debugImage;                  // drawing overlay
	BufferedImage debugImageDisplay;        // overlay + original
	BufferedImage displayImages[] = new BufferedImage[ORIGINAL+1]; // our images

	Log currentLog;

	static int currentBottom;  // track current selection
	boolean firstLoad;

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

        displayw = width*2;
        displayh = height*2;

        displayImages[ORIGINAL] = Utility.biFromLog(newlog);
		currentLog = newlog;
    }

    /* Our parameters have been adjusted. Get their values, make an expression
	 * and ship it off to Vision.
    */
    public void adjustParams() {

        // Don't make an extra initial call
        if (firstLoad) {
			System.out.println("Skipping parameter adjustments");
            return;
		}
        //zeroParam();

        SExpr newParams = SExpr.newList(SExpr.newKeyValue("CameraHorizon", displayParams[0]),
										SExpr.newKeyValue("FieldHorizon", displayParams[1]),
										SExpr.newKeyValue("DebugHorizon", displayParams[2]),
										SExpr.newKeyValue("DebugField", displayParams[3]));

        // Look for existing Params atom in current this.log description
        SExpr oldParams = currentLog.tree().find("DebugDrawing");

        // Add params or replace params
        if (oldParams.exists()) {
            oldParams.setList( SExpr.atom("DebugDrawing"), newParams);
        } else {
            this.log.tree().append(SExpr.pair("DebugDrawing", newParams));
        }

        rerunLog();
		repaint();
    }


    public void paintComponent(Graphics g) {
		final int BOX_HEIGHT = 25;
        if (debugImage != null) {
            g.drawImage(debugImageDisplay, 0, 0, displayw, displayh, null);
			g.drawImage(displayImages[currentBottom], 0, displayh + 5, displayw,
						displayh, null);
			viewList.setBounds(0, displayh * 2 + 10, displayw / 2, BOX_HEIGHT);
			checkBoxPanel.setBounds(displayw + 10, 0, displayw, displayh);
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

    public DebugImageView() {
        super();
        setLayout(null);
		// set up combo box to select views
		viewList = new JComboBox(imageViews);
		viewList.setSelectedIndex(0);
		viewList.addActionListener(this);

		// set up check boxes
		checkListener = new CheckBoxListener();
		showCameraHorizon = new JCheckBox("Show camera horizon");
		showFieldHorizon = new JCheckBox("Show field convex hull");
		debugHorizon = new JCheckBox("Debug Field Horizon");
		debugFieldEdge = new JCheckBox("Debug Field Edge");

		// add their listeners
		showCameraHorizon.addItemListener(checkListener);
		showFieldHorizon.addItemListener(checkListener);
		debugHorizon.addItemListener(checkListener);
		debugFieldEdge.addItemListener(checkListener);

		// put them into one panel
		checkBoxPanel = new JPanel();
		checkBoxPanel.setLayout(new GridLayout(0, 1)); // 0 rows, 1 column
		checkBoxPanel.add(showCameraHorizon);
		checkBoxPanel.add(showFieldHorizon);
		checkBoxPanel.add(debugHorizon);
		checkBoxPanel.add(debugFieldEdge);

		// default all checkboxes to false
		showCameraHorizon.setSelected(false);
		showFieldHorizon.setSelected(false);
		debugHorizon.setSelected(false);
		debugFieldEdge.setSelected(false);

        this.addMouseListener(new DistanceGetter());

		// default image to display - save across instances
		if (currentBottom == 0) {
			for (int i = 0; i < NUMBER_OF_PARAMS; i++) {
				displayParams[i] = 0;
			}

			firstLoad = true;
			currentBottom = ORIGINAL;
		} else {
			System.out.println("Reloading");
			}
		add(checkBoxPanel);
		add(viewList);
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

	class CheckBoxListener implements ItemListener {
		public void itemStateChanged(ItemEvent e) {
			int index = 0;
			Object source = e.getSource();
			if (source == showCameraHorizon) {
				index = 0;
			} else if (source == showFieldHorizon) {
				index = 1;
			} else if (source == debugHorizon) {
				index = 2;
			} else if (source == debugFieldEdge) {
				index = 3;
			}
			// flip the value of the parameter checked
			if (displayParams[index] == 0) {
				displayParams[index] = 1;
			} else {
				displayParams[index] = 0;
			}
		adjustParams();
		}
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

		firstLoad = false;

        repaint();

    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }
}
