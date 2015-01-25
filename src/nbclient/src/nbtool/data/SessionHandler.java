package nbtool.data;

import javax.swing.SwingUtilities;

import nbtool.io.CommandIO;
import nbtool.io.FileIO;
import nbtool.io.NetIO;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.NBConstants;
import nbtool.util.NBConstants.MODE;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.U;

public class SessionHandler implements NetIO.Boss, FileIO.Boss, CommandIO.Boss{
			
	private FileIO fileioRunnable;
	private NetIO netioRunnable;
	private CommandIO cncRunnable;
	
	public SessionHandler() {
		fileioRunnable = null;
		netioRunnable = null;
		cncRunnable = null;
	}

	/*
	 * NetIO
	 * */
	public void takeDelivery(Log log) {
		if (fileioRunnable != null) {
			FileIO.addObject(log);
		}
		
		assert(log.bytes != null);
				
		N.notifyEDT(EVENT.LOG_FOUND, this, log);
	}
	
	public synchronized void netThreadExiting() {
		this.netioRunnable = null;		
		this.updateStopping();
	}
	
	/*
	 * FileIO
	 * */
	public synchronized void fileioThreadExiting() {
		this.fileioRunnable = null;		
		this.updateStopping();
	}
	
	/*
	 * CommandIO
	 * */
	public synchronized void cncThreadExiting() {
		this.cncRunnable = null;
		this.updateStopping();
	}
	
	
	/*Try to start new data collection mode.*/
	
	public boolean start(MODE m, String p, String s) {
		assert(SwingUtilities.isEventDispatchThread());
		String primary = p.trim();
		//Expand tilde
		primary = U.localizePath(primary) + "/";
		
		String address = s.trim();
		
		if (m == MODE.FILESYSTEM || m == MODE.NETWORK_SAVING) {
			if (primary.isEmpty()) {
				U.w("SessionHandler.start(): Empty primary (log folder name) setting.  Could not start.");
				return false;
			}
			
			if (!FileIO.checkLogFolder(primary)) {
				U.w("SessionHandler.start(): Invalid log folder, could not start.");
				return false;
			} else {
				U.w("SessionHandler.start(): Set log folder.");
			}
		}
				
		if (m == MODE.NETWORK_NOSAVE || m == MODE.NETWORK_SAVING) {
			
			if (address.isEmpty()) {
				U.w("SessionHandler.start(): Empty secondary (address:port) setting.  Could not start.");
				return false;
			}
		}
		
		N.notify(EVENT.STATUS, this, STATUS.RUNNING, m);
		trying_to_stop = false;
		U.w("SessionHandler.start(): settings good, starting.");
		
		switch(m) {
		case NETWORK_SAVING:
			//start fileio thread, move to network_nosave
			fileioRunnable = new FileIO(this, primary);
			
			Thread fileioThread = new Thread(fileioRunnable);
			fileioThread.start();
			
		case NETWORK_NOSAVE:
			//start net thread
			netioRunnable = new NetIO(address, NBConstants.SERVER_PORT, this);
			Thread netioThread = new Thread(netioRunnable);			
			netioThread.start();
			
			cncRunnable = new CommandIO(address, NBConstants.CNC_PORT, this);
			Thread cncThread = new Thread(cncRunnable);
			cncThread.start();
			
			break;
		case FILESYSTEM:
			Log[] logArray = FileIO.fetchLogs(primary);
			N.notify(EVENT.LOG_FOUND, this, (Object[]) logArray);
			
			N.notify(EVENT.STATUS, this, STATUS.IDLE, m);
			break;
		default:{
			U.w("SessionHandler.start(): Cannot start that mode:" + m.toString());
			return false;
			}
		}
		
		return true;
	}
	
	private boolean trying_to_stop;
	
	//Called from display (so EDThread)
	public void stop() {
		assert(SwingUtilities.isEventDispatchThread());

		N.notify(EVENT.STATUS, this, STATUS.STOPPING, MODE.NONE);
		trying_to_stop = true;
		
		if (fileioRunnable != null)
			fileioRunnable.stop();
		if (netioRunnable != null)
			netioRunnable.stop();
		if (cncRunnable != null)
			cncRunnable.stop();
	}
	
	//ONLY called from other threads.
	//Synch'd because we don't want two threads checking stop status at once
	public synchronized void updateStopping() {
		if (!trying_to_stop) {
			//One of the IO threads encountered an error.  End the session.
			N.notifyEDT(EVENT.STATUS, this, STATUS.STOPPING, MODE.NONE);
			trying_to_stop = true;
			
			if (fileioRunnable != null)
				fileioRunnable.stop();
			if (netioRunnable != null)
				netioRunnable.stop();
			if (cncRunnable != null)
				cncRunnable.stop();
		}
		
		if (fileioRunnable == null && 
				netioRunnable == null &&
				cncRunnable == null) {
			N.notifyEDT(EVENT.STATUS, this, STATUS.IDLE, MODE.NONE);
		}
	}

}
