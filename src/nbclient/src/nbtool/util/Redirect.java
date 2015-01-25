package nbtool.util;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;

import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.text.BadLocationException;
import javax.swing.text.StyledDocument;

public class Redirect extends OutputStream implements Runnable {

	public String lineSoFar;
	private JTextPane pane;
	public String styleName;
	private PrintStream old;
	
	//Creating a new redirection
	public Redirect(JTextPane pn, String stynam, PrintStream old) {
		pane = pn;
		styleName = stynam;
		this.old = old;
		lineSoFar = "";
	}
	
	//Creating the update runnable.
	public Redirect(JTextPane pn, String str, String sn) {
		assert(pn != null && str != null);
		lineSoFar = str;
		pane = pn;
		styleName = sn;
	}
	
	public void write(int arg0) throws IOException {
		char c = (char) arg0;
		lineSoFar = lineSoFar.concat(String.valueOf(c));
		
		if (c == '\n') {
			Redirect runnable = new Redirect(pane, lineSoFar, styleName);
			SwingUtilities.invokeLater(runnable);
			
			lineSoFar = "";
		}
		
		old.write(arg0);
	}

	public void run() {
		StyledDocument sd = pane.getStyledDocument();
		
		try {
			sd.insertString(sd.getLength(), lineSoFar, sd.getStyle(styleName));
		} catch(BadLocationException ble) {
			ble.printStackTrace(System.err);
		}
		
		pane.repaint();
	}

	

}
