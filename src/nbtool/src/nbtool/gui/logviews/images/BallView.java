package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CppIO;
import nbtool.io.CppIO.CppFuncCall;
import nbtool.io.CppIO.CppFuncListener;
import nbtool.util.U;
import nbtool.images.Ballimage;

public class BallView extends ViewParent implements CppFuncListener {
    Log in;
    BufferedImage original;
    BufferedImage orangeImage;
    BufferedImage ballImage;

    String[] balls;

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponents(g);

        if(orangeImage != null) {
            g.drawImage(ballImage, 0, 0, ballImage.getWidth(), ballImage.getHeight(), null);
            g.drawImage(orangeImage, original.getWidth(), 0, orangeImage.getWidth(),
                        orangeImage.getHeight(), null);

            int count = 0;
            for (String s: balls) {
                if(s.length() < 2) 
                    continue;
                String[] split = s.substring(1, s.length() - 1).split(",");
                if(split.length != 5) continue;

                String toWrite = "Ball: " + count + " rated as: " + split[3];
                toWrite += " at distance: " + split[4] + "mm";
                g.drawString(toWrite, 30, ballImage.getHeight() + 12 + count * 12);
                count++;
            }
        }
    }

    @Override
    public void setLog(Log l) {
        System.out.println("Setting the log!");

        in = l;
        original = U.biFromLog(l);

        int findex = CppIO.current.indexOfFunc("BallImage");
        if (findex < 0) return;

        CppFuncCall func = new CppFuncCall();

        func.index = findex;
        func.name = "BallImage";
        func.args = new ArrayList<Log>(Arrays.asList(l));
        func.listener = this;

        CppIO.current.tryAddCall(func);
    }

    public BallView() {
        super();
    }

    @Override
    public void returned(int ret, Log... out) {
        orangeImage = U.biFromLog(out[0]);

        String b = new String(out[1].bytes);
        balls = b.split(" ");

        Ballimage im = new Ballimage(original.getWidth(), original.getHeight(),
                                     in.bytes, balls);
        ballImage = im.toBufferedImage();
        repaint();
    }
}