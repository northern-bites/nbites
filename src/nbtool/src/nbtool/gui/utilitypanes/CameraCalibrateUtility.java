package nbtool.gui.utilitypanes;

import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JToggleButton;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.io.FileIO;
import nbtool.io.CrossIO;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.util.Events;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.Utility;
import nbtool.util.Center;

import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;

import java.util.List;
import java.util.ArrayList;
import java.util.Vector;
import java.util.Map;

public class CameraCalibrateUtility extends UtilityParent {

    private CCU_Frame display = null;

    @Override
    public JFrame supplyDisplay() {
        if (display == null) {
            return (display = new CCU_Frame());
        }
        return display;
    }

    @Override
    public String purpose() {
        return "Calibrate camera from logs of star mat";
    }

    @Override
    public char preferredMemnonic() {
        return 'c';
    }

    private class CCU_Frame extends JFrame implements IOFirstResponder, ActionListener, Events.SessionSelected {
        private JButton top;
        private JButton getOffsets;
        private JButton save;

        private Session getSession;

        private String robotName;
        private boolean topCam = true;
        private boolean calculatedTopCam = true;


        final int windowW = 600;
        final int windowH = 300;

        final int buf = 10;
        final int col1 = 200;   // column 1

        private double rollOffset = 0.0;
        private double tiltOffset = 0.0; 

        public CCU_Frame() {
            super();

            Center.listen(Events.SessionSelected.class, this, true);

            this.setTitle("Cheddar's Cool Camera Calibrator");
            this.setBounds(500, 0, windowW, windowH);

            try {
                getSession = SessionMaster.get().getLatestSession();
            } catch (Exception e) {
                System.out.printf("\nSelect a session with at least 7 logs of starmat to calibrate.\n\n");
                // TODO close window? display message?
                return;
            }

            initiate();
           
        }

        private void initiate() {

            final Session session = getSession;

            if (session.logs_ALL.size() < 7) {
                System.out.printf("\nSelect a session with at least 7 logs of starmat to calibrate.\n\n");                
                // TODO close window? display message?
            }

            top = new JButton("Top");
            top.setToolTipText("toggle calibrating top or bottom logs in session");
            top.addActionListener(this);
            top.setBounds(buf, 50, 85, 30);

            getOffsets = new JButton("Get calibration offsets");
            getOffsets.setToolTipText("calculate and display roll and tilt offsets");
            getOffsets.addActionListener(this);
            getOffsets.setBounds(buf*2 + 85, 50, 200, 30);
            
            save = new JButton("Save");
            save.setToolTipText("save camera params to config/calibrationParams.txt");
            save.addActionListener(this);
            save.setBounds(windowW/2 - 100/2, windowH - 40, 100, 30);
            save.setEnabled(false);

            final SessionMaster sm = new SessionMaster();

            JPanel panel = new JPanel() {
                @Override
                public void paintComponent(Graphics g) {
                    super.paintComponent(g);
         //               System.out.printf("Sess1: %s\n", session);
                    g.drawString("Selected session: " + session.name, buf, buf*2);
                    g.drawString("Logs in session: " + session.logs_ALL.size(), buf, buf*4);

                    // Check to see if we have recieved io from nbcross to display
                    if (rollOffset != 0.0 || tiltOffset != 0.0) {
                        g.drawString("Calculated calibration offsets:", buf*6, buf*11);
                        g.drawString("Roll offset: " + Double.toString(rollOffset) + " rad.", buf*8, buf*14);
                        g.drawString("Tilt offset: " + Double.toString(tiltOffset) + " rad.", buf*8, buf*16);
                        g.drawString("For camera: " +  (topCam ? "TOP" : "BOTTOM"), buf*8, buf*18);
                        g.drawString("For robot: " + robotName, buf*8, buf*20);

                        save.setEnabled(true);
                    }
                }                
            };

            add(top);
            add(getOffsets);
            add(save);
            add(panel);

            this.validate();
            this.repaint(); 
        }

        @Override
        public void actionPerformed(ActionEvent e) {       
            if ("Top".equals(e.getActionCommand())) {
                top.setText("Bottom");
                topCam = false; 
            } else if ("Bottom".equals(e.getActionCommand())) {
                top.setText("Top");
                topCam = true;
            } else if ("Get calibration offsets".equals(e.getActionCommand())) {

                // Check to see if there are at least 7 tomCam tripoints in the session
                ArrayList<Log> sessionLogs = new ArrayList<Log>(getSession.logs_ALL);
                String search = topCam ? "(from camera_TOP)" : "(from camera_BOT)";
                List<Log> usableLogs = new Vector<Log>();
                for (Log l : sessionLogs) {
                    if (l.description().indexOf(search) != -1) {
                        usableLogs.add(l);
                    }
                }
                
                if (usableLogs.size() < 7) {
                    System.out.printf("\nFewer than seven ");
                    if (topCam)
                        System.out.printf("top ");
                    else 
                        System.out.printf("bottom ");
                    System.out.printf("logs in selected session. Cannot calibrate.\n\n");
                } else {

                    // Load all logs to avoid lazy load bugs
                    for (Log l : usableLogs) {
                        if (l.bytes == null) {
                            try {
                                FileIO.loadLog(l, getSession.directoryFrom);
                            } catch (IOException e1) {
                                e1.printStackTrace();
                                return;
                            }
                        }
                    }

                    // Fetch name of robot
                    SExpr name = usableLogs.get(0).tree().find("from_address");
                    if (!name.exists()) {
                        System.out.printf("COULD NOT LOAD ROBOT NAME. ABORTING.\n");
                    } else {
                        robotName = name.get(1).value();

                        // Call calibrate nbfunc with the 7 logs
                        callNBFunction(usableLogs);
                        calculatedTopCam = topCam;
                        
                    }
                }
            } else if ("Save".equals(e.getActionCommand())) {
                saveParamsToLisp();
            }
        }



        private void saveParamsToLisp() {
            String filePath = System.getenv().get("NBITES_DIR");
            filePath += "/src/man/config/calibrationParams.txt";

            String lisp = "(" + (calculatedTopCam ? "top" : "bottom") + " ";
            lisp += Double.toString(rollOffset) + " " + Double.toString(tiltOffset) + ")";
            
            String text = "";
            try {
                FileReader fr = new FileReader(filePath);
                BufferedReader bf = new BufferedReader(fr);
                String line;
                while ((line = bf.readLine()) != null) {
                    text += line;
                }
                SExpr saved = SExpr.deserializeFrom(text);
                SExpr bot = saved.get(1).find(robotName);
                if (!bot.exists()) {
                    System.out.printf("Invalid robot name! Could not find exiting params for \"%s\"\n", robotName);
                } else {
                    if (calculatedTopCam) {
                        bot.setList(bot.get(0), SExpr.deserializeFrom(lisp), bot.get(2));
                    } else {
                        bot.setList(bot.get(0), bot.get(1), SExpr.deserializeFrom(lisp));
                    }
                }

                // Write out to file
                FileOutputStream fos = new FileOutputStream(filePath, false);
                byte[] data = saved.print().getBytes();
                fos.write(data);

                fr.close();
                bf.close();
                fos.close();

            } catch (FileNotFoundException e1) {
                e1.printStackTrace();
            } catch (IOException e1) {
                e1.printStackTrace();
            }


        }

        private void callNBFunction(List<Log> logs) {
            CrossInstance inst = CrossIO.instanceByIndex(0);
            if (inst == null)
                return;

            CrossFunc func = inst.functionWithName("CameraCalibration");
            if (func == null) {
                return;
            }

            Log[] logArray = new Log[7];
            for (int i = 0; i < 7; i++) {
                logArray[i] = logs.get(i);
            }

            CrossCall call = new CrossCall(this, func, logArray);
            assert(inst.tryAddCall(call));
        }

        @Override
        public void sessionSelected(Object source, Session s) {
            // TODO: make work
            if (!this.isEnabled())
                return;
            getSession = s;
            initiate();
         //   System.out.printf("Sess: %s\n",getSession);

            // TODO: change session when a new session is selected? UX choice
        }

        @Override
        public void ioFinished(IOInstance instance) {}

        @Override
        public void ioReceived(IOInstance inst, int ret, Log... out) {
            System.out.println("CamCal ioReceived");

            SExpr atom = out[0].tree().find("roll");
            if (atom.exists()) {
                rollOffset = atom.get(1).valueAsDouble();
            } else {
                System.out.printf("IORecieved without roll offset!\n");
            }

            atom = out[0].tree().find("tilt");
            if (atom.exists()) {
                tiltOffset = atom.get(1).valueAsDouble();
            } else {
                System.out.printf("IORecieved without tilt offset!\n");
            }

            if (out[0].tree().find("failure").exists()) {
                System.out.printf("Log calibreation failure! Take better logs.\n");
                // TODO: handle failue better
            } else {
                repaint();
            }
            
        }

        @Override
        public boolean ioMayRespondOnCenterThread(IOInstance inst) {
            return false;
        }



    }

}
