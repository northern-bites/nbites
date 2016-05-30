package nbtool.nio;

import static nbtool.util.ToolSettings.NBCROSS_CALL_TIMEOUT;
import static nbtool.util.ToolSettings.NBCROSS_PORT;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.Timer;
import java.util.TimerTask;

import nbtool.data.SExpr;
import nbtool.data.log.Log;
import nbtool.io.CommonIO;
import nbtool.io.CommonIO.GIOFirstResponder;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.nio.LogRPC.RemoteCall;
import nbtool.util.Debug;
import nbtool.util.Events;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

public class CrossServer {
	
	private static final Debug.DebugSettings debug =
			Debug.createSettings(true, true, true, Debug.WARN, null);

	public static void _NBL_REQUIRED_START_() {
		startCrossServer();
	}

	public static void startCrossServer() {
		if (crossServerLive()) {
			Debug.error( "CrossServer already running!");
			return;
		}
		
		Debug.info( "starting nbtool-CrossServer...");
		
		serverThread = new Thread(new Server(), "nbtool-CrossServer");
		serverThread.setDaemon(true);
		serverThread.start();
	}
	
	public static boolean crossServerLive() {
		if (serverThread == null)
			return false;
		return serverThread.isAlive();
	}
	
	public static class CrossInstance extends CommonIO.IOInstance {
		
		protected CrossInstance(Socket sock) {
			this.socket = sock;
			this.ifr = ifr;
		}
		
		public String name = null;
		private final LinkedList<RemoteCall> calls = new LinkedList<>();
		
		public boolean tryAddCall(RemoteCall call) {
			synchronized(calls) {
				calls.add(call);
				calls.notify();
			}
			
			return true;
		}
		
		public boolean tryAddCall(IOFirstResponder ifr, String name, Log...args) {
			return this.tryAddCall(new RemoteCall(ifr, name, args));
		}
		
		private static final Object indexLock = new Object();
		private static long class_index = 0;
		private static long getID() {
			long ret;
			synchronized(indexLock) {
				ret = class_index++;
			}
			
			return ret;
		}
		public final long unique_id = getID();

		@Override
		public void run() {
			Debug.info( "CrossInstance %d starting up.", this.unique_id);
			assert(socket != null);
			Log heartbeat = Log.explicitLog(null, null, SharedConstants.LogClass_Null(), 0);
			int timeout = SharedConstants.REMOTE_HOST_TIMEOUT() / 2000;
			
			try {
				this.host = socket.getInetAddress().getHostAddress();
				//Setup connection and gather functions.
				BufferedOutputStream os = new BufferedOutputStream(socket.getOutputStream());
				BufferedInputStream is = new BufferedInputStream(socket.getInputStream());
				
				move(IOState.STARTING, IOState.RUNNING);
				
				Events.GCrossStatus.generate(this, true);
				while (this.state() == IOState.RUNNING) {
					
					RemoteCall call = null;
					synchronized(calls) {
						if(calls.isEmpty()) {
							calls.wait(timeout);
						}
						
						call = calls.pollFirst();
					}
					
					if (call == null) {
						debug.info("%s heartbeat...", this);
						++(heartbeat.createdWhen);
						heartbeat.writeTo(os);
						os.flush();
						Log hbBack = Log.parseFromStream(is);
						assert(hbBack.logClass.equals(SharedConstants.LogClass_Null()));
					} else {
						debug.info("%s call...", this);
						call.call.writeTo(os);
						os.flush();
						
						Log ret = Log.parseFromStream(is);
						while(ret.logClass.equals(SharedConstants.LogClass_Null()))
							ret = Log.parseFromStream(is);
						
						assert(ret.logClass.equals(SharedConstants.LogClass_RPC_Return()));
						call.finish(this, ret);
					}
					
				}
				
			} catch (IOException e) {
				Debug.warn("%s got IOException: %s (terminating connection)", this, e.getMessage());
			} catch (Throwable t) {
				t.printStackTrace();
			} finally {
				Debug.warn( "%s dieing.", this);
				finish();
				remove(this);
				Events.GCrossStatus.generate(this, false);
			}
		}

		@Override
		public String name() {
			if (name != null) {
				return String.format("CrossInstance{%d:%s}", unique_id, name);
			} else {
				return String.format("CrossInstance{%d}", unique_id);
			}
		}
	}
	
	private static final LinkedList<CrossInstance> instances = new LinkedList<>();
	private static Thread serverThread = null;
	
	public static CrossInstance instanceByIndex(int i) {
		synchronized(instances) {
			if (i < instances.size())
				return instances.get(i);
		}
		
		return null;
	}
	
	public static CrossInstance instanceByName(String name) {
		synchronized(instances) {
			for (CrossInstance ci : instances) {
				if (ci.name != null && ci.name.equals(name)) {
					return ci;
				}
			}
		}
		return null;
	}
	
	public static CrossInstance[] allInstances() {
		synchronized(instances) {
			return instances.toArray(new CrossInstance[0]);
		}
	}
	
	private static void remove(CrossInstance i) {
		synchronized(instances) {
			assert(instances.remove(i));
		}
	}
	
	private static class Server implements Runnable {
		@Override
		public void run() {
			ServerSocket server = null;
			
			try {
				server = new ServerSocket(SharedConstants.CROSS_PORT(), 1, null);
				
			} catch (UnknownHostException e1) {
				e1.printStackTrace();
				return;
			} catch (IOException e1) {
				e1.printStackTrace();
				return;
			}
			
			Debug.info( "CrossServer.Server bound and up.");
			try {
				while (true) {
					Socket socket = null;
					
					try {
						socket = server.accept();
						socket.setSoTimeout(SharedConstants.REMOTE_HOST_TIMEOUT() / 1000);
						
						CrossInstance ci = new CrossInstance(socket);

						synchronized(instances) {
							instances.add(ci);
						}
						
						Thread t = new Thread(ci, String.format("nbtool-CrossInstance-%d", ci.unique_id));
						t.setDaemon(true);
						t.start();
						
					} catch (Exception e) {
						Debug.error( "Exception in CrossServer while accepting connection.");
						e.printStackTrace();
					}
				}
				
			} catch (Throwable t) {
				Debug.error( "CrossServer dieing because: %s", t.getMessage());
				t.printStackTrace();
			} finally {
				if (server != null) {
					try {
						server.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}				
			}
		}
	}
	
	public static void main(String[] args) throws InterruptedException, IOException {
		startCrossServer();
		Scanner in = new Scanner(System.in);
		while(true) {
			Thread.sleep(1000);
			CrossInstance ci = CrossServer.instanceByIndex(0);
			if (ci != null) {
				in.nextLine();
				ci.tryAddCall(null, "Test");
			}
		}
	}
	
}	//CrossServer
