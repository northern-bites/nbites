package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;

import nbtool.data.log.Block;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

public class ImageView extends ViewParent implements MouseMotionListener {
	BufferedImage img;
	private String label = null;
	
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
        
		if (img != null)
			g.drawImage(img, 0, 0, null);
		if (label != null)
			g.drawString(label, 10, img.getHeight() + 20);
    }
	
	public ImageView() {
		super();
		setLayout(null);
		this.addMouseMotionListener(this);
	}

	@Override
	public void mouseDragged(MouseEvent e) {}

	@Override
	public void mouseMoved(MouseEvent e) {
		if (img == null || displayedLog == null)
			return;
		
		int col = e.getX();
		int row = e.getY();
		
		if (col < 0 || row < 0 || col >= img.getWidth() || row >= img.getHeight())
			return;
		
		boolean first = (col & 1) == 0;
		int cbase = (col & ~1);
		int i = (row * img.getWidth() * 2) + (cbase * 2);
		
		Block image = displayedLog.blocks.get(0);
		int y = image.data[first ? i : i + 2] & 0xff;
		int u = image.data[i + 1] & 0xff;
		int v = image.data[i + 3] & 0xff;
		label = String.format("(%d,%d): y=%d u=%d v=%d", col, row, y, u, v);
		repaint();
	}

	@Override
	public void setupDisplay() {
		this.img = displayedLog.blocks.get(0).parseAsYUVImage().toBufferedImage();
		repaint();
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint(),
				SharedConstants.LogClass_YUVImage()};
	}

	
}
