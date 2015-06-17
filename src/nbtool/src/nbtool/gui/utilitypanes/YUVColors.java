package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.images.YUYV8888image;

public class YUVColors extends UtilityParent {
	
	private class YUV_Frame extends JFrame implements ChangeListener {
		public YUV_Frame() {
			super();
			setLayout(null);
			Dimension s = slider.getPreferredSize();
			slider.setBounds(0, 0, s.width, s.height);
			
			slider.addChangeListener(this);
	
			add(slider);
			setSize(800, 800);
		}
		
		public void paint(Graphics g){
			super.paint(g);
			
			BufferedImage bi = new BufferedImage(512, 512, BufferedImage.TYPE_4BYTE_ABGR);
	        for (int x = 0; x < 512; ++x) {
	        	for (int y = 0; y < 512; ++y) {
	        		byte Y = (byte) (slider.getValue() & 0xFF);
	        		byte U = (byte) (x / 2);
	        		byte V = (byte) (y / 2);
	        		
	        		bi.setRGB(x, y, YUYV8888image.yuv444ToARGB888Pixel(Y, U, V));
	        	}
	        }
	        
	        g.drawImage(bi, 0, slider.getHeight() + 50, null);
	    }
		
		private JSlider slider = new JSlider(0, 255, 128);
		private JLabel yl, ul, vl;

		public void stateChanged(ChangeEvent e) {
			this.repaint();
		}
	}


	private YUV_Frame display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display == null) {
			return (display = new YUV_Frame());
		}
		
		return display;
	}



	@Override
	public String purpose() {
		return "Show YUV colorspace";
	}


	@Override
	public char preferredMemnonic() {
		return 0;
	}
}
