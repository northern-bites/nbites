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
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.SwingUtilities;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.data.SExpr;
import nbtool.io.CommonIO.GIOFirstResponder;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.Utility;
import static nbtool.util.NBConstants.*;

public class CrossIO {
	
	public static void main(String[] args) throws InterruptedException, IOException {
		//testing...
		
		String path = "~/Documents/testdir/cross";
		startNBCrossAt(path, "hello", true, false, true).waitFor();
	}
	
	private static ArrayList<Process> children = null;
		
	public static synchronized Process startNBCrossAt(String pathToExecutable, String instName,
			boolean pipeNBCrossOutput, boolean silenceNBCross,
			boolean killOnJVMexit) throws IOException {
		String execPath = Utility.localizePath(pathToExecutable);
		ProcessBuilder cross = new ProcessBuilder();
		
		if (pipeNBCrossOutput) {
			cross.inheritIO();
		}
		
		if (silenceNBCross) {
			cross.command(execPath, instName, "silent");
		} else {
			cross.command(execPath, instName);
		}
		
		Process process = cross.start();
		
		if (killOnJVMexit) {
			if (children != null) {
				children.add(process);
			} else {
				children = new ArrayList<>();
				
				Runtime.getRuntime().addShutdownHook(new Thread(new Runnable(){

					@Override
					public void run() {
						Logger.logf(Logger.WARN, "CrossIO attempting to destroy child processes.");
						try {
							
							for (Process p : children) {
								p.destroy();
							}
							
						} catch (Exception e) {
							e.printStackTrace();
						}
						
					}
					
				}));
			}
		}
		
		return process;
	}
		
	public static class CrossFunc {
		public String name;
		public String[] args;
		
		public CrossFunc(String n, String[] a) {
			this.name = n;
			this.args = a;
		}
		
		public boolean accepts(CrossCall call) {
			if (call.function != this)
				return false;
			
			if (call.arguments.length != args.length)
				return false;
			
			for (int i = 0; i < args.length; ++i) {
				String cs = call.arguments[i].primaryType();
				String as = args[i];
				
				if (cs.equals(Log.NBCROSS_WILDCARD_TYPE) || 
						as.equals(Log.NBCROSS_WILDCARD_TYPE))
					continue;
				
				if (!cs.equals(as))
					return false;
			}
			
			return true;
		}
		
		@Override
		public String toString() {
			return String.format("%s (%d args)", name, args.length);
		}
	}
	
	public static class CrossCall {
		public IOFirstResponder listener;
		public CrossFunc function;
		
		public Log[] arguments;
		
		public CrossCall(IOFirstResponder listener,
				CrossFunc function,
				Log ... args) {
			this.listener = listener;
			this.function = function;
			this.arguments = args;
		}
	}
	
	public static class CrossInstance extends CommonIO.IOInstance {
		
		public final ArrayList<CrossFunc> functions = new ArrayList<>();
		private final LinkedList<CrossCall> calls = new LinkedList<>();
		
		public boolean tryAddCall(CrossCall call) {
			if (call == TIMER_CALL) {
				//If we have calls don't need TIMER_CALL
				if (!calls.isEmpty()) return true;
			}
			else if (call.listener == null ||
					!functions.contains(call.function) ||
					!call.function.accepts(call)) {
				return false;
			}
			
			synchronized(calls) {
				calls.add(call);
				calls.notify();
			}
			
			return true;
		}
		
		public CrossFunc functionWithName(String fn) {
			for (CrossFunc f : functions) {
				if (f.name.equals(fn))
					return f;
			}
			
			return null;
		}
		
		public String name = null;
		
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
			Logger.logf(Logger.INFO, "CrossInstance %d starting up.", this.unique_id);
			assert(socket != null && ifr != null);
			
			try {
				//Setup connection and gather functions.
				BufferedOutputStream _os = new BufferedOutputStream(socket.getOutputStream());
				BufferedInputStream _is = new BufferedInputStream(socket.getInputStream());

				DataOutputStream dos = new DataOutputStream(_os);
				DataInputStream dis = new DataInputStream(_is);
								
				dos.writeInt(0);
				dos.flush();
				int init = dis.readInt();
				if (init != 0)
					throw new CommonIO.SequenceErrorException(0, init);
				
				Log funcLog = CommonIO.readLog(dis);
				this.parseFunctions(funcLog);
				dos.writeInt(functions.size());
				dos.flush();
				
				synchronized(this) {
					if (this.state != IOState.STARTING)
						return;
					this.state = IOState.RUNNING;
				}
				
				Events.GCrossStatus.generate(this, true);
				while (this.state() == IOState.RUNNING) {
					
					CrossCall call = null;
					synchronized(calls) {
						if (calls.isEmpty()) {
							calls.wait();
						}
						
						call = calls.removeFirst();
					}
					
					assert(call != null);
					if (call == TIMER_CALL) {
						dos.writeInt(0);
						dos.flush();
						int ping = dis.readInt();
						if (ping != 0)
							throw new CommonIO.SequenceErrorException(0, ping);
					} else {
						Logger.logf(Logger.EVENT, "%s calling function %s", name(), call.function.name);
						dos.writeInt(1);
						dos.flush();
						
						dos.writeInt(functions.indexOf(call.function));
						
						for (Log l : call.arguments) {
							CommonIO.writeLog(dos, l);
						}
						dos.flush();
												
						final int ret = dis.readInt();
						int num_out = dis.readInt();
						

						final Log[] outs = new Log[num_out];
						for (int i = 0; i < num_out; ++i) {
							Log nl = CommonIO.readLog(dis);
							nl.source = SOURCE.DERIVED;
							outs[i] = nl;
						}
						
						dos.writeInt(num_out);
						
						Logger.logf(Logger.EVENT, "%s finished function %s, ret=%d nout=%d", name(), call.function.name,
								ret, num_out);
						
						GIOFirstResponder.generateReceived(this, call.listener, ret, outs);
					}
					
				}
				
			} catch (Throwable t) {
				if (t instanceof CommonIO.SequenceErrorException) {
					Logger.logf(Logger.ERROR, "%s got bad sequence exception: %s", name(), t.toString());
				}
				t.printStackTrace();
			} finally {
				Logger.logf(Logger.INFO, "CrossInstance %d dieing.", this.unique_id);
				finish();
				
				GIOFirstResponder.generateFinished(this, this.ifr);
				Events.GCrossStatus.generate(this, false);
			}
		}
		
		private void parseFunctions(Log funclog) {
			SExpr tree = funclog.tree();
			tree = tree.find("contents");
			
			//System.out.printf("%s\n", tree.print());
			if (!tree.find("name").exists() ||
					tree.find("name").count() < 2 ||
					tree.find("name").get(1).value().trim().isEmpty()) {
				this.name = null;
			} else {
				this.name = tree.find("name").get(1).value();
			}
			
			int nf = tree.find("nfuncs").get(1).valueAsInt();
			SExpr funcs = tree.find("functions").get(1);
			for (int i = 0; i < funcs.count(); ++i) {
				SExpr thisFunc = funcs.get(i);
				CrossFunc cf = new CrossFunc(thisFunc.get(0).value(), new String[thisFunc.count() - 1]);
				for (int j = 1; j < thisFunc.count(); ++j) {
					cf.args[j - 1] = thisFunc.get(j).value();
				}
				
				this.functions.add(cf);
			}
			
			assert(functions.size() == nf);
		}

		@Override
		public String name() {
			if (name != null) {
				return String.format("CrossInstance{%d:%s}", unique_id, name);
			} else {
				return String.format("CrossInstance{%d}", unique_id);
			}
		}
		
		@Override
		public String toString() {
			return name();
		}
		
	}
	
	private static final LinkedList<CrossInstance> instances = new LinkedList<>();
	private static final CrossCall TIMER_CALL = new CrossCall(null, null, new Log[0]);
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
	
	private static class CrossServer implements Runnable, IOFirstResponder {
		@Override
		public void run() {
			ServerSocket server = null;
			Timer timer = new Timer(true);
			TimerTask tt = new TimerTask() {
				@Override
				public void run() {
					synchronized(instances) {
						for (CrossInstance ci : instances) {
							ci.tryAddCall(TIMER_CALL);
						}
					}
				}
			};
			
			try {
				server = new ServerSocket(NBCROSS_PORT, 1, InetAddress.getByName("127.0.0.1"));
			} catch (UnknownHostException e1) {
				e1.printStackTrace();
				return;
			} catch (IOException e1) {
				e1.printStackTrace();
				return;
			}
			
			timer.schedule(tt, 5000, 1000);
			Logger.log(Logger.INFO, "CrossServer up.");

			try {
				
				while (true) {
					Socket socket = null;
					try {
						socket = server.accept();
						socket.setSoTimeout(NBCROSS_CALL_TIMEOUT);
						
						CrossInstance ci = new CrossInstance();
						ci.socket = socket;
						ci.ifr = this;
						
						synchronized(instances) {
							instances.add(ci);
						}
						
						Thread t = new Thread(ci, String.format("nbtool-ci%d", ci.unique_id));
						t.setDaemon(true);
						t.start();
						
					} catch (Exception e) {
						Logger.log(Logger.ERROR, "Exception in CrossServer while accepting connection.");
						e.printStackTrace();
					}
				}
				
			} catch (Throwable t) {
				Logger.logf(Logger.ERROR, "CrossServer dieing because: %s", t.getMessage());
				t.printStackTrace();
			} finally {
				if (server != null) {
					try {
						server.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
				
				timer.cancel();
			}
			
		}

		@Override
		public void ioFinished(IOInstance instance) {
			synchronized(instances) {
				instances.remove(instance);
			}
		}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			Logger.logf(Logger.ERROR, "%s tried to notify CrossServer of ioReceived!", inst.name());
			throw new Error("CrossServer got ioReceived call!");
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return true;
		}
		
	}
	
	public static void startCrossServer() {
		if (serverThread != null) {
			Logger.logf(Logger.ERROR, "CrossServer already running!");
			return;
		}
		
		Logger.log(Logger.INFO, "starting nbtool-CrossServer...");
		
		serverThread = new Thread(new CrossServer(), "nbtool-CrossServer");
		serverThread.setDaemon(true);
		serverThread.start();
	}
	
	public static boolean crossServerLive() {
		if (serverThread == null)
			return false;
		return serverThread.isAlive();
	}
}
