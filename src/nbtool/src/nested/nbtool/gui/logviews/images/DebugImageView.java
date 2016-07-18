package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.Container;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import javax.swing.event.*;
import java.awt.BasicStroke;
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
import javax.swing.JSpinner;
import javax.swing.JLabel;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import java.awt.GridLayout;

import nbtool.data.SExpr;
import nbtool.data.log.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.logviews.misc.VisionView;
import nbtool.images.DebugImage;
import nbtool.images.EdgeImage;
import nbtool.images.Y8Image;
import nbtool.images.Y8ThreshImage;
import nbtool.images.Y16Image;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class DebugImageView extends VisionView implements
												   ActionListener, ChangeListener, MouseMotionListener {

    // Values according to nbcross/vision_defs.cpp - must be kept in sync
    static final int YIMAGE = 0;
    static final int WHITE_IMAGE = 1;
    static final int GREEN_IMAGE = 2;
    static final int BLACK_IMAGE = 3;
    static final int SEGMENTED = 4;
    static final int EDGE_IMAGE = 5;
    static final int LINE_IMAGE = 6;
    static final int BALL_IMAGE = 7;
    static final int CENTER_CIRCLE = 8;
    static final int DRAWING = 9;
    static final int THRESH = 10;
    static final int LEARN = 11;
    static final int ORIGINAL = 12;

    static final int DEFAULT_WIDTH = 320;
    static final int DEFAULT_HEIGHT = 240;

    static final int BUFFER = 5;
    static final int STARTSIZE = 3;
    static final int FIELDW = 640;
    static final int FIELDH = 554;

    /*
     *
     * this.displayedLog points to the log the view was shown with
     * this.latestVisionLog points to the log Vision() most recently returned
     *
     * */

    // Images that we can view in this view using the combo box
    String[] imageViews = { "Original", "Green", "Y", "White", "Edge", "Thresh", "Learn" };
    JComboBox<String> viewList;

    JSlider greenThreshold;
    ChangeListener sliderListener;
    static int thresh = 128;

    static final int NUMBER_OF_PARAMS = 10; // update as new params are added
    static int displayParams[] = new int[NUMBER_OF_PARAMS];
	int filterThresholdDark;
	int greenThresholdDark;
	int filterThresholdBrite;
	int greenThresholdBrite;

    // Dimensions of the image that we are working with
    int width;
    int height;

    // Dimensions as we want to display them
    int displayw;
    int displayh;

    Vector<Double> lines;
    Vector<Double> ccPoints;

    BufferedImage originalImage;            // what the robot saw
    DebugImage debugImage;                  // drawing overlay
    BufferedImage debugImageDisplay;        // overlay + original
    BufferedImage displayImages[] = new BufferedImage[ORIGINAL+1]; // our images
    Y8ThreshImage greenCheck;
    Y8Image green8;
    Y8Image white8;
    Y8Image black8;

    private String label = null;

    static int currentBottom;  // track current selection
    static boolean firstLoad = true;
    boolean newLogLoaded = true;

    boolean parametersNeedSetting = false;
    static PersistantStuff persistant;

    public DebugImageView() {
        super();
        setLayout(null);
		// set up combo box to select views
		viewList = new JComboBox<String>(imageViews);
		viewList.setSelectedIndex(0);
		viewList.addActionListener(this);

		// set up slider
		greenThreshold = new JSlider(JSlider.HORIZONTAL, 128, 220, thresh);
		greenThreshold.addChangeListener(this);
		greenThreshold.setMajorTickSpacing(10);
		greenThreshold.setMinorTickSpacing(1);
		greenThreshold.setPaintTicks(true);
		greenThreshold.setPaintLabels(true);

		add(viewList);
		add(greenThreshold);
        this.addMouseListener(new DistanceGetter());
		this.addMouseMotionListener(this);

		// default image to display - save across instances
		if (firstLoad) {
			for (int i = 0; i < NUMBER_OF_PARAMS; i++) {
				displayParams[i] = 0;
			}
			// ideally these would actually be read from NBCROSS
			filterThresholdDark = 104;
			greenThresholdDark = 12;
			filterThresholdBrite = 130;
			greenThresholdBrite = 80;
			displayParams[6] = filterThresholdDark;
			displayParams[7] = greenThresholdDark;
			displayParams[8] = filterThresholdBrite;
			displayParams[9] = greenThresholdBrite;

			firstLoad = false;
			currentBottom = ORIGINAL;
			persistant = new PersistantStuff(this);
		} else {
			System.out.println("Reloading");
			newLogLoaded = true;
			persistant.setParent(this);
			//adjustParams();
		}
		add(persistant);
    }

    @Override
	protected void setupVisionDisplay() {
    	width = this.originalWidth() / 2;
        height = this.originalHeight() / 2;

        displayw = 640; //width*2;
        displayh = 480; //height*2;

        displayImages[ORIGINAL] = this.getOriginal().toBufferedImage();
	}

    /* Our parameters have been adjusted. Get their values, make an expression
     * and ship it off to Vision.
     */
    public void adjustParams() {

        // Don't make an extra initial call
        if (newLogLoaded) {
			System.out.println("Skipping parameter adjustments");
            return;
		}
        //zeroParam();

        SExpr newParams = SExpr.newList(SExpr.newKeyValue("CameraHorizon", displayParams[0]),
										SExpr.newKeyValue("FieldHorizon", displayParams[1]),
										SExpr.newKeyValue("DebugHorizon", displayParams[2]),
										SExpr.newKeyValue("DebugField", displayParams[3]),
										SExpr.newKeyValue("DebugBall", displayParams[4]),
										SExpr.newKeyValue("ShowSpotSizes", displayParams[5]),
										SExpr.newKeyValue("FilterDark", displayParams[6]),
										SExpr.newKeyValue("GreenDark", displayParams[7]),
										SExpr.newKeyValue("FilterBrite", displayParams[8]),
										SExpr.newKeyValue("GreenBrite", displayParams[9]));


        // Look for existing Params atom in current this.log description
        displayedLog.topLevelDictionary.put("DebugDrawing", SExpr.pair("DebugDrawing", newParams).serialize());

        rerunLog();
        //repaint();
    }


    public void paintComponent(Graphics g) {
		final int BOX_HEIGHT = 25;
		super.paintComponent(g);

        if (debugImage != null) {
            g.drawImage(debugImageDisplay, 0, 0, displayw, displayh, null);
			drawLines(g);
			drawBlobs(g);
			//displayImages[THRESH].setThresh(persistant.greenThreshold);
			if (currentBottom != LEARN) {
				g.drawImage(displayImages[currentBottom], 0, displayh + 25, displayw / 2,
							displayh / 2, null);
			} else {
				findGreen(g);
			}
			viewList.setBounds(displayw / 2 + 10, displayh  + 10, displayw / 2, BOX_HEIGHT);
			if (label != null) {
				g.setColor(Color.BLACK);
				g.drawString(label, 10, displayh + 20);
			}

			greenThreshold.setBounds(displayw / 2, 2 * displayh + 15 + BOX_HEIGHT, 500, BOX_HEIGHT+20);
			greenThreshold.repaint();
			persistant.setBounds(displayw+10, 0, 300, 300);
        }
    }

    /* Taken from LineView.java
     */
    public void drawLines(Graphics g) {
		// This code stolen from LineView.java
		// TODO: obviously this should be moved into its own function
		if (persistant != null && persistant.displayFieldLines) {
			for (int i = 0; i < lines.size(); i += 10) {
				double icR = lines.get(i);
				double icT = lines.get(i + 1);
				double icEP0 = lines.get(i + 2);
				double icEP1 = lines.get(i + 3);
				double houghIndex = lines.get(i + 4);
				double fieldIndex = lines.get(i + 5);
				double fcR = lines.get(i + 6);
				double fcT = lines.get(i + 7);
				double fcEP0 = lines.get(i + 8);
				double fcEP1 = lines.get(i + 9);

				// Draw it in image coordinates
				if (fieldIndex == -1)
					g.setColor(Color.red);
				else
					g.setColor(Color.blue);

				double x0 = 2*icR * Math.cos(icT) + displayImages[ORIGINAL].getWidth() / 2;
				double y0 = -2*icR * Math.sin(icT) + displayImages[ORIGINAL].getHeight() / 2;
				int x1 = (int) Math.round(x0 + 2*icEP0 * Math.sin(icT));
				int y1 = (int) Math.round(y0 + 2*icEP0 * Math.cos(icT));
				int x2 = (int) Math.round(x0 + 2*icEP1 * Math.sin(icT));
				int y2 = (int) Math.round(y0 + 2*icEP1 * Math.cos(icT));

				g.drawLine(x1, y1, x2, y2);

				// Image view line labels
				double xstring = (x1 + x2) / 2;
				double ystring = (y1 + y2) / 2;

				double scale = 0;
				if (icR > 0)
					scale = 10;
				else
					scale = 3;
				xstring += scale*Math.cos(icT);
				ystring += scale*Math.sin(icT);

				g.drawString(Integer.toString((int) houghIndex) + "/" +
							 Integer.toString((int) fieldIndex),
							 (int) xstring,
							 (int) ystring);
			}
		}
    }
    /* Taken directly from BallView.java (where it was undocumented). Draws blobs
     * related to the ball.
     */
    public void drawBlobs(Graphics g)
    {
		int multiplier = 2;
		if (width != DEFAULT_WIDTH) {
			multiplier = 4;
		}
		// if we don't have an black image we're in trouble
		if (displayImages[BLACK_IMAGE] == null) {
			System.out.println("No Y image");
			return;
		}
        //Graphics2D graph = black.createGraphics();
		Graphics2D graph = (Graphics2D)g;
        graph.setColor(Color.YELLOW);
        String b = "blob";

		SExpr tree = this.getBallBlock().parseAsSExpr();
		SExpr whiteTree = this.getWhiteSpotBlock().parseAsSExpr();
		SExpr blackTree = this.getBlackSpotBlock().parseAsSExpr();

		// loop through all of the white blobs we find in the tree
		b = "whiteSpot";
		graph.setColor(Color.BLUE);
		graph.setStroke(new BasicStroke(2.0F));
		for (int i= 0; ; i++)
			{
				SExpr bl = whiteTree.find(b+i);
				if (!bl.exists()) {
					break;
				}
				SExpr blob = bl.get(1);
				if (persistant != null && persistant.drawAllBalls) {
					drawBlob(graph, blob, multiplier);
				}
			}

		// loop through all of the black spots we find in the tree
		b = "darkSpot";
		graph.setColor(Color.YELLOW);
		for (int i= 0; ; i++)
			{
				SExpr bl = blackTree.find(b+i);
				if (!bl.exists()) {
					break;
				}
				SExpr blob = bl.get(1);
				if (persistant != null && persistant.drawAllBalls) {
					drawBlob(graph, blob, multiplier);
				}
			}

        graph.setColor(Color.BLACK);

        b = "ball";

        for(int i=0; ;i++) {
			SExpr ball = tree.find(b+i);
			if (!ball.exists()){
				break;
			}
			SExpr blob = ball.get(1).find("blob").get(1);
			double diam = ball.get(1). find("expectedDiam").get(1).valueAsDouble();
			SExpr loc = blob.find("center").get(1);

			int x = (int) Math.round(loc.get(0).valueAsDouble());
			int y = (int) Math.round(loc.get(1).valueAsDouble());
			graph.draw(new Ellipse2D.Double((x - diam/2) * multiplier,
											(y - diam/2)* multiplier,
											diam*multiplier, diam*multiplier));
		}
    }

    private void drawBlob(Graphics2D g, SExpr blob, int multiplier)
    {
		final int WHITE_CANDIDATE = 1;
		final int WHITE_REJECT = 2;
		final int DARK_CANDIDATE = 3;
		final int DARK_REJECT = 4;
		final int WHITE_BLOB = 5;
		final int WHITE_BLOB_BAD = 6;

        SExpr loc = blob.find("center").get(1);

        int x = (int) Math.round(loc.get(0).valueAsDouble());
        int y = (int) Math.round(loc.get(1).valueAsDouble());

        double len1 = blob.find("inner").get(1).valueAsDouble() / 2.0;
        double len2 = len1;
		int spotType = blob.find("spottype").get(1).valueAsInt();
        double ang1 = 0.0;
		switch (spotType) {
		case WHITE_CANDIDATE: g.setColor(Color.RED);
			break;
		case WHITE_REJECT: g.setColor(Color.WHITE);
			break;
		case DARK_CANDIDATE: g.setColor(Color.ORANGE);
			break;
		case DARK_REJECT: g.setColor(Color.BLUE);
			break;
		case WHITE_BLOB: g.setColor(Color.MAGENTA);
			break;
		case WHITE_BLOB_BAD: g.setColor(Color.WHITE);
			break;
		}

        Ellipse2D.Double ellipse = new Ellipse2D.Double((x-len1)*multiplier, (y-len2)*multiplier,
														len1*2*multiplier, len2*2*multiplier);
        Shape rotated = (AffineTransform.getRotateInstance(ang1, x*multiplier, y*multiplier).
						 createTransformedShape(ellipse));
        g.draw(rotated);
    }


    /* Called when our display conditions have changed, but we still want to
     * run on the current log.
     */

    public void rerunLog() {
    	System.out.println("Rerunning log");
        this.callVision();
    }

    /* Currently only called by the JComboBox, if we start adding more actions
     * then we will need to update this accordingly.
     */
    public void actionPerformed(ActionEvent e) {
		JComboBox<String> cb = (JComboBox<String>) e.getSource();
		String viewName = (String) cb.getSelectedItem();
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
		} else if (viewName == "Y") {
			currentBottom = BLACK_IMAGE;
		} else if (viewName == "Edge") {
			currentBottom = EDGE_IMAGE;
		} else if (viewName == "Original") {
			currentBottom = ORIGINAL;
		} else if (viewName == "Thresh") {
			currentBottom = THRESH;
		} else if (viewName == "Learn") {
			currentBottom = LEARN;
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
		if (displayedLog == null) {
			return;
		}

		int col = e.getX();
		int row = e.getY();
		byte[] data = originalImageBytes();

		if (col < 0 || row < 0 || col >= displayw || row >= displayh) {
			return;
		}

		if (width != DEFAULT_WIDTH) {
			col = col/2;
			row = row/2;
			boolean first = (col & 1) == 0;
			int cbase = (col & ~1);
			int i = (row * 160) + (cbase * 2);

			int y = data[first ? i : i + 2] & 0xff;
			int u = data[i + 1] & 0xff;
			int v = data[i + 3] & 0xff;
			label = String.format("(%d,%d): y=%d u=%d v=%d", col/2, row/2, y, u, v);
		} else {
			boolean first = (col & 1) == 0;
			int cbase = (col & ~1);
			int i = (row * displayw * 2) + (cbase * 2);

			int y = data[first ? i : i + 2] & 0xff;
			int u = data[i + 1] & 0xff;
			int v = data[i + 3] & 0xff;
			label = String.format("(%d,%d): y=%d u=%d v=%d", col/2, row/2, y, u, v);
		}
		repaint();
    }


    class PersistantStuff extends JPanel
		implements ItemListener, ChangeListener {
		JPanel checkBoxPanel;
		JPanel paramPanel;
		JCheckBox showCameraHorizon;
		JCheckBox showFieldHorizon;
		JCheckBox debugHorizon;
		JCheckBox debugFieldEdge;
		JCheckBox debugBall;
		JCheckBox showFieldLines;
		JCheckBox showSpotSizes;
		boolean displayFieldLines;
		boolean drawAllBalls;
		DebugImageView parent;
		JSpinner filterDark;
		JSpinner greenDark;
		JSpinner filterBrite;
		JSpinner greenBrite;

		PersistantStuff(DebugImageView p) {
			parent = p;
			// set up check boxes
			showCameraHorizon = new JCheckBox("Show camera horizon");
			showFieldHorizon = new JCheckBox("Show field convex hull");
			debugHorizon = new JCheckBox("Debug Field Horizon");
			debugFieldEdge = new JCheckBox("Debug Field Edge");
			debugBall = new JCheckBox("Debug Ball");
			showFieldLines = new JCheckBox("Hide Field Lines");
			showSpotSizes = new JCheckBox("Show Spot Sizes");

			// add their listeners
			showCameraHorizon.addItemListener(this);
			showFieldHorizon.addItemListener(this);
			debugHorizon.addItemListener(this);
			debugFieldEdge.addItemListener(this);
			debugBall.addItemListener(this);
			showFieldLines.addItemListener(this);
			showSpotSizes.addItemListener(this);

			// put them into one panel
			checkBoxPanel = new JPanel();
			checkBoxPanel.setLayout(new GridLayout(0, 1)); // 0 rows, 1 column
			checkBoxPanel.add(showCameraHorizon);
			checkBoxPanel.add(showFieldHorizon);
			checkBoxPanel.add(debugHorizon);
			checkBoxPanel.add(debugFieldEdge);
			checkBoxPanel.add(debugBall);
			checkBoxPanel.add(showFieldLines);
			checkBoxPanel.add(showSpotSizes);

			// default all checkboxes to false
			showCameraHorizon.setSelected(false);
			showFieldHorizon.setSelected(false);
			debugHorizon.setSelected(false);
			debugFieldEdge.setSelected(false);
			debugBall.setSelected(false);
			showFieldLines.setSelected(false);
			showSpotSizes.setSelected(false);

			SpinnerModel filterDarkModel = new
				SpinnerNumberModel(parent.displayParams[6], 0, 512, 4);
			SpinnerModel greenDarkModel = new
				SpinnerNumberModel(parent.displayParams[7], 0, 255, 4);
			SpinnerModel filterBriteModel = new
				SpinnerNumberModel(parent.displayParams[8], 0, 512, 4);
			SpinnerModel greenBriteModel = new
				SpinnerNumberModel(parent.displayParams[9], 0, 255, 4);
			paramPanel = new JPanel();
			paramPanel.setLayout(new GridLayout(0, 2));
			filterDark = addLabeledSpinner(paramPanel, "filterThresholdDark",
										   filterDarkModel);
			greenDark = addLabeledSpinner(paramPanel, "greenThresholdDark",
										  greenDarkModel);
			filterBrite = addLabeledSpinner(paramPanel, "filterThresholdBrite",
											filterBriteModel);
			greenBrite = addLabeledSpinner(paramPanel, "greenThresholdBrite",
										   greenBriteModel);
			greenBrite.addChangeListener(this);
			filterBrite.addChangeListener(this);
			greenDark.addChangeListener(this);
			filterDark.addChangeListener(this);


			add(checkBoxPanel);
			add(paramPanel);
			setSize(400, 500);
		}

		protected JSpinner addLabeledSpinner(Container c, String label,
													SpinnerModel model) {
			JLabel l = new JLabel(label);
			c.add(l);
			JSpinner spinner = new JSpinner(model);
			l.setLabelFor(spinner);
			c.add(spinner);
			return spinner;
		}

		public void stateChanged(ChangeEvent e) {
			parent.displayParams[6] = ((Integer)filterDark.getValue()).intValue();
			parent.displayParams[7] = ((Integer)greenDark.getValue()).intValue();
			parent.displayParams[8] = ((Integer)filterBrite.getValue()).intValue();
			parent.displayParams[9] = ((Integer)greenBrite.getValue()).intValue();
			parent.adjustParams();
			parent.repaint();
		}

		public void setParent(DebugImageView p) {
			parent = p;
		}

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
			} else if (source == debugBall) {
				index = 4;
				drawAllBalls = !drawAllBalls;
			} else if (source == showFieldLines) {
				index = -1;
				displayFieldLines = !displayFieldLines;
			} else if (source == showSpotSizes) {
				index = 5;
			}
			// flip the value of the parameter checked
			if (index >= 0) {
				if (parent.displayParams[index] == 0) {
					parent.displayParams[index] = 1;
				} else {
					parent.displayParams[index] = 0;
				}
			}
			parent.adjustParams();
			parent.repaint();
		}


    }

    public void findGreen(Graphics g) {
		int max = 0;
		int maxY = 0;
		int maxU = 0;
		int maxV = 0;

		for (int col = 0; col < width; col++) {
			for (int row = 0; row < height; row++) {
				int gr = (green8.data[row * width + col]) & 0xFF;
				int wh = (white8.data[row * width + col]) & 0xFF;

				if (gr < 100 && wh < 100) {
					g.setColor(Color.GRAY);
				} else if (gr > wh) {
					g.setColor(Color.GREEN);
				} else {
					g.setColor(Color.WHITE);
				}
				g.fillRect(col, row+displayh+30, 1, 1);
			}
		}
    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
		System.out.println("IO received in Debug");
		//yuv = out[0].bytes;
		if (this.getGreenBlock() != null) {
			green8 = new Y8Image(width, height, this.getGreenBlock().data);
			displayImages[GREEN_IMAGE] = green8.toBufferedImage();
			greenCheck = new Y8ThreshImage(width, height, this.getGreenBlock().data);
			greenCheck.setThresh(thresh);
			displayImages[THRESH] = greenCheck.toBufferedImage();
		}

		if (this.getWhiteBlock() != null) {
			System.out.println("here");
			white8 = new Y8Image(width, height, this.getWhiteBlock().data);
			displayImages[WHITE_IMAGE] = white8.toBufferedImage();
		}

		if (this.getYBlock() != null) {
            Y16Image yImg = new Y16Image(width, height, this.getYBlock().data);
            displayImages[BLACK_IMAGE] = yImg.toBufferedImage();
		}

		if (this.getEdgeBlock() != null) {
			EdgeImage ei = new EdgeImage(width, height,  this.getEdgeBlock().data);
			displayImages[EDGE_IMAGE] = ei.toBufferedImage();
		}

		lines = new Vector<Double>();
		byte[] lineBytes = this.getLineBlock().data;
		int numLines = lineBytes.length / (9 * 8);
		try {
			DataInputStream dis = new DataInputStream(new ByteArrayInputStream(lineBytes));
			for (int i = 0; i < numLines; ++i) {
				lines.add(dis.readDouble()); // image coord r
				lines.add(dis.readDouble()); // image coord t
				lines.add(dis.readDouble()); // image coord ep0
				lines.add(dis.readDouble()); // image coord ep1
				lines.add((double)dis.readInt()); // hough index
				lines.add((double)dis.readInt()); // fieldline index
				lines.add(dis.readDouble()); // field coord r
				lines.add(dis.readDouble()); // field coord t
				lines.add(dis.readDouble()); // field coord ep0
				lines.add(dis.readDouble()); // field coord ep1
			}
		} catch (Exception e) {
			Debug.error("Conversion to hough coord lines failed.");
			e.printStackTrace();
		}

		ccPoints = new Vector<Double>();
		byte[] pointBytes = this.getCCDBlock().data;
		int numPoints = pointBytes.length / (2 * 8);

		try {
			DataInputStream dis = new DataInputStream(new ByteArrayInputStream(pointBytes));
			for (int i = 0; i < numPoints; i ++) {
				ccPoints.add(dis.readDouble()); // X coordinate
				ccPoints.add(dis.readDouble()); // Y coodinrate
			}
		} catch (Exception e) {
			Debug.error("Conversion from bytes to center failed.");
			e.printStackTrace();
		}

		//	SExpr otree = out[BLACK_IMAGE].tree();
		//        Y8image o = new Y8image(otree.find("width").get(1).valueAsInt(),
		//                                otree.find("height").get(1).valueAsInt(),
		//                                out[BLACK_IMAGE].bytes);
		//        balls = out[BALL_IMAGE];


		debugImage = new DebugImage(width, height, this.getDebugImageBlock().data,
									displayImages[ORIGINAL]);
		debugImageDisplay = debugImage.toBufferedImage();

		if (newLogLoaded) {
			newLogLoaded = false;
			adjustParams();
		}

		repaint();

    }
}
