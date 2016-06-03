package teamcomm.net;

import common.Log;
import data.SPLStandardMessage;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.util.Enumeration;
import java.util.concurrent.LinkedBlockingQueue;
import javax.swing.JOptionPane;
import teamcomm.PluginLoader;
import teamcomm.data.GameState;
import teamcomm.data.AdvancedMessage;
import teamcomm.net.logging.LogReplayer;
import teamcomm.net.logging.Logger;

/**
 * Singleton class for the thread which handles messages from the robots. It
 * spawns one thread for listening on each team port up to team number 100 and
 * processes the messages received by these threads.
 *
 * @author Felix Thielke
 */
public class SPLStandardMessageReceiver extends Thread {
    
    private class ReceiverThread extends Thread {
        
        private final MulticastSocket socket;
        private final int team;
        
        public ReceiverThread(final int team) throws IOException {
            setName("SPLStandardMessageReceiver_team" + team);
            
            this.team = team;

            // Bind socket to team port
            socket = new MulticastSocket(null);
            socket.setReuseAddress(true);
            socket.bind(new InetSocketAddress("0.0.0.0", getTeamport(team)));
            
            try {
                // Join multicast group on all network interfaces (for compatibility with SimRobot)
                socket.joinGroup(InetAddress.getByName("239.0.0.1"));
                final byte[] localaddr = InetAddress.getLocalHost().getAddress();
                localaddr[0] = (byte) 239;
                socket.joinGroup(InetAddress.getByAddress(localaddr));
                final Enumeration<NetworkInterface> nis = NetworkInterface.getNetworkInterfaces();
                while (nis.hasMoreElements()) {
                    final NetworkInterface ni = nis.nextElement();
                    final Enumeration<InetAddress> addrs = ni.getInetAddresses();
                    while (addrs.hasMoreElements()) {
                        final byte[] addr = addrs.nextElement().getAddress();
                        addr[0] = (byte) 239;
                        socket.joinGroup(InetAddress.getByAddress(addr));
                    }
                }
            } catch (SocketException ex) {
                // Ignore, because this is only for testing and does not work everywhere
            }
        }
        
        @Override
        public void run() {
            byte[] buffer = new byte[SPLStandardMessage.SIZE];
            while (!isInterrupted()) {
                try {
                    final DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                    socket.receive(packet);
                    
                    if (!LogReplayer.getInstance().isReplaying()) {
                        if (packet.getAddress().getAddress()[0] != 10) {
                            queue.add(new SPLStandardMessagePackage("10.0." + team + "." + buffer[5], team, buffer));
                        } else {
                            queue.add(new SPLStandardMessagePackage(packet.getAddress().getHostAddress(), team, buffer));
                        }
                    }
                    
                    buffer = new byte[SPLStandardMessage.SIZE];
                } catch (SocketTimeoutException e) {
                } catch (IOException e) {
                    Log.error("something went wrong while receiving the message packages: " + e.getMessage());
                }
            }
            
        }
    }
    
    private static SPLStandardMessageReceiver instance;
    
    private static final int MAX_TEAMNUMBER = 100;
    
    private final ReceiverThread[] receivers = new ReceiverThread[MAX_TEAMNUMBER];
    private final LinkedBlockingQueue<SPLStandardMessagePackage> queue = new LinkedBlockingQueue<>();

    /**
     * Constructor.
     *
     * @throws IOException if a problem occurs while creating the receiver
     * threads
     */
    public SPLStandardMessageReceiver() throws IOException {
        // Create receiver threads
        for (int i = 0; i < MAX_TEAMNUMBER; i++) {
            receivers[i] = new ReceiverThread(i + 1);
        }
    }

    /**
     * Returns the only instance of the SPLStandardMessageReceiver.
     *
     * @return instance
     */
    public static SPLStandardMessageReceiver getInstance() {
        if (instance == null) {
            try {
                instance = new SPLStandardMessageReceiver();
            } catch (IOException ex) {
                JOptionPane.showMessageDialog(null,
                        "Error while setting up packet listeners: " + ex.getMessage(),
                        "IOException",
                        JOptionPane.ERROR_MESSAGE);
                System.exit(-1);
            }
        }
        return instance;
    }
    
    @Override
    public void run() {
        try {
            // Start receivers
            for (final ReceiverThread receiver : receivers) {
                receiver.start();
            }

            // Handle received packages
            while (!isInterrupted()) {
                final SPLStandardMessagePackage p = queue.take();
                final SPLStandardMessage message;
                final Class<? extends SPLStandardMessage> c = PluginLoader.getInstance().getMessageClass(p.team);

                // Log package
                Logger.getInstance().log(p);

                // Handle message
                try {
                    message = c.newInstance();
                    message.fromByteArray(ByteBuffer.wrap(p.message));
                    if (message.teamNumValid && message.teamNum != p.team) {
                        message.teamNumValid = false;
                        message.valid = false;
                    }
                    
                    SPLStandardMessage m = message;
                    if (message instanceof AdvancedMessage && message.valid) {
                        try {
                            ((AdvancedMessage) message).init();
                        } catch (final Throwable e) {
                            m = SPLStandardMessage.createFrom(message);
                            Log.error(e.getClass().getSimpleName() + " was thrown while initializing custom message class " + c.getSimpleName() + ": " + e.getMessage());
                        }
                    }
                    
                    GameState.getInstance().receiveMessage(p.host, m.teamNumValid ? m.teamNum : p.team, m);
                } catch (InstantiationException | IllegalAccessException ex) {
                    Log.error("a problem occured while instantiating custom message class " + c.getSimpleName() + ": " + ex.getMessage());
                }
                Thread.yield();
            }
        } catch (InterruptedException ex) {
        } finally {
            for (final ReceiverThread receiver : receivers) {
                receiver.interrupt();
            }
            
            try {
                for (final ReceiverThread receiver : receivers) {
                    receiver.join();
                }
            } catch (InterruptedException ex) {
                
            }
        }
    }

    /**
     * Adds the given package to the queue in order to be processed.
     *
     * @param p package
     */
    public void addToPackageQueue(final SPLStandardMessagePackage p) {
        queue.add(p);
    }

    /**
     * Removes all pending packages from the queue.
     */
    public void clearPackageQueue() {
        queue.clear();
        try {
            Thread.sleep(100);
        } catch (InterruptedException ex) {
        }
    }
    
    private static int getTeamport(final int teamNumber) {
        return teamNumber + 10000;
    }
}
