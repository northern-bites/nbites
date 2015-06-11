package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;

import nbtool.util.Logger;
import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;

import nbtool.util.Utility;
import nbtool.images.Ballimage;

public class BallView extends ViewParent implements IOFirstResponder {
    Log in;

    String[] balls;

    @Override
    public void setLog(Log newlog)
    {
        CrossInstance cross = CrossIO.instanceByIndex(0);
        if (cross == null) return;

        CrossFunc func = cross.functionWithName("Vision");
        assert(func != null);

        CrossCall call = new CrossCall(this, func, newlog);
        assert(cross.tryAddCall(call));

        in = newlog;
    }

    @Override
    public void paintComponent(Graphics g) {

    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out)
    {
        // Do something
    }

    @Override
    public boolean ioMayRespondOnCenterThread(IOInstance inst) {
        return false;
    }

}