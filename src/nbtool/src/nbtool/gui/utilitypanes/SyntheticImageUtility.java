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

	    private int windowX = 600;
	    private int windowY = 500;

	    private int textBuffer = 5;
	    private int textBoxW = windowX - textBuffer - 10;
	    private int textBoxH = 30;
	    private int buf = 5; 

	    private int butX = 100;
	    private int butY = 30;

	    private int fieldW = 400;
	    private int fieldH = 300;
	    

	    Session sess;

		public SIU_Frame() {
	        super();
	        

	        this.setTitle("synthetics");
			this.setBounds(0, 0, windowX, windowY);

	        create = new JButton("Create");
	        create.setActionCommand("create");
	        x = new JTextField("X Coordinate");
	        y = new JTextField("Y Coordinate");
	        h = new JTextField("Heading Offset");
	        fullres = new JToggleButton("640x480");
	        top = new JToggleButton("Top");


	        create.addActionListener(this);
	        x.addActionListener(this);
	        y.addActionListener(this);
	        h.addActionListener(this);
	        fullres.addActionListener(this);
	        top.addActionListener(this);
	        
	        JPanel field = new JPanel() {
				@Override
				public void paintComponent(Graphics g) {
					super.paintComponent(g);
					// TODO: paint little field

                	// TODO: draw arrow on field based on x and y and h

				}

				// TODO: add listener and set x and y based on click, and repaint
				
	        };
	        
	        add(create);
	        add(x);
	        add(y);
	        add(h);
	        add(fullres);
	        add(top);
	        add(field);
	      	add(new JPanel()); // Drawing last added item the size of the window. This is hacked

	    	// Draw button on the bottom in the middle
	    	create.setBounds(windowX/2 - butX/2, windowY - butY - buf, butX, butY);
	        
	        // But three text boxes on top
	        x.setBounds(textBuffer, textBoxH*0 + buf*1, textBoxW, textBoxH);
	        y.setBounds(textBuffer, textBoxH*1 + buf*2, textBoxW, textBoxH);
	        h.setBounds(textBuffer, textBoxH*2 + buf*3, textBoxW, textBoxH);

	        // Place setting buttons and field view
	       	fullres.setBounds(buf*1 + butX*0, textBoxH*3 + buf*4, butX, butY);
	    	top.    setBounds(buf*2 + butX*1, textBoxH*3 + buf*4, butX, butY);
	    	field.setBounds(windowX/2 - fieldW/2, textBoxH*4 + buf*5, fieldW, fieldH);


	        sess = null;

	        this.validate();
        	this.repaint(); 
		}



	    @Override
	    public void actionPerformed(ActionEvent e) {	   
	        if ("create".equals(e.getActionCommand())) {

	            // Parse gui
	            String xToSend = x.getText();
	            String yToSend = y.getText();
	            String hToSend = h.getText();
	            String fullresToSend = Boolean.toString(!fullres.isSelected());
	            String topToSend = Boolean.toString(!top.isSelected());

	            // TODO: check that x y and h are in bounds and are numbers

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
	        } else if ("640x480".equals(e.getActionCommand())) {
	        	fullres.setText("320x240");
	        } else if ("Top".equals(e.getActionCommand())) {
	        	top.setText("Bottom");
	        } else if ("320x240".equals(e.getActionCommand())) {
	        	fullres.setText("640x480");
	        } else if ("Bottom".equals(e.getActionCommand())) {
	        	top.setText("Top");
	        }

	        // TODO: if x, y, or h is changes, repaint 'field'
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
