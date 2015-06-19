package nbtool.gui.utilitypanes;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
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

public class SyntheticImageUtility extends UtilityParent {

	private SIU_Frame display = null;
	
	@Override
	public JFrame supplyDisplay() {
		if (display == null) {
			return (display = new SIU_Frame());
		}
		
		return display;
	}

	@Override
	public String purpose() {
		return "Programmatically generate images of the field";
	}

	@Override
	public char preferredMemnonic() {
		return 's';
	}

	private class SIU_Frame extends JFrame implements IOFirstResponder, ActionListener {
		private static final long serialVersionUID = 1L;

	    private JButton create;
	    private JTextField x;
	    private JTextField y;
	    private JTextField h;
	    private JToggleButton fullres;
	    private JToggleButton top;

	    Session sess;

		public SIU_Frame() {
	        super();
	        
	        this.setTitle("synthetics");
			this.setBounds(0, 0, 600, 400);

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
	        
	        this.setLayout(new BoxLayout(this.getContentPane(), BoxLayout.PAGE_AXIS));

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
