package nbtool.io;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.LinkedList;

import nbtool.data.SExpr;
import nbtool.data._log._Log;
import nbtool.io.CommonIO.GIOFirstResponder;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOState;
import nbtool.io.CommonIO.SequenceErrorException;
import nbtool.util.Events;
import nbtool.util.Debug;
import messages.*;

/*
 * Astute observers will notice this code is remarkably similar to the netIO code
 * */

public class ControlIO {
		
	public static _Log createCmndSetFlag(int flagi, boolean val) {
		byte[] bytes = new byte[2];
		bytes[0] = (byte) flagi;
		bytes[1] = (byte) (val ? 1 : 0);
		
		SExpr commandTree = SExpr.newList(SExpr.newAtom(_Log.COMMAND_FIRST_ATOM_S), SExpr.newAtom("setFlag"));
		_Log cmnd = new _Log(commandTree, bytes);
		return cmnd;
	}
	
	public static _Log createCmndReqFlags() {
		byte[] bytes = new byte[3];
		bytes[0] = (byte) Byte.MAX_VALUE;
		bytes[1] = (byte) Byte.MAX_VALUE;
		bytes[2] = (byte) Byte.MAX_VALUE;
		
		SExpr commandTree = SExpr.newList(SExpr.newAtom(_Log.COMMAND_FIRST_ATOM_S), SExpr.newAtom("setFlag"));
		_Log cmnd = new _Log(commandTree, bytes);
		return cmnd;
	}
 	
	public static _Log createCmndSetCameraParams(CameraParams cam) {
		byte[] paramsSerialized = cam.toByteArray();
		SExpr commandTree = SExpr.newList(SExpr.newAtom("command"),SExpr.newAtom("setCameraParams"));
		_Log cmnd = new _Log(commandTree,paramsSerialized);
		return cmnd;
	}
	
	public static _Log createCmndTest() {
		SExpr commandTree = SExpr.newList(SExpr.newAtom(_Log.COMMAND_FIRST_ATOM_S), SExpr.newAtom("test"));
		_Log cmnd = new _Log(commandTree, null);
		return cmnd;
	}
	
	public static _Log createCmndExit() {
		SExpr commandTree = SExpr.newList(SExpr.newAtom(_Log.COMMAND_FIRST_ATOM_S), SExpr.newAtom("exit"));
		_Log cmnd = new _Log(commandTree, null);
		return cmnd;
	}
	
	public static _Log createSimpleCommand(String name, byte[] data) {
		SExpr commandTree = SExpr.list(SExpr.newAtom(_Log.COMMAND_FIRST_ATOM_S), SExpr.newAtom(name));
		_Log cmnd = new _Log(commandTree, data);
		return cmnd;
	}
	
	private static final LinkedList<ControlInstance> instances = new LinkedList<>();

	public static ControlInstance create(IOFirstResponder ifr, String host, int port) {
		ControlInstance si = new ControlInstance(host, port);
		si.ifr = ifr;
		
		synchronized(instances) {
			instances.add(si);
		}
		
		Thread t = new Thread(si, String.format("nbtool-%s", si.name()));
		t.setDaemon(true);
		t.start();

		return si;
	}

	public static ControlInstance getByIndex(int index) {
		synchronized(instances) {
			if (index < instances.size())
				return instances.get(index);
			else return null;
		}
	}

	public static ControlInstance getByHost(String host) {
		synchronized(instances) {
			for (ControlInstance si : instances) {
				if (si.host.equals(host))
					return si;
			}

			return null;
		}
	}

	public static ControlInstance[] getAll() {
		synchronized(instances) {
			return instances.toArray(new ControlInstance[0]);
		}
	}

	private static void remove(ControlInstance toRem) {
		synchronized(instances) {
			if (instances.contains(toRem))
				instances.remove(toRem);
		}
	}


	public static class ControlInstance extends CommonIO.IOInstance {
		
		private class Cmnd {
			_Log log;
			IOFirstResponder listener;
			
			Cmnd(_Log l, IOFirstResponder ls) {
				this.log = l; this.listener = ls;
			}
		}
		
		private final LinkedList<Cmnd> commands = new LinkedList<>();
		
		public boolean tryAddCmnd(_Log cmnd) {
			if (cmnd == null ||
					cmnd.tree().count() < 2 ||
					!cmnd.tree().get(0).isAtom() ||
					!cmnd.tree().get(1).isAtom() ||
					!cmnd.tree().get(0).value().equals(_Log.COMMAND_FIRST_ATOM_S)) {
				Debug.error( "invalid format for command log: %s", cmnd.toString());
				return false;
			}
			
			synchronized(commands) {
				commands.add(new Cmnd(cmnd, null));
			}
			
			return true;
		}
		
		/* listener notified, in addition to IOfr of the ControlInstance, when cmnd returns */
		/* listener not notified if instance dies prior to sending cmnd. */
		public boolean tryAddCmnd(_Log cmnd, IOFirstResponder listener) {
			if (cmnd == null ||
					cmnd.tree().count() < 2 ||
					!cmnd.tree().get(0).isAtom() ||
					!cmnd.tree().get(1).isAtom() ||
					!cmnd.tree().get(0).value().equals(_Log.COMMAND_FIRST_ATOM_S)) {
				Debug.error( "invalid format for command log: %s", cmnd.toString());
				return false;
			}
			
			synchronized(commands) {
				commands.add(new Cmnd(cmnd, listener));
			}
			
			return true;
		}

		protected ControlInstance(String host, int port) {
			this.host = host;
			this.port = port;
		}

		@Override
		public void run() {

			assert(socket == null);

			try {
				//Setup Socket
				Debug.info( "%s starting.", name());
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

				synchronized(this) {
					if (this.state != IOState.STARTING)
						return;
					this.state = IOState.RUNNING;
				}
				
				Events.GControlStatus.generate(this, true);

				//control...
				while (state() == IOState.RUNNING) {
					//Log l = null;
					Cmnd c = null;
					synchronized (commands) {
						c = commands.poll();
					}
					
					if (c != null) {
						Debug.info( name() + ": sending command: " + c.log.description());
						out.writeInt(1);
						out.flush();
						
						recv = in.readInt();
						if (recv != 0)
							throw new SequenceErrorException(0, recv);
						
						CommonIO.writeLog(out, c.log);
						
						int ret = in.readInt();
						int nout = in.readInt();
						Debug.info( "%s: [%s] got ret [%d](and %d logs back)\n", name(), c.log.description(), ret, nout);

						_Log[] outa = new _Log[nout];
						for (int i = 0; i < nout; ++i) {
							outa[i] = CommonIO.readLog(in);
						}
						
						GIOFirstResponder.generateReceived(this, ifr, ret, outa);
						if (c.listener != null)
							GIOFirstResponder.generateReceived(this, c.listener, ret, outa);
						
					} else {
						out.writeInt(0);
						out.flush();
						recv = in.readInt();
						if (recv != 0)
							throw new SequenceErrorException(0, recv);
						
						Thread.sleep(400);
					}
				}

			} catch (Throwable t) {
				if (t instanceof SequenceErrorException) {
					Debug.error( "SequenceError: %s", ((SequenceErrorException) t).toString());
				} else {
					Debug.warn( "%s got exception: %s:%s", this.name(),
							t.getClass().getSimpleName(), t.getMessage());
				}

				t.printStackTrace();
			} finally {
				Debug.info( "%s cleaning up...", name());

				this.finish();

				ControlIO.remove(this);
				GIOFirstResponder.generateFinished(this, this.ifr);
				Events.GControlStatus.generate(this, false);
			}
		}

		@Override
		public String name() {
			return String.format("ControlInstance{%s:%d}", this.host, this.port);
		}
	}
}
