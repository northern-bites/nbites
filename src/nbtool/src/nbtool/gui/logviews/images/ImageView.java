package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.ImageParent;
import nbtool.images.UV88image;
import nbtool.images.Y16image;
import nbtool.images.YUYV8888image;
import nbtool.util.U;

public class ImageView extends ViewParent{
	BufferedImage img;
	
	public void paintComponent(Graphics g) {
		if (img != null)
			g.drawImage(img, 0, 0, null);
    }
	
	public void setLog(Log newlog) {
		log = newlog;
		
		this.img = U.biFromLog(newlog);
		
		repaint();
	}
	
	public ImageView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
	}

	protected void useSize(Dimension s) {
		
	}
}
