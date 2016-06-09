package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

public class ImagePanel extends JPanel {
	BufferedImage img;
	
	public ImagePanel() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
	}
	
	public Dimension getPreferredSize() {
		return new Dimension(img.getWidth(), img.getHeight());
	}
	
	public void paintComponent(Graphics g) {
		if (img != null)
			g.drawImage(img, 0, 0, null);
    }
	
	public void setLog(BufferedImage img_) {
		img = img_;
		
		repaint();
	}

	protected void useSize(Dimension s) {}
}
