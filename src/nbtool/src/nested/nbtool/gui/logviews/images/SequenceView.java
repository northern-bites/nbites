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
import javax.swing.JCheckBox;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import javax.swing.JPanel;
import java.awt.GridLayout;


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
import nbtool.util.Debug;


public class SequenceView extends ViewParent implements MouseMotionListener, ItemListener {
    private ArrayList<Integer[]> blackSpotCoordinates = new ArrayList<Integer[]>();
    private ArrayList<Integer[]> whiteSpotCoordinates = new ArrayList<Integer[]>();

    private BufferedImage images[];
    private BufferedImage diffImage;
    private BufferedImage originalDiffImage = null;
    private int numImages;
    private double frame_width;
    private double frame_height;
    private int diff_img_width;
    private int diff_img_height;
    JPanel checkBoxPanel;
    private JCheckBox showBlackSpots;
    private JCheckBox showWhiteSpots;
    private boolean showBlack;
    private boolean showWhite;

    static final int DEFAULT_WIDTH = 320;
    static final int DEFAULT_HEIGHT = 240;
    
    
    public SequenceView() {
        super();
        setLayout(null);
        
        this.images = null;

        showBlack = false;
        showWhite = false;
    }
    
    
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        int offsetY = 5;
        int offsetX = 5;
        frame_width = this.getSize().getWidth();
        frame_height = this.getSize().getHeight();
        int offX = (int)frame_width/3-300;
        int offY = (int)frame_width/3-30;
        
        setDiffImage();

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

                if(showBlackSpots.isSelected()){
                    drawSpots(blackSpotCoordinates, Color.RED);
                }
                if(showWhiteSpots.isSelected()) {
                    drawSpots(whiteSpotCoordinates,Color.YELLOW);
                }
                checkBoxPanel.setBounds(offX+diff_img_width+50,offY, 200, 200);

                g.drawImage(diffImage,offX, offY,diff_img_width ,diff_img_height , null);

            }
        } else if(images.length<16){
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
        } else {
            Debug.warn("NO MORE THAN 16 PHOTOS DUDE!");
        }
    }

    
    @Override
    public void mouseDragged(MouseEvent e) {}
    @Override
    public void mouseMoved(MouseEvent e) {}
    
    private void setDiffImage() {
        if (originalDiffImage == null) {
            diffImage = null;
        } else {
            BufferedImage convertedImg = new BufferedImage(originalDiffImage.getWidth(), originalDiffImage.getHeight(), BufferedImage.TYPE_INT_ARGB);
            
            convertedImg.getGraphics().drawImage(originalDiffImage, 0, 0, null);
            
            diffImage = convertedImg;
        }
    }
    
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
                    
                    ywidth = 320;
                    yheight = 240;
                    
                    originalDiffImage = new Y16Image(ywidth,yheight, out[0].blocks.get(0).data).toBufferedImage();
                    
                    setDiffImage();
                    
                    JsonArray blkpts = out[0].blocks.get(0).dict.get("blackspots").asArray();
                    for (Json.JsonValue value : blkpts) {
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
                        
                        System.out.println("B:("+coordinate[0]+","+coordinate[1]+"):"+innerDiam);

                        blackSpotCoordinates.add(coordinate);
                    }
                    
                    
                    JsonArray whtpts = out[0].blocks.get(0).dict.get("whitespots").asArray();
                    for (Json.JsonValue value : whtpts) {
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
                        System.out.println("W:("+coordinate[0]+","+coordinate[1]+"):"+innerDiam);
                        whiteSpotCoordinates.add(coordinate);
                    }
                }
                createCheckBoxes();

                repaint();
            }
            
            @Override
            public boolean ioMayRespondOnCenterThread(IOInstance inst) {return false;}
            
            @Override
            public void ioFinished(IOInstance instance) {}
            
        }, "Vision", logs.toArray(new Log[0])));
    }
    private void createCheckBoxes() {
        checkBoxPanel = new JPanel();
        checkBoxPanel.setLayout(new GridLayout(0, 1)); // 0 rows, 1 column
        showBlackSpots = new JCheckBox("Show Black Spots: "+blackSpotCoordinates.size());
        showWhiteSpots = new JCheckBox("Show White Spots: "+ whiteSpotCoordinates.size());
        
        // add their listeners
        showBlackSpots.addItemListener(this);
        showWhiteSpots.addItemListener(this);
        
        checkBoxPanel.add(showBlackSpots);
        checkBoxPanel.add(showWhiteSpots);
        
        showBlackSpots.setSelected(false);
        showWhiteSpots.setSelected(false);
        
        add(checkBoxPanel);
    }
    public void itemStateChanged(ItemEvent e) {
        Object source = e.getSource();
        if (source == showBlackSpots) {
            showBlack = !showBlack;
            System.out.println("Black is"+showBlack);

        } else if (source == showWhiteSpots) {
            showWhite = !showWhite;
            System.out.println("White is"+showWhite);
        } else {
            Debug.warn("Error: item state listener in sequence view!");

        }
        
//        Object source = e.getItemSelectable();
//        if (e.getStateChange() == ItemEvent.SELECTED) {
//            if (source == showBlackSpots) {
//                showBlack = true;
//            } else if (source == showWhiteSpots) {
//                showWhite = true;
//            } else {
//                Debug.warn("Error: item state listener in sequence view SELECTED!!");
//            }
//        } else if (e.getStateChange() == ItemEvent.DESELECTED) {
//            if (source == showBlackSpots) {
//                showBlack = false;
//            } else if (source == showWhiteSpots) {
//                showWhite = false;
//            } else {
//                Debug.warn("Error: item state listener in sequence view DESELECTED!!");
//            }
//        }

        repaint();
    }
    
    private void drawSpots(ArrayList<Integer[]> spotCoordinates, Color color) {
        int multiplier = 2;
        Graphics2D graph = diffImage.createGraphics();
        
        int width = diffImage.getWidth();
        int height = diffImage.getHeight();

//        Graphics2D graph = (Graphics2D)g;
        graph.setColor(color);
        
        for(int i = 0; i < spotCoordinates.size(); i++) {
            Integer coordinate[] = new Integer[3];
            coordinate = spotCoordinates.get(i);
            int midX = coordinate[0] + width/2;
            int midY = -coordinate[1] + height/2;
            int len =coordinate[2];
            int x = (midX-len);//+offsetX;
            int y = (midY-len);//+offsetY;
            
//            int x = coordinate[0]-len;
//            int y = coordinate[1]-len;
            double ang1 = 0.0;
            
            Ellipse2D.Double ellipse = new Ellipse2D.Double(x, y,len*2, len*2);
            graph.draw(ellipse);
        }
        
        graph.dispose();
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
