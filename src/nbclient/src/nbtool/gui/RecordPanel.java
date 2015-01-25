package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.PrintStream;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;

import nbtool.util.Redirect;

public class RecordPanel extends JPanel implements ActionListener{
	private static final long serialVersionUID = 3563150586352832989L;
	protected RecordPanel() {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		stdout_view = new JTextPane() {
		    public boolean getScrollableTracksViewportWidth()
		    {
		        return false;
		    }
		};
		
		stdout_view.setEditable(false);
		StyledDocument sd  = stdout_view.getStyledDocument();
		
		
		Style stdoutStyle = sd.addStyle("stdout", null);
		Style remoteStyle = sd.addStyle("remote", null);
		Style stderrStyle = sd.addStyle("stderr", null);
		
		StyleConstants.setForeground(stdoutStyle, Color.DARK_GRAY);
		StyleConstants.setForeground(remoteStyle, Color.BLUE);
		StyleConstants.setForeground(stderrStyle, Color.RED);
		
		System.setOut(new PrintStream(new Redirect(stdout_view, "stdout", System.out)));
		System.setErr(new PrintStream(new Redirect(stdout_view, "stderr", System.err)));
		
		clear_button = new JButton("clear");
		clear_button.addActionListener(this);
		
		sp = new JScrollPane(stdout_view);
		add(clear_button);
		add(sp);
		setVisible(true);
	}
	
	public void useSize(Dimension size) {
		// TODO Auto-generated method stub
		Dimension d = clear_button.getPreferredSize();
		clear_button.setBounds(5, 5, d.width, d.height);
		
		sp.setBounds(0, 5 + d.height, size.width, 
				size.height - 5 - d.height);
	}
	
	public void actionPerformed(ActionEvent e) {
		stdout_view.setText("");
	}
	
	private JScrollPane sp;
	private JTextPane stdout_view;
	private JButton clear_button;
}
