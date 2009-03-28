package TOOL.WorldController;

import java.io.IOException;
import java.util.Iterator;
import java.util.Vector;
import java.net.*;

public class UDPServer extends Thread {

    final static boolean QUIET = true;

    //Default values for the port and the buffer space used in packets
    final static int DEF_BUF = 1024;
    final static int DEF_PORT = 14501;

    private DatagramSocket socket;    // Socket on which server listens
    private boolean receiving;  // Whether this server is currently listening
    private DatagramPacket packet;    // UDP packet (buffer) used to recieve data
    private Vector<RobotListener> listens; // Objects that will be notfied of data

    public UDPServer() {
	this(DEF_PORT);
    }

    /**
     * Creates a new UDPServer on the given port, which notifies the given UDPGui
     * with new Robots generated from packets recieved.
     *
     * @param port   The port on which the server should bind and listen for
     * packets
     */
    public UDPServer(int port) {
	super();
	try {
	    if (!QUIET)
		System.out.println("Binding socket to port " + port);
	    socket = new DatagramSocket(port);
	} catch (SocketException e) {
	    System.err.println("Could not open socket");
	    e.printStackTrace();
	    System.exit(1);
	}

	packet = new DatagramPacket(new byte[DEF_BUF],200);
	listens = new Vector<RobotListener>();
	receiving = false;
	start();
    }

    /**
     * The executing body of the server thread, listens on the port (while open)
     * and notifies the listeners with new Robots.  Exits upon socket 
     * closure/unbinding or when server is no longer receiving.  Does not exit on
     * packet receival error.
     */
    public void run() {
	while (socket != null && socket.isBound()) {
	    try {
		socket.receive(packet);
		if (isReceiving()) {
		    Robot robot = Robot.parseData(packet.getData(),packet.getLength());
		    if (robot != null) {
			notifyListeners(robot);
		    }
		    else {
			System.out.println("robot is null");
		    }
		}
	    }catch (IOException e) {
		System.err.println("Error receiving packet");
		e.printStackTrace();
	    }
	}
    }

    public void addRobotListener(RobotListener dl) {
	listens.add(dl);
    }

    public void removeRobotListener(RobotListener dl) {
	listens.remove(dl);
    }

    private void notifyListeners(Robot robot) {
	Iterator<RobotListener> itr = listens.iterator();
	while (itr.hasNext()) {
	    itr.next().updateRobot(robot);
	}
    }

    /**
     * Whether the server is currently receiving
     */
    public boolean isReceiving() {
	//if (receiving) System.out.println("isReceiving");
	return receiving;
    }

    /**
     * Changes the recieving state of this server.  When argument is true and
     * server is currently not receiving, starts a thread that recieves UDP
     * packets sent to the server and parses them for RobotData content
     *
     * @param toReceive   Whether or not the server should receive packets
     */
    public void setReceiving(boolean toReceive) {
	if (receiving && toReceive)
	    return;
    
	receiving = toReceive;
    
	if (receiving)
	    System.out.println("Listening to packets on the socket");
	else if (!QUIET)
	    System.out.println("No longer listening");
    }
  
    public static void main(String[] args) {
	(new UDPServer()).setReceiving(true);
    }
}
