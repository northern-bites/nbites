package nbtool.gui.logviews.sound2;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.util.Logger;

public abstract class SoundPane extends JPanel implements ChangeListener {
	private int channels, frames;
	
	private JSlider multiplier;
	private JLabel peak;
	private Display display;
	
	public SoundPane(int channel, int frame) {
		this.channels = channel;
		this.frames = frame;
		
		requested = new Dimension(frames + 100, 150 * channels);
		multiplier = new JSlider(JSlider.VERTICAL);
		multiplier.setMinimum(1);
		multiplier.setValue(1);
		multiplier.addChangeListener(this);
		
		peak = new JLabel(peakString());
		
		this.setLayout(new BorderLayout());
		this.add(peak, BorderLayout.NORTH);
		this.add(multiplier, BorderLayout.WEST);
		this.add(display = new Display(), BorderLayout.CENTER);
	}
	
	public abstract int pixels(int c, int f, int radius);
	public abstract String peakString();
	
	private class Display extends JPanel {
		
		private Color[] defined = {Color.RED, Color.GREEN, Color.BLUE, Color.ORANGE};
		private Color getChannelColor(int i) {
			if (i < defined.length)
				return defined[i];
			return Color.ORANGE;
		}
				
		@Override
		public void paintComponent(Graphics g) {
			super.paintComponent(g);
			
			Dimension size = this.getSize();
			if (size.height < 100 || size.width < frames) {
				g.drawString("make me bigger", 0, 0);
				return;
			}
			
			int channelSpace = (size.height - channels * 5) / channels;
			Logger.printf("%s space for each channel.", channelSpace);
			int mult = multiplier.getValue();

			for (int c = 0; c < channels; ++c) {
				int channelBottom = c * (channelSpace + 5);
				int channelTop = channelBottom + channelSpace;
				
				int diameter = channelTop - channelBottom;
				int channelMid = channelBottom + diameter / 2;
				
				g.setColor(getChannelColor(c));
				for (int i = 0; i < frames; ++i) {
					int pix = pixels(c, i, diameter / 2);
					pix *= mult;
					
					g.drawLine(i, channelMid, i, channelMid + pix);
				}
				
				g.setColor(Color.BLACK);
				g.drawLine(0, channelBottom, frames, channelBottom);
				g.drawLine(0, channelMid, frames, channelMid);
				g.drawLine(0, channelTop, frames, channelTop);
			}
		}
	}
	
	private Dimension requested;
	@Override
	public Dimension getPreferredSize() {
		return requested;
	}
	
	@Override
	public Dimension getMinimumSize() {
		return requested;
	}

	@Override
	public void stateChanged(ChangeEvent e) {
		display.repaint();
	}
}
