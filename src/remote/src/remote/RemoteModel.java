package remote;

import ch.ethz.ssh2.Connection;
import ch.ethz.ssh2.Session;
import ch.ethz.ssh2.StreamGobbler;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;

/**
 * Underlying model for the robot Remote. Handles state of connections to robot.
 * Serves as gateway for sending commands to robot.
 */
public class RemoteModel {
    public static final String NAO_USERNAME = "nao";

    // Model State variables
    private Connection connection;
    private Session sesh;
    private BufferedWriter stdinWriter;
    private StreamGobbler stdout;
    private TerminalDisplayController displayController;

    // Robot identifiers
    private String host;
    private String robotName;
    private String username;

    private boolean hasConnection;

    public RemoteModel(){
        hasConnection = false;
    }

    /**
     * Connect to the host set previously.
     */
    public void connect() {
        if ((host == null) ||
            (hasConnection && connection.getHostname().equals(host))) {
            return;
        }

        String password = askForPassword();
        if (hasConnection = connectToHost(password)){
            startSession();
            spawnNewDisplay();
        }
    }

    /**
     * Close any old connections and start a new connection with the host.
     */
    private boolean connectToHost(String password){
        closeOldConnection();

        try {
            connection = new Connection(host);
            connection.connect();

            boolean isAuthenticated =
                connection.authenticateWithPassword(username, password);

            if (isAuthenticated == false) {
                throw new IOException("Authentication failed.");
            }

        } catch (IOException e) {
            System.out.println(e.getMessage());
            return false;
        }
        return true;
    }

    private void closeOldConnection(){
        if (connection != null){
            connection.close();
        }

        if (sesh != null){
            sesh.close();
        }
        hasConnection = false;
    }

    /**
     * Prompt the user for a password.
     */
    private String askForPassword(){
        JPasswordField passField = new JPasswordField();
        JOptionPane.showMessageDialog(null,
                passField,
                "Please enter Nao password",
                JOptionPane.QUESTION_MESSAGE);
        String password = new String(passField.getPassword());
        if (password == null || password.length() == 0){
            System.out.println("You must enter a password.");
        }
        return password;
    }

    /**
     * Start a shell over the connection which can accept commands.
     */
    private void startSession(){
        try {
            sesh = connection.openSession();
            sesh.requestDumbPTY();
            sesh.startShell();

            stdinWriter =
                new BufferedWriter(new OutputStreamWriter(sesh.getStdin()));

        } catch (IOException ex) {
            Logger.getLogger(RemoteModel.
                             class.getName()).log(Level.SEVERE, null, ex);
        }

        stdout = new StreamGobbler(sesh.getStdout());
    }

    public void restartNaoQi(String host){
        setHost(host);
        connect();
        runRemoteCommand("/etc/init.d/naoqi restart");
    }

    public void stopNaoQi(String host){
        setHost(host);
        connect();
        runRemoteCommand("/etc/init.d/naoqi stop");
    }

    public void logNaoQi(String host){
        setHost(host);
        connect();
        runRemoteCommand("naolog");
    }

    public void shutdown(String host){
        setHost(host);
        connect();
        int meantToDoThat =
            JOptionPane.
            showConfirmDialog(null,
                              "Did you mean to shutdown " + host + "?",
                              "Just checking", JOptionPane.YES_NO_OPTION);

        if (meantToDoThat == JOptionPane.YES_OPTION){
            runRemoteCommand("shutdown -h 0");
        }
    }

    private void runRemoteCommand(String command){
        if (!isConnected()){
            return;
        }

        try {
            stdinWriter.write(command);
            stdinWriter.newLine(); // End the line (like a terminal command)

            stdinWriter.flush(); // Need to do this so the buffer is
                                 // actually written out, otherwise
                                 // nothing happens

            /* Show exit status, if available (otherwise "null") */
            if (sesh.getExitStatus() != null && sesh.getExitStatus() != 0) {
                System.out.println("Command exited with code: " +
                                   sesh.getExitStatus());
            }
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
            Logger.getLogger(RemoteModel.
                             class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     * Create a new display of the connection's output
     */
    private void spawnNewDisplay(){
        displayController =
            new TerminalDisplayController(stdout,connection.getHostname());
    }

    public StreamGobbler getStdout(){
        return stdout;
    }

    public boolean isOutputAvailable(){
        return (sesh != null && sesh.getStdout() != null);
    }

    public boolean isConnected(){
        return hasConnection && sesh != null;
    }

    // @TODO Make these model state variables update on their own
    // while text is being entered
    public void setRobot(String robot){
        this.robotName = robot;
    }

    public void setUsername(String username){
        this.username = username;
    }

    public void setHost(String host){
        this.host = host;
    }
}

