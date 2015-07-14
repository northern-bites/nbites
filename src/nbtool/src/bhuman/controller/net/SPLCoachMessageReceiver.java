package controller.net;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;

import common.Log;
import controller.action.net.SPLCoachMessageReceived;
import data.SPLCoachMessage;

public class SPLCoachMessageReceiver extends Thread
{
    private static SPLCoachMessageReceiver instance;
    private final DatagramSocket datagramSocket;

    private SPLCoachMessageReceiver() throws SocketException
    {
        datagramSocket = new DatagramSocket(null);
        datagramSocket.setReuseAddress(true);
        datagramSocket.setSoTimeout(500);
        datagramSocket.bind(new InetSocketAddress(SPLCoachMessage.SPL_COACH_MESSAGE_PORT));
    }

    public synchronized static SPLCoachMessageReceiver getInstance()
    {
        if (instance == null) {
            try {
                instance = new SPLCoachMessageReceiver();
            } catch (SocketException e) {
                throw new IllegalStateException("fatal: Error while setting up Receiver.", e);
            }
        }
        return instance;
    }

    @Override
    public void run()
    {
        while (!isInterrupted()) {
            try {
                final ByteBuffer buffer = ByteBuffer.wrap(new byte[SPLCoachMessage.SIZE]);
                final DatagramPacket packet = new DatagramPacket(buffer.array(), buffer.array().length);
                datagramSocket.receive(packet);
                buffer.rewind();

                final SPLCoachMessage coach = new SPLCoachMessage();
                if (coach.fromByteArray(buffer)) {
                    new SPLCoachMessageReceived(coach).actionPerformed(null);
                }
            } catch (SocketTimeoutException e) { // ignore, because we set a timeout
            } catch (IOException e) {
                Log.error("something went wrong while receiving the coach packages : " + e.getMessage());
            }
        }

        datagramSocket.close();
    }
}
