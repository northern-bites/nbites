package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;

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
	
	@Override
	public void setLog(Log newlog) {
		CrossInstance ci = CrossIO.instanceByIndex(0);
		if (ci == null)
			return;
		CrossFunc func = ci.functionWithName("Vision");
		assert(func != null);
		
		CrossCall cc = new CrossCall(this, func, newlog);
		
		assert(ci.tryAddCall(cc));
	}
	
	BufferedImage img;
	public void paintComponent(Graphics g) {
		if (img != null)
			g.drawImage(img, 0, 0, displayw, displayh, null);
    }
	
	public LineView() {
		super();
		setLayout(null);
	}

	@Override
	public void ioFinished(IOInstance instance) {}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		EdgeImage ei = new EdgeImage(width, height,  out[5].bytes);
		img = ei.toBufferedImage();
		repaint();

        byte[] lines = out[6].bytes;
        int numLines = lines.length / (8 * 4);
        Logger.logf(Logger.INFO, "%d field lines expected.", numLines);
		try {
			DataInputStream dis = new DataInputStream(new ByteArrayInputStream(lines));
			for (int i = 0; i < numLines; ++i) {
                double r = dis.readDouble();
                double t = dis.readDouble();
                System.out.println("r: " + r);
                System.out.println("t: " + t);
                double ep0 = dis.readDouble();
                double ep1 = dis.readDouble();
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
