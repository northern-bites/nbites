package nbtool.io;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

import javax.swing.SwingUtilities;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.NBConstants;
import nbtool.util.U;
import static nbtool.util.NBConstants.*;

public class CppIO implements Runnable {
	
	public static void ref(){} //Force the JRE to init CppIO by statically referencing the class.

	private static CppIO init() {
		CppIO io = new CppIO();
		thread = new Thread(io, "nbtool-cppio");
		thread.start();

		return io;
	}

	public static final CppIO current = init();
	public static Thread thread;
	
	private ArrayList<CppFunc> foundFuncs;
	private Queue<CppFuncCall> calls;

	public void run() {
		ServerSocket server = null;
		foundFuncs = null;
		calls = null;
		
		N.notifyEDT(EVENT.CPP_CONNECTION, this, false);
		
		try {
			server = new ServerSocket(CPP_PORT, 1, InetAddress.getByName("127.0.0.1"));
		} catch (UnknownHostException e1) {
			e1.printStackTrace();
			return;
		} catch (IOException e1) {
			e1.printStackTrace();
			return;
		}

		for(;;) {
			U.w("CppIO: server looking for client...");
			Socket socket = null;
			try {
				socket = server.accept();
				socket.setSoTimeout(SOCKET_TIMEOUT);
				N.notifyEDT(EVENT.CPP_CONNECTION, this, true);
				U.w("CppIO: found c++ sister process.");
				
				BufferedOutputStream _os = new BufferedOutputStream(socket.getOutputStream());
				BufferedInputStream _is = new BufferedInputStream(socket.getInputStream());

				DataOutputStream dos = new DataOutputStream(_os);
				DataInputStream dis = new DataInputStream(_is);
				
				foundFuncs = new ArrayList<CppFunc>();
				calls = new LinkedList<CppFuncCall>();
				
				dos.writeInt(0);
				dos.flush();
				int init = dis.readInt();
				if (init != 0)
					throw new CppIOException("bad initiation ping:" + init);
				
				findFunctions(dis, dos);
				
				N.notifyEDT(EVENT.CPP_FUNCS_FOUND, this, foundFuncs);
				U.w("CppIO: got function list:");
				for (CppFunc f: foundFuncs) {
					U.wf("\t%s(", f.name);
					for (String a : f.args) {
						U.wf(" [%s] ", a);
					}
					
					U.w(")");
				}
				
				dos.writeInt(foundFuncs.size());
				dos.flush();
				
				callLoop(dis, dos);

			} catch(CppIOException cie) {
				U.w("Malformed communication with m: " + cie.message);
				cie.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			
			N.notifyEDT(EVENT.CPP_CONNECTION, this, false);
			foundFuncs = null;
			calls = null;
			
			/*connection to c++ process died.*/
		}
	}
	
	private void findFunctions(DataInputStream dis, DataOutputStream dos) throws IOException {
		U.w("CppIO: getting function list.");
		int nfuncs_1 = dis.readInt();
		Log funcLog = CommonIO.readLog(dis);
		int nfuncs_2 = Integer.parseInt(funcLog.getAttributes().get("fn"));
		
		String funcstr = new String(funcLog.bytes);
		String[] funcs = funcstr.split("\n");
		int nfuncs_3 = funcs.length;
		
		if (nfuncs_1 != nfuncs_2 || nfuncs_2 != nfuncs_3) 
			throw new CppIOException(String.format("nfuncs: %d %d %d",
					nfuncs_1, nfuncs_2, nfuncs_3));
		
		for (String f : funcs) {
			String[] parts = f.split("=");
			assert(parts.length == 2);
			String[] args = parts[1].trim().split(" ");
			
			foundFuncs.add(new CppFunc(parts[0].trim(), args));
		}
	}
	
	private void callLoop(DataInputStream dis, DataOutputStream dos) throws IOException, InterruptedException {
		//Wait for calls to those functions.
		for(;;) {
			//Need both to ping so that we know the status of the connection.
			dos.writeInt(0);
			dos.flush();
			int ping = dis.readInt();
			if (ping != 0)
				throw new CppIOException("bad wait ping:" + ping);
			
			//Check calls.
			CppFuncCall _c = null;
			synchronized(calls) {
				_c = calls.poll();
			}
			
			if (_c != null) {
				U.w("CppIO: beginning call to: " + _c.name);
				final CppFuncCall c = _c;
				dos.writeInt(1);
				dos.flush();
				
				dos.writeInt(c.index);
				
				for (Log l : c.args) {
					CommonIO.writeLog(dos, l);
				}
				dos.flush();
				
				U.w("Sent call.");
				
				final int ret = dis.readInt();
				int num_out = dis.readInt();
				U.w("CppIO: function finished call, num out: " + num_out);
				final ArrayList<Log> outs = new ArrayList<Log>();
				for (int i = 0; i < num_out; ++i) {
					Log nl = CommonIO.readLog(dis);
					nl.source = SOURCE.DERIVED;
					U.w("CppIO: got out: " + nl.description);
					outs.add(nl);
				}
				
				dos.writeInt(outs.size());
				
				//Done calling function.
				SwingUtilities.invokeLater(new Runnable(){
					public void run() {
						c.listener.returned(ret, outs.toArray(new Log[0]));
					}
				});
				
			} 
			else Thread.sleep(10); //No calls waiting
		}
	}

	
	public class CppFunc {
		public String name;
		public String[] args;
		
		public CppFunc(String n, String[] a) {
			name = n;
			args = a;
		}
	}
	
	public static class CppFuncCall {
		public int index;
		public String name;
		public ArrayList<Log> args;
		public CppFuncListener listener;
	}
	
	public boolean tryAddCall(CppFuncCall c) {
		if (calls == null || foundFuncs == null)
			return false;
		if (c.index >= foundFuncs.size())
			return false;
		if (!(foundFuncs.get(c.index).name.equalsIgnoreCase(c.name)))
			return false;
		if (!(c.args.size() == foundFuncs.get(c.index).args.length))
			return false;
		if (c.listener == null)
			return false;
		
		synchronized(calls) {
			calls.add(c);
		}
		
		U.w("CppIO: added call to function: " + c.name);
		return true;
	}
	
	public int indexOfFunc(String name) {
		if (foundFuncs == null) return -1;
		for (int i = 0; i < foundFuncs.size(); ++i) {
			if (foundFuncs.get(i).name.startsWith(name))
				return i;
		}
		
		return -1;
	}
	
	private class CppIOException extends IOException {
		private static final long serialVersionUID = 1L;
		String message;
		protected CppIOException(String m) {message = m;}
	}
	
	/*
	 * Not using the N notifiation center because different parts of the client might want to call c++ funcs, 
	 * and this makes it much easier to differentiate between the calls.
	 * */
	public static interface CppFuncListener {
		public void returned(int ret, Log ... out);
	}
}
