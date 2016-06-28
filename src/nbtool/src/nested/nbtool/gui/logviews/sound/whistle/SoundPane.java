package nbtool.gui.logviews.sound.whistle;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public abstract class SoundPane extends JPanel implements ChangeListener, MouseMotionListener {
	private int channels, frames;

	private JSlider multiplier;
	private Display display;
	private JLabel peak;
	private JLabel selection;

	public SoundPane(int channel, int frame) {
		this.channels = channel;
		this.frames = frame;

		channelRectLast = new Rectangle[channels];

		requested = new Dimension(frames * 2, 150 * channels);
		multiplier = new JSlider(JSlider.VERTICAL);
		multiplier.setMinimum(1);
		multiplier.setValue(1);
		multiplier.addChangeListener(this);

		peak = new JLabel(peakString());
		selection = new JLabel("");

		display = new Display();
		display.addMouseMotionListener(this);

		this.setLayout(new BorderLayout());
		this.add(peak, BorderLayout.NORTH);
		this.add(multiplier, BorderLayout.WEST);
		this.add(display, BorderLayout.CENTER);
		this.add(selection, BorderLayout.SOUTH);
	}

	public abstract int pixels(int c, int f, int radius);
	public abstract String peakString();
	public abstract String selectionString(int c, int f);

	private Rectangle[] channelRectLast;

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
			//Logger.printf("%s space for each channel.", channelSpace);
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

				channelRectLast[c] = new Rectangle(0, channelBottom, frames, diameter);
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
	public Dimension getMaximumSize() {
		return new Dimension(Short.MAX_VALUE, Short.MAX_VALUE);
	}

	@Override
	public void stateChanged(ChangeEvent e) {
		display.repaint();
	}

	@Override
	public void mouseDragged(MouseEvent e) {

	}

	@Override
	public void mouseMoved(MouseEvent e) {
		for (int i = 0; i < channels; ++i) {
			if (channelRectLast[i] != null && channelRectLast[i].contains(e.getPoint())) {
				int f = e.getX();

				String text = selectionString(i, f);
				selection.setText(text);
				return;
			}
		}
	}
}
