package nbclient.gui.logviews.lines;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import nbclient.data.OpaqueLog;
import nbclient.gui.logviews.parent.ViewParent;
import nbclient.images.YUYV8888image;

public class EdgeView extends ViewParent{
	BufferedImage gray;
	
	public void paintComponent(Graphics g) {
		if (gray != null)
			g.drawImage(gray, 0, 0, null);
    }
	
	public void setLog(OpaqueLog newlog) {
		log = newlog;
		String encoding = (String) log.getAttributes().get("encoding");
		
		//|| because could be old log, see U.biFromLog
		if (encoding == null || encoding.equalsIgnoreCase(YUYV8888image.INSTANCE.encoding())) {
			int width = (Integer) log.getAttributes().get("width");
			int height = (Integer) log.getAttributes().get("height");
			YUYV8888image img = new YUYV8888image(width, height, log.bytes);
			this.gray = img.toY8().toBufferedImage();
		}
		
		repaint();
	}

	
	protected void useSize(Dimension s) {}
	
	public static Boolean shouldLoadInParallel(){return true;}
	
	public EdgeView() {
		super();
	}

}
