package nbtool.gui.logviews.misc;

import java.awt.Graphics;
import java.awt.image.BufferedImage;

import nbtool.data.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.util.Utility;

public class CrossBright extends ViewParent implements IOFirstResponder {
	BufferedImage img;
	
	public void paintComponent(Graphics g) {
		if (img != null)
			g.drawImage(img, 0, 0, null);
    }
	
	@Override
	public void setLog(Log newlog) {		
		CrossInstance inst = CrossIO.instanceByIndex(0);
		if (inst == null)
			return;

		CrossFunc func = inst.functionWithName("CrossBright");
		if (func == null)
			return;
		
		CrossCall call = new CrossCall(this, func, this.log);
		inst.tryAddCall(call);

	}
	
	public CrossBright() {
		super();
	}

	@Override
	public void ioFinished(IOInstance instance) {}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		this.img = Utility.biFromLog(out[0]);
		repaint();
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
