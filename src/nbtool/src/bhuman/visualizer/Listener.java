package visualizer;

import common.Log;
import data.GameControlData;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;

/**
 * @author Michel Bartsch
 * 
 * This class receives the GameControlData from the GameController.
 */
public class Listener extends Thread
{
    /** The GUI to listen for, its update method will be called. */
    private GUI gui;
    /** Some attributes for receiving. */
    private DatagramSocket datagramSocket;

    /**
     * Creates a new Listener.
     */
    public Listener(GUI gui)
    {
        this.gui = gui;
        try {
            datagramSocket = new DatagramSocket(null);
            datagramSocket.setReuseAddress(true);
            datagramSocket.setSoTimeout(500);
            datagramSocket.bind(new InetSocketAddress(GameControlData.GAMECONTROLLER_GAMEDATA_PORT));
        } catch (SocketException e) {
            Log.error("Error on start listening to port " + GameControlData.GAMECONTROLLER_GAMEDATA_PORT);
            System.exit(1);
        }
    }

    @Override
    public void run()
    {
        while (!isInterrupted()) {
            final ByteBuffer buffer = ByteBuffer.wrap(new byte[GameControlData.SIZE]);
            final GameControlData data = new GameControlData();

            final DatagramPacket packet = new DatagramPacket(buffer.array(), buffer.array().length);

            try {
                datagramSocket.receive(packet);
                buffer.rewind();
                if (data.fromByteArray(buffer)) {
                    gui.update(data);
                }
            } catch (SocketTimeoutException e) { // ignore, because we set a timeout
            } catch (IOException e) {
                Log.error("Error while listening to port " + GameControlData.GAMECONTROLLER_GAMEDATA_PORT);
            }
        }
    }
}