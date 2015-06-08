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
import nbtool.data.SExpr;
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

    // private ColorLoader whiteColor;
    // private ColorLoader greenColor;
    // private ColorLoader orangeColor;


    private boolean firstLoad;

    public ImageConverterView() {
        super();
        firstLoad = true;

        System.out.printf("CONSTRUCTING\n");

        // TODO save button: pass color params back to nbfunc (how?)
            // have it rewrite the json data

        ChangeListener slide = new ChangeListener(){
            public void stateChanged(ChangeEvent e) {
                adjustParams();
            }
        };

        // TODO fix slider glitch

        // Init and add white sliders. TODO: init val to 0
        wDarkU  = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        wDarkV  = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        wLightU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        wLightV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        wFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        wFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);

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

        // Init and add green sliders.
        gDarkU  = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        gDarkV  = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        gLightU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        gLightV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        gFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        gFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);

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

        // Init and add green sliders.
        oDarkU  = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        oDarkV  = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        oLightU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        oLightV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        oFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        oFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);

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

    public void adjustParams() {

        SExpr newParams = SExpr.newList(
                        SExpr.newAtom("Params"),
                        SExpr.newAtom("White"),
                        SExpr.newKeyValue("dark_u",  (float)(wDarkU. getValue()) / 100.00),
                        SExpr.newKeyValue("dark_v",  (float)(wDarkV. getValue()) / 100.00),
                        SExpr.newKeyValue("light_u", (float)(wLightU.getValue()) / 100.00),
                        SExpr.newKeyValue("light_v", (float)(wLightV.getValue()) / 100.00),
                        SExpr.newKeyValue("fuzzy_u", (float)(wFuzzyU.getValue()) / 100.00),
                        SExpr.newKeyValue("fuzzy_v", (float)(wFuzzyV.getValue()) / 100.00)
        );

        System.out.printf("NEW PARAMS: %d\n", (newParams.serialize()));    

        // whiteColor. setDarkU( (float)wDarkU. getValue() / 100);
        // whiteColor. setDarkV( (float)wDarkV. getValue() / 100);
        // whiteColor. setLightU((float)wLightU.getValue() / 100);
        // whiteColor. setLightV((float)wLightV.getValue() / 100);
        // whiteColor. setFuzzyU((float)wFuzzyU.getValue() / 100);
        // whiteColor. setFuzzyV((float)wFuzzyV.getValue() / 100);

        // greenColor. setDarkU( (float)gDarkU. getValue() / 100);
        // greenColor. setDarkV( (float)gDarkV. getValue() / 100);
        // greenColor. setLightU((float)gLightU.getValue() / 100);
        // greenColor. setLightV((float)gLightV.getValue() / 100);
        // greenColor. setFuzzyU((float)gFuzzyU.getValue() / 100);
        // greenColor. setFuzzyV((float)gFuzzyV.getValue() / 100);
    
        // orangeColor.setDarkU( (float)oDarkU. getValue() / 100);
        // orangeColor.setDarkV( (float)oDarkV. getValue() / 100);
        // orangeColor.setLightU((float)oLightU.getValue() / 100);
        // orangeColor.setLightV((float)oLightV.getValue() / 100);
        // orangeColor.setFuzzyU((float)oFuzzyU.getValue() / 100);
        // orangeColor.setFuzzyV((float)oFuzzyV.getValue() / 100);

        CrossInstance inst = CrossIO.instanceByIndex(0);
        if (inst == null)
            return;

        CrossFunc func = inst.functionWithName("Vision");
        if (func == null)
            return;
        
        // TODO great sexpresion of 3 param sets, each field from slider vals, add to this.log


        CrossCall call = new CrossCall(this, func, this.log);
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

        wDarkU. setBounds(width*0 + vB*1, height + sH*0 + tB*1, width - vB*2, sH);
        wDarkV. setBounds(width*0 + vB*1, height + sH*1 + tB*1, width - vB*2, sH);
        wLightU.setBounds(width*0 + vB*1, height + sH*2 + tB*2, width - vB*2, sH);
        wLightV.setBounds(width*0 + vB*1, height + sH*3 + tB*2, width - vB*2, sH);
        wFuzzyU.setBounds(width*0 + vB*1, height + sH*4 + tB*3, width - vB*2, sH);
        wFuzzyV.setBounds(width*0 + vB*1, height + sH*5 + tB*3, width - vB*2, sH);

        gDarkU. setBounds(width*1 + vB*2, height + sH*0 + tB*1, width - vB*2, sH);
        gDarkV. setBounds(width*1 + vB*2, height + sH*1 + tB*1, width - vB*2, sH);
        gLightU.setBounds(width*1 + vB*2, height + sH*2 + tB*2, width - vB*2, sH);
        gLightV.setBounds(width*1 + vB*2, height + sH*3 + tB*2, width - vB*2, sH);
        gFuzzyU.setBounds(width*1 + vB*2, height + sH*4 + tB*3, width - vB*2, sH);
        gFuzzyV.setBounds(width*1 + vB*2, height + sH*5 + tB*3, width - vB*2, sH);

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
        
    //    System.out.printf("DESC STRING: %s\n", log.description());
        
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

        if (firstLoad) {
            firstIoReceived(out);
            firstLoad = false;
        }

        repaint();
    }

    private void firstIoReceived(Log... out) {
        
        // Set sliders to positions based on white, green, then orange images descriptions' s-exps
        SExpr colors = out[1].tree();

        wDarkU. setValue((int)(Float.parseFloat(colors.get(1).get(1).value()) * 100));
        wDarkV. setValue((int)(Float.parseFloat(colors.get(2).get(1).value()) * 100));
        wLightU.setValue((int)(Float.parseFloat(colors.get(3).get(1).value()) * 100));
        wLightV.setValue((int)(Float.parseFloat(colors.get(4).get(1).value()) * 100));
        wFuzzyU.setValue((int)(Float.parseFloat(colors.get(5).get(1).value()) * 100));
        wFuzzyV.setValue((int)(Float.parseFloat(colors.get(6).get(1).value()) * 100));

        colors = out[2].tree();

        gDarkU. setValue((int)(Float.parseFloat(colors.get(1).get(1).value()) * 100));
        gDarkV. setValue((int)(Float.parseFloat(colors.get(2).get(1).value()) * 100));
        gLightU.setValue((int)(Float.parseFloat(colors.get(3).get(1).value()) * 100));
        gLightV.setValue((int)(Float.parseFloat(colors.get(4).get(1).value()) * 100));
        gFuzzyU.setValue((int)(Float.parseFloat(colors.get(5).get(1).value()) * 100));
        gFuzzyV.setValue((int)(Float.parseFloat(colors.get(6).get(1).value()) * 100));

        colors = out[3].tree();

        oDarkU. setValue((int)(Float.parseFloat(colors.get(1).get(1).value()) * 100));
        oDarkV. setValue((int)(Float.parseFloat(colors.get(2).get(1).value()) * 100));
        oLightU.setValue((int)(Float.parseFloat(colors.get(3).get(1).value()) * 100));
        oLightV.setValue((int)(Float.parseFloat(colors.get(4).get(1).value()) * 100));
        oFuzzyU.setValue((int)(Float.parseFloat(colors.get(5).get(1).value()) * 100));
        oFuzzyV.setValue((int)(Float.parseFloat(colors.get(6).get(1).value()) * 100));
        
    }

 //   private SExpr getColorSExpFromSliders(int color) {
    //     if (color == 0) {

    //     } else if (color == 1) {    // Green

    //     } else {                    // Orange

    //     }
    // }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }

}
