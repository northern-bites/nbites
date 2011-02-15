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
 * A Deployer controller for the Nao Robot and the Northern Bites
 * nbites repository.
 *
 * Gives a GUI Front to common command line tasks to simplify robot
 * setup and control.
 *
 * This RemoteController is the backend for the GUI application.
 */
public class RemoteController {

    private static final String NAO_USERNAME = "nao";

    // The GUI front-end
    private RemoteView view;
    private RemoteModel model;

    public RemoteController() {
        model = new RemoteModel();
        view = new RemoteView(this, model);
    }

    /**
     * Restart NaoQi on the robot at the given IP Address.
     */
    public void restartNaoQi(String host){
        model.restartNaoQi(host);
    }

    /**
     * Stop NaoQi on the robot at the given IP Address.
     */
    public void stopNaoQi(String host){
        model.stopNaoQi(host);
    }

    public void shutdownRobot(String host){
        model.shutdown(host);
    }

    public void logNaoQi(String host){
        model.logNaoQi(host);
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
            printProcessOutput(p,command);
        } catch (java.io.IOException exc){

        }
    }

    /**
     * @param Process    The process from which output is to be gathered and printed.
     */
    private void printProcessOutput(Process p, String command){

            BufferedReader stdInput = new
                BufferedReader(new InputStreamReader(p.getInputStream()));

            BufferedReader stdErr = new
                BufferedReader(new InputStreamReader(p.getErrorStream()));

            TerminalDisplayController displayController =
                new TerminalDisplayController(new StreamGobbler(p.getInputStream()),
                                              command);
    }


    /**
     * Use a GUI to configure the build.
     */
    public void configure(String buildType, String robotName, String ip){
        runShellCommand("make " + buildType + "_gui -C ../../src/man/", false);
    }

    /**
     * Compile the man source.
     */
    public void compile(String buildType){
        runShellCommand("make build_" + buildType + " -C ../../src/man/ -j3", false);
    }

    /**
     * Install the compiled libman and Python to the robot.
     */
    public void install(String buildType){
        runShellCommand("../../build/man/" + buildType + "/upload.sh", false);
    }

    /**
     * @param args the command line arguments
     */
}
