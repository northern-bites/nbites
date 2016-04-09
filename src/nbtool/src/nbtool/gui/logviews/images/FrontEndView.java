package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Dimension;
import java.awt.image.BufferedImage;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import nbtool.images.Y8image;
import nbtool.images.Y16image;
import nbtool.images.ColorSegmentedImage;

import javax.swing.JSlider;
import javax.swing.JButton;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;

import nbtool.util.Utility;
import java.util.Vector;


public class FrontEndView extends ViewParent implements IOFirstResponder {

    // FrontEnd output images 
    private BufferedImage yImage;
    private BufferedImage whiteImage;
    private BufferedImage greenImage;
    private BufferedImage orangeImage;
    private BufferedImage segmentedImage;

    // Save button
    private JButton saveButton;

    // White sliders
    private JSlider wDarkU;
    private JSlider wDarkV;
    private JSlider wLightU;
    private JSlider wLightV;
    private JSlider wFuzzyU;
    private JSlider wFuzzyV;

    // Green sliders
    private JSlider gDarkU;
    private JSlider gDarkV;
    private JSlider gLightU;
    private JSlider gLightV;
    private JSlider gFuzzyU;
    private JSlider gFuzzyV;

    // Orange sliders
    private JSlider oDarkU;
    private JSlider oDarkV;
    private JSlider oLightU;
    private JSlider oLightV;
    private JSlider oFuzzyU;
    private JSlider oFuzzyV;

    // Some operations should only happen on first load
    private boolean firstLoad;

    private int width;
    private int height;

    final double wPrecision = 200.0;
    final double gPrecision = 300.0;
    final double oPrecision = 200.0;

    public FrontEndView() {
        super();
        firstLoad = true;

        ChangeListener slide = new ChangeListener(){
            public void stateChanged(ChangeEvent e) {
                adjustParams();
                repaint();
            }
        };

        ActionListener press = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                saveParams();
            }
        };


        // Save button
        saveButton = new JButton("Save color parameters");
        saveButton.setToolTipText("Save current color parms to config/colorParams.txt");
        saveButton.addActionListener(press);
        add(saveButton);

        // Init and add white sliders
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

    @Override
    public void setLog(Log newlog) {
        log = newlog;

        Vector<SExpr> vec = log.tree().recursiveFind("width");
        if (vec.size() > 0) {
            SExpr w = vec.get(vec.size()-1);
            width =  w.get(1).valueAsInt() / 2;
        } else {
            System.out.printf("COULD NOT READ WIDTH FROM LOG DESC\n");
            width = 320;
        }

        vec = log.tree().recursiveFind("height");
        if (vec.size() > 0) {
            SExpr h = vec.get(vec.size()-1);
            height = h.get(1).valueAsInt() / 2;
        } else {
            System.out.printf("COULD NOT READ HEIGHT FROM LOG DESC\n");
            height = 240;
        }

        callNBFunction();
    }
    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }

    /* Called upon slide of any of the 18 sliders. Make an SExpr from the psitions 
        of each of the sliders. If this.log doesn't have Params saved, add them. Else,
        replace them. Call nbfunction with updated log description.
    */
    public void adjustParams() {

        // This is called when the sliders are initialized, so dont run if it's first load,
        //  or if any values are zero b/c devide by zero error in frontEnd processing
        if (firstLoad)
            return;
        zeroParam();

        SExpr newParams = SExpr.newList(
            SExpr.newKeyValue("White", SExpr.newList(
                SExpr.newKeyValue("dark_u",  (float)(wDarkU. getValue()) / wPrecision),
                SExpr.newKeyValue("dark_v",  (float)(wDarkV. getValue()) / wPrecision),
                SExpr.newKeyValue("light_u", (float)(wLightU.getValue()) / wPrecision),
                SExpr.newKeyValue("light_v", (float)(wLightV.getValue()) / wPrecision),
                SExpr.newKeyValue("fuzzy_u", (float)(wFuzzyU.getValue()) / wPrecision),
                SExpr.newKeyValue("fuzzy_v", (float)(wFuzzyV.getValue()) / wPrecision))),

            SExpr.newKeyValue("Green", SExpr.newList(
                SExpr.newKeyValue("dark_u",  (float)(gDarkU. getValue()) / gPrecision),
                SExpr.newKeyValue("dark_v",  (float)(gDarkV. getValue()) / gPrecision),
                SExpr.newKeyValue("light_u", (float)(gLightU.getValue()) / gPrecision),
                SExpr.newKeyValue("light_v", (float)(gLightV.getValue()) / gPrecision),
                SExpr.newKeyValue("fuzzy_u", (float)(gFuzzyU.getValue()) / gPrecision),
                SExpr.newKeyValue("fuzzy_v", (float)(gFuzzyV.getValue()) / gPrecision))),

            SExpr.newKeyValue("Orange", SExpr.newList(
                SExpr.newKeyValue("dark_u",  (float)(oDarkU. getValue()) / oPrecision),
                SExpr.newKeyValue("dark_v",  (float)(oDarkV. getValue()) / oPrecision),
                SExpr.newKeyValue("light_u", (float)(oLightU.getValue()) / oPrecision),
                SExpr.newKeyValue("light_v", (float)(oLightV.getValue()) / oPrecision),
                SExpr.newKeyValue("fuzzy_u", (float)(oFuzzyU.getValue()) / oPrecision),
                SExpr.newKeyValue("fuzzy_v", (float)(oFuzzyV.getValue()) / oPrecision)))
            );
        
        // Look for existing Params atom in current this.log description
        SExpr oldColor, newColor, oldParams = this.log.tree().find("Params");

        // Add params or replace params
        if (oldParams.exists()) {
            SExpr saveAtom = oldParams.get(1).find("SaveParams");
            this.log.tree().remove(saveAtom);
            oldParams.setList( SExpr.atom("Params"), newParams); 
        } else {
            this.log.tree().append(SExpr.pair("Params", newParams));
        }

        callNBFunction();
    }

    public void saveParams() {
        SExpr params = this.log.tree().find("Params");

        // Check to see if sliders have been moved (slider adjust would have saved params)
        if (!params.exists())
            return;

        // Add flag for nbfunction to save the params
        params.get(1).append(SExpr.newKeyValue("SaveParams", "True"));

        callNBFunction();
    }

    private void callNBFunction() {
        CrossInstance inst = CrossIO.instanceByIndex(0);
        if (inst == null)
            return;

        CrossFunc func = inst.functionWithName("Vision");
        if (func == null)
            return;

        CrossCall call = new CrossCall(this, func, this.log);
        inst.tryAddCall(call);
    }

    // Check to see if any parameters are zero to avoid devide-by-zero error later
    private void zeroParam() {
        if (wDarkU. getValue() == 0) wDarkU.setValue(1);
        if (wDarkV. getValue() == 0) wDarkV.setValue(1);
        if (wLightU.getValue() == 0) wLightU.setValue(1);
        if (wLightV.getValue() == 0) wLightV.setValue(1);
        if (wFuzzyU.getValue() == 0) wFuzzyU.setValue(1);
        if (wFuzzyV.getValue() == 0) wFuzzyV.setValue(1);

        if (gDarkU. getValue() == 0) gDarkU.setValue(1);
        if (gDarkV. getValue() == 0) gDarkV.setValue(1);
        if (gLightU.getValue() == 0) gLightU.setValue(1);
        if (gLightV.getValue() == 0) gLightV.setValue(1);
        if (gFuzzyU.getValue() == 0) gFuzzyU.setValue(1);
        if (gFuzzyV.getValue() == 0) gFuzzyV.setValue(1);

        if (oDarkU. getValue() == 0) oDarkU.setValue(1);
        if (oDarkV. getValue() == 0) oDarkV.setValue(1);
        if (oLightU.getValue() == 0) oLightU.setValue(1);
        if (oLightV.getValue() == 0) oLightV.setValue(1);
        if (oFuzzyU.getValue() == 0) oFuzzyU.setValue(1);
        if (oFuzzyV.getValue() == 0) oFuzzyV.setValue(1);
    }

    // Draw 5 output images, 18 sliders, 6 texts, and a button
    public void paintComponent(Graphics g) {
    	
    	super.paintComponent(g);
       
        int vB = 5;  // verticle buffer
        int sH = 15; // slider height
        int tB = 20; // text buffer
        int lB = 5;  // little buffer

        if (width < 320) vB = 160;

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
            //g.drawImage(segmentedImage, width + vB,  height + sH*6 + tB*3, null);
            g.drawImage(whiteImage, width + vB,  height + sH*6 + tB*3, null);
            g.drawImage(greenImage, width + vB,  height + sH*6 + tB*3, null);
        }

        // TODO fix slider glitch
        // Draw 18 sliders
        wDarkU. setBounds(width*0 + vB*0 + lB, height + sH*0 + tB*1, width - lB*2, sH);
        wDarkV. setBounds(width*0 + vB*0 + lB, height + sH*1 + tB*1, width - lB*2, sH);
        wLightU.setBounds(width*0 + vB*0 + lB, height + sH*2 + tB*2, width - lB*2, sH);
        wLightV.setBounds(width*0 + vB*0 + lB, height + sH*3 + tB*2, width - lB*2, sH);
        wFuzzyU.setBounds(width*0 + vB*0 + lB, height + sH*4 + tB*3, width - lB*2, sH);
        wFuzzyV.setBounds(width*0 + vB*0 + lB, height + sH*5 + tB*3, width - lB*2, sH);

        gDarkU. setBounds(width*1 + vB*1 + lB, height + sH*0 + tB*1, width - lB*2, sH);
        gDarkV. setBounds(width*1 + vB*1 + lB, height + sH*1 + tB*1, width - lB*2, sH);
        gLightU.setBounds(width*1 + vB*1 + lB, height + sH*2 + tB*2, width - lB*2, sH);
        gLightV.setBounds(width*1 + vB*1 + lB, height + sH*3 + tB*2, width - lB*2, sH);
        gFuzzyU.setBounds(width*1 + vB*1 + lB, height + sH*4 + tB*3, width - lB*2, sH);
        gFuzzyV.setBounds(width*1 + vB*1 + lB, height + sH*5 + tB*3, width - lB*2, sH);

        oDarkU. setBounds(width*2 + vB*2 + lB, height + sH*0 + tB*1, width - lB*2, sH);
        oDarkV. setBounds(width*2 + vB*2 + lB, height + sH*1 + tB*1, width - lB*2, sH);
        oLightU.setBounds(width*2 + vB*2 + lB, height + sH*2 + tB*2, width - lB*2, sH);
        oLightV.setBounds(width*2 + vB*2 + lB, height + sH*3 + tB*2, width - lB*2, sH);
        oFuzzyU.setBounds(width*2 + vB*2 + lB, height + sH*4 + tB*3, width - lB*2, sH);
        oFuzzyV.setBounds(width*2 + vB*2 + lB, height + sH*5 + tB*3, width - lB*2, sH);

        // Draw help text
        g.drawString("white U and V thresholds for when Y is 0",    width*0 + tB*0, height + tB*1 + sH*0 - lB);
        g.drawString("white U and V thresholds for when Y is 255",  width*0 + tB*0, height + tB*2 + sH*2 - lB);
        g.drawString("width of fuzzy threshold for U and V",        width*0 + tB*0, height + tB*3 + sH*4 - lB);

        g.drawString("green U and V thresholds for when Y is 0",    width*1 + vB*1, height + tB*1 + sH*0 - lB);
        g.drawString("green U and V thresholds for when Y is 255",  width*1 + vB*1, height + tB*2 + sH*2 - lB);
        g.drawString("width of fuzzy threshold for U and V",        width*1 + vB*1, height + tB*3 + sH*4 - lB);

        g.drawString("orange U and V thresholds for when Y is 0",   width*2 + vB*2, height + tB*1 + sH*0 - lB);
        g.drawString("orange U and V thresholds for when Y is 255", width*2 + vB*2, height + tB*2 + sH*2 - lB);
        g.drawString("width of fuzzy threshold for U and V",        width*2 + vB*2, height + tB*3 + sH*4 - lB);
       
        // Draw button
        saveButton.setBounds(width*2 + vB*2,  height + sH*6 + tB*3, width, tB*3);
    }


    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
        if (out.length > 0) {
            Y16image yImg = new Y16image(width, height, out[0].bytes);
            this.yImage = yImg.toBufferedImage();
        }

        if (out.length > 1) {
            Y8image white8 = new Y8image(width, height, out[1].bytes);
            this.whiteImage = white8.toBufferedImage();
        }

        if (out.length > 2) {
            Y8image green8 = new Y8image(width, height, out[2].bytes);
            this.greenImage = green8.toBufferedImage();
        }

        if (out.length > 3) {
            Y8image orange8 = new Y8image(width, height, out[3].bytes);
            this.orangeImage = orange8.toBufferedImage();
        }

        if (out.length > 4) {
            ColorSegmentedImage colorSegImg = new ColorSegmentedImage(width, height, out[4].bytes);
            this.segmentedImage = colorSegImg.toBufferedImage();
        }

        if (firstLoad) {
            firstIoReceived(out);
        }

        repaint();
    }

    // If and only if it is the first load, we need to set the positions of the sliders
    private void firstIoReceived(Log... out) {
        
        // Set sliders to positions based on white, green, then orange images descriptions' s-exps
        SExpr colors = out[1].tree();

        wDarkU. setValue((int)(Float.parseFloat(colors.get(1).get(0).get(1).value()) * wPrecision));
        wDarkV. setValue((int)(Float.parseFloat(colors.get(1).get(1).get(1).value()) * wPrecision));
        wLightU.setValue((int)(Float.parseFloat(colors.get(1).get(2).get(1).value()) * wPrecision));
        wLightV.setValue((int)(Float.parseFloat(colors.get(1).get(3).get(1).value()) * wPrecision));
        wFuzzyU.setValue((int)(Float.parseFloat(colors.get(1).get(4).get(1).value()) * wPrecision));
        wFuzzyV.setValue((int)(Float.parseFloat(colors.get(1).get(5).get(1).value()) * wPrecision));

        colors = out[2].tree();

        gDarkU. setValue((int)(Float.parseFloat(colors.get(1).get(0).get(1).value()) * gPrecision));
        gDarkV. setValue((int)(Float.parseFloat(colors.get(1).get(1).get(1).value()) * gPrecision));
        gLightU.setValue((int)(Float.parseFloat(colors.get(1).get(2).get(1).value()) * gPrecision));
        gLightV.setValue((int)(Float.parseFloat(colors.get(1).get(3).get(1).value()) * gPrecision));
        gFuzzyU.setValue((int)(Float.parseFloat(colors.get(1).get(4).get(1).value()) * gPrecision));
        gFuzzyV.setValue((int)(Float.parseFloat(colors.get(1).get(5).get(1).value()) * gPrecision));

        colors = out[3].tree();

        oDarkU. setValue((int)(Float.parseFloat(colors.get(1).get(0).get(1).value()) * oPrecision));
        oDarkV. setValue((int)(Float.parseFloat(colors.get(1).get(1).get(1).value()) * oPrecision));
        oLightU.setValue((int)(Float.parseFloat(colors.get(1).get(2).get(1).value()) * oPrecision));
        oLightV.setValue((int)(Float.parseFloat(colors.get(1).get(3).get(1).value()) * oPrecision));
        oFuzzyU.setValue((int)(Float.parseFloat(colors.get(1).get(4).get(1).value()) * oPrecision));
        oFuzzyV.setValue((int)(Float.parseFloat(colors.get(1).get(5).get(1).value()) * oPrecision));

        firstLoad = false;
    }
}
