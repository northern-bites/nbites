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

    private boolean firstLoad;

    public ImageConverterView() {
        super();
        firstLoad = true;

        // TODO save button: pass color params back to nbfunc (how?)
            // have it rewrite the json data

        ChangeListener slide = new ChangeListener(){
            public void stateChanged(ChangeEvent e) {
                adjustParams();
                repaint();
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

        if (firstLoad || zeroParam())
            return;

        SExpr newParams = SExpr.newList(
            SExpr.newKeyValue("White", SExpr.newList(
            SExpr.newKeyValue("dark_u",  (float)(wDarkU. getValue()) / 100.00),
            SExpr.newKeyValue("dark_v",  (float)(wDarkV. getValue()) / 100.00),
            SExpr.newKeyValue("light_u", (float)(wLightU.getValue()) / 100.00),
            SExpr.newKeyValue("light_v", (float)(wLightV.getValue()) / 100.00),
            SExpr.newKeyValue("fuzzy_u", (float)(wFuzzyU.getValue()) / 100.00),
            SExpr.newKeyValue("fuzzy_v", (float)(wFuzzyV.getValue()) / 100.00))),

            SExpr.newKeyValue("Green", SExpr.newList(
            SExpr.newKeyValue("dark_u",  (float)(gDarkU. getValue()) / 100.00),
            SExpr.newKeyValue("dark_v",  (float)(gDarkV. getValue()) / 100.00),
            SExpr.newKeyValue("light_u", (float)(gLightU.getValue()) / 100.00),
            SExpr.newKeyValue("light_v", (float)(gLightV.getValue()) / 100.00),
            SExpr.newKeyValue("fuzzy_u", (float)(gFuzzyU.getValue()) / 100.00),
            SExpr.newKeyValue("fuzzy_v", (float)(gFuzzyV.getValue()) / 100.00))),

            SExpr.newKeyValue("Orange", SExpr.newList(
            SExpr.newKeyValue("dark_u",  (float)(oDarkU. getValue()) / 100.00),
            SExpr.newKeyValue("dark_v",  (float)(oDarkV. getValue()) / 100.00),
            SExpr.newKeyValue("light_u", (float)(oLightU.getValue()) / 100.00),
            SExpr.newKeyValue("light_v", (float)(oLightV.getValue()) / 100.00),
            SExpr.newKeyValue("fuzzy_u", (float)(oFuzzyU.getValue()) / 100.00),
            SExpr.newKeyValue("fuzzy_v", (float)(oFuzzyV.getValue()) / 100.00)))
            );

        System.out.printf("NEW PARAMS: %s\n\n", (newParams.serialize()));   

       SExpr oldColor, newColor, oldParams = this.log.tree().find("Params");

        if (oldParams.exists()) {
            
            oldParams.setList( SExpr.atom("Params"), newParams); 
            // TODO: use new funcitonality to swap old params for new params

            // for (int i = 0; i < 3; i++) {
            //     for (int j = 0; j < 6; j++) {
            //         oldColor = this.log.tree().find("Params").get(1).get(i).get(1).get(j);
            //         System.out.printf("Old Color: %s\n", oldColor.serialize());   
            //         newColor = newParams.get(1).get(i).get(1).get(j);
            //         System.out.printf("New Color: %s\n", newColor.serialize());

            //         // oldColor  = this.log.tree().find("Params").get(1).get(i).get(1).get(j).get(1);
            //         // oldColor = newColor.get(1);

            //         oldColor.setList(newColor.getList());

            //         System.out.printf("TEST: %s\n", oldColor.list().serialize());

            //         System.out.printf("N/O Color: %s\n", (oldColor.serialize()));   

            //         System.out.printf("LOG Color: %s\n\n", this.log.tree().find("Params").get(1).get(i).get(1).get(j).serialize());

            //     }
            // }

            
        } else {

            System.out.printf("NOT FOUND: %s\n\n", (this.log.tree().serialize()));    

            this.log.tree().append(SExpr.pair("Params", newParams));
        }

        System.out.printf("Thos.Log description: %s\n\n", (this.log.tree().print()));   


        CrossInstance inst = CrossIO.instanceByIndex(0);
        if (inst == null)
            return;

        CrossFunc func = inst.functionWithName("Vision");
        if (func == null)
            return;

        CrossCall call = new CrossCall(this, func, this.log);
        inst.tryAddCall(call);

    }

    private boolean zeroParam() {
        if (wDarkU. getValue() == 0 ||
            wDarkV. getValue() == 0 ||
            wLightU.getValue() == 0 ||
            wLightV.getValue() == 0 ||
            wFuzzyU.getValue() == 0 ||
            wFuzzyV.getValue() == 0 ||

            gDarkU. getValue() == 0 ||
            gDarkV. getValue() == 0 ||
            gLightU.getValue() == 0 ||
            gLightV.getValue() == 0 ||
            gFuzzyU.getValue() == 0 ||
            gFuzzyV.getValue() == 0 ||

            oDarkU. getValue() == 0 ||
            oDarkV. getValue() == 0 ||
            oLightU.getValue() == 0 ||
            oLightV.getValue() == 0 ||
            oFuzzyU.getValue() == 0 ||
            oFuzzyV.getValue() == 0) {

            return true;
        } else return false;
    }

    public void paintComponent(Graphics g) {
       
        int vB = 5;  // verticle buffer
        int sH = 15; // slider height
        int tB = 20; // text buffer
        int lB = 5;  // little buffer

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

        g.drawString("white U and V thresholds for when Y is 0",    width*0 + tB*0, height + tB*1 + sH*0 - lB);
        g.drawString("white U and V thresholds for when Y is 255",  width*0 + tB*0, height + tB*2 + sH*2 - lB);
        g.drawString("width of fuzzy threshold for U and V",        width*0 + tB*0, height + tB*3 + sH*4 - lB);

        g.drawString("green U and V thresholds for when Y is 0",    width*1 + vB*1, height + tB*1 + sH*0 - lB);
        g.drawString("green U and V thresholds for when Y is 255",  width*1 + vB*1, height + tB*2 + sH*2 - lB);
        g.drawString("width of fuzzy threshold for U and V",        width*1 + vB*1, height + tB*3 + sH*4 - lB);

        g.drawString("orange U and V thresholds for when Y is 0",   width*2 + vB*2, height + tB*1 + sH*0 - lB);
        g.drawString("orange U and V thresholds for when Y is 255", width*2 + vB*2, height + tB*2 + sH*2 - lB);
        g.drawString("width of fuzzy threshold for U and V",        width*2 + vB*2, height + tB*3 + sH*4 - lB);

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

        firstLoad = false;
        
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
