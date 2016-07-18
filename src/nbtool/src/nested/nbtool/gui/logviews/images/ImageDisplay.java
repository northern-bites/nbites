package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

import nbtool.util.Debug;

public class ImageDisplay extends JPanel {

	private BufferedImage image = null;
	public ImageDisplayAnnotator ida = null;

	private Dimension prefSizeOverride = null;

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

	public ImageDisplay(final Dimension pref) {
		this(null, null);
		this.prefSizeOverride = pref;

		this.addComponentListener(new ComponentAdapter(){
			final double heightPerWidth = pref.height / (double) pref.width;
			final double widthPerHeight = pref.width / (double) pref.height;

			@Override
			public void componentResized(ComponentEvent e) {
				int width = e.getComponent().getWidth();
				int height = e.getComponent().getHeight();

				Dimension takeWidth = new Dimension(width, (int) (width * heightPerWidth) );
				Dimension takeHeight = new Dimension((int) (height * widthPerHeight), height);

				Dimension use = null;
				if (takeWidth.height <= height && takeWidth.width <= width) {
					use = takeWidth;
				} else if (takeHeight.height <= height && takeHeight.width <= width) {
					use = takeHeight;
				} else {
					Debug.error("logic for dimensions wrong in ImageDisplay!");
					assert(false);
				}

				e.getComponent().setSize(use);
			}
		});
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
		if (this.prefSizeOverride != null)
			return prefSizeOverride;

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
