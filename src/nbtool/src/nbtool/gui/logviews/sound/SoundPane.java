package nbtool.gui.logviews.sound;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JPanel;

public class SoundPane <T extends Number> extends JPanel{
	private T[] left;
	private T[] right;
	
	private Scaler<T> scaler;
	
	public SoundPane(T[] l, T[] r, Scaler<T> scaler) {
		assert(l.length == r.length);
		
		this.left = l;
		this.right = r;
		this.scaler = scaler;
	}
	
	public static abstract class Scaler<S> {
		//return signed number of pixels to display for val inside display radius pixels.
		//may return a value greater than pixels;
		public abstract int pixelsFor(S val, int pixels);
	}
	
	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		
		Dimension size = this.getSize();
		if (size.height < 100 || size.width < left.length) {
			g.drawString("make me bigger", 0, 0);
			return;
		}
		
		int diameter = (size.height - 15) / 2;
		int radius = diameter / 2;
		int fs = 5;
		int ss = 10 + diameter;
		
		g.setColor(Color.RED);
		for (int i = 0; i < left.length; ++i) {
			int yoff = scaler.pixelsFor(left[i], radius);
			g.drawLine(i, fs + radius,
					i, fs + radius + yoff);
		}
		
		g.setColor(Color.GREEN);
		for (int i = 0; i < right.length; ++i) {
			int yoff = scaler.pixelsFor(right[i], radius);
			g.drawLine(i, ss + radius,
					i, ss + radius + yoff);
		}
		
		g.setColor(Color.black);
		g.drawLine(0, fs, left.length, fs);
		g.drawLine(0, fs + radius, left.length, fs + radius);
		g.drawLine(0, fs + diameter, left.length, fs + diameter);
		
		g.drawLine(0, ss, right.length, ss);
		g.drawLine(0, ss + radius, right.length, ss + radius);
		g.drawLine(0, ss + diameter, right.length, ss + diameter);
	}
	
	@Override
	public Dimension getPreferredSize() {
		return new Dimension(left.length + 40, 300);
	}
	
	@Override
	public Dimension getMinimumSize() {
		return new Dimension(left.length + 40, 300);
	}
}
