package nbtool.gui.logviews.misc;

import java.awt.Graphics;
import java.awt.image.BufferedImage;

import nbtool.images.Y8image;
import nbtool.images.Y16image;

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

    public void paintComponent(Graphics g) {
        if (yImage != null) {
            g.drawImage(yImage, 0, 0, null);
        }
        if (whiteImage != null) {
            g.drawImage(whiteImage, 325, 0, null);
        }
        if (greenImage != null) {
            g.drawImage(greenImage, 325, 245, null);
        }
        if (orangeImage != null) {
            g.drawImage(orangeImage, 0, 245, null);
        }
    }

    @Override
    public void setLog(Log newlog) {
        log = newlog;
        
        CrossInstance inst = CrossIO.instanceByIndex(0);
        if (inst == null)
            return;

        CrossFunc func = inst.functionWithName("ImageConverter");
        if (func == null)
            return;
        
        CrossCall call = new CrossCall(this, func, this.log);
        inst.tryAddCall(call);
        System.out.println("Called!");
        
    }
    
    public ImageConverterView() {
        super();
    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
        Y16image yImg = new Y16image(320, 240, out[0].bytes);
        this.yImage = yImg.toBufferedImage();

        Y8image pixelImg = new Y8image(320, 240, out[1].bytes);
        this.whiteImage = pixelImg.toBufferedImage();

        pixelImg = new Y8image(320, 240, out[2].bytes);
        this.greenImage = pixelImg.toBufferedImage();

        pixelImg = new Y8image(320, 240, out[3].bytes);
        this.orangeImage = pixelImg.toBufferedImage();

        repaint();
    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }
}
