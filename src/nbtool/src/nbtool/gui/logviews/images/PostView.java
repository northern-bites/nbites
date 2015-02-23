package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.image.BufferedImage;
import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.YUYV8888image;
import nbtool.util.U;

// TODO add via layouts?
// TODO include all relevant images?
// TODO directory structure

public class PostView extends ViewParent {
	Log log;
	PostDetector d;
	
	public PostView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
	}
	
	public void setLog(Log log_) {
		log = log_;
		
		d = new PostDetector((YUYV8888image) U.imageFromLog(log));
		
		repaint();
	}
	
	public void paintComponent(Graphics g) {
		if (d == null)
			return;
		
		// Display local feature images
		BufferedImage bOriginal = d.original.toBufferedImage();
		g.drawImage(bOriginal, bOriginal.getWidth(), 0, null);
		g.drawImage(d.gradient, 0, 0, null);
		//g.drawImage(d.yellow, 0, 0, null);
		//g.drawImage(d.field, 0, 0, null);
		g.drawImage(d.post, 0, bOriginal.getHeight(), null);
		
		// Display histogram
        g.setColor(Color.black);
        int barWidth = bOriginal.getWidth() / d.scores.length;
        for(int i = 0; i < d.scores.length; i++){
            int barHeight = (int)(d.scores[i]);
            g.fillRect(bOriginal.getWidth() + i*barWidth, 
            		   bOriginal.getHeight() + (int)(bOriginal.getHeight()-d.scores[i]), 
            		   barWidth,
            		   barHeight);
        }
        
        // Display line using to do peak detection
        g.setColor(Color.red);
        for (int x = 0; x < d.scores.length; x++) {
			double m = d.line.getFirstPrincipalAxisV() / d.line.getFirstPrincipalAxisU();
			int y = (int)(m*x - m*d.line.getCenterX() + d.line.getCenterY());
	        g.fillRect(bOriginal.getWidth() + x*barWidth, 
	        		   bOriginal.getHeight() + (bOriginal.getHeight() - y), 
	        		   2, 
	        		   2);
        }
        
        // Mark goalposts
        g.fillRect(bOriginal.getWidth() + d.leftPost*barWidth, 
     		   	   0, 
     		       2, 
     		       2*bOriginal.getHeight());
        if (d.rightPost > 0)
        	g.fillRect(bOriginal.getWidth() + d.rightPost*barWidth, 
  		   	   	       0, 
  		               2, 
  		               2*bOriginal.getHeight());
    }
	
	protected void useSize(Dimension s) {}
	
	public static Boolean shouldLoadInParallel() { return true; }
}