package nbtool.gui.logviews.images;

import java.awt.Graphics;
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
    Vector<Double> houghCoordLines;
    Vector<Double> fieldCoordLines;

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
		if (edgeImage != null) {
			g.drawImage(originalImage, 0, 0, displayw, displayh, null);
			g.drawImage(edgeImage, 0, 485, displayw, displayh, null);
            
            // Field coordinate image
            int fx0 = 645;
            int fy0 = 0;

            g.setColor(new Color(90, 130, 90));
            g.fillRect(645, 0, 640, 480);
            g.setColor(Color.lightGray);
            g.fillOval(645 + 320 - 30, 480 - 20, 60, 40);

            for (int i = 0; i < houghCoordLines.size(); i += 6) {
                double r = houghCoordLines.get(i);
                double t = houghCoordLines.get(i + 1);
                double end0 = houghCoordLines.get(i + 2);
                double end1 = houghCoordLines.get(i + 3);
                double houghIndex = houghCoordLines.get(i + 4);
                double fieldIndex = houghCoordLines.get(i + 5);

                if (fieldIndex == -1)
                    g.setColor(Color.red);
                else
                    g.setColor(Color.blue);

                double x0 = 2*r * Math.cos(t) + originalImage.getWidth() / 2;
                double y0 = -2*r * Math.sin(t) + originalImage.getHeight() / 2;
                int x1 = (int) Math.round(x0 + 2*end0 * Math.sin(t));
                int y1 = (int) Math.round(y0 + 2*end0 * Math.cos(t));
                int x2 = (int) Math.round(x0 + 2*end1 * Math.sin(t));
                int y2 = (int) Math.round(y0 + 2*end1 * Math.cos(t));

                double xstring = (x1 + x2) / 2;
                double ystring = (y1 + y2) / 2;

                double scale = 0;
                if (r > 0)
                    scale = 10;
                else
                    scale = 3;
                xstring += scale*Math.cos(t);
                ystring += scale*Math.sin(t);

                g.drawLine(x1, y1, x2, y2);
                g.drawString(Integer.toString((int) houghIndex) + "/" + Integer.toString((int) fieldIndex), 
                             (int) xstring, 
                             (int) ystring);
            }
        }
    }
	
	public LineView() {
		super();
		setLayout(null);
        lines = new Vector<Double>();
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
        int numLines = lineBytes.length / (18 * 4);
        Logger.logf(Logger.INFO, "%d field lines expected.", numLines);
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
			Logger.logf(Logger.ERROR, "Conversion from bytes to hough coord lines in LineView failed.");
			e.printStackTrace();
		}
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
