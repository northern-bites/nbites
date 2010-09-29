/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package remote;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.*;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import ch.ethz.ssh2.Connection;
import ch.ethz.ssh2.Session;
import ch.ethz.ssh2.StreamGobbler;


/**
 * A Deployer controller for the Nao Robot and the Northern Bites nbites repository.
 *
 * Gives a GUI Front to common command line tasks to simplify robot setup and control.
 *
 * This RemoteController is the backend for the GUI application.
 */
public class RemoteController {

	private static final String NAO_USERNAME = "nao";

	// The GUI front-end
	private RemoteView view;

	public RemoteController() {
        view = new RemoteView(this);
	}

	/**
	 * Update our knowledge of whether NaoQi is running or not.
	 */
	public void updateNaoQiStatus(String ip){

	}

	/**
	 * Restart NaoQi on the robot at the given IP Address.
	 */
	public void restartNaoQi(String ip){
		executeRemoteNaoCommand(ip, "/etc/init.d/naoqi restart");
	}

	/**
	 * Stop NaoQi on the robot at the given IP Address.
	 */
	public void stopNaoQi(String ip){
		executeRemoteNaoCommand(ip, "/etc/init.d/naoqi stop");
	}

	/**
	 * @param ip        IP Address of the robot to execute the command on.
	 * @param command   Shell command to execute remotely on the Nao.
	 */
	public void executeRemoteNaoCommand(String ip, String command){

		// We must be given a robot to execute a command on
		if (ip == null || ip.length() == 0){
			System.out.println("No IP Address given, cannot execute remote command.");

		} else {
			// Ask for password to Nao
			String password = (String)JOptionPane.showInputDialog(view,
																  "Please enter password for nao@" + ip);

			if (password != null && password.length() > 0){
				sshExecCommand(ip, NAO_USERNAME, password, command);
			}
		}
	}

	/**
	 * SSH into the given host and execute the given shell command.
	 *
	 * Uses Ganymed-SSH2 Java Library for SSH interaction with robot
	 */
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


	/**
	 * Run a local shell command and print the output.
	 */
	private void runShellCommand(String command, boolean blocks){
		try{
			// using the Runtime exec method:
			Process p = Runtime.getRuntime().exec(command);
			if (blocks) {
				try{
					p.waitFor();
				} catch (InterruptedException e){}
			}
			System.out.println(command);
			printProcessOutput(p);
		} catch (java.io.IOException exc){

		}
	}

	/**
	 * @param Process    The process from which output is to be gathered and printed.
	 */
	private void printProcessOutput(Process p){
		try {
			BufferedReader stdInput = new
				BufferedReader(new InputStreamReader(p.getInputStream()));

			BufferedReader stdErr = new
				BufferedReader(new InputStreamReader(p.getErrorStream()));

			// print the output from the command
			String s;
			while ((s = stdInput.readLine()) != null) {
				System.out.println(s);
			}

		} catch (java.io.IOException exc) {

		}
	}


	/**
	 * Use a GUI to configure the build.
	 */
	public void configure(String buildType){
		runShellCommand("make " + buildType + "_gui -C ../../src/man/", false);
	}

	/**
	 * Compile the man source.
	 */
	public void compile(String buildType){
		runShellCommand("make build_" + buildType + " -C ../../src/man/ -j3", false);
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
