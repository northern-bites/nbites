package nbtool.gui.logviews.images;

import java.awt.Graphics;
import java.awt.Color;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.util.Map;
import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;

import nbtool.util.Logger;
import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.images.EdgeImage;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.util.Utility;

import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Message;

import messages.Vision.*;

public class BasicLineView extends ViewParent {
    
    List<FieldLine> lines;

    int width;
    int height;
    
    int displayw;
    int displayh;

    final int fieldw = 640;
    final int fieldh = 554;

    final int buffer = 5;

    double resize = 1;

    boolean click = false;
    boolean drag = false;
    
    // Click and release values
    int clickX1 = 0;
    int clickY1 = 0;
    int clickX2 = 0;
    int clickY2 = 0;

    // Field coordinate image upper left hand corder
    int fx0;
    int fy0;

    // Center of field cordinate system
    int fxc = displayw + buffer + fieldw/2; 
    int fyc = fieldh;

    @Override
    public void setLog(Log newlog) {
        String t = (String) newlog.primaryType();
        Class<? extends com.google.protobuf.GeneratedMessage> lClass = Utility.protobufClassFromType(t);
        Logger.logf(Logger.INFO, "ProtoBufView: using class %s for type %s.\n", lClass.getName(), t);

        if (!t.equals("proto-FieldLines")) {
            return;
        }
        try {
            FieldLines msg = FieldLines.parseFrom(newlog.bytes);
            for (FieldLine line : msg.getLineList()) {
                // System.out.println(line.toString());
                lines.add(line);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        repaint();


        // CrossInstance ci = CrossIO.instanceByIndex(0);
        // if (ci == null)
        //     return;
        // CrossFunc func = ci.functionWithName("Vision");
        // assert(func != null);
        
        // CrossCall cc = new CrossCall(this, func, newlog);

        // assert(ci.tryAddCall(cc));

        // // TODO: Don't hard code SExpr paths
        width =  120;
        height = 240;

        displayw = width*2;
        displayh = height*2;

        fx0 = displayw + buffer;
        fy0 = 0;

        fxc = displayw + buffer + fieldw/2; 
        fyc = fieldh;

    }
    
    public void paintComponent(Graphics g) {

        int offsetY = this.getHeight()/2 + 20;
        int offsetX = 10;

        int multiplier = 1;

        int originX = this.getWidth()/2;
        int originY = this.getHeight()/2 - 50;

        g.setColor(new Color(40, 140, 40));
        g.fillRect(0, 0, this.getWidth(), this.getHeight()/2);

        g.setColor(Color.red);
        g.fillRect(originX-10, originY-10, 20, 20);

        if (lines.size() < 1) return;

        for (int i = 0; i <lines.size(); i++) {
            double icR = lines.get(i).getInner().getR();
            double icT = lines.get(i).getInner().getT();
            double icEP0 = lines.get(i).getInner().getEp0();
            double icEP1 = lines.get(i).getInner().getEp1();
            double mP = (icEP1 + icEP0) / 2.0;

            String stringr = "R: " + (double)Math.round(icR * 100000) / 100000;
            String stringt = "t: " + (double)Math.round(icT * 100000) / 100000;
            String stringep0 = "ep0: " + Math.round(icEP0);
            String stringep1 = "ep1: " + Math.round(icEP1);
            String stringid = "id: " + lines.get(i).getId();

            g.setColor(Color.black);
            g.drawString(stringr, offsetX, offsetY + 160);
            g.drawString(stringt, offsetX, offsetY + 180);
            g.drawString(stringep0, offsetX, offsetY + 200);
            g.drawString(stringep1, offsetX, offsetY + 220);
            g.drawString(stringid, offsetX, offsetY + 240);

            g.setColor(Color.white);

            double x0 = originX + multiplier*icR * Math.cos(icT);
            double y0 = originY + -multiplier*icR * Math.sin(icT);
            int x1 = (int) Math.round(x0 + multiplier*icEP0 * Math.sin(icT));
            int y1 = (int) Math.round(y0 + multiplier*icEP0 * Math.cos(icT));
            int x2 = (int) Math.round(x0 + multiplier*icEP1 * Math.sin(icT));
            int y2 = (int) Math.round(y0 + multiplier*icEP1 * Math.cos(icT));
            int xm = (int) Math.round(x0 + multiplier*mP * Math.sin(icT));
            int ym = (int) Math.round(y0 + multiplier*mP * Math.cos(icT));

            System.out.println("x0: " + x0);
            System.out.println("y0: " + y0 + "\n");

            String string0 = "x0,y0: " + (int)x0 + ", " + (int)y0;
            String string1 = "x1,y1: " + x1 + ", " + y1;
            String string2 = "x2,y2: " + x2 + ", " + y2;
            g.drawLine(x1, y1, x2, y2);
            g.drawLine(x1+1, y1+1, x2+1, y2+1);
            g.drawLine(x1+2, y1+2, x2+2, y2+2);
            g.setColor(Color.yellow);
            g.fillRect(xm - 4, ym - 4, 8, 8);

            g.setColor(Color.pink);
            g.drawLine(originX, originY, xm, ym);

            g.setColor(Color.black);
            g.drawString(string0, offsetX, offsetY);
            g.drawString(string1, offsetX, offsetY + 20);
            g.drawString(string2, offsetX, offsetY + 40);
            offsetX += 150;
        }


    }

    public BasicLineView() {
        super();
        setLayout(null);
        lines = new ArrayList<FieldLine>();

    }


}


/*
enum class LineID {
  // Most general
  Line,

  // Two possibilities
  EndlineOrSideline,
  TopGoalboxOrSideGoalbox,
  SideGoalboxOrMidline,

  // More specific
  Sideline,
  SideGoalbox,

  // Most specific
  Endline,
  TopGoalbox,
  Midline
};
*/
