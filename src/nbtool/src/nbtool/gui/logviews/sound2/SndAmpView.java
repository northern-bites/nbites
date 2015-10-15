package nbtool.gui.logviews.sound2;

import java.awt.BorderLayout;

import nbtool.data.log._Log;
import nbtool.gui.logviews.misc.ViewParent;

public class SndAmpView extends ViewParent {

	private ShortBuffer sb;
	@Override
	public void setLog(_Log newlog) {
		sb = new ShortBuffer();
		sb.parse(newlog);
		
		SoundPane pane = new SoundPane(sb.channels, sb.frames) {

			@Override
			public int pixels(int c, int f, int radius) {
				return (sb.get(c, f) * radius) / Short.MAX_VALUE;
			}

			@Override
			public String peakString() {
				return String.format("peak left = %d, peak right = %d", sb.peaks[0], sb.peaks[1]);
			}

			@Override
			public String selectionString(int c, int f) {
				return "" + sb.get(c, f);
			}
		};
		
		this.add(pane, BorderLayout.CENTER);
	}
	
	public SndAmpView() {
		super();
		this.setLayout(new BorderLayout());
	}

}
