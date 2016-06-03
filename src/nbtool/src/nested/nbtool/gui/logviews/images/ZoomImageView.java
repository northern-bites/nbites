package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.util.Map;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.ImageParent;
import nbtool.images.Y16Image;
import nbtool.images.YUYV8888Image;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;


public class ZoomImageView extends ViewParent {
	private static final long serialVersionUID = 1L;

	protected void useSize(Dimension s) {
		sp.setBounds(0,0, s.width, s.height);
	}
	
	public ZoomImageView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
		
		iv = new ZoomImageDisplay();
		sp = new JScrollPane(iv);
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		this.add(sp);
	}
	
	
	
	private JScrollPane sp;
	private ZoomImageDisplay iv;

	@Override
	public void setupDisplay() {
		iv.useImage(displayedLog.blocks.get(0).parseAsYUVImage().toBufferedImage());
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint(), SharedConstants.LogClass_YUVImage()};
	}
}
