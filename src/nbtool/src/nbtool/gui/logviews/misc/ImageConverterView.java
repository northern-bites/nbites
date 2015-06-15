package nbtool.gui.logviews.misc;

import java.awt.Graphics;
import java.awt.image.BufferedImage;

import nbtool.images.Y8image;
import nbtool.images.Y16image;
import nbtool.images.ColorSegmentedImage;

import nbtool.data.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.util.Utility;

public class ImageConverterView extends ViewParent implements IOFirstResponder {
    BufferedImage yImage;
    BufferedImage whiteImage;
    BufferedImage greenImage;
    BufferedImage orangeImage;
    BufferedImage segmentedImage;

    public void paintComponent(Graphics g) {
        if (yImage != null) {
            g.drawImage(yImage, 0, 0, null);
        }
        if (whiteImage != null) {
            g.drawImage(whiteImage, 325, 0, null);
        }
        if (greenImage != null) {
            g.drawImage(greenImage, 0, 245, null);
        }
        if (orangeImage != null) {
            g.drawImage(orangeImage, 325, 245, null);
        }

        if (segmentedImage != null) {
            g.drawImage(segmentedImage, 162, 490, null);
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
    
    public ImageConverterView() {
        super();
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
