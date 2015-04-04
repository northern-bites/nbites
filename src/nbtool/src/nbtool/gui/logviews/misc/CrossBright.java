package nbtool.gui.logviews.misc;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;

import nbtool.data.Log;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CppFuncCall;
import nbtool.io.CrossIO.CppFuncListener;
import nbtool.util.U;

public class CrossBright extends ViewParent implements CppFuncListener{
	BufferedImage img;
	
	public void paintComponent(Graphics g) {
		if (img != null)
			g.drawImage(img, 0, 0, null);
    }
	
	@Override
	public void setLog(Log newlog) {
		log = newlog;
		
		int fi = CrossIO.current.indexOfFunc("CrossBright");
		if (fi < 0) return;
		
		CppFuncCall fc = new CppFuncCall();
		
		fc.index = fi;
		fc.name = "CrossBright";
		fc.args = new ArrayList<Log>(Arrays.asList(log));
		fc.listener = this;
		
		CrossIO.current.tryAddCall(fc);
	}
	
	public CrossBright() {
		super();
	}

	@Override
	public void returned(int ret, Log... out) {
		this.img = U.biFromLog(out[0]);
		repaint();
	}
}
