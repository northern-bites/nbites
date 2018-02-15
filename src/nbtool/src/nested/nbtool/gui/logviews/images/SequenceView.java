package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Container;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;
import java.util.LinkedList;
import java.util.List;
import java.util.ArrayList;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import javax.swing.JCheckBox;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import javax.swing.JPanel;
import java.awt.GridLayout;
import javax.swing.JSpinner;
import javax.swing.JLabel;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.log.Log;
import nbtool.data.json.JsonArray;
import nbtool.data.json.Json;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.Y16Image;

import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;


public class SequenceView extends ViewParent implements MouseMotionListener, ChangeListener,ItemListener {
    private ArrayList<Integer[]> blackSpotCoordinates = new ArrayList<Integer[]>();
    private ArrayList<Integer[]> whiteSpotCoordinates = new ArrayList<Integer[]>();
    static final int NUM_PARAMS = 4; // update as new params are added
    static int displayParams[] = new int[NUM_PARAMS];
    static boolean firstLoad = true;
    static boolean diffImageExists = true;

    private BufferedImage images[];
    private BufferedImage diffImage;
    private BufferedImage originalDiffImage = null;
    private int numImages;
    private double frame_width;
    private double frame_height;
    private int diff_img_width;
    private int diff_img_height;
    private JPanel checkBoxPanel;
    private JPanel paramPanel;
    private JCheckBox showBlackSpots;
    private JCheckBox showWhiteSpots;
    private JSpinner filterDark;
    private JSpinner greenDark;
    private JSpinner filterBrite;
    private JSpinner greenBrite;
    
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
        if (firstLoad) {
            Debug.warn("this is considered a first load");
            for (int i = 0; i < NUM_PARAMS; i++) {
                displayParams[i] = 0;
            }
            // ideally these would actually be read from NBCROSS
            displayParams[0]  = 104;
            displayParams[1]  = 12;
            displayParams[2] = 130;
            displayParams[3] = 80;
            firstLoad = false;
            
        }
        createAdjusters();

    }
    
    
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        int offsetY = 5;
        int offsetX = 5;
        frame_width = this.getSize().getWidth();
        frame_height = this.getSize().getHeight();

        
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
                
                int offX = (int)frame_width/3-300;
                int offY = offsetY+(int)frame_width/3-30;
                
                diff_img_width = (int)frame_width/2;
                diff_img_height= (int)frame_height/2;
                
                checkBoxPanel.setBounds(offX+diff_img_width+50,offY, 200, 50);
                paramPanel.setBounds(offX+diff_img_width+50,offY+50, 300, 100);
                    
                if(showBlackSpots.isSelected()){
                    drawSpots(blackSpotCoordinates, Color.RED);
                }
                if(showWhiteSpots.isSelected()) {
                    drawSpots(whiteSpotCoordinates,Color.YELLOW);
                }
               
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
        System.out.println("sendtonbcross called");
        List<Log> logs = new LinkedList<>();
        //lay out what to do

        for(int i = 0; i<NUM_PARAMS;i++){ //key is param0 or param1 
            String str = "param"+i;
            Debug.warn("%s: %d",str,displayParams[i]);
            displayedLog.topLevelDictionary.put(str, displayParams[i]);
        }
        
        logs.add(displayedLog);
        for (int i = 0; i < alsoSelected.size(); ++i) {
            logs.add(alsoSelected.get(i));
        }
                
        assert(ci.tryAddCall(new IOFirstResponder(){
            
            @Override
            public void ioReceived(IOInstance inst, int ret, Log... out) {
                
                assert(out != null);
                assert(out.length > 0);
                if(out.length > 0 && numImages > 1) {
                    Debug.warn("GOT IO RECEIVED ABOUT TO PARSE");
                    int ywidth = 320;
                    int yheight = 240;
                    originalDiffImage = new Y16Image(ywidth,yheight, out[0].blocks.get(0).data).toBufferedImage();
                    
                    setDiffImage();
                    blackSpotCoordinates.clear();
                    whiteSpotCoordinates.clear();
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
//                        System.out.println("W:("+coordinate[0]+","+coordinate[1]+"):"+innerDiam);
                        whiteSpotCoordinates.add(coordinate);
                    }
                    showBlackSpots.setText("Show Black Spots: "+blackSpotCoordinates.size());
                    showWhiteSpots.setText("Show White Spots: "+ whiteSpotCoordinates.size());
                }
                repaint();
            }
            
            @Override
            public boolean ioMayRespondOnCenterThread(IOInstance inst) {return false;}
            
            @Override
            public void ioFinished(IOInstance instance) {}
            
        }, "Vision", logs.toArray(new Log[0])));
    }
    
    private void createAdjusters() {
        checkBoxPanel = new JPanel();
        checkBoxPanel.setLayout(new GridLayout(0, 1)); // 0 rows, 1 column
        showBlackSpots = new JCheckBox();
        showWhiteSpots = new JCheckBox();
        
        // add their listeners
        showBlackSpots.addItemListener(this);
        showWhiteSpots.addItemListener(this);
        
        checkBoxPanel.add(showBlackSpots);
        checkBoxPanel.add(showWhiteSpots);
        
        showBlackSpots.setSelected(false);
        showWhiteSpots.setSelected(false);
        add(checkBoxPanel);

        
        SpinnerModel filterDarkModel = new
        SpinnerNumberModel(104, 0, 512, 4);
        SpinnerModel greenDarkModel = new
        SpinnerNumberModel(12, 0, 255, 4);
        SpinnerModel filterBriteModel = new
        SpinnerNumberModel(130, 0, 512, 4);
        SpinnerModel greenBriteModel = new
        SpinnerNumberModel(80, 0, 255, 4);
        
        
        paramPanel = new JPanel();
        paramPanel.setLayout(new GridLayout(0, 2));
        filterDark = addLabeledSpinner(paramPanel, "filterThresholdDark",
                                       filterDarkModel);
        greenDark = addLabeledSpinner(paramPanel, "greenThresholdDark",
                                      greenDarkModel);
        filterBrite = addLabeledSpinner(paramPanel, "filterThresholdBrite",
                                        filterBriteModel);
        greenBrite = addLabeledSpinner(paramPanel, "greenThresholdBrite",
                                       greenBriteModel);
        greenBrite.addChangeListener(this);
        filterBrite.addChangeListener(this);
        greenDark.addChangeListener(this);
        filterDark.addChangeListener(this);
        add(paramPanel);

    }
    public void itemStateChanged(ItemEvent e) {
        Object source = e.getSource();
        if (source == showBlackSpots) {
            showBlack = !showBlack;
            System.out.println("Black is"+showBlack);
        } else if (source == showWhiteSpots) {
            showWhite = !showWhite;
            System.out.println("White is"+showWhite);
        } else if(source == filterDark||source == greenDark||source == filterBrite||source == greenBrite) {
            System.out.println("There was a change in"+source);

        } else {
            Debug.error("Error: item state listener: %s!", source);
        }
    

        repaint();
    }
    public void stateChanged(ChangeEvent e) {
        System.out.println("DID YOU GET CALLED? in seq");
        displayParams[0] = ((Integer)filterDark.getValue()).intValue();
        displayParams[1] = ((Integer)greenDark.getValue()).intValue();
        displayParams[2] = ((Integer)filterBrite.getValue()).intValue();
        displayParams[3] = ((Integer)greenBrite.getValue()).intValue();
        sendToNbCross();
    }

    protected JSpinner addLabeledSpinner(Container c, String label,
                                         SpinnerModel model) {
        JLabel l = new JLabel(label);
        c.add(l);
        JSpinner spinner = new JSpinner(model);
        l.setLabelFor(spinner);
        c.add(spinner);
        return spinner;
    }
    private void drawSpots(ArrayList<Integer[]> spotCoordinates, Color color) {
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
