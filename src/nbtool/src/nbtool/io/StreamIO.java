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
import java.util.ArrayList;
import java.util.LinkedList;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.data.SExpr;
import nbtool.io.CommonIO.GIOFirstResponder;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOState;
import nbtool.io.CommonIO.SequenceErrorException;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.Prefs;
import nbtool.util.Utility;

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
 * netThreadExiting() should always (read: if you change this class, make sure this happens)
 * 	 be called when run() will return, meaning all exceptions need to be handled inside the run loop.
 * */

public class StreamIO {
	
	private static final LinkedList<StreamInstance> instances = new LinkedList<>();

	public static StreamInstance create(IOFirstResponder ifr, String host, int port) {
		StreamInstance si = new StreamInstance(host, port);
		si.ifr = ifr;
		
		synchronized(instances) {
			instances.add(si);
		}
		
		Thread t = new Thread(si, String.format("nbtool-%s", si.name()));
		t.setDaemon(true);
		t.start();

		return si;
	}

	public static StreamInstance getByIndex(int index) {
		synchronized(instances) {
			if (index < instances.size())
				return instances.get(index);
			else return null;
		}
	}

	public static StreamInstance getByHost(String host) {
		synchronized(instances) {
			for (StreamInstance si : instances) {
				if (si.host.equals(host))
					return si;
			}

			return null;
		}
	}

	public static StreamInstance[] getAll() {
		synchronized(instances) {
			return instances.toArray(new StreamInstance[0]);
		}
	}

	private static void remove(StreamInstance toRem) {
		synchronized(instances) {
			if (instances.contains(toRem))
				instances.remove(toRem);
		}
	}


	public static class StreamInstance extends CommonIO.IOInstance {

		protected StreamInstance(String host, int port) {
			this.host = host;
			this.port = port;
		}

		@Override
		public void run() {

			assert(socket == null);

			try {
				//Setup Socket
				Logger.logf(Logger.INFO, "%s starting.", name());
				this.socket = CommonIO.setupNetSocket(host, port);

				//Initialize
				BufferedOutputStream _out =  new BufferedOutputStream(socket.getOutputStream());
				BufferedInputStream _in = new BufferedInputStream(socket.getInputStream());

				DataOutputStream out = new DataOutputStream(_out);
				DataInputStream in = new DataInputStream(_in);

				out.writeInt(0);
				out.flush();

				int recv = in.readInt();
				if (recv != 0)
					throw new SequenceErrorException(0, recv);

				out.writeInt(NBConstants.VERSION);
				out.flush();
				recv = in.readInt();
				if (recv != NBConstants.VERSION) {
					Logger.log(Logger.WARN,"WARNING: NetIO connected to robot with version " + recv + 
							" but client is running version " + NBConstants.VERSION + " !!\n");
				}

				synchronized(this) {
					if (this.state != IOState.STARTING)
						return;
					this.state = IOState.RUNNING;
				}
				
				Events.GStreamIOStatus.generate(this, true);

				//Stream
				int seq_num = 1;
				while (state() == IOState.RUNNING) {
					recv = in.readInt();

					if (recv == 0) {
						//Pinged, no logs to receive.
						out.writeInt(0);
						out.flush();
					} else if (recv == seq_num) {
						Log nl = CommonIO.readLog(in);

						nl.tree().append(SExpr.newKeyValue("from_address", this.host));
						Logger.log(Logger.INFO, this.name() + ": thread got packet of data size: " + nl.bytes.length + " desc: " + nl.description(50));

						nl.source = SOURCE.NETWORK;

						GIOFirstResponder.generateReceived(this, ifr, 0, nl);
						++seq_num;
					} else {
						throw new SequenceErrorException(seq_num, recv);
					}

				}

			} catch (Throwable t) {
				if (t instanceof SequenceErrorException) {
					Logger.logf(Logger.ERROR, "%s", ((SequenceErrorException) t).toString());
				}

				t.printStackTrace();
			} finally {
				Logger.logf(Logger.INFO, "%s cleaning up...", name());

				this.finish();

				StreamIO.remove(this);
				GIOFirstResponder.generateFinished(this, this.ifr);
				Events.GStreamIOStatus.generate(this, false);
			}
		}

		@Override
		public String name() {
			return String.format("StreamInstance{%s:%d}", this.host, this.port);
		}

	}
}
