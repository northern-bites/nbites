package nbtool.gui.logviews.misc;

import java.awt.Graphics;
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

    public ImageConverterView() {
        super();

        wDarkU = new JSlider(JSlider.HORIZONTAL, 0, 30, 15);

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
        
        CrossCall call = new CrossCall(this, func, this.log);
        inst.tryAddCall(call);

    }

    public void paintComponent(Graphics g) {
        if (whiteImage != null) {
            g.drawImage(whiteImage, 0, 0, null);
        }
        if (greenImage != null) {
            g.drawImage(greenImage, 325, 0, null);
        }
        if (orangeImage != null) {
            g.drawImage(orangeImage, 650, 0, null);
        }
        if (yImage != null) {
            g.drawImage(yImage, 0, 450, null);
        }
        if (segmentedImage != null) {
            g.drawImage(segmentedImage, 325, 450, null);
        }
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
