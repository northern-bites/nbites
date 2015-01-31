package nbtool.io;


import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.NBConstants;
import nbtool.util.P;
import nbtool.util.U;

/**
 * NetIO objects are connections to a single robot.  They are intended to constitute the run loop of their own thread.
 * 
 * When the connection is closed or the NetIO object believes the connection has been lost, 
 * they return run(), ending the thread.
 * 
 * When a full object has been retrieved from the network it is delivered to <boss>.
 * 
 * the 'Boss' object is expected to determine what type of Object it is being given...
 * 
 * NOTE:  <takeDelivery> is called in the NetIO run-loop.  Therefore the implementation of takeDelivery should NOT do
 * any extensive calculations; it should in some way mark that data has arrived, save the data, and wait for a separate
 * thread to pick up on the new data.
 * 
 * NOTE2: NetIO threads can end for one of two reasons:
 * 	!running, typically because another thread called stop()
 * 
 * 	catching an exception, indicating some error in finding the host or the stability of the connection
 * 
 * netThreadExiting() should always be called when run() will return, meaning all exceptions should be handled.
 * */

public class NetIO implements Runnable {
	private String server_address;
	private int server_port;
	
	public interface Boss {
		void takeDelivery(Log log);
		void netThreadExiting();
	}
	private Boss boss;
	
	private volatile boolean running;
	
	public NetIO(String addr, int p, Boss b) {
		server_address = addr;
		server_port = p;
		boss = b;
		
		running = false;
	}
	
	//Intended to be called from another thread, presumably by the boss.
	public void stop() {
		running = false;
	}
	
	public void run() {
		running = true;
		Socket socket = null;
		try {
			U.w("NetIO: thread created with sa=" + server_address + " sp=" + server_port);
			assert(server_address != null && server_port != 0 && boss != null);

			socket = CommonIO.setupNetSocket(server_address, server_port);
			
			BufferedOutputStream _out =  new BufferedOutputStream(socket.getOutputStream());
			BufferedInputStream _in = new BufferedInputStream(socket.getInputStream());
			
			DataOutputStream out = new DataOutputStream(_out);
			DataInputStream in = new DataInputStream(_in);
			
			U.w("NetIO: thread connected.");
			N.notifyEDT(EVENT.SIO_THREAD, this, true);

			//Init connection.
			out.writeInt(0);
			out.flush();
			
			int recv = in.readInt();
			if (recv != 0)
				throw new SequenceErrorException(0, recv);
			
			out.writeInt(NBConstants.VERSION);
			out.flush();
			recv = in.readInt();
			if (recv != NBConstants.VERSION) {
				U.w("WARNING: NetIO connected to robot with version " + recv + 
						" but client is running version " + NBConstants.VERSION + " !!\n");
			}
			
			int seq_num = 1;
			while(running) {
				recv = in.readInt();
				
				if (recv == 0) {
					//Pinged, no logs to receive.
					out.writeInt(0);
					out.flush();
				} else if (recv == seq_num) {
					Log nl = CommonIO.readLog(in);
					U.w("NetIO: thread got packet of data size: " + nl.bytes.length + " desc: " + nl.description);
					
					nl.source = SOURCE.NETWORK;
					
					boss.takeDelivery(nl);
					++seq_num;
				} else {
					throw new SequenceErrorException(seq_num, recv);
				}
			}
		}
		
		catch (UnknownHostException uke) {
			uke.printStackTrace();
			U.w("NetIO thread:" + uke.getMessage());
		}
		catch(SocketTimeoutException ste) {
			ste.printStackTrace();
			U.w("NetIO thread: socket TIMEOUT.");
		}
		catch (IOException ie) {
			ie.printStackTrace();
			U.w("NetIO thread:" + ie.getMessage());
		}
		catch(OutOfMemoryError e) {
			e.printStackTrace();
			U.w("NetIO thread got OutOfMemoryError.");
		}
		catch(NegativeArraySizeException nase) {
			nase.printStackTrace();
			U.w("NetIO got negative incoming data size!");
		}
		
		catch(SequenceErrorException see) {
			U.w("NetIO thread got out of sequence, exiting!" +
					"\n\texpected:" + see.expected + " was:" + see.was);
		}
		
		U.w("NetIO thread exiting...");
		if (running) {
			U.w("WARNING: NetIO thread exiting ATYPICALLY! (running == true)");
		}
		
		if (socket != null) {
			try {
				socket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		N.notifyEDT(EVENT.SIO_THREAD, this, false);
		boss.netThreadExiting();
	}
	
	private static class SequenceErrorException extends Exception {
		private static final long serialVersionUID = 1L;
		public int expected, was;
		SequenceErrorException(int e, int w) {
			super();
			expected = e; was = w;
		}
	}
}
