package teamcomm;

import com.jogamp.opengl.GLProfile;
import java.net.SocketException;
import javax.swing.JOptionPane;
import teamcomm.data.GameState;
import teamcomm.gui.MainWindow;
import teamcomm.net.GameControlDataReceiver;
import teamcomm.net.SPLStandardMessageReceiver;
import teamcomm.net.logging.LogReplayer;
import teamcomm.net.logging.Logger;

/**
 * The team communication monitor starts in this class.
 *
 * @author Felix Thielke
 */
public class TeamCommunicationMonitor {

    private static boolean shutdown = false;
    private static final Object shutdownMutex = new Object();

    /**
     * Startup method of the team communication monitor.
     *
     * @param args This is ignored.
     */
    public static void main(final String[] args) {
        GameControlDataReceiver gcDataReceiver = null;
        SPLStandardMessageReceiver receiver = null;

        // Initialize the JOGL profile for 3D drawing
        GLProfile.initSingleton();

        // Initialize listener for GameController messages
        try {
            gcDataReceiver = new GameControlDataReceiver();
        } catch (SocketException ex) {
            JOptionPane.showMessageDialog(null,
                    "Error while setting up GameController listener.",
                    "SocketException",
                    JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }

        // Initialize listeners for robots
        receiver = SPLStandardMessageReceiver.getInstance();

        // Initialize robot view part of the GUI
        final MainWindow robotView = new MainWindow();

        // Start threads
        gcDataReceiver.start();
        receiver.start();

        // Wait for shutdown
        try {
            synchronized (shutdownMutex) {
                while (!shutdown) {
                    shutdownMutex.wait();
                }
            }
        } catch (InterruptedException ex) {
        }

        // Shutdown threads and clean up
        GameState.getInstance().shutdown();
        receiver.interrupt();
        gcDataReceiver.interrupt();
        robotView.terminate();
        LogReplayer.getInstance().close();
        Logger.getInstance().closeLogfile();

        // Try to join receiver threads
        try {
            gcDataReceiver.join(1000);
            receiver.join(1000);
        } catch (InterruptedException ex) {
        }

        // Force exit
        System.exit(0);
    }

    /**
     * Shuts down the program by notifying the main thread.
     */
    public static void shutdown() {
        synchronized (shutdownMutex) {
            shutdown = true;
            shutdownMutex.notifyAll();
        }
    }
}
