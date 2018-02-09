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
import java.util.ArrayList;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.awt.geom.AffineTransform;

import nbtool.data.calibration.ColorParam;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.data.json.Json;
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
    ArrayList<Integer[]> spotCoordinates = new ArrayList<Integer[]>();
    BufferedImage images[];
    BufferedImage diffImage;
    int numImages;
    //    YUYV8888Image yuvImages[];
    double frame_width;
    double frame_height;
    int diff_img_width;
    int diff_img_height;
    
    static final int DEFAULT_WIDTH = 320;
    static final int DEFAULT_HEIGHT = 240;
    
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        int offsetY = 5;
        int offsetX = 5;
        frame_width = this.getSize().getWidth();
        frame_height = this.getSize().getHeight();
        
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
            if(diffImage != null){
                diff_img_width = (int)frame_width/2;
                diff_img_height= (int)frame_height/2;
                int offX = (int)frame_width/4;
                int offY = (int)frame_width/3;

                g.drawImage(diffImage,offX, offY,diff_img_width ,diff_img_height , null);
                drawSpots(g,offX, offY);

            }
        } else {
            for (BufferedImage img : images) {
                if (img != null) {
                    int imageWidth = (int)frame_width/4-10;
                    int imageHeight = (int)frame_height/4;
                    
                    g.drawImage(img,offsetX, offsetY,imageWidth ,imageHeight , null);
                    
                    if (offsetX+imageWidth >= frame_width-10) {
                        offsetY += 10+imageHeight;
                        offsetX = 5;
                    } else {
                        offsetX +=imageWidth+ 10;
                    }
                }
            }
            if(diffImage != null){
                int imageWidth = (int)frame_width/4;
                int imageHeight = (int)frame_height/4;
                g.drawImage(diffImage,offsetX, offsetY,imageWidth ,imageHeight , null);
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
                if(out.length > 0 && numImages > 1) {
                    Debug.warn("GOT IO RECEIVED ABOUT TO PARSE");
                    int ywidth, yheight;
                    
//                    System.out.println("Height: "+outerThis.yuvImages[0].height+" width: "+outerThis.yuvImages[0].width);
                    
                    //                    if (outerThis.yuvImages[0].height == 480) {
                    //                        ywidth=640; yheight = 480;
                    //                    } else {
                    //                        ywidth=320; yheight = 240;
                    //                    }
                    ywidth = 320;
                    yheight = 240;
                    diffImage = new Y16Image(ywidth,yheight, out[0].blocks.get(0).data).toBufferedImage();
                    
                    JsonArray pnts = out[0].blocks.get(0).dict.get("spots").asArray();
                    for (Json.JsonValue value : pnts) {
                        Integer coordinate[] = new Integer[3];
                        Float _x = value.asObject().get("x").asNumber().asFloat();
                        Float _y = value.asObject().get("y").asNumber().asFloat();
                        Float _innerDiam = value.asObject().get("innerDiam").asNumber().asFloat();
                        Integer x = (int)(float)_x;
                        Integer y = (int)(float)_y;
                        Integer innerDiam = (int)(float)_innerDiam;


                        coordinate[0]=x;
                        coordinate[1]=y;
                        coordinate[2]=innerDiam;
                        
                        System.out.println("("+coordinate[0]+","+coordinate[1]+"):"+innerDiam);

                        spotCoordinates.add(coordinate);
                    }
                }
                repaint();
            }
            
            @Override
            public boolean ioMayRespondOnCenterThread(IOInstance inst) {return false;}
            
            @Override
            public void ioFinished(IOInstance instance) {}
            
        }, "Vision", logs.toArray(new Log[0])));
    }
    private void drawSpots(Graphics g,int offsetX, int offsetY){
        int multiplier = 2;
        
        Graphics2D graph = (Graphics2D)g;
        g.setColor(Color.RED);

        for(int i = 0; i < spotCoordinates.size(); i++) {
            Integer coordinate[] = new Integer[3];
            coordinate = spotCoordinates.get(i);
            int midX = coordinate[0] + diff_img_width/2;
            int midY = -coordinate[1] + diff_img_height/2;
            int len =coordinate[2];
            int x = (midX-len);//+offsetX;
            int y = (midY-len);//+offsetY;
            
//            int x = coordinate[0]+offsetX;
//            int y = coordinate[1]+offsetY;
            double ang1 = 0.0;
            Ellipse2D.Double ellipse = new Ellipse2D.Double(x, y,
                                                            len*2, len*2);//Ellipse2D.Double(x,y,10,10);
            
            
            
            Shape rotated = (AffineTransform.getRotateInstance(ang1,midX+offsetX , midY+offsetY).createTransformedShape(ellipse));
//            graph.draw(rotated);
            graph.draw(ellipse);

        }
    }
    
    
    @Override
    public void setupDisplay() {
        sendToNbCross();
        this.numImages = alsoSelected.size()+1;
        this.images = new BufferedImage[numImages];
        this.images[0] = displayedLog.blocks.get(0).parseAsYUVImage().toBufferedImage();
        int i = 0;
        while(i < alsoSelected.size()){
            this.images[i+1] = alsoSelected.get(i).blocks.get(0).parseAsYUVImage().toBufferedImage();
            i++;
        }
        
//        this.yuvImages = new YUYV8888Image[this.images.length];
//        this.yuvImages[0] = displayedLog.blocks.get(0).parseAsYUVImage();
//        this.yuvImages[i+1] = alsoSelected.get(i).blocks.get(0).parseAsYUVImage();

        //        for (int i = 0; i < alsoSelected.size(); ++i) {
        //            this.yuvImages[i+1] = alsoSelected.get(i).blocks.get(0).parseAsYUVImage();
        //            this.images[i+1] = alsoSelected.get(i).blocks.get(0).parseAsYUVImage().toBufferedImage();
        //        }
        
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
