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

public class BasicLineView extends ViewParent {
    
    List<Line> lines;

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

    class Line {
        public Line(){}
        public Line(int id, float r, float t, float ep0, float ep1) {
            this.id = id;
            this.r = r;
            this.t = t;
            this.ep0 = ep0;
            this.ep1 = ep1;
        }
        public int id;
        public float r, t, ep0, ep1;
    }

    @Override
    public void setLog(Log newlog) {
        String t = (String) newlog.primaryType();
        Class<? extends com.google.protobuf.GeneratedMessage> lClass = Utility.protobufClassFromType(t);
        Logger.logf(Logger.INFO, "ProtoBufView: using class %s for type %s.\n", lClass.getName(), t);
        com.google.protobuf.Message msg = Utility.protobufInstanceForClassWithData(lClass, newlog.bytes);

        Map<FieldDescriptor, Object> fields = msg.getAllFields();

        for (Map.Entry<FieldDescriptor, Object> entry : fields.entrySet()) {
            Map<FieldDescriptor, Object> lineFields = ((com.google.protobuf.Message)entry.getValue()).getAllFields();
            Line temp = new Line();
            for (Map.Entry<FieldDescriptor, Object> lineEntry : lineFields.entrySet()) {
                if (lineEntry.getKey().getName().equals("inner")) {
                    Map<FieldDescriptor, Object> houghFields = ((com.google.protobuf.Message)lineEntry.getValue()).getAllFields();
                    for (Map.Entry<FieldDescriptor, Object> houghEntry : houghFields.entrySet()) {
                        if (houghEntry.getKey().getName().equals("r")) { temp.r = (float)houghEntry.getValue();}
                        else if (houghEntry.getKey().getName().equals("t")) { temp.t = (float)houghEntry.getValue();}
                        else if (houghEntry.getKey().getName().equals("ep0")) { temp.ep0 = (float)houghEntry.getValue();}
                        else if (houghEntry.getKey().getName().equals("ep1")) { temp.ep1 = (float)houghEntry.getValue();}
                    }
                }
                else if (lineEntry.getKey().getName().equals("id")) {
                    temp.id = (int)lineEntry.getValue();
                }
            }
            lines.add(temp);
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
        // width =  newlog.tree().get(4).get(1).get(5).get(1).valueAsInt() / 2;
        // height = newlog.tree().get(4).get(1).get(6).get(1).valueAsInt() / 2;

        // displayw = width*2;
        // displayh = height*2;

        // fx0 = displayw + buffer;
        // fy0 = 0;

        // fxc = displayw + buffer + fieldw/2; 
        // fyc = fieldh;

        // originalImage = Utility.biFromLog(newlog);
    }
    
    public void paintComponent(Graphics g) {

        // if (originalImage != null) g.drawImage(originalImage, 0, 0, null);


        // if (edgeImage != null) { 
        //     g.drawImage(originalImage, 0, 0, displayw, displayh, null);
        //     g.drawImage(edgeImage, 0, displayh + buffer, displayw, displayh, null);
            

        //     g.setColor(new Color(90, 130, 90));
        //     g.fillRect(displayw + buffer, 0, fieldw, fieldh);

        //     int[] xPoints1 = {0 + fx0, 0 + fx0, fieldw/2 + fx0};
        //     int[] yPoints1 = {2, fieldh, fieldh};
        //     int[] xPoints2 = {fieldw + fx0, fieldw/2 + fx0, fieldw + fx0};
        //     int[] yPoints2 = {2, fieldh, fieldh};
        //     int n = 3;
        //     g.setColor(new Color(46, 99, 28));
        //     g.fillPolygon(xPoints1, yPoints1, n);
        //     g.fillPolygon(xPoints2, yPoints2, n);

        //     g.setColor(Color.lightGray);
        //     g.fillOval(fxc - 30, fyc - 20, 60, 40);

        //     for (int i = 0; i < lines.size(); i += 10) {
        //         double icR = lines.get(i);
        //         double icT = lines.get(i + 1);
        //         double icEP0 = lines.get(i + 2);
        //         double icEP1 = lines.get(i + 3);
        //         double houghIndex = lines.get(i + 4);
        //         double fieldIndex = lines.get(i + 5);
        //         double fcR = lines.get(i + 6);
        //         double fcT = lines.get(i + 7);
        //         double fcEP0 = lines.get(i + 8);
        //         double fcEP1 = lines.get(i + 9);

        //         // Draw it in image coordinates
        //         if (fieldIndex == -1)
        //             g.setColor(Color.red);
        //         else
        //             g.setColor(Color.blue);

        //         double x0 = 2*icR * Math.cos(icT) + originalImage.getWidth() / 2;
        //         double y0 = -2*icR * Math.sin(icT) + originalImage.getHeight() / 2;
        //         int x1 = (int) Math.round(x0 + 2*icEP0 * Math.sin(icT));
        //         int y1 = (int) Math.round(y0 + 2*icEP0 * Math.cos(icT));
        //         int x2 = (int) Math.round(x0 + 2*icEP1 * Math.sin(icT));
        //         int y2 = (int) Math.round(y0 + 2*icEP1 * Math.cos(icT));

        //         g.drawLine(x1, y1, x2, y2);

        //         // Image view line labels 
        //         double xstring = (x1 + x2) / 2;
        //         double ystring = (y1 + y2) / 2;

        //         double scale = 0;
        //         if (icR > 0)
        //             scale = 10;
        //         else
        //             scale = 3;
        //         xstring += scale*Math.cos(icT);
        //         ystring += scale*Math.sin(icT);

        //         g.drawString(Integer.toString((int) houghIndex) + "/" + Integer.toString((int) fieldIndex), 
        //                      (int) xstring, 
        //                      (int) ystring);

        //         // Calculate field coordinates to find resize value
        //         x0 =  3*fcR * Math.cos(fcT) + displayw + buffer + fieldw/2;
        //         y0 = -3*fcR * Math.sin(fcT) + fieldh;
        //         x1 = (int) Math.round(x0 + 3*fcEP0 * Math.sin(fcT));
        //         y1 = (int) Math.round(y0 + 3*fcEP0 * Math.cos(fcT));
        //         x2 = (int) Math.round(x0 + 3*fcEP1 * Math.sin(fcT));
        //         y2 = (int) Math.round(y0 + 3*fcEP1 * Math.cos(fcT));

        //         // Scale down if a line is outside the view, but not if its too far (false field line)
        //         if (y1 < 0 && y1 > -2500) {
        //             resize = Math.min(resize, (double)fieldh/(-y1 + fieldh));
        //         }
        //         if (y2 < 0 && y2 > -2500) {
        //             resize = Math.min(resize, (double)fieldh/(-y2 + fieldh));
        //         }

        //         // Don't draw it if it's way out
        //         if (y1 < -3500) {
        //             lines.set(i+4, -1.0);
        //         }
        //         if (y2 < -3500) {
        //             lines.set(i+4, -1.0);
        //         }
        //     }

        //     // Loop through again to draw lines in field space with calucluated resize value
        //     for (int i = 0; i < lines.size(); i += 10) {
        //         double houghIndex = lines.get(i + 4);

        //         if (houghIndex != -1) {
        //             double fieldIndex = lines.get(i + 5);
        //             double fcR = lines.get(i + 6);
        //             double fcT = lines.get(i + 7);
        //             double fcEP0 = lines.get(i + 8);
        //             double fcEP1 = lines.get(i + 9);

        //             // Draw it in field coordinates
        //             if (fieldIndex >= 0)
        //                 g.setColor(Color.white);
        //             else
        //                 g.setColor(Color.red);

        //             // Recalculate with resize
        //             double x0 =  3*resize*fcR * Math.cos(fcT) + displayw + buffer + fieldw/2;
        //             double y0 = -3*resize*fcR * Math.sin(fcT) + fieldh;
        //             int x1 = (int) Math.round(x0 + 3*resize*fcEP0 * Math.sin(fcT));
        //             int y1 = (int) Math.round(y0 + 3*resize*fcEP0 * Math.cos(fcT));
        //             int x2 = (int) Math.round(x0 + 3*resize*fcEP1 * Math.sin(fcT));
        //             int y2 = (int) Math.round(y0 + 3*resize*fcEP1 * Math.cos(fcT));

        //             g.drawLine(x1, y1, x2, y2);
        //         }
        //     }

        //     // Draw distance if click
        //     g.setColor(Color.black);
        //     if (click && clickX1 > fx0 && clickX1 < fx0+fieldw && clickY1 < fieldh) {
        //         g.drawLine(fxc, fyc, clickX1, clickY1);
        //         double distanceCM = Math.sqrt((clickX1-fxc)*(clickX1-fxc) + (clickY1-fyc)*(clickY1-fyc));
        //         distanceCM *= (1.0/3.0)*(1/resize);
        //         g.drawString(Double.toString((double)Math.round(distanceCM* 1000)/1000) + "cm",
        //             (fxc+clickX1)/2 + 5, (fyc+clickY1)/2);
        //         click = false;
        //     }

        //     // Draw distance if drag
        //     if (drag && clickX1 > fx0 && clickX1 < fx0 + fieldw && clickY1 < fieldh &&
        //                 clickX2 > fx0 && clickX2 < fx0 + fieldw && clickY2 < fieldh) {
        //         g.drawLine(clickX1, clickY1, clickX2, clickY2);
        //         double distanceCM = Math.sqrt((clickX1-clickX2)*(clickX1-clickX2) + (clickY1-clickY2)*(clickY1-clickY2));
        //         distanceCM *= (1.0/3.0)*(1/resize);
        //         double dString = (double)Math.round(distanceCM* 1000)/1000;
        //         if (dString != 0) {
        //             g.drawString(Double.toString(dString) + "cm", (clickX1+clickX2)/2 + 5, (clickY1+clickY2)/2);
        //         }
        //         drag = false;

        //     }
        // }
    }
    
    public BasicLineView() {
        super();
        setLayout(null);
        lines = new ArrayList<Line>();

    }


}
