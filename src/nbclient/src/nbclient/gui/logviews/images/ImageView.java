package nbclient.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import nbclient.data.Log;
import nbclient.gui.logviews.parent.ViewParent;
import nbclient.images.ImageParent;
import nbclient.images.UV88image;
import nbclient.images.Y16image;
import nbclient.images.YUYV8888image;
import nbclient.util.U;

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
	}

	protected void useSize(Dimension s) {
		
	}
}
