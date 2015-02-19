package nbtool.gui.logviews.images;

import java.awt.*;
import javax.swing.JPanel;
import javax.swing.JLabel;

public class HistogramPanel extends JPanel{
    JLabel histLabel;
    double[] histogram;
    WeightedFit line;
        
    public HistogramPanel(double[] histogram_, WeightedFit line_) {
        histogram = histogram_;
        line = line_;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        drawHist(g, histogram);
        g.drawString("Histogram", 20, 20);
    }
    
    private void drawHist(Graphics g, double arr[]){
        double graphHeight = getHeight()*.1;
        g.setColor(Color.black);
        
        int barWidth = getWidth() / arr.length;
        for(int i = 0; i < arr.length; i++){
            int barHeight = (int)(arr[i]*2.5);
            g.fillRect(i*barWidth, getHeight()-barHeight, barWidth, barHeight);
        }
        
        g.setColor(Color.red);
        for (int x = 0; x < arr.length; x++) {
			double m = line.getFirstPrincipalAxisV() / line.getFirstPrincipalAxisU();
			int y = (int)(m*x - m*line.getCenterX() + line.getCenterY());
			int barHeight = (int)(2.5*(double)y);
	        g.fillRect(x*barWidth, getHeight()-barHeight, 5, 5);
        }
    }
}
