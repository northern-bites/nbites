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
        return 's';
    }

    private class CCU_Frame extends JFrame implements IOFirstResponder, ActionListener {
        private JButton top;
        private JButton getOffsets;
        private JButton save;

        final int windowW = 400;
        final int windowH = 300;

        final int buf = 10;
        final int col1 = 200;   // column 1

        Session sess;

        public CCU_Frame() {
            super();

            this.setTitle("Camera Calibrator");
            this.setBounds(500, 0, windowW, windowH);

            top = new JButton("Top");
            top.setToolTipText("toggle calibrating top or bottom logs in session");
            top.addActionListener(this);
            top.setBounds(buf, 40, 100, 30);

            getOffsets = new JButton("Get calibration offsets");
            getOffsets.setToolTipText("calculate and display roll and pitch offsets");
            getOffsets.addActionListener(this);
            getOffsets.setBounds(buf*2 + 100, 40, 200, 30);
            
            save = new JButton("Save");
            save.setToolTipText("save camera params to config/cameraParams.txt");
            save.addActionListener(this);
            save.setBounds(windowW/2 - 100/2, windowH - 40, 100, 30);
            save.setEnabled(false);

            JPanel panel = new JPanel() {
                @Override
                public void paintComponent(Graphics g) {
                    super.paintComponent(g);
                    g.drawString("Selected session: ", buf, buf*2);

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
                    } else if ("Bottom".equals(e.getActionCommand())) {
                        top.setText("Top");
                    }
                }

        @Override
        public void ioFinished(IOInstance instance) {}

        @Override
        public void ioReceived(IOInstance inst, int ret, Log... out) {
            if (sess == null) {
                //sess = new Session(null, null);
                //master.sessions.add(sess);
                SessionMaster master = SessionMaster.get();
                sess = master.requestSession("synthetics");
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
