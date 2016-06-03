package teamcomm;

import com.jogamp.opengl.GLProfile;
import common.ApplicationLock;
import java.awt.HeadlessException;
import java.io.IOException;
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

    private static boolean silentMode = false;

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

        parseArgs(args);

        // try to acquire the application lock
        final ApplicationLock applicationLock = new ApplicationLock("TeamCommunicationMonitor");
        try {
            if (!applicationLock.acquire()) {
                if (silentMode) {
                    System.out.println("An instance of TeamCommunicationMonitor already exists.");
                } else {
                    JOptionPane.showMessageDialog(null,
                            "An instance of TeamCommunicationMonitor already exists.",
                            "Multiple instances",
                            JOptionPane.WARNING_MESSAGE);
                }
                System.exit(0);
            }
        } catch (IOException | HeadlessException e) {
            if (silentMode) {
                System.out.println("Error while trying to acquire the application lock.");
            } else {
                JOptionPane.showMessageDialog(null,
                        "Error while trying to acquire the application lock.",
                        e.getClass().getSimpleName(),
                        JOptionPane.ERROR_MESSAGE);
            }
            System.exit(-1);
        }

        if (silentMode) {
            System.out.println("Team Communication Monitor was started in silent mode.\nMessages will be received and logged but not displayed.");
        }

        if (!silentMode) {
            // Initialize the JOGL profile for 3D drawing
            GLProfile.initSingleton();
        }

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
        final MainWindow robotView = silentMode ? null : new MainWindow();

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

        // Release the application lock
        try {
            applicationLock.release();
        } catch (IOException e) {
        }

        // Shutdown threads and clean up
        GameState.getInstance().shutdown();
        receiver.interrupt();
        gcDataReceiver.interrupt();
        if (robotView != null) {
            robotView.terminate();
        }
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

    private static final String ARG_HELP_SHORT = "-h";
    private static final String ARG_HELP = "--help";
    private static final String ARG_SILENT_SHORT = "-s";
    private static final String ARG_SILENT = "--silent";

    private static void parseArgs(final String[] args) {
        for (final String arg : args) {
            switch (arg) {
                case ARG_HELP_SHORT:
                case ARG_HELP:
                    System.out.println("Usage: java -jar TeamCommunicationMonitor.jar {options}"
                            + "\n  (-h | --help)                   display help"
                            + "\n  (-s | --silent)                 start in silent mode");
                    System.exit(0);
                case ARG_SILENT_SHORT:
                case ARG_SILENT:
                    silentMode = true;
            }
        }
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
