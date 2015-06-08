package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.ImageParent;
import nbtool.images.UV88image;
import nbtool.images.Y16image;
import nbtool.images.YUYV8888image;
import nbtool.util.Utility;

public class ImageView extends ViewParent {
	ImagePanel imgPanel;
	
	public void setLog(Log newlog) {
		log = newlog;

		//this.img = Utility.biFromLog(newlog);

		repaint();
	}
	
	public ImageView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		setLayout(new GridLayout(1, 1));
		imgPanel = new ImagePanel();
		add(imgPanel);
	}

	protected void useSize(Dimension s) {}
}
