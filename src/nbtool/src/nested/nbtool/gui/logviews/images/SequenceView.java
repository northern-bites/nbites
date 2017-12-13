package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;
import java.util.LinkedList;
import java.util.List;

import nbtool.data.calibration.ColorParam;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.gui.logviews.misc.VisionView;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.images.Y16Image;
import nbtool.images.YUYV8888Image;

import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

//this is an important file

public class SequenceView extends ViewParent implements MouseMotionListener {
	BufferedImage images[];
	YUYV8888Image yuvImages[];
	
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		int offsetY = 5;
		int offsetX = 5;
		double frame_width = this.getSize().getWidth();
		double frame_height = this.getSize().getHeight();
		
		if (images.length < 4) {
			 for (BufferedImage img : images) {
		        	if (img != null) {
		        		int imageWidth = (int)frame_width/3;
		        		int imageHeight = (int)frame_height/3;
		        		
		        		g.drawImage(img,offsetX, offsetY,imageWidth ,imageHeight , null);


		        		if (offsetX+imageWidth >= frame_width) {
		        			offsetY += 10+imageHeight;
		        			offsetX = 5;
		        		} else {
		            		offsetX +=imageWidth+ 10;

		        		}

		        	}
		        }
		} else {
			 for (BufferedImage img : images) {
		        	if (img != null) {
		        		int imageWidth = (int)frame_width/4;
		        		int imageHeight = (int)frame_height/4;
		        		
		        		g.drawImage(img,offsetX, offsetY,imageWidth ,imageHeight , null);


		        		if (offsetX+imageWidth >= frame_width) {
		        			offsetY += 10+imageHeight;
		        			offsetX = 5;
		        		} else {
		            		offsetX +=imageWidth+ 10;

		        		}

		        	}
		        }
		}

       
    }
	
	public SequenceView() {
		super();
		setLayout(null);
		
		this.images = null;
	}

	@Override
	public void mouseDragged(MouseEvent e) {}
	@Override
	public void mouseMoved(MouseEvent e) {}
	

	private void sendToNbCross() {
		CrossInstance ci = CrossServer.instanceByIndex(0);
		if (ci == null) {
			return;
		}
		
		List<Log> logs = new LinkedList<>();
		logs.add(displayedLog);
		for (int i = 0; i < alsoSelected.size(); ++i) {
			logs.add(alsoSelected.get(i));
		}

		final SequenceView outerThis = this;
		
		assert(ci.tryAddCall(new IOFirstResponder(){

			@Override
			public void ioReceived(IOInstance inst, int ret, Log... out) {
					
				assert(out != null);
				assert(out.length > 0);
				Debug.warn("GOT IO RECEIVED");
				//for (int i = 0; i < out.length; ++i) {
				if(out.length > 0) {//todo
					//images[i] = out[i].blocks.get(0).parseAsYUVImage().toBufferedImage();
					Debug.warn("GOT IO RECEIVED ABOUT TO PARSE");

					int ywidth, yheight;

					System.out.println("Height: "+outerThis.yuvImages[0].height+" width: "+outerThis.yuvImages[0].width);
					//System.out.println(outerThis.yuvImages[0].size());

					if (outerThis.yuvImages[0].height == 480) {
						ywidth=640; yheight = 480;
					} else {
						ywidth=320; yheight = 240;

					}

					ywidth = 320;
					yheight = 240;

					//new YImage //todo
					images[2] = new Y16Image(
							ywidth,
							yheight,
							out[0].blocks.get(0).data).toBufferedImage(); 
					//out[i].blocks.get(0).toBufferedImage();
				}
				repaint();
				
//				assert(out[0].logClass.equals("VisionReturn"));
//				latestVisionLog = out[0];
//				outer.ioReceived(inst, ret, out);
			}

			@Override
			public boolean ioMayRespondOnCenterThread(IOInstance inst) {
				return false;
			}

			@Override
			public void ioFinished(IOInstance instance) {
				// TODO Auto-generated method stub
				
			}

		}, "Vision", logs.toArray(new Log[0])));
	}


	@Override
	public void setupDisplay() {
		sendToNbCross();
//		System.out.print(alsoSelected.size());
		this.images = new BufferedImage[alsoSelected.size()+1];
		this.yuvImages = new YUYV8888Image[this.images.length];
		
		this.yuvImages[0] = displayedLog.blocks.get(0).parseAsYUVImage();

		this.images[0] = displayedLog.blocks.get(0).parseAsYUVImage().toBufferedImage();
//
		for (int i = 0; i < alsoSelected.size(); ++i) {
//			this.images[i] = displayedLog.blocks.get(i).parseAsYUVImage().toBufferedImage();
			this.yuvImages[i+1] = alsoSelected.get(i).blocks.get(0).parseAsYUVImage();
			this.images[i+1] = alsoSelected.get(i).blocks.get(0).parseAsYUVImage().toBufferedImage();
		}
		
//		for (BufferedImage image : this.images) {
//			for (int x = 0; x < image.getWidth(); ++x) {
//				for (int y = 0; y < image.getHeight(); ++y){
//					Color c = new Color(image.getRGB(x, y));
//					if (c.getGreen() > 100) {
//						image.setRGB(x, y, Color.PINK.getRGB());
//					}
//				}
//			}
//		}
		
		
		
		
		if(alsoSelected.size() == 16){
			Debug.warn("This view can only can display 16 images at a time.");
		}
		repaint();
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint(),
				SharedConstants.LogClass_YUVImage()};
	}
}
