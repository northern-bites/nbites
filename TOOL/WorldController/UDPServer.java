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
    private Vector<DogListener> listens; // Objects that will be notfied of data

    public UDPServer() {
	this(DEF_PORT);
    }

    /**
     * Creates a new UDPServer on the given port, which notifies the given UDPGui
     * with new Dogs generated from packets recieved.
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
	listens = new Vector<DogListener>();
	receiving = false;
	start();
    }

    /**
     * The executing body of the server thread, listens on the port (while open)
     * and notifies the listeners with new Dogs.  Exits upon socket 
     * closure/unbinding or when server is no longer receiving.  Does not exit on
     * packet receival error.
     */
    public void run() {
	while (socket != null && socket.isBound()) {
	    try {
		socket.receive(packet);
		if (isReceiving()) {
		    Dog dog = Dog.parseData(packet.getData(),packet.getLength());
		    if (dog != null) {
			notifyListeners(dog);
		    }
		    else {
			System.out.println("dog is null");
		    }
		}
	    }catch (IOException e) {
		System.err.println("Error receiving packet");
		e.printStackTrace();
	    }
	}
    }

    public void addDogListener(DogListener dl) {
	listens.add(dl);
    }

    public void removeDogListener(DogListener dl) {
	listens.remove(dl);
    }

    private void notifyListeners(Dog dog) {
	Iterator<DogListener> itr = listens.iterator();
	while (itr.hasNext()) {
	    itr.next().updateDog(dog);
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
     * packets sent to the server and parses them for DogData content
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
