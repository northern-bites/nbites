package nbtool.gui.logviews.sound2;

import java.awt.BorderLayout;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;

public class FFTWView extends ViewParent {
	
	FFTWBuffer buffer = null;

	@Override
	public void setLog(Log newlog) {
		buffer = new FFTWBuffer();
		buffer.parse(newlog);
		
		SoundPane sp = new SoundPane(buffer.channels, buffer.frames) {
			@Override
			public int pixels(int c, int f, int radius) {
				return (int) ((buffer.get(f, c) / buffer.max) * radius);
			}

			@Override
			public String peakString() {
				return "max abs val = " + buffer.max;
			}

			@Override
			public String selectionString(int c, int f) {
				return "" + buffer.get(f, c);
			}
		};
		
		this.add(sp, BorderLayout.CENTER);
	}
	

}
