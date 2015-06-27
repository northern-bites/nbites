package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;

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

public class EdgeView extends ViewParent implements IOFirstResponder {
	
	final int width = 320;
	final int height = 240;
	
	final int displayw = 640;
	final int displayh = 480;
	
	final String crossFuncName = "edges_func";

	@Override
	public void setLog(Log newlog) {
		
		CrossInstance ci = CrossIO.instanceByIndex(0);
		if (ci == null)
			return;
		CrossFunc func = ci.functionWithName(crossFuncName);
		assert(func != null);
		
		CrossCall cc = new CrossCall(this, func, newlog);
		
		assert(ci.tryAddCall(cc));
	}
	
	BufferedImage img;
	public void paintComponent(Graphics g) {
		if (img != null)
			g.drawImage(img, 0, 0, displayw, displayh, null);
    }
	
	public EdgeView() {
		super();
		setLayout(null);
	}

	@Override
	public void ioFinished(IOInstance instance) {		}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		EdgeImage ei = new EdgeImage(width, height,  out[0].bytes);
		img = ei.toBufferedImage();
		repaint();
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
