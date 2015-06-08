package nbtool.gui.logviews.misc;

import java.awt.Graphics;
import java.awt.Dimension;
import java.awt.image.BufferedImage;

import nbtool.images.Y8image;
import nbtool.images.Y16image;
import nbtool.images.ColorSegmentedImage;

import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.util.Utility;
import nbtool.util.ColorLoader;

public class ImageConverterView extends ViewParent implements IOFirstResponder {
    private BufferedImage yImage;
    private BufferedImage whiteImage;
    private BufferedImage greenImage;
    private BufferedImage orangeImage;
    private BufferedImage segmentedImage;

    private JSlider wDarkU;
    private JSlider wDarkV;
    private JSlider wLightU;
    private JSlider wLightV;
    private JSlider wFuzzyU;
    private JSlider wFuzzyV;

    private JSlider gDarkU;
    private JSlider gDarkV;
    private JSlider gLightU;
    private JSlider gLightV;
    private JSlider gFuzzyU;
    private JSlider gFuzzyV;

    private JSlider oDarkU;
    private JSlider oDarkV;
    private JSlider oLightU;
    private JSlider oLightV;
    private JSlider oFuzzyU;
    private JSlider oFuzzyV;

    private ColorLoader whiteColor;
    private ColorLoader greenColor;
    private ColorLoader orangeColor;

    private ColorLoader[] colors;

    public ImageConverterView() {
        super();

        // TODO init ColorLoaders from ???
        whiteColor = new ColorLoader(-0.02f, -0.02f, 0.25f, 0.25f, -0.055f, -0.055f);
        greenColor = new ColorLoader( 0.077f, 0.010f, -0.057f, -0.230f, -0.06f, -0.06f);
        orangeColor = new ColorLoader( 0.133f, 0.053f, -0.133f, 0.107f, -0.06f, 0.06f);


        colors =  new ColorLoader[3];
        colors[0] = whiteColor;
        colors[1] = greenColor;
        colors[2] = orangeColor;

        // TODO save button: pass color params back to nbfunc (how?)
            // have it rewrite the json data

        ChangeListener slide = new ChangeListener(){
            public void stateChanged(ChangeEvent e) {
                adjustParams();
            }
        };

        // TODO fix slider glitch

        // Init and add white sliders. TODO: init val from ColorLoaders
        wDarkU  = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(whiteColor.getDarkU() * 100));
        wDarkV  = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(whiteColor.getDarkV() * 100));
        wLightU = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(whiteColor.getLightU() * 100));
        wLightV = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(whiteColor.getLightV() * 100));
        wFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(whiteColor.getFuzzyU() * 100));
        wFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(whiteColor.getFuzzyV() * 100));

        wDarkU.addChangeListener(slide);
        wDarkV.addChangeListener(slide);
        wLightU.addChangeListener(slide);
        wLightV.addChangeListener(slide);
        wFuzzyU.addChangeListener(slide);
        wFuzzyV.addChangeListener(slide);

        add(wDarkU);
        add(wDarkV);
        add(wLightU);
        add(wLightV);
        add(wFuzzyU);
        add(wFuzzyV);

        // Init and add green sliders. TODO: init val from ColorLoaders
        gDarkU  = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(greenColor.getDarkU() * 100));
        gDarkV  = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(greenColor.getDarkV() * 100));
        gLightU = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(greenColor.getLightU() * 100));
        gLightV = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(greenColor.getLightV() * 100));
        gFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(greenColor.getFuzzyU() * 100));
        gFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(greenColor.getFuzzyV() * 100));

        gDarkU.addChangeListener(slide);
        gDarkV.addChangeListener(slide);
        gLightU.addChangeListener(slide);
        gLightV.addChangeListener(slide);
        gFuzzyU.addChangeListener(slide);
        gFuzzyV.addChangeListener(slide);

        add(gDarkU);
        add(gDarkV);
        add(gLightU);
        add(gLightV);
        add(gFuzzyU);
        add(gFuzzyV);

        // Init and add green sliders. TODO: init val from ColorLoaders
        oDarkU  = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(orangeColor.getDarkU() * 100));
        oDarkV  = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(orangeColor.getDarkV() * 100));
        oLightU = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(orangeColor.getLightU() * 100));
        oLightV = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(orangeColor.getLightV() * 100));
        oFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(orangeColor.getFuzzyU() * 100));
        oFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, (int)(orangeColor.getFuzzyV() * 100));

        oDarkU.addChangeListener(slide);
        oDarkV.addChangeListener(slide);
        oLightU.addChangeListener(slide);
        oLightV.addChangeListener(slide);
        oFuzzyU.addChangeListener(slide);
        oFuzzyV.addChangeListener(slide);

        add(oDarkU);
        add(oDarkV);
        add(oLightU);
        add(oLightV);
        add(oFuzzyU);
        add(oFuzzyV);
    }

    public void stateChanged(ChangeEvent e) {
        adjustParams();
    }

    public void adjustParams() {

        whiteColor. setDarkU( (float)wDarkU. getValue() / 100);
        whiteColor. setDarkV( (float)wDarkV. getValue() / 100);
        whiteColor. setLightU((float)wLightU.getValue() / 100);
        whiteColor. setLightV((float)wLightV.getValue() / 100);
        whiteColor. setFuzzyU((float)wFuzzyU.getValue() / 100);
        whiteColor. setFuzzyV((float)wFuzzyV.getValue() / 100);

        greenColor. setDarkU( (float)gDarkU. getValue() / 100);
        greenColor. setDarkV( (float)gDarkV. getValue() / 100);
        greenColor. setLightU((float)gLightU.getValue() / 100);
        greenColor. setLightV((float)gLightV.getValue() / 100);
        greenColor. setFuzzyU((float)gFuzzyU.getValue() / 100);
        greenColor. setFuzzyV((float)gFuzzyV.getValue() / 100);
    
        orangeColor.setDarkU( (float)oDarkU. getValue() / 100);
        orangeColor.setDarkV( (float)oDarkV. getValue() / 100);
        orangeColor.setLightU((float)oLightU.getValue() / 100);
        orangeColor.setLightV((float)oLightV.getValue() / 100);
        orangeColor.setFuzzyU((float)oFuzzyU.getValue() / 100);
        orangeColor.setFuzzyV((float)oFuzzyV.getValue() / 100);

        CrossInstance inst = CrossIO.instanceByIndex(0);
        if (inst == null)
            return;

        CrossFunc func = inst.functionWithName("Vision");
        if (func == null)
            return;
        
        // TODO passing color params back to c++?
        // Call to nbfunc has the json color params overrided by new params
        CrossCall call = new CrossCall(this, func, this.log);//, colors);
        inst.tryAddCall(call);

    }

    public void paintComponent(Graphics g) {
       
        int vB = 5;  // verticle buffer
        int sH = 15; // slider height
        int tB = 20; // text buffer

        int width = 320;
        int height = 240;

        // Draw five output image
        if (whiteImage != null) {
            g.drawImage(whiteImage, 0, 0, null);
        }
        if (greenImage != null) {
            g.drawImage(greenImage, width + vB, 0, null);
        }
        if (orangeImage != null) {
            g.drawImage(orangeImage, width*2 + vB*2, 0, null);
        }
        if (yImage != null) {
            g.drawImage(yImage, 0, height + sH*6 + tB*3, null);
        }
        if (segmentedImage != null) {
            g.drawImage(segmentedImage, width + vB,  height + sH*6 + tB*3, null);
        }

        // // Set demension of 16 sliders

        wDarkU. setBounds(vB, height + sH*0 + tB*1, width - vB*2, sH);
        wDarkV. setBounds(vB, height + sH*1 + tB*1, width - vB*2, sH);
        wLightU.setBounds(vB, height + sH*2 + tB*2, width - vB*2, sH);
        wLightV.setBounds(vB, height + sH*3 + tB*2, width - vB*2, sH);
        wFuzzyU.setBounds(vB, height + sH*4 + tB*3, width - vB*2, sH);
        wFuzzyV.setBounds(vB, height + sH*5 + tB*3, width - vB*2, sH);

        gDarkU. setBounds(width + vB*2, height + sH*0 + tB*1, width - vB*2, sH);
        gDarkV. setBounds(width + vB*2, height + sH*1 + tB*1, width - vB*2, sH);
        gLightU.setBounds(width + vB*2, height + sH*2 + tB*2, width - vB*2, sH);
        gLightV.setBounds(width + vB*2, height + sH*3 + tB*2, width - vB*2, sH);
        gFuzzyU.setBounds(width + vB*2, height + sH*4 + tB*3, width - vB*2, sH);
        gFuzzyV.setBounds(width + vB*2, height + sH*5 + tB*3, width - vB*2, sH);

        oDarkU. setBounds(width*2 + vB*3, height + sH*0 + tB*1, width - vB*2, sH);
        oDarkV. setBounds(width*2 + vB*3, height + sH*1 + tB*1, width - vB*2, sH);
        oLightU.setBounds(width*2 + vB*3, height + sH*2 + tB*2, width - vB*2, sH);
        oLightV.setBounds(width*2 + vB*3, height + sH*3 + tB*2, width - vB*2, sH);
        oFuzzyU.setBounds(width*2 + vB*3, height + sH*4 + tB*3, width - vB*2, sH);
        oFuzzyV.setBounds(width*2 + vB*3, height + sH*5 + tB*3, width - vB*2, sH);
    }

    @Override
    public void setLog(Log newlog) {
        log = newlog;
        
        CrossInstance inst = CrossIO.instanceByIndex(0);
        if (inst == null)
            return;

        CrossFunc func = inst.functionWithName("Vision");
        if (func == null)
            return;
        
        CrossCall call = new CrossCall(this, func, this.log);
        inst.tryAddCall(call);
        
    }
    

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
        if (out.length > 0) {
            Y16image yImg = new Y16image(320, 240, out[0].bytes);
            this.yImage = yImg.toBufferedImage();
        }

        if (out.length > 1) {
            Y8image white8 = new Y8image(320, 240, out[1].bytes);
            this.whiteImage = white8.toBufferedImage();
        }

        if (out.length > 2) {
            Y8image green8 = new Y8image(320, 240, out[2].bytes);
            this.greenImage = green8.toBufferedImage();
        }

        if (out.length > 3) {
            Y8image orange8 = new Y8image(320, 240, out[3].bytes);
            this.orangeImage = orange8.toBufferedImage();
        }

        if (out.length > 4) {
            ColorSegmentedImage colorSegImg = new ColorSegmentedImage(320, 240, out[4].bytes);
            this.segmentedImage = colorSegImg.toBufferedImage();
        }

        repaint();
    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }

}
