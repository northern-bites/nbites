package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.util.Utility;

class ZoomImageDisplay extends JPanel {
	
	private static final long serialVersionUID = 1L;

	public ZoomImageDisplay() {
		full = part = null;
		sub_rect = null;
		
		setLayout(null);
		
		slider = new JSlider(JSlider.HORIZONTAL);
		add(slider);
		slider.setVisible(false);
		slider.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e) {
				//U.w("slider change");
				recalcPart(null);
			}
		});
		
		addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				//U.w("click");
				if (full != null && e.getX() < full.getWidth() && e.getY() < full.getHeight()) {
					//U.w("\tinside");
					recalcPart(e.getPoint());
				}
			}
		});
	}
	
	public void useImage(BufferedImage img) {
		full = part = img;
		sub_rect = new Rectangle(0, 0, img.getWidth(), img.getHeight());
		slider.setMaximum(img.getWidth());
		slider.setMinimum(1);
		slider.setValue(slider.getMaximum());
		slider.setVisible(true);
		
		this.repaint();
	}
	
	private void recalcPart(Point click) {
		if (full == null) return;
		int max_width = full.getWidth();
		int max_height = full.getHeight();
		
		int req_width = slider.getValue();
		int req_height = (max_height * req_width) / max_width;
		
		//For now, we prioritize dimensions and adjust point to fit.
		
		int req_x, req_y;
		if (click != null) {
			req_x = click.x;
			req_y = click.y;
		} else {
			req_x = sub_rect.x;
			req_y = sub_rect.y;
		}
		
		if (req_x + req_width > max_width)
			req_x = max_width - req_width;
		if (req_y + req_height > max_height)
			req_y = max_height - req_height;
		
		if (req_width <= 0)
			req_width = 1;
		if (req_height <= 0)
			req_height = 1;
		
		sub_rect = new Rectangle(req_x, req_y, req_width, req_height);
		part = full.getSubimage(sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height);
		
		this.repaint();
	}
	
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		if (full == null) return;
		
		int y_offset = 0;
		g.drawImage(full, 0, 0, null);
		y_offset += full.getHeight() + 10;
		Dimension d = slider.getPreferredSize();
		slider.setBounds(0, y_offset, d.width, d.height);
		y_offset += d.height + 10;
		
		g.setColor(Color.PINK);
		g.drawRect(sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height);
		
		g.drawImage(part, 0, y_offset, full.getWidth(), full.getHeight(), null);
		
		y_offset += full.getHeight();
		
		this.setPreferredSize(new Dimension(full.getWidth(), y_offset));
	}
	
	private BufferedImage full, part;
	private Rectangle sub_rect;
	
	//Width, from 1 to full.getWidth()
	private JSlider slider;
}
