
// This file is part of TOOL, a robotics interaction and development
// package created by the Northern Bites RoboCup team of Bowdoin College
// in Brunswick, Maine.
//
// TOOL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TOOL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TOOL.  If not, see <http://www.gnu.org/licenses/>.

package edu.bowdoin.robocup.TOOL.Net;

import java.io.IOException;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.net.InetAddress;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

import edu.bowdoin.robocup.TOOL.TOOL;

public class TOOLNet implements ActionListener, Runnable {

    public static final int UDP_PORT  = 4000;
    public static final int TCP_PORT  = 4001;
    public static final int TOOL_PORT = 4002;
    public static final int UDP_BUFFER_SIZE = 1024;

    public static final int READ_INTERVAL = 100; // .1 second/s
    public static final int SEND_INTERVAL = 1000; // 1 second/s
    public static final int SO_TIMEOUT = 100; // .1 seconds per socket

    public static final int INET4_BRDCST_OFFSET = 3;
    public static final byte INET4_BRDCST_ADDR = (byte)255;
    public static final byte[] INET6_ALLHOSTS_ADDR = 
        new byte[] {(byte)255, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3};

    public static final String REQUEST_MSG = "TOOL:request";
    public static final String ACCEPT_MSG = "TOOL:accept";

    public static final String START_ACTION = "start";
    public static final String STOP_ACTION  = "stop";
    public static final String REFRESH_ACTION = "refresh";


    private Thread thread;
    private boolean running;
    private boolean updated;
    private long lastRequest;
    private long lastCheck;

    private DatagramSocket outgoing;
    private Map<String, NetworkInterface> interfaces;
    private Map<InetAddress, DatagramSocket> sockets;
    private Set<InetAddress> extraAddrs;

    private Map<String, InetAddress> hosts;
    private Map<InetAddress, String> reverseHosts;

    private Set<RobotListener> listeners;


    public TOOLNet() {
        thread = null;
        running = false;
        lastRequest = 0;
        lastCheck = 0;

        interfaces = getInterfaceMap();
        sockets = new ConcurrentHashMap<InetAddress, DatagramSocket>();
        extraAddrs = new HashSet<InetAddress>();

        hosts = new HashMap<String, InetAddress>();
        reverseHosts = new HashMap<InetAddress, String>();

        listeners = new HashSet<RobotListener>();

        outgoing = initSocket();
    }

    public synchronized void start() {
        while (thread != null && (running || thread.isAlive())) {
            running = false;
            try {
                thread.join();
            }catch (InterruptedException e) {
                // do nothing
            }
        }

        thread = new Thread(this);
        thread.start();
    }

    public void run() {
        running = true;

        while (running) {
            doRequest();

            while (running && timeSince(lastRequest) < SEND_INTERVAL) {
                if (updated) {
                    updated = false;
                    doRequest();
                }

                if (timeSince(lastCheck) > READ_INTERVAL)
                    doCheck();

                try {
                    Thread.sleep(READ_INTERVAL);
                }catch(InterruptedException e) {
                    // do nothing
                }
            }
        }
    }

    public synchronized void stop() {
        running = false;
    }

    /**
     * Add a specific host to the list of extra addresses to 'ping' with TOOL
     * request UDP packets.  By default, request packet is sent to the
     * broadcast address.  This method allows inclusion of specific host
     * addresses that may not recieve the broadcast message.
     *
     * @param host The name of the host to send additional targeted UDP
     * messages to
     * @throws UnknownHostException If the host name specified is not found in
     * DNS lookup and the address cannot be retrieved.
     */
    public synchronized void addCheckHost(String host)
            throws UnknownHostException {
        addCheckHost(InetAddress.getByName(host));
    }

    /**
     * Add a specific host to the list of extra addresses to 'ping' with TOOL
     * request UDP packets.  By default, request packet is sent to the
     * broadcast address.  This method allows inclusion of specific host
     * addresses that may not recieve the broadcast message.
     *
     * @param host The name of the host to send additional targeted UDP
     * messages to
     */
    public synchronized void addCheckHost(InetAddress addr) {
        TOOL.CONSOLE.message("Using address " + addr.getHostAddress() +
            " directly for TOOLNet robot discovery");
        extraAddrs.add(addr);
        updated = true;
    }

    /**
     * Return a list of the network interfaces available of the current host.
     * Used for selection of network interfaces and addresses.
     */
    public List<String> availableInterfaces() {
        Vector<String> v = new Vector<String>();
        try {
            Enumeration<NetworkInterface> en = 
                NetworkInterface.getNetworkInterfaces();

            while (en.hasMoreElements())
                v.add(en.nextElement().getDisplayName());
        }catch (SocketException e) {
            TOOL.CONSOLE.error(e, "Cannot retrieve network interfaces");
        }
        
        return v;
    }

    /**
     * Return a list of the addresses available on a given network interface.
     */
    public List<InetAddress> availableAddresses(String name) {
        Vector<InetAddress> v = new Vector<InetAddress>();

        NetworkInterface intface = interfaces.get(name);
        Enumeration<InetAddress> en = intface.getInetAddresses();
        InetAddress addr;
        byte[] rawBytes = null;
            
        while (en.hasMoreElements())
            v.add(en.nextElement());

        return v;
    }

    /**
     * Create a socket for use in broadcasting on this interface address.
     * Opens socket and adds the socket to the socket maps.
     */
    public synchronized void useAddress(InetAddress addr) {

        TOOL.CONSOLE.message("Using address " + addr.getHostAddress() + 
            " in TOOLNet broadcasts");
        DatagramSocket s = initSocket(addr);
        if (s != null) {
            sockets.put(addr, s);
            updated = true;
        }
    }

    public synchronized boolean usingAddress(InetAddress addr) {
        return sockets.containsKey(addr);
    }

    /**
     * Stop broadcasting and receiving on the given socket.  Will remove from
     * socket maps and close the socket.
     */
    public synchronized void haltAddress(InetAddress addr) {

        TOOL.CONSOLE.message("No longer using address " + 
            addr.getHostAddress() + " in TOOLNet broadcasts");
        DatagramSocket s = sockets.get(addr);
        if (s != null) {
            sockets.remove(addr);
            s.close();
            updated = true;
        }
    }

    /**
     * Create and return a generic UDP socket on the local host on the UDP
     * port, or null on error.
     */
    private DatagramSocket initSocket() {
        DatagramSocket s = null;
        try {
            s = new DatagramSocket();
        }catch (SocketException e) {
            TOOL.CONSOLE.error(e, "Cannot open outgoing UDP socket.  " +
                "Only broadcast-accessible robots will be available.");
        }

        return s;
    }

    /**
     * Create and return a UDP socket bound to the given address on the UDP
     * port and connected to the broadcast address on the UDP port, or null on
     * error.
     */
    private DatagramSocket initSocket(InetAddress addr) {
        DatagramSocket s = null;
        try {
            
            // Create and bind the socket
            s = new DatagramSocket(TOOL_PORT, addr);
            // Set broadcast ability for the socket
            s.setBroadcast(true);
            // Set a low timeout, to facilitate checking sockets in-line
            s.setSoTimeout(SO_TIMEOUT);

        }catch (SocketException e) {
            TOOL.CONSOLE.error(e, "Could not open broadcast UDP socket on " +
                "an interface address.  Broadcast on this interface will be " +
                "unavailable.");
        }

        return s;
    }

    private InetAddress getBroadcast(InetAddress addr) 
            throws UnknownHostException {
        byte[] rawBytes = addr.getAddress();
        if (addr instanceof Inet4Address)
            rawBytes[INET4_BRDCST_OFFSET] = INET4_BRDCST_ADDR;
        else
            rawBytes = INET6_ALLHOSTS_ADDR;

        //return InetAddress.getByAddress(rawBytes);
        //  hacked, for now, until we figure out something better to fix the
        //  broadcasting issue
        return InetAddress.getByName("255.255.255.255");
    }

    /**
     * Perform a network request operation, where broadcast and targeted
     * messages are sent over UDP.  Hopefully, any robots on the networks of
     * active interfaces will respond.
     */
    private synchronized void doRequest() {

        lastRequest = System.currentTimeMillis();

        DatagramPacket packet;

        for (Map.Entry<InetAddress, DatagramSocket> entry : 
                sockets.entrySet()) {

            // Send a broadcast message over each of the addresses being used
            try {
                packet = requestPacket(getBroadcast(entry.getKey()));
                entry.getValue().send(packet);
            }catch (UnknownHostException e) {
                TOOL.CONSOLE.error(e, "Couldn't resolve the broadcast " +
                    "address for address " + entry.getKey().getHostAddress());
            }catch (IOException e) {
                TOOL.CONSOLE.error(e, "Couldn't send broadcast message on a " +
                    "socket");
            }
        }

        if (outgoing != null) {
            for (InetAddress extra : extraAddrs) {
                // Also, send a request meesage directly to the hosts specified
                try {
                    packet = requestPacket(extra);
                    outgoing.send(packet);
                }catch (IOException e) {
                    TOOL.CONSOLE.error(e, "Couldn't send targeted message " +
                        "to " + extra.toString());
                }
            }
        }
    }

    /**
     * Performs a cycle of checking all active cycles for notification messages
     * from robots on the network.  Iterates through all the sockets and
     * attempts to retrieve a message.  Each socket times-out quickly
     * (SO_TIMEOUT constant) when a message is not available, but receives
     * continuously as long as messages are available.  Duration of check time
     * period is not guaranteed to be bounded by (# of sockets * SO_TIMEOUT).
     */
    private synchronized void doCheck() {

        lastCheck = System.currentTimeMillis();

        for (DatagramSocket sock : sockets.values())
            receiveNotify(sock);
    }

    /**
     * Attempt to receive a messages from a particular socket.  Catches timeout
     * exceptions, and returns.  As long as (valid) messages are recieved from
     * the socket, will continue to process them.
     */
    private void receiveNotify(DatagramSocket s) {
        byte[] buffer = new byte[UDP_BUFFER_SIZE];
        DatagramPacket packet = new DatagramPacket(buffer, UDP_BUFFER_SIZE);

        try {
            s.receive(packet);

            if (packet.getLength() < ACCEPT_MSG.length() + 1)
                // not enough data sent
                return;

            String head = new String(buffer, 0, ACCEPT_MSG.length(), "ASCII");
            if (head.equals(ACCEPT_MSG)) {
                // FORMAT - 'TOOL:accept:LENGTH:NAME'
                //    where LENGTH is a single byte length of NAME
                //
                int len = (int)buffer[ACCEPT_MSG.length() + 1];
                if (packet.getLength() < ACCEPT_MSG.length() + 3 + len)
                    // not enough data sent
                    return;


                String name = new String(buffer, ACCEPT_MSG.length() + 3,
                    len, "ASCII");
                InetAddress addr = packet.getAddress();

                //TOOL.CONSOLE.message("Got a packet!  From " + name + " at " +
                //    addr.getHostAddress());

                handleRobotResponse(name, addr);
            }
        }catch (SocketTimeoutException e) {
            // do nothing
        }catch (IOException e) {
            // TODO - better error handling
            TOOL.CONSOLE.error(e, "Error receiving notify message from host");
        }
    }





    private static Map<String, NetworkInterface> getInterfaceMap() {
        Map<String, NetworkInterface> map =
            new ConcurrentHashMap<String, NetworkInterface>();

        try {
            Enumeration<NetworkInterface> en = 
                NetworkInterface.getNetworkInterfaces();

            while (en.hasMoreElements()) {
                NetworkInterface intface = en.nextElement();
                map.put(intface.getDisplayName(), intface);
            }
        }catch (SocketException e) {
            // TODO - error handling
        }

        return map;
    }

    private static DatagramPacket requestPacket(InetAddress addr) {
        return new DatagramPacket(REQUEST_MSG.getBytes(),
            REQUEST_MSG.length(), addr, UDP_PORT);
    }

    private static long timeSince(long time) {
        return System.currentTimeMillis() - time;
    }

    //
    // RobotListener handling methods
    //

    public void addRobotListener(RobotListener r) {
        listeners.add(r);
    }

    public void removeRobotListener(RobotListener r) {
        listeners.remove(r);
    }

    private void handleRobotResponse(String name, InetAddress addr) {
        if (hosts.containsKey(name)) {

            if (!hosts.get(name).equals(addr)) {
                fireRobotLost(name);
                fireRobotFound(name, addr);
            }

        }else {
            if (reverseHosts.containsKey(addr)) {
                fireRobotLost(reverseHosts.get(addr));
            }

            fireRobotFound(name, addr);
        }
    }

    private void fireRobotFound(String name, InetAddress addr) {
        hosts.put(name, addr);
        reverseHosts.put(addr, name);

        for (RobotListener l : listeners)
            l.robotFound(name, addr);
    }
    
    private void fireRobotLost(String name) {
        reverseHosts.remove(hosts.get(name));
        hosts.remove(name);

        for (RobotListener l : listeners)
            l.robotLost(name);
    }

    //
    // ActionListeenr contract
    //

    public void actionPerformed(ActionEvent e) {
    }


    // static test method

    public static void main(String[] args) {

        TOOLNet net = new TOOLNet();
        net.start();

        String intface = net.availableInterfaces().get(0);
        List<InetAddress> addrs = net.availableAddresses(intface);
        net.useAddress(addrs.get(1));

        TOOL.CONSOLE.message("Initialized TOOLNet");
    }
}

