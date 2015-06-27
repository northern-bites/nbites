package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

public class ImageDisplay extends JPanel {
	
	private BufferedImage image = null;
	private ImageDisplayAnnotator ida = null;
	
	public void setImage(BufferedImage bi) {this.image = bi; this.repaint();}
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
		}
		
		if (ida != null) {
			ida.imageDisplayAnnotate(this, g, image);
		}
	}
	
	@Override
	public Dimension getMaximumSize() {
		if (image != null) {
			return new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE);
		} else {
			return super.getMaximumSize();
		}
	}
	
	@Override
	public Dimension getMinimumSize() {
		if (image != null) {
			return new Dimension(image.getWidth(), image.getHeight());
		} else {
			return super.getMaximumSize();
		}
	}
	
	@Override
	public Dimension getPreferredSize() {
		if (image != null) {
			return new Dimension(image.getWidth(), image.getHeight());
		} else {
			return super.getMaximumSize();
		}
	}
	
	public static interface ImageDisplayAnnotator {
		public void imageDisplayAnnotate(ImageDisplay source, Graphics g, BufferedImage bi);
	}
	
	
}
