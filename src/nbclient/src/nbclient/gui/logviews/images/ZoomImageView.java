package nbclient.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.util.Map;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbclient.data.Log;
import nbclient.gui.logviews.misc.ViewParent;
import nbclient.images.ImageParent;
import nbclient.images.UV88image;
import nbclient.images.Y16image;
import nbclient.images.YUYV8888image;
import nbclient.util.U;


public class ZoomImageView extends ViewParent {
	private static final long serialVersionUID = 1L;

	public void setLog(Log newlog) {
		this.log = newlog;
		
		iv.useImage(U.biFromLog(newlog));
	}

	protected void useSize(Dimension s) {
		sp.setBounds(0,0, s.width, s.height);
	}
	
	public ZoomImageView() {
		super();
		
		iv = new BIWithZoom();
		sp = new JScrollPane(iv);
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		
		this.add(sp);
	}
	
	
	
	private JScrollPane sp;
	private BIWithZoom iv;
}
