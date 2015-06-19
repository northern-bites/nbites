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
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.io.CrossIO;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.util.Events;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.Utility;

import java.util.List;
import java.util.ArrayList;
import java.util.Vector;

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

    private class CCU_Frame extends JFrame implements IOFirstResponder, ActionListener {
        private JButton top;
        private JButton getOffsets;
        private JButton save;

        private Session getSession;

        private boolean topCam = true;

        final int windowW = 600;
        final int windowH = 300;

        final int buf = 10;
        final int col1 = 200;   // column 1

        Session sess;

        public CCU_Frame() {
            super();

            this.setTitle("Camera Calibrator");
            this.setBounds(500, 0, windowW, windowH);

            try {
                getSession = SessionMaster.get().getLatestSession();
            } catch (Exception e) {
                System.out.printf("\nSelect a session with at least 7 logs of starmat to calibrate.\n\n");
                // TODO close window? display message?
                return;
            }
            final Session session = getSession;

            if (session.logs_ALL.size() < 7) {
                System.out.printf("\nSelect a session with at least 7 logs of starmat to calibrate.\n\n");
                
                // TODO close window? display message?
            }

            top = new JButton("Top");
            top.setToolTipText("toggle calibrating top or bottom logs in session");
            top.addActionListener(this);
            top.setBounds(buf, 50, 70, 30);

            getOffsets = new JButton("Get calibration offsets");
            getOffsets.setToolTipText("calculate and display roll and pitch offsets");
            getOffsets.addActionListener(this);
            getOffsets.setBounds(buf*2 + 100, 50, 200, 30);
            
            save = new JButton("Save");
            save.setToolTipText("save camera params to config/cameraParams.txt");
            save.addActionListener(this);
            save.setBounds(windowW/2 - 100/2, windowH - 40, 100, 30);
            save.setEnabled(false);

            final SessionMaster sm = new SessionMaster();

            JPanel panel = new JPanel() {
                @Override
                public void paintComponent(Graphics g) {
                    super.paintComponent(g);
                    g.drawString("Selected session: " + session.name, buf, buf*2);
                    g.drawString("Logs in session: " + session.logs_ALL.size(), buf, buf*4);
                    g.fillRect(500, 300, 100, 100);
                }                
            };

            add(top);
            add(getOffsets);
            add(save);
            add(panel);

            sess = null;

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
                System.out.printf("Number of logs before: %d\n", sessionLogs.size());
                for (Log l : sessionLogs) {
                    if (l.description().indexOf(search) != -1) {
                        usableLogs.add(l);
                        System.out.printf("Found log with %s\n", search);
                    }
                }
                
                System.out.printf("Number of logs after: %d\n", usableLogs.size());

                
                if (usableLogs.size() < 7) {
                    System.out.printf("\nFewer than seven ");
                    if (topCam)
                        System.out.printf("top ");
                    else 
                        System.out.printf("bottom ");
                    System.out.printf("logs in selected session. Cannot calibrate.\n\n");
                } else {

                    // For call calibrate nbfunc with the 7 logs
                    callNBFunction(usableLogs);
                }
            }
        }

        private void callNBFunction(List<Log> logs) {
            CrossInstance inst = CrossIO.instanceByIndex(0);
            if (inst == null)
                return;

            CrossFunc func = inst.functionWithName("CameraCalibration");
            if (func == null)
                return;

            Log[] logArray = new Log[7];
            int i = 0;
            for (Log l : logs) {
                logArray[i++] = logs.get(i);
            }

            CrossCall call = new CrossCall(this, func, logArray);
            inst.tryAddCall(call);
        }

        @Override
        public void ioFinished(IOInstance instance) {}

        @Override
        public void ioReceived(IOInstance inst, int ret, Log... out) {
            System.out.println("CamCal ioReceived");

            if (sess == null) {
                //sess = new Session(null, null);
                //master.sessions.add(sess);
                SessionMaster master = SessionMaster.get();
                sess = master.requestSession("cameraCalibrate");
            }

            System.out.println(out[0].description());
            sess.addLog(out[0]);
            Events.GLogsFound.generate(this, out);
        }

        @Override
        public boolean ioMayRespondOnCenterThread(IOInstance inst) {
            return false;
        }

    }

}
