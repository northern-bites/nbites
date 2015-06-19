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
                lines.add(line);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        repaint();
    }

    public void paintComponent(Graphics g) {

        int offsetY = this.getHeight()/2 + 20;
        int offsetX = 10;

        int multiplier = 2;
        double resize = 1.0;

        int originX = this.getWidth()/2;
        int originY = this.getHeight()/2 - 100;
        int height = this.getHeight()/2;
        int width = this.getWidth();

        g.setColor(new Color(40, 140, 40));
        g.fillRect(0, 0, this.getWidth(), this.getHeight()/2);

        g.setColor(Color.red);
        g.fillRect(originX-10, originY-10, 20, 20);

        System.out.println("origin: " + originX + ", " + originY + "\nheight: " + height + "\nwidth: " + width);

        if (lines.size() < 1) return;

        for (int i = 0; i <lines.size(); i++) {
            double r = lines.get(i).getInner().getR();
            double t = lines.get(i).getInner().getT();
            double eP0 = lines.get(i).getInner().getEp0();
            double eP1 = lines.get(i).getInner().getEp1();
            double mP = (eP1 + eP0) / 2.0;

            String stringr = "R: " + (double)Math.round(r * 100000) / 100000;
            String stringt = "t: " + (double)Math.round(t * 100000) / 100000;
            String stringep0 = "ep0: " + Math.round(eP0);
            String stringep1 = "ep1: " + Math.round(eP1);
            String stringid = "id: " + lines.get(i).getId();

            double x0 = originX + multiplier*r * Math.cos(t);
            double y0 = originY + -multiplier*r * Math.sin(t);
            int x1 = (int) Math.round(x0 + multiplier*eP0 * Math.sin(t));
            int y1 = (int) Math.round(y0 + multiplier*eP0 * Math.cos(t));
            int x2 = (int) Math.round(x0 + multiplier*eP1 * Math.sin(t));
            int y2 = (int) Math.round(y0 + multiplier*eP1 * Math.cos(t));
            int xm = (int) Math.round(x0 + multiplier*mP * Math.sin(t));
            int ym = (int) Math.round(y0 + multiplier*mP * Math.cos(t));

            if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0) continue;

            g.setColor(Color.blue);
            g.drawString(stringr, offsetX, offsetY + 80);
            g.setColor(Color.black);
            g.drawString(stringt, offsetX, offsetY + 100);
            g.drawString(stringep0, offsetX, offsetY + 120);
            g.drawString(stringep1, offsetX, offsetY + 140);
            g.drawString(stringid, offsetX, offsetY + 160);
            g.setColor(Color.white);

            String string0 = "x0,y0: " + (int)x0 + ", " + (int)y0;
            String string1 = "x1,y1: " + x1 + ", " + y1;
            String string2 = "x2,y2: " + x2 + ", " + y2;
            g.drawLine(x1, y1, x2, y2);
            g.drawLine(x1+1, y1+1, x2+1, y2+1);
            g.drawLine(x1+2, y1+2, x2+2, y2+2);
            g.setColor(Color.yellow);
            g.fillRect(xm - 4, ym - 4, 8, 8);
            g.setColor(Color.blue);
            g.fillRect((int)x0 - 4, (int)y0 - 4, 8, 8);

            g.setColor(Color.pink);
            g.drawLine(originX, originY, xm, ym);
            g.setColor(Color.blue);
            g.drawLine(originX, originY, (int)x0, (int)y0);

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
