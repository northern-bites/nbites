package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

import nbtool.util.Debug;

public class ImageDisplay extends JPanel {

	private BufferedImage image = null;
	public ImageDisplayAnnotator ida = null;

	public void setImage(BufferedImage bi) {
		Debug.info("new image!");
		this.image = bi; this.repaint();
		}
	public void setIDA(ImageDisplayAnnotator ida) {this.ida = ida;}

	public ImageDisplay(BufferedImage image, ImageDisplayAnnotator ida) {
		this.image = image;
		this.ida = ida;
		this.setLayout(null);
	}

	public ImageDisplay(BufferedImage image) {
		this(image, null);
	}

	public ImageDisplay() {
		this(null, null);
	}

	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);

		if (image != null) {
			Dimension d = this.getSize();
			g.drawImage(image, 0, 0, d.width, d.height, null);
		} else {
			g.drawString("no image set", 10, 10);
		}

		if (ida != null) {
			ida.imageDisplayAnnotate(this, g, image);
		}
	}

	@Override
	public Dimension getMaximumSize() {
		return new Dimension(Short.MAX_VALUE, Short.MAX_VALUE);

//		if (image != null) {
//
//		} else {
//			return new Dimension();
//		}
	}

	@Override
	public Dimension getMinimumSize() {
		if (image != null) {
			return new Dimension(image.getWidth(), image.getHeight());
		} else {
			return new Dimension(120,80);
		}
	}

	@Override
	public Dimension getPreferredSize() {
		if (image != null) {
			return new Dimension(image.getWidth(), image.getHeight());
		} else {
			return new Dimension(320,240);
		}
	}

	public static interface ImageDisplayAnnotator {
		public void imageDisplayAnnotate(ImageDisplay source, Graphics g, BufferedImage bi);
	}


}
