package nbtool.io;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.LinkedList;
import java.util.Queue;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.util.N;
import nbtool.util.NBConstants;
import nbtool.util.U;
import nbtool.util.N.EVENT;

/*
 * Astute observers will notice this code is remarkably similar to the netIO code
 * */

public class CommandIO implements Runnable{
	
	//Set by any creator if they want their CommandIO instance to be publicly used.
	public static CommandIO INSTANCE;
	
	private String cnc_address;
	private int cnc_port;
	
	public interface Boss {
		void cncThreadExiting();
	}
	private Boss boss;
	
	private volatile boolean running;
	
	public CommandIO(String addr, int p, Boss b) {
		cnc_address = addr;
		cnc_port = p;
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
		commands = new LinkedList<Log>();
		
		try {
			U.w("CommandIO: thread created with sa=" + cnc_address + " sp=" + cnc_port);
			assert(cnc_address != null && cnc_port != 0 && boss != null);
			
			socket = CommonIO.setupNetSocket(cnc_address, cnc_port);
			
			BufferedOutputStream _out =  new BufferedOutputStream(socket.getOutputStream());
			BufferedInputStream _in = new BufferedInputStream(socket.getInputStream());
			
			DataOutputStream out = new DataOutputStream(_out);
			DataInputStream in = new DataInputStream(_in);
			
			U.w("CommandIO: thread connected.");
			N.notifyEDT(EVENT.CNC_CONNECTION, this, true);

			//Init connection.
			out.writeInt(0);
			out.flush();
			
			int recv = in.readInt();
			if (recv != 0)
				throw new SequenceErrorException(0, recv);
			
			while(running) {
				Log l = null;
				synchronized (commands) {
					l = commands.poll();
				}
				
				if (l != null) {
					U.w("CommandIO: sending command: " + l.description);
					out.writeInt(1);
					out.flush();
					
					recv = in.readInt();
					if (recv != 0)
						throw new SequenceErrorException(0, recv);
					
					CommonIO.writeLog(out, l);
					
					int ret = in.readInt();
					U.wf("CommandIO: [%s] got ret [%d]\n", l.description, ret);
				} else {
					out.writeInt(0);
					out.flush();
					recv = in.readInt();
					if (recv != 0)
						throw new SequenceErrorException(0, recv);
					
					Thread.sleep(400);
				}
			}
		}
		catch (UnknownHostException uke) {
			uke.printStackTrace();
			U.w("CommandIO thread:" + uke.getMessage());
		}
		catch(SocketTimeoutException ste) {
			ste.printStackTrace();
			U.w("CommandIO thread: socket TIMEOUT.");
		}
		catch (IOException ie) {
			ie.printStackTrace();
			U.w("CommandIO thread:" + ie.getMessage());
		}
		catch(OutOfMemoryError e) {
			e.printStackTrace();
			U.w("CommandIO thread got OutOfMemoryError.");
		}
		catch(NegativeArraySizeException nase) {
			nase.printStackTrace();
			U.w("CommandIO got negative incoming data size!");
		}
		catch(SequenceErrorException see) {
			U.w("CommandIO thread got out of sequence, exiting!" +
					"\n\texpected:" + see.expected + " was:" + see.was);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		commands = null;
		
		U.w("CommandIO thread exiting...");
		if (running) {
			U.w("WARNING: CommandIO thread exiting ATYPICALLY! (running == true)");
		}
		
		if (socket != null) {
			try {
				socket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		N.notifyEDT(EVENT.CNC_CONNECTION, this, false);
		boss.cncThreadExiting();
	}
	
	public static Queue<Log> commands = null;
	
	public static boolean tryAddCall(Log cmnd) {
		synchronized(commands) {
			if (commands == null || cmnd == null)
				return false;
			if (!cmnd.description.startsWith("cmnd="))
				return false;
			
			commands.add(cmnd);
		}
		
		return true;
	}
	
	private static class SequenceErrorException extends Exception {
		private static final long serialVersionUID = 1L;
		public int expected, was;
		SequenceErrorException(int e, int w) {
			super();
			expected = e; was = w;
		}
	}
	
	//A few simple commands
	//--------------------------------------------- 
	
	public static boolean tryAddSetFlag(int findex, boolean fval) {
		byte[] bytes = new byte[2];
		bytes[0] = (byte) findex;
		bytes[1] = (byte) (fval ? 1 : 0);
		
		Log cmnd = new Log("cmnd=setFlag", bytes);
		
		return tryAddCall(cmnd);
	}
	
	public static boolean tryAddTest() {
		Log cmnd = new Log("cmnd=test", null);
		
		return tryAddCall(cmnd);
	}
}
