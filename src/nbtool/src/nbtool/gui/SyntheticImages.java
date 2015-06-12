package nbtool.gui;

import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CrossIO.CrossCall;
import nbtool.data.SessionMaster;
import nbtool.data.Session;
import nbtool.data.Log;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.Events;

public class SyntheticImages extends JPanel implements IOFirstResponder, ActionListener {
	private static final long serialVersionUID = 1L;

    private JButton create;
    private JTextField x;
    private JTextField y;
    private JTextField h;
    private JToggleButton fullres;
    private JToggleButton top;

    Session sess;

	public SyntheticImages() {
        super();

        create = new JButton("Create");
        create.setActionCommand("create");
        x = new JTextField(20);
        y = new JTextField(20);
        h = new JTextField(20);
        fullres = new JToggleButton("640x480");
        top = new JToggleButton("Top");

        create.addActionListener(this);
        x.addActionListener(this);
        y.addActionListener(this);
        h.addActionListener(this);
        fullres.addActionListener(this);
        top.addActionListener(this);

        add(create);
        add(x);
        add(y);
        add(h);
        add(fullres);
        add(top);

        sess = null;
	}

    @Override
    public void actionPerformed(ActionEvent e) {
        if ("create".equals(e.getActionCommand())) {
            // Parse gui
            String xToSend = x.getText();
            String yToSend = y.getText();
            String hToSend = h.getText();
            String fullresToSend = Boolean.toString(fullres.isSelected());
            String topToSend = Boolean.toString(top.isSelected());

            // Create log
            String sexpr = "(nblog (version 6) (contents ((type SyntheticParams) (params " + 
                           xToSend + " " + yToSend + " " + hToSend +
                           " " + fullresToSend + " " + topToSend + "))))";
            Log params = new Log(sexpr, null);

            // Call nbcross
            CrossInstance ci = CrossIO.instanceByIndex(0);
            if (ci == null)
                return;
            CrossFunc func = ci.functionWithName("Synthetics");
            assert(func != null);
            
            CrossCall cc = new CrossCall(this, func, params);

            assert(ci.tryAddCall(cc));
        }
    }

	@Override
	public void ioFinished(IOInstance instance) {}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
        SessionMaster master = SessionMaster.get();
        if (sess == null) {
            sess = new Session(null, null);
            master.sessions.add(sess);     
        }

        System.out.println(out[0].description());
        sess.addLog(out[0]);
		Events.GToolStatus.generate(this, STATUS.RUNNING, sess.name);
		Events.GLogsFound.generate(this, out);
		Events.GToolStatus.generate(this, STATUS.IDLE, "idle");
    }

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
