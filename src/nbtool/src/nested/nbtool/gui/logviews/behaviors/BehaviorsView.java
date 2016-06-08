package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
// import java.awt.image.BufferedImage;
// import java.io.ByteArrayInputStream;
// import java.io.DataInputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.gui.logviews.misc.ViewParent;
// import nbtool.images.EdgeImage;
import nbtool.io.CommonIO.IOInstance;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;

public class BehaviorsView extends ViewParent {
	
	private static final Debug.DebugSettings debug = Debug.createSettings(true, true, true, null, null);

	@Override
	public void setupDisplay() {
		// width = this.originalWidth();
  //       height = this.originalHeight();

  //       displayw = width;
  //       displayh = height;

        // fx0 = displayw + buffer;
        // fy0 = 0;

        // fxc = displayw + buffer + fieldw/2; 
        // fyc = fieldh;

        // originalImage = this.getOriginal().toBufferedImage();
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint()};
	}
	
	// @Override
	// protected String functionName() {
	// 	return "CheckCameraOffsets";
	// }
	
	///
	
	int width;
	int height;

	int displayw;
	int displayh;

	final int fieldw = 640;
	final int fieldh = 554;

	// final int buffer = 5;

	// double resize = 1;

	// Starting size. The larger the number, the smaller the field ratio
	// final int startSize = 1;

	boolean click = false;
	boolean drag = false;

	// Click and release values
	int clickX1 = 0;
	int clickY1 = 0;
	int clickX2 = 0;
	int clickY2 = 0;

	// Field coordinate image upper left hand corder
	// int fx0;
	// int fy0;

	// Center of field cordinate system
	// int fxc = displayw + buffer + fieldw/2; 
	// int fyc = fieldh;

	// BufferedImage originalImage;
	// BufferedImage edgeImage;
	// Vector<Double> lines;

	// boolean calibrationSuccess = false;

	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		
		Font f = g.getFont();
		Font calFont = f.deriveFont( (float) f.getSize() * 1.5f);
		g.setFont(calFont);
		g.setColor(new Color(26, 44, 116));
		g.drawString("BehaviorsView 1.0.0", 20, 20);
		g.setFont(calFont.deriveFont((float)calFont.getSize() * 0.66f));
		g.setColor(new Color(56, 56, 56));
		g.drawString("Last updated by James Little on June 8, 2016", 20, 35);
	}

	public BehaviorsView() {
		super();
		setLayout(null);
		// lines = new Vector<Double>();
	}

	// @Override
	// public void ioFinished(IOInstance instance) {}

	// @Override
	// public void ioReceived(IOInstance inst, int ret, Log... out) {
		// assert(out[0] == latestVisionLog);
		
		// this.calibrationSuccess = this.latestVisionLog.
		// 		topLevelDictionary.get("CalibrationSuccess").asBoolean().bool();
    	
  //   	Block edgeBlock, lineBlock;
  //   	edgeBlock = this.getEdgeBlock();
  //   	if (edgeBlock != null) {
  //   		debug.info("found edgeBlock");
  //   		EdgeImage ei = new EdgeImage(width, height,  edgeBlock.data);
  //           edgeImage = ei.toBufferedImage();
  //   	}

  //       // TODO refactor. Protobuf?
  //       lines = new Vector<Double>();
        
  //       lineBlock = this.getLineBlock();
  //       if (lineBlock != null) {
  //   		debug.info("found lineBlock");

  //       	byte[] lineBytes = lineBlock.data;
  //           int numLines = lineBytes.length / (9 * 8);
  //           Debug.info("%d field lines expected.", numLines);
  //           try {
  //               DataInputStream dis = new DataInputStream(new ByteArrayInputStream(lineBytes));
  //               for (int i = 0; i < numLines; ++i) {
  //                   lines.add(dis.readDouble()); // image coord r
  //                   lines.add(dis.readDouble()); // image coord t
  //                   lines.add(dis.readDouble()); // image coord ep0
  //                   lines.add(dis.readDouble()); // image coord ep1
  //                   lines.add((double)dis.readInt()); // hough index
  //                   lines.add((double)dis.readInt()); // fieldline index
  //                   lines.add(dis.readDouble()); // field coord r
  //                   lines.add(dis.readDouble()); // field coord t
  //                   lines.add(dis.readDouble()); // field coord ep0
  //                   lines.add(dis.readDouble()); // field coord ep1
  //               }
  //           } catch (Exception e) {
  //               Debug.error("Conversion from bytes to hough coord lines in LineView failed.");
  //               e.printStackTrace();
  //           }
  //       }
                
 //        repaint();
	// }

}
