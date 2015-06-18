package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Color;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.util.Vector;

import nbtool.util.Logger;
import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.EdgeImage;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.util.Utility;

public class LineView extends ViewParent implements IOFirstResponder {
	
	final int width = 320;
	final int height = 240;
	
	final int displayw = 640;
	final int displayh = 480;
	
	BufferedImage originalImage;
	BufferedImage edgeImage;
    Vector<Double> lines;
    Vector<GeoLine> geoLines;

	@Override
	public void setLog(Log newlog) {
		CrossInstance ci = CrossIO.instanceByIndex(0);
		if (ci == null)
			return;
		CrossFunc func = ci.functionWithName("Vision");
		assert(func != null);
		
        CrossCall cc = new CrossCall(this, func, newlog);

		assert(ci.tryAddCall(cc));

		originalImage = Utility.biFromLog(newlog);
	}
	
	public void paintComponent(Graphics g) {
		Graphics2D g2 = (Graphics2D) g;
		if (edgeImage != null) {
			g.drawImage(originalImage, 0, 0, displayw, displayh, null);
			g.drawImage(edgeImage, 0, 480, displayw, displayh, null);

            for (int i = 0; i < geoLines.size(); i++) {
            	GeoLine tempGeoLine = geoLines.get(i);
                tempGeoLine.draw(g2,originalImage);
            }
        }
    }
	
	public LineView() {
		super();
		setLayout(null);
        geoLines = new Vector<GeoLine>();
	}

	@Override
	public void ioFinished(IOInstance instance) {}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		EdgeImage ei = new EdgeImage(width, height,  out[5].bytes);
		edgeImage = ei.toBufferedImage();
		repaint();

        // TODO refactor into hough line class
        byte[] lineBytes = out[6].bytes;
        int numLines = lineBytes.length / (10 * 4);
        Logger.logf(Logger.INFO, "%d field lines expected.", numLines);
		try {
			DataInputStream dis = new DataInputStream(new ByteArrayInputStream(lineBytes));
			
			for (int i = 0; i < numLines; ++i) {
				GeoLine tempLine = new GeoLine();
				tempLine.r = dis.readDouble();
				tempLine.t = dis.readDouble();
				tempLine.end0 = dis.readDouble();
				tempLine.end1 = dis.readDouble();
				tempLine.houghIndex = (double)dis.readInt();
				tempLine.fieldIndex = (double)dis.readInt();
                geoLines.add(tempLine);
            }
		} catch (Exception e) {
			Logger.logf(Logger.ERROR, "Conversion from bytes to hough lines in LineView failed.");
			e.printStackTrace();
		}
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
