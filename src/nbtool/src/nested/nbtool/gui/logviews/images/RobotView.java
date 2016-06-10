package nbtool.gui.logviews.images;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.Shape;
import java.awt.Color;

import java.awt.image.BufferedImage;
import nbtool.images.Y8Image;
import nbtool.images.Y8ThreshImage;

import nbtool.data.log.Log;
import nbtool.data.SExpr;

import javax.swing.JSlider;
import javax.swing.JButton;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import javax.swing.event.*;

import nbtool.gui.logviews.misc.VisionView;
// import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
// import nbtool.util.Debug;
// import nbtool.io.CrossIO;
// import nbtool.io.CrossIO.CrossFunc;
// import nbtool.io.CrossIO.CrossInstance;
// import nbtool.io.CrossIO.CrossCall;

import nbtool.util.Utility;
import java.util.Vector;

public class RobotView extends VisionView
     implements IOFirstResponder {

    // Log in;
    BufferedImage original;
    BufferedImage whiteImage;
    BufferedImage wgradientImage; // right now just do green image until i get grad working
    Log robots;

    // Gradient sliders
    private JSlider gFuzzyU;
    private JSlider gFuzzyV;

    // Save button
    private JButton saveButton;

    // Some operations should only happen on first load
    private boolean firstLoad;

    int width;
    int height;

    final double wPrecision = 200.0;
    final double gPrecision = 300.0;
    final double oPrecision = 200.0;

    public RobotView() {
        super();
        setLayout(null);
        firstLoad = true;

        ChangeListener slide = new ChangeListener(){
            public void stateChanged(ChangeEvent e) {
                // adjustParams();
                repaint();
            }
        };

        ActionListener press = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                // saveParams();
            }
        };

        // Save button
        saveButton = new JButton("Save fuzzy gradient parameters");
        saveButton.setToolTipText("Save current fuzzy parms to config/gradientParams.txt");
        saveButton.addActionListener(press);
        add(saveButton);

        // Init and add green sliders.
        gFuzzyU = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        gFuzzyV = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);

        gFuzzyU.addChangeListener(slide);
        gFuzzyV.addChangeListener(slide);

        add(gFuzzyU);
        add(gFuzzyV);
    }

    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        int vB = height/2 + 20;  // verticle buffer
        int hB = width/2 + 20; // horizontal buffer
        int sH = 15; // slider height
        int tB = 20; // text buffer
        int lB = 5;  // little buffer

        if (original != null) {
            g.drawImage(original, lB, lB, width/2, height/2, null);
        }

        if (whiteImage != null) {
            g.drawImage(whiteImage, hB, lB, width/2, height/2, null);
        } else {
            System.out.println("[ROBOT VIEW] white image was null");
        }

        if (wgradientImage != null) {
            drawRobots();
            g.drawImage(wgradientImage, lB, vB, width/2, height/2, null);
        } else {
            System.out.printf("[ROBOT VIEW] gradient image was null\n");
        }

        gFuzzyU.setBounds(hB*1 + lB, vB + sH*2 + tB*3, width/2, sH);
        gFuzzyV.setBounds(hB*1 + lB, vB + sH*4 + tB*3, width/2, sH);
        g.drawString("width of fuzzy threshold for gradient", hB*1 + lB, vB + sH + tB*3);

        // Draw button
        saveButton.setBounds(hB*1 + lB,  vB + sH*6 + tB*3, width/2, tB*3);
    }

    public void drawRobots() {
        Graphics2D graph = wgradientImage.createGraphics();
        graph.setColor(Color.WHITE);
        String r = "robot";

        for(int i=0; ;i++)
        {
            SExpr tree = this.getRobotCandidatesBlock().parseAsSExpr();
            SExpr robot = tree.find(r+i);
            if (!robot.exists()){
                break;
            }
            int top = (int) robot.get(1).find("top").get(1).valueAsDouble();
            int bottom = robot.get(1).find("bottom").get(1).valueAsInt();
            int left = robot.get(1).find("left").get(1).valueAsInt();
            int right = robot.get(1).find("right").get(1).valueAsInt();

            graph.draw(new Rectangle2D.Double(left, top, right-left, bottom-top));
        }

        // this doesn't always draw - but it's not because of the SEXpr / message passing
        Graphics2D u_graph = original.createGraphics();
        u_graph.setColor(Color.RED);
        String ur = "urobot";

        for(int i=0; ;i++)
        {
            SExpr tree = this.getUnmergedRobotCandidatesBlock().parseAsSExpr();
            SExpr robot = tree.find(ur+i);
            if (!robot.exists()){
                break;
            }
            int top = (int) robot.get(1).find("top").get(1).valueAsDouble();
            int bottom = robot.get(1).find("bottom").get(1).valueAsInt();
            int left = robot.get(1).find("left").get(1).valueAsInt();
            int right = robot.get(1).find("right").get(1).valueAsInt();

            u_graph.draw(new Rectangle2D.Double(left*2, top*2, (right-left)*2, (bottom-top)*2));
        }
    }

    @Override
    public void setupVisionDisplay() {
        System.out.printf("[ROBOT VIEW] setting log\n");

        width = this.originalWidth();
        height = this.originalHeight();

        callNBFunction();

        this.original = this.getOriginal().toBufferedImage();
    }

    @Override
    public void ioFinished(IOInstance instance) {}

    @Override
    public void ioReceived(IOInstance inst, int ret, Log... out) {
        System.out.printf("[ROBOT VIEW] IO received\n");

        if (this.getWhiteBlock() != null) {
            Y8Image white8 = new Y8Image(width/2, height/2, this.getWhiteBlock().data);
            this.whiteImage = white8.toBufferedImage();
        }

        if (this.getRobotBlock() != null) {
            Y8Image robot8 = new Y8Image(width/2, height/2, this.getRobotBlock().data);
            this.wgradientImage = robot8.toBufferedImage();
        }

        if (firstLoad) {
            firstIoReceived(out);
        }

        repaint();
    }

    /* Called upon slide of any of the 18 sliders. Make an SExpr from the psitions
        of each of the sliders. If this.log doesn't have Params saved, add them. Else,
        replace them. Call nbfunction with updated log description.
    */
    public void adjustParams() {

        // This is called when the sliders are initialized, so dont run if it's first load,
        //  or if any values are zero b/c devide by zero error in frontEnd processing
        // if (firstLoad) { return; }
        // zeroParam();

        // SExpr newParams = SExpr.newKeyValue("Green", SExpr.newList(
        //     SExpr.newKeyValue("fuzzy_u", (float)(gFuzzyU.getValue()) / wPrecision),
        //     SExpr.newKeyValue("fuzzy_v", (float)(gFuzzyV.getValue()) / wPrecision))
        // );

        // // Look for existing Params atom in current this.log description
        // SExpr oldColor, newColor, oldParams = this.log.tree().find("Params");

        // // Add params or replace params
        // if (oldParams.exists()) {
        //     SExpr saveAtom = oldParams.get(1).find("SaveParams");
        //     this.log.tree().remove(saveAtom);
        //     oldParams.setList( SExpr.atom("Params"), newParams);
        // } else {
        //     this.log.tree().append(SExpr.pair("Params", newParams));
        // }

        // callNBFunction();
    }

   public void saveParams() {
        // SExpr params = this.log.tree().find("Params");

        // // Check to see if sliders have been moved (slider adjust would have saved params)
        // if (!params.exists())
        //     return;

        // // Add flag for nbfunction to save the params
        // params.get(1).append(SExpr.newKeyValue("SaveParams", "True"));

        // callNBFunction();
    }

    private void callNBFunction() {
        this.callVision();
        // CrossInstance inst = CrossIO.instanceByIndex(0);
        // if (inst == null)
        //     return;

        // CrossFunc func = inst.functionWithName("Vision");
        // if (func == null)
        //     return;

        // CrossCall call = new CrossCall(this, func, this.log);
        // inst.tryAddCall(call);
    }

    // Check to see if any parameters are zero to avoid devide-by-zero error later
    private void zeroParam() {
        if (gFuzzyU.getValue() == 0) gFuzzyU.setValue(1);
        if (gFuzzyV.getValue() == 0) gFuzzyV.setValue(1);
    }

    // If and only if it is the first load, we need to set the positions of the sliders
    private void firstIoReceived(Log... out) {
        // Set sliders to positions based on white, green, then orange images descriptions' s-exps
        // SExpr colors = out[2].tree();

        // gFuzzyU.setValue((int)(Float.parseFloat(colors.get(1).get(4).get(1).value()) * gPrecision));
        // gFuzzyV.setValue((int)(Float.parseFloat(colors.get(1).get(5).get(1).value()) * gPrecision));
        gFuzzyU.setValue(1);
        gFuzzyV.setValue(1);

        firstLoad = false;
    }
}
