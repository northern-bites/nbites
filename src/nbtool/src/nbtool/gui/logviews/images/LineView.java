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
    Vector<Double> lines;

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
			g.drawImage(edgeImage, 0, 480, displayw, displayh, null);

            for (int i = 0; i < lines.size(); i += 5) {
                double r = lines.get(i);
                double t = lines.get(i + 1);
                double end0 = lines.get(i + 2);
                double end1 = lines.get(i + 3);
                double lineIndex = lines.get(i + 4);

                if (lineIndex == -1)
                    g.setColor(Color.blue);
                else
                    g.setColor(Color.red);

                double x0 = 2*r * Math.cos(t) + originalImage.getWidth() / 2;
                double y0 = -2*r * Math.sin(t) + originalImage.getHeight() / 2;
                int x1 = (int)Math.round(x0 + 2*end0 * Math.sin(t));
                int y1 = (int)Math.round(y0 + 2*end0 * Math.cos(t));
                int x2 = (int)Math.round(x0 + 2*end1 * Math.sin(t));
                int y2 = (int)Math.round(y0 + 2*end1 * Math.cos(t));

                g.drawLine(x1, y1, x2, y2);
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
        int numLines = lineBytes.length / (9 * 4);
        Logger.logf(Logger.INFO, "%d field lines expected.", numLines);
		try {
			DataInputStream dis = new DataInputStream(new ByteArrayInputStream(lineBytes));
			for (int i = 0; i < numLines; ++i) {
                lines.add(dis.readDouble()); // r
                lines.add(dis.readDouble()); // t
                lines.add(dis.readDouble()); // ep0
                lines.add(dis.readDouble()); // ep1
                lines.add((double)dis.readInt()); // field line index
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
