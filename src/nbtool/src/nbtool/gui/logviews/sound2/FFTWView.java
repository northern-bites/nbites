package nbtool.gui.logviews.sound2;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;

import javax.swing.JLabel;

import nbtool.data.log._Log;
import nbtool.gui.logviews.misc.ViewParent;

public class FFTWView extends ViewParent {
	
	FFTWBuffer buffer = null;

	@Override
	public void setLog(_Log newlog) {
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
				return String.format("c%d f%d val=%f", c, f, buffer.get(f, c));
			}
		};
		
		this.add(sp, BorderLayout.CENTER);
		
		String ftext = "";
		for (int i = 0; i < buffer.channels; ++i) {
			if (WhistleDetector.detect(buffer, i)) {
				ftext += "[found in " + i + "]";
			}
		}
		
		if (!ftext.isEmpty()) {
			JLabel ft = new JLabel(ftext);
			Font font = ft.getFont();
			ft.setForeground(Color.RED);
			ft.setFont(font.deriveFont(Font.BOLD));
			
			this.add(ft, BorderLayout.SOUTH);
		}
		
		/*
		if (newlog.sexprForContentItem(0).find("whistle").exists()) {
			this.add(new JLabel("found!"), BorderLayout.SOUTH);
		} */
	}
	
	public FFTWView() {
		super();
		this.setLayout(new BorderLayout());
	}
}
