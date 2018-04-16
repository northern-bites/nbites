package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.image.BufferedImage;
import java.util.LinkedList;
import java.util.List;
import java.util.ArrayList;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JCheckBox;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

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


public class SequenceView extends ViewParent implements ChangeListener,ItemListener, KeyListener {
    private ArrayList<Integer[]> blackSpotCoordinates = new ArrayList<Integer[]>();
    private ArrayList<Integer[]> whiteSpotCoordinates = new ArrayList<Integer[]>();
    static final int NUM_PARAMS = 8; // update as new params are added
    static int displayParams[] = new int[NUM_PARAMS];
    static boolean firstLoad = true;
    static boolean diffImageExists = true;
    
    private static int static_success_count = 0;
    private static int static_failure_count = 0;
    private static int static_total_count = 0;
    
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
    private JPanel radioPanel;
    private JCheckBox showBlackSpots;
    private JCheckBox showWhiteSpots;
    private JCheckBox showColor;
    private JSpinner filterDark;
    private JSpinner greenDark;
    private JSpinner filterBrite;
    private JSpinner greenBrite;
    private int currentThreshold;
    private int NOTHRESH = 0, BINARY= 1,LINEAR= 2,QUADRATIC= 3;
    private JSpinner slopeLinear;
    private JSpinner lowerThreshold;
    private JSpinner divideQuadratic;
    
    private JRadioButton none;
    private JRadioButton binary;
    private JRadioButton linear;
    private JRadioButton quadratic;;
    
    
    
    private boolean showBlack;
    private boolean showWhite;
    private boolean showColorBool;
    
    
    static final int DEFAULT_WIDTH = 320;
    static final int DEFAULT_HEIGHT = 240;
    float color = 0f;
    float colorIncr = .00036f;
    
    public SequenceView() {
        super();
        setLayout(null);
        addKeyListener((java.awt.event.KeyListener) this);
        this.images = null;
        
        showBlack = false;
        showWhite = false;
        showColorBool = false;
        if (firstLoad) {
            for (int i = 0; i < NUM_PARAMS; i++) {
                displayParams[i] = 0;
            }
            // ideally these would actually be read from NBCROSS
            displayParams[0]  = 104;
            displayParams[1]  = 12;
            displayParams[2] = 130;
            displayParams[3] = 80;
            displayParams[4] = 0;
            displayParams[5] =7;
            displayParams[6] = 9999;
            displayParams[7] = 7;
            firstLoad = false;
            
        }
        
        currentThreshold = 0;
        createAdjusters();
    }
    
    
    
    
    private static final Color[] angleMap = initColorMap();
    
    private static Color[] initColorMap(){
        
        Color[] ret = new Color[256];
        
        
        
        Color[] top = {Color.MAGENTA,
            Color.BLUE,
            Color.CYAN,
            Color.GREEN,
            Color.YELLOW,
            Color.ORANGE,
            Color.RED,
            Color.WHITE,
        };
        
        assert(top.length == 8);
        
        for (int i = 0; i < 256; ++i) {
            int bi = i / 32;
            int ni = (bi + 1) % 8;
            
            int dist = i % 32;
            
            Color bc = top[bi];
            Color nc = top[ni];
            
            int dr = nc.getRed() - bc.getRed();
            int dg = nc.getGreen() - bc.getGreen();
            int db = nc.getBlue() - bc.getBlue();
            
            Color tc = new Color(
                                 bc.getRed() + (dist * dr) / 32,
                                 bc.getGreen() + (dist * dg) / 32,
                                 bc.getBlue() + (dist * db) / 32
                                 );
            ret[i] = tc;
        }
        
        return ret;
    };
    
    private Color gradientColorFor(Color pixel) {
        if (pixel.getBlue() != pixel.getGreen() || pixel.getGreen() != pixel.getRed()) {
            throw new RuntimeException("ahhhhhh!");
        }
        
        final int brightness = pixel.getBlue();
        
        assert(brightness < 256);
        return angleMap[brightness];
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
                int offX =  (int)frame_width/3/3;
                //                int offX = (int)frame_width/3-350;
                //                if(offX< 0){
                //                    offX = 0;
                //                }
                int offY = offsetY+(int)frame_height/3+ 10;
                //                if(offY<offsetY){
                //                    offY =offsetY+ 20;
                //                }
                diff_img_width = (int)frame_width/2;
                diff_img_height= (int)frame_height/2;
                
                checkBoxPanel.setBounds(offX+diff_img_width+50,offY, 200, 50);
                paramPanel.setBounds(offX+diff_img_width+50,offY+50, 300, 100);
                radioPanel.setBounds(offX+diff_img_width+50,offY+150, 300, 150);
                if(showColor.isSelected()){
                    System.out.println("HERE");
                    for (int x = 0; x < diffImage.getWidth(); ++x) {
                        for (int y = 0; y < diffImage.getHeight(); ++y){
                            Color c = new Color(diffImage.getRGB(x, y));
                            
                            //                            if(diffImage.getRGB(x,y) != 0x000000) {
                            //                                diffImage.setRGB(x, y, Color.RED.getRGB());
                            //                            }
                            
                            diffImage.setRGB(x, y, this.gradientColorFor(c).getRGB());
                            //
                            //                            if(c.getGreen() != 0x000000) {
                            //                                diffImage.setRGB(x, y, Color.RED.getRGB());
                            //                            }
                        }
                    }
                }
                if(showBlackSpots.isSelected()){
                    drawSpots(blackSpotCoordinates, Color.RED,false);
                }
                if(showWhiteSpots.isSelected()) {
                    drawSpots(whiteSpotCoordinates,Color.YELLOW,true);
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
        //lay out what to do
        
        for(int i = 0; i<NUM_PARAMS;i++){ //key is param0 or param1
            String str = "param"+i;
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
                        whiteSpotCoordinates.add(coordinate);
                    }
                    showColor.setText("Show gradient colors");
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
        //CHECKBOXES
        checkBoxPanel = new JPanel();
        checkBoxPanel.setLayout(new GridLayout(0, 1)); // 0 rows, 1 column
        checkBoxPanel.setBorder(BorderFactory.createLineBorder(Color.black));
        
        showBlackSpots = new JCheckBox();
        showWhiteSpots = new JCheckBox();
        showColor = new JCheckBox();
        
        // add their listeners
        showBlackSpots.addItemListener(this);
        showWhiteSpots.addItemListener(this);
        showColor.addItemListener(this);
        
        checkBoxPanel.add(showBlackSpots);
        checkBoxPanel.add(showWhiteSpots);
        checkBoxPanel.add(showColor);
        
        showColor.setSelected(true);
        showBlackSpots.setSelected(false);
        showWhiteSpots.setSelected(true);
        
        add(checkBoxPanel);
        
        
        //RADIO BUTTONS
        radioPanel = new JPanel();
        radioPanel.setLayout(new GridLayout(4, 3)); // 0 rows, 1 column
        radioPanel.setBorder(BorderFactory.createLineBorder(Color.black));
        
        
        none = new JRadioButton("No Thresholding");
        none.setSelected(true);
        binary = new JRadioButton("Binary");
        linear = new JRadioButton("Linear");
        quadratic = new JRadioButton("Quadratic");
        
        
        none.addItemListener(this);
        binary.addItemListener(this);
        linear.addItemListener(this);
        quadratic.addItemListener(this);
        
        none.addChangeListener(this);
        binary.addChangeListener(this);
        linear.addChangeListener(this);
        quadratic.addChangeListener(this);
        
        none.setRolloverEnabled(false);
        binary.setRolloverEnabled(false);
        linear.setRolloverEnabled(false);
        quadratic.setRolloverEnabled(false);
        
        
        
        ButtonGroup bG = new ButtonGroup();
        bG.add(none);
        bG.add(binary);
        bG.add(linear);
        bG.add(quadratic);
        
        radioPanel.add(none);
        radioPanel.add(new JLabel(""));  // for empty cell
        radioPanel.add(new JLabel(""));  // for empty cell
        
        radioPanel.add(binary);
        radioPanel.add(new JLabel(""));  // for empty cell
        radioPanel.add(new JLabel(""));  // for empty cell
        
        
        radioPanel.add(linear);
        JPanel linearSpinner = new JPanel();
        linearSpinner.setLayout(new GridLayout(1, 0));
        SpinnerModel slopeLinearModel = new SpinnerNumberModel(7, 0, 100, 1);
        slopeLinear = addLabeledSpinner(linearSpinner, "slope:",
                                        slopeLinearModel);
        slopeLinear.addChangeListener(this);
        radioPanel.add(linearSpinner);
        
        
        JPanel lowerThreshSpinner = new JPanel();
        lowerThreshSpinner.setLayout(new GridLayout(1, 0));
        SpinnerModel lowerThreshModel = new SpinnerNumberModel(9999, 0, 9999, 1);
        lowerThreshold = addLabeledSpinner(lowerThreshSpinner, "lower threshold:",
                                           lowerThreshModel);
        lowerThreshold.addChangeListener(this);
        radioPanel.add(lowerThreshSpinner);
        
        
        
        
        radioPanel.add(quadratic);
        
        JPanel quadraticSpinner = new JPanel();
        quadraticSpinner.setLayout(new GridLayout(1, 0));
        SpinnerModel divideQuadraticModel = new SpinnerNumberModel(7, 0, 100, 1);
        divideQuadratic = addLabeledSpinner(quadraticSpinner, "divide:",
                                            divideQuadraticModel);
        divideQuadratic.addChangeListener(this);
        radioPanel.add(quadraticSpinner);
        
        radioPanel.add(new JLabel(""));  // for empty cell
        
        
        add(radioPanel);
        
        
        
        //PARAM PANEL
        paramPanel = new JPanel();
        paramPanel.setLayout(new GridLayout(0, 2));
        paramPanel.setBorder(BorderFactory.createLineBorder(Color.black));
        
        SpinnerModel filterDarkModel = new
        SpinnerNumberModel(104, 0, 512, 4);
        SpinnerModel greenDarkModel = new
        SpinnerNumberModel(12, 0, 255, 4);
        SpinnerModel filterBriteModel = new
        SpinnerNumberModel(130, 0, 512, 4);
        SpinnerModel greenBriteModel = new
        SpinnerNumberModel(80, 0, 255, 4);
        
        
        
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
        } else if (source == showWhiteSpots) {
            showWhite = !showWhite;
        } else if (source == showColor){
            showColorBool = !showColorBool;
        } else if(source == none) {
            currentThreshold = NOTHRESH;
            //            System.out.println("Threshold is now "+source);
        } else if(source == binary) {
            currentThreshold = BINARY;
            //            System.out.println("Threshold is now "+source);
        } else if (source == linear) {
            currentThreshold = LINEAR;
            //            System.out.println("Threshold is now "+source);
        }else if (source == quadratic) {
            currentThreshold = QUADRATIC;
            //            System.out.println("Threshold is now "+source);
        }   else if(source == filterDark||source == greenDark||source == filterBrite||source == greenBrite) {
            //            System.out.println("There was a change in"+source);
        } else if(source == slopeLinear||source == divideQuadratic||source == lowerThreshold) {
            //            System.out.println("There was a change in"+source);
            
        } else {
            Debug.error("Error: item state listener: %s!", source);
        }
        
        
        repaint();
    }
    public void stateChanged(ChangeEvent e) {
        displayParams[0] = ((Integer)filterDark.getValue()).intValue();
        displayParams[1] = ((Integer)greenDark.getValue()).intValue();
        displayParams[2] = ((Integer)filterBrite.getValue()).intValue();
        displayParams[3] = ((Integer)greenBrite.getValue()).intValue();
        displayParams[4] = currentThreshold;
        displayParams[5] = ((Integer)slopeLinear.getValue()).intValue();
        displayParams[6] = ((Integer)lowerThreshold.getValue()).intValue();
        displayParams[7] = ((Integer)divideQuadratic.getValue()).intValue();
        
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
    private void drawSpots(ArrayList<Integer[]> spotCoordinates, Color color,boolean drawNumbers) {
        Graphics2D graph = diffImage.createGraphics();
        
        int width = diffImage.getWidth();
        int height = diffImage.getHeight();
        
        
        for(int i = 0; i < spotCoordinates.size(); i++) {
            graph.setColor(color);
            Integer coordinate[] = new Integer[3];
            coordinate = spotCoordinates.get(i);
            int midX = coordinate[0] + width/2;
            int midY = -coordinate[1] + height/2;
            int len =coordinate[2];
            int x = (midX-len/2);//+offsetX;
            int y = (midY-len/2);//+offsetY;
            
            //            Ellipse2D.Double ellipse = new Ellipse2D.Double(x, y,len*2, len*2);
            Ellipse2D.Double ellipse = new Ellipse2D.Double(x, y,len, len);
            graph.draw(ellipse);
            if(drawNumbers){
                graph.setColor(Color.RED);
                graph.drawString(Integer.toString(i),x,y);
            }
            
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
    //    public void keyPressed(KeyEvent e){
    
    
    //    }
    //    @Override
    public void keyPressed(KeyEvent arg0) {
        // TODO Auto-generated method stub
        int keyCode = arg0.getKeyCode();
        if(keyCode == KeyEvent.VK_MINUS){
            static_success_count++;
            static_total_count++;
            System.out.println("+: "+static_success_count+" -:"+static_failure_count+" total: "+ static_total_count);
        } else if(keyCode == KeyEvent.VK_PLUS){
            static_failure_count++;
            static_total_count++;
            System.out.println("+: "+static_success_count+" -:"+static_failure_count+" total: "+ static_total_count);
            
        }
    }
    
    //    @Override
    public void keyReleased(com.jogamp.newt.event.KeyEvent arg0) {
        // TODO Auto-generated method stub
        
    }
}

