package nbtool.gui.logviews.lines;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.YUYV8888image;

public class EdgeView extends ViewParent{
	BufferedImage gray;
	
	public void paintComponent(Graphics g) {
		if (gray != null)
			g.drawImage(gray, 0, 0, null);
    }
	
	public void setLog(Log newlog) {
		log = newlog;
		String encoding = (String) log.getAttributes().get("encoding");
		
		//|| because could be old log, see U.biFromLog
		if (encoding == null || encoding.equalsIgnoreCase(YUYV8888image.INSTANCE.encoding())) {
			int width = log.width();
			int height = log.height();
			YUYV8888image img = new YUYV8888image(width, height, log.bytes);
			this.gray = img.toY8().toBufferedImage();
		}
		
		repaint();
	}

	
	private void useSize(Dimension s) {}
	
	public static Boolean shouldLoadInParallel(){return true;}
	
	public EdgeView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
	}

}
