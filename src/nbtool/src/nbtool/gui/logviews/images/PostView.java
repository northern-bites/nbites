// package nbtool.gui.logviews.images;

// import java.awt.Color;
// import java.awt.Dimension;
// import java.awt.Graphics;
// import java.awt.event.ComponentAdapter;
// import java.awt.event.ComponentEvent;
// import java.awt.image.BufferedImage;
// import nbtool.data.Log;
// import nbtool.gui.logviews.misc.ViewParent;

// // TODO add via layouts?
// // TODO include all relevant images?
// // TODO directory structure

// public class PostView extends ViewParent {
// 	PostDetector d;
	
// 	public PostView() {
// 		super();
		
// 		addComponentListener(new ComponentAdapter() {
// 			public void componentResized(ComponentEvent e) {
// 				useSize(e.getComponent().getSize());
// 			}
// 		});
// 		setLayout(null);
// 	}
	
// 	public void setLog(Log log_) {
// 		log = log_;
		
// 		d = new PostDetector(log);
		
// 		repaint();
// 	}
	
// 	public void paintComponent(Graphics g) {
// 		if (d == null)
// 			return;
		
// 		// Display local feature images
// 		BufferedImage bOriginal = d.original.toBufferedImage();
// 		g.drawImage(bOriginal, bOriginal.getWidth(), 0, null);
// 		//g.drawImage(d.gradient, 0, 0, null);
// 		//g.drawImage(d.yellow, 0, 0, null);
// 		//g.drawImage(d.field, 0, 0, null);
// 		g.drawImage(d.gradient, 0, 0, null);
		
// 		// Display raw scores
//         g.setColor(Color.black);
//         int barWidth = bOriginal.getWidth() / d.rawScores.length;
//         for(int i = 0; i < d.rawScores.length; i++){
//             int barHeight = (int)(d.rawScores[i]);
//             g.fillRect(bOriginal.getWidth() + i*barWidth, 
//             		   bOriginal.getHeight() + (int)(bOriginal.getHeight()-d.rawScores[i]), 
//             		   barWidth,
//             		   barHeight);
//         }
        
//         // Display processed scores
//         g.setColor(Color.black);
//         for(int i = 0; i < d.processedScores.length; i++){
//             int barHeight = (int)(d.processedScores[i]);
//             g.fillRect(i*barWidth, 
//             		   bOriginal.getHeight() + (int)(bOriginal.getHeight()-d.processedScores[i]), 
//             		   barWidth,
//             		   barHeight);
//         }
        
//         // Mark goalpost candidates
//         g.setColor(Color.red);
//         for (int i = 0; i < d.candidates.size(); i++) {
//         	g.fillRect(bOriginal.getWidth() + d.candidates.get(i)*barWidth, 
//      		   	       0, 
//      		           2, 
//      		           2*bOriginal.getHeight());
//         }
//     }
	
// 	protected void useSize(Dimension s) {}
	
// 	public static Boolean shouldLoadInParallel() { return true; }
// }