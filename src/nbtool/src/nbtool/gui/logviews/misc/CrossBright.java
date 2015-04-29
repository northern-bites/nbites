package nbtool.gui.logviews.misc;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;

import nbtool.data.Log;
import nbtool.io.CppIO;
import nbtool.io.CppIO.CppFuncCall;
import nbtool.io.CppIO.CppFuncListener;
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
		
		int fi = CppIO.current.indexOfFunc("CrossBright");
		if (fi < 0) return;
		
		CppFuncCall fc = new CppFuncCall();
		
		fc.index = fi;
		fc.name = "CrossBright";
		fc.args = new ArrayList<Log>(Arrays.asList(log));
		fc.listener = this;
		
		CppIO.current.tryAddCall(fc);
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
