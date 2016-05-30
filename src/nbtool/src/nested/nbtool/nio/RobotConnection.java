package nbtool.nio;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.net.Socket;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import nbtool.data.json.JsonObject;
import nbtool.data.json.Json;
import nbtool.data.log.Log;
import nbtool.io.CommonIO;
import nbtool.io.CommonIO.GIOFirstResponder;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.nio.LogRPC.RemoteCall;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.Events;
import nbtool.util.SharedConstants;
import nbtool.util.ToolSettings;
import nbtool.util.Utility;

public class RobotConnection extends IOInstance {
	
	private static final DebugSettings debug = Debug.createSettings(true, true, true, Debug.EVENT, null);
	
	private static final LinkedList<RobotConnection> instances = new LinkedList<>();
	
	/* STATIC METHODS */
	/* -------------------------------------------------- */
	
	/* returns RobotConnection on success, null on failure */
	public static RobotConnection connectToRobot(String hostName, IOFirstResponder responder) {
		RobotConnection conn = new RobotConnection(hostName,
				SharedConstants.ROBOT_PORT());
		conn.ifr = responder;
		
		if (conn.tryStart()) {
			synchronized(instances) {
				instances.add(conn);
			}
			
			return conn;
		}
		
		return null;		
	}
	
	public static RobotConnection getByIndex(int index) {
		synchronized(instances) {
			if (index < instances.size())
				return instances.get(index);
			else return null;
		}
	}

	public static RobotConnection getByHost(String host) {
		synchronized(instances) {
			for (RobotConnection si : instances) {
				if (si.host.equals(host))
					return si;
			}

			return null;
		}
	}

	public static RobotConnection[] getAll() {
		synchronized(instances) {
			return instances.toArray(new RobotConnection[0]);
		}
	}

	private static void remove(RobotConnection toRem) {
		synchronized(instances) {
			if (instances.contains(toRem))
				instances.remove(toRem);
		}
	}

	/* INSTANCE STRUCTURE */
	/* -------------------------------------------------- */
	
	public void addControlCall(IOFirstResponder caller, String functionName, Log ... args) {
		RemoteCall call = new RemoteCall(caller, functionName, args);
		synchronized(calls) {
			calls.add(call);
			calls.notify();
		}
	}
	
	private class RobotConnectionThreadBody implements Runnable {
		private final RobotConnection conn;
		private final boolean which;
		protected RobotConnectionThreadBody(RobotConnection conn, boolean w) 
			{this.conn = conn; this.which = w;}	
		@Override
		public void run() {
			try {
				if (which) 	conn.input();
				else 		conn.output();
			} catch (Exception e) {
				debug.warn("RobotConnectionThreadBody %s of %s thread caught exception, killing...", 
						which ? "INPUT" : "OUTPUT", conn);
				e.printStackTrace();
			} finally {
				debug.warn("RobotConnectionThreadBody %s of %s ending...", 
						which ? "INPUT" : "OUTPUT", conn);
				conn.kill();
				
				if (which) {
					try {
						outputThread.join();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					
					socket = null;
					debug.info("RobotConnectionThreadBody INPUT of %s cleaning up...", conn);
					conn.cleanup();
					conn.finish();
					remove(conn);
					Events.GRobotConnectionStatus.generate(conn, false);
				}
			}
		}
	}
	
	protected RobotConnection(String host, int port) {
		this.host = host; this.port = port;
	}
	
	private Thread inputThread, outputThread;
	private final RobotConnectionThreadBody inputBody = 
			new RobotConnectionThreadBody(this, true);
	private final RobotConnectionThreadBody outputBody =
			new RobotConnectionThreadBody(this, false);
	
	private final LinkedList<RemoteCall> calls = new LinkedList<>();
	private final Map<String, RemoteCall> pending = new HashMap<>();
	private final Log heartbeat = Log.explicitLog(null, null,
			SharedConstants.LogClass_Null(), 0);
	
	protected boolean tryStart() {
		assert(socket == null);
		assert(state() == IOState.STARTING);
		
		try {
			this.socket = CommonIO.setupNetSocket(host, port);			
		} catch(Exception e) {
			debug.warn("connection attempt to %s:%d FAILED", host, port);
			e.printStackTrace();
			this.state = IOState.FINISHED;
			return false;
		}
		
		assert(socket != null);
		assert(socket.isConnected());
		this.state = IOState.RUNNING;
		
		inputThread = new Thread(inputBody);
		inputThread.setDaemon(true);
		inputThread.setName(this.toString() + "(INPUT THREAD)");
		
		outputThread = new Thread(outputBody);
		outputThread.setDaemon(true);
		outputThread.setName(this.toString() + "(OUTPUT THREAD)");

		inputThread.start();
		outputThread.start();
		
		Events.GRobotConnectionStatus.generate(this, true);
		
		return true;
	}
	
	@Override
	public void run() {
		debug.error("run() called on RobotConnection, that's not how this works!");
		assert(false);
	}
	
	private void input() throws Exception {
		debug.info("%s input thread...", this);
		assert(socket != null);
		BufferedInputStream bis = new BufferedInputStream(socket.getInputStream());
		
		while (state() == IOState.RUNNING) {
			Log input = Log.parseFromStream(bis);
			
			if (input.version() != ToolSettings.VERSION) {
				debug.warn("%s talking to robot of different version! tool:%d robot:%d", 
						ToolSettings.VERSION, input.version());
			}
			
			String lclass = input.logClass;
			
			if (lclass.equals(SharedConstants.LogClass_Null())) {
				debug.info("%s got heartbeat %d", this, input.createdWhen);
			} else if (lclass.equals(SharedConstants.LogClass_RPC_Return())) {
				String key = input.topLevelDictionary.get(SharedConstants.RPC_KEY()).asString().value;
				if (pending.containsKey(key)) {
					RemoteCall call = pending.remove(key);
					call.finish(this, input);
				} else {
					debug.warn("%s got RPCReturn of %s key %s WITH NO MATCHING CALLS PENDING", 
							input.topLevelDictionary.get(SharedConstants.RPC_NAME()).asString().value,
							key);
				}
			} else {
				if (input.host_addr == null || input.host_addr.isEmpty() ||
						input.host_addr.equals("n/a")) {
					input.host_addr = this.host();
				}
				
				GIOFirstResponder.generateReceived(this, ifr, 0, input);
//				debug.error("RobotConnection.input() not actually calling generateReceived yet!");
			}
		}
	}
	
	private void output() throws Exception {
		debug.info("%s output thread...", this);
		assert(socket != null);
		BufferedOutputStream bos = new BufferedOutputStream(socket.getOutputStream());
		int timeout = (SharedConstants.REMOTE_HOST_TIMEOUT() / 1000) / 2;
		
		while (state() == IOState.RUNNING) {
			RemoteCall call = null;
			
			synchronized(calls) {
				if (calls.isEmpty()) {
					calls.wait(timeout);
				}
				
				call = calls.pollFirst();
			}
			
			if (call == null) {
				++heartbeat.createdWhen;
				debug.info("%s sending heartbeat %d...", this, heartbeat.createdWhen);
				heartbeat.writeTo(bos);
				bos.flush();
			} else {
				debug.warn("%s sending %s...", this, call.callName());
				pending.put(call.key, call);
				call.call.writeTo(bos);
				call.call = null;
				bos.flush();
			}
		}
	}
	
	private void cleanup() {
		for (RemoteCall call : calls) {
			GIOFirstResponder.generateFinished(this, call.caller);
		}
		calls.clear();
		
		for (RemoteCall call : pending.values()) {
			GIOFirstResponder.generateFinished(this, call.caller);
		}
		pending.clear();
		
		GIOFirstResponder.generateFinished(this, ifr);
		ifr = null;
	}
	
	@Override
	public String name() {
		return String.format("RobotConnection(%s,%d)", host, port);
	}
	
	public static void main(String[] _ARGS_) throws InterruptedException {
		Debug.warn("RobotConnection stand-alone main...");
		
		RobotConnection conn = RobotConnection.connectToRobot("127.0.0.1", new IOFirstResponder(){

			@Override
			public void ioFinished(IOInstance instance) {
				Debug.print("got ioFinished: %s", instance);
			}

			@Override
			public void ioReceived(IOInstance inst, int ret, Log... out) {
				Debug.print("got ioReceived: %s", inst);
			}

			@Override
			public boolean ioMayRespondOnCenterThread(IOInstance inst) {
				return false;
			}
			
		});

		Debug.print("running...");
		for(;;) Thread.sleep(1000);
	}
	
}
