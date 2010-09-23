/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package remote;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import ch.ethz.ssh2.Connection;
import ch.ethz.ssh2.Session;
import ch.ethz.ssh2.StreamGobbler;


/**
 *
 */
public class RemoteController {

	private static final String NAO_USERNAME = "nao";

	private RemoteView view;

	public RemoteController() {
        view = new RemoteView(this);
	}

	public void updateNaoQiStatus(String ip){

	}

	public void restartNaoQi(String ip){
		executeRemoteNaoCommand(ip, "/etc/init.d/naoqi restart");
	}

	public void stopNaoQi(String ip){
		executeRemoteNaoCommand(ip, "/etc/init.d/naoqi stop");
	}

	public void executeRemoteNaoCommand(String ip, String command){
		// Ask for password to Nao
		if (ip == null || ip.length() == 0){
			System.out.println("No IP Address given, cannot execute remote command.");

		} else {
			String password = (String)JOptionPane.showInputDialog(view,
																  "Please enter password for nao@" + ip);

			if (password != null && password.length() > 0){
				sshExecCommand(ip, NAO_USERNAME, password, command);
			}
		}
	}

	public void sshExecCommand(String host, String username, String password,
							   String command){
		try{
			Connection conn = new Connection(host);

			conn.connect();

			boolean isAuthenticated = conn.authenticateWithPassword(username, password);

			if (isAuthenticated == false)
				throw new IOException("Authentication failed.");

			Session sess = conn.openSession();
			sess.execCommand(command);

			/* Show exit status, if available (otherwise "null") */
			if (sess.getExitStatus() != null && sess.getExitStatus() != 0){
				System.out.println("Command exited with code: " + sess.getExitStatus());
			}

			// Terminate
			sess.close();
			conn.close();

		} catch (IOException e) {
			System.out.println(e.getMessage());
		};
	}

	private void runShellCommand(String command){
		try{
			// using the Runtime exec method:
			Process p = Runtime.getRuntime().exec(command);
			try{
				p.waitFor();
			} catch (InterruptedException e){
			}
			System.out.println(command);
			printProcessOutput(p);
		} catch (java.io.IOException exc){

		}
	}

	private void printProcessOutput(Process p){
		try {
			BufferedReader stdInput = new
				BufferedReader(new InputStreamReader(p.getInputStream()));

			BufferedReader stdErr = new
				BufferedReader(new InputStreamReader(p.getErrorStream()));

			// read the output from the command
			String s;
			System.out.println("Here is the standard output of the command:\n");
			while ((s = stdInput.readLine()) != null) {
				System.out.println(s);
			}
			System.out.println("Here is the standard error of the command:\n");
			while ((s = stdErr.readLine()) != null) {
				System.out.println(s);
			}

		} catch (java.io.IOException exc) {

		}
	}

    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
		try {
			// Set System L&F
			UIManager.setLookAndFeel(
									 UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException excp){

		} catch (InstantiationException exc) {
		} catch (IllegalAccessException exc) {
		} catch (UnsupportedLookAndFeelException exc) {}
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new RemoteController();
            }
        });
    }

}
