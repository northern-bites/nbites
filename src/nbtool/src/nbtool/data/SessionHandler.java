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
	
	private MODE workingMode;
	
	public SessionHandler(SessionMaster m) {
		ms = m;
		workingMode = null;
		
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
				
		//N.notifyEDT(EVENT.LOG_FOUND, this, log);
		deliver(log);
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
	
	public boolean start(MODE m, String fp, String addr) {
		assert(SwingUtilities.isEventDispatchThread());
		String filepath = fp.trim();
		//Expand tilde
		filepath = U.localizePath(filepath) + "/";
		
		String address = addr.trim();
		
		if (m == MODE.FILESYSTEM || m == MODE.NETWORK_SAVING) {
			if (filepath.isEmpty()) {
				U.w("SessionHandler.start(): Empty primary (log folder name) setting.  Could not start.");
				return false;
			}
			
			if (!FileIO.checkLogFolder(filepath)) {
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
		
		N.notifyEDT(EVENT.STATUS, this, STATUS.RUNNING, m);
		trying_to_stop = false;
		U.w("SessionHandler.start(): settings good, starting.");
		workingMode = m;
		
		switch(m) {
		case NETWORK_SAVING:
			//start fileio thread, move to network_nosave
			fileioRunnable = new FileIO(this, filepath);
			
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
			Log[] logArray = FileIO.fetchLogs(filepath);
			//N.notify(EVENT.LOG_FOUND, this, (Object[]) logArray);
			deliver(logArray);
			
			N.notifyEDT(EVENT.STATUS, this, STATUS.IDLE, m);
			break;
		default:{
			U.w("SessionHandler.start(): Cannot start that mode:" + m.toString());
			N.notifyEDT(EVENT.STATUS, this, STATUS.IDLE, m);
			return false;
			}
		}
		
		return true;
	}
	
	private boolean trying_to_stop;
	
	//Called from display (so EDThread)
	public void stop() {
		assert(SwingUtilities.isEventDispatchThread());

		N.notifyEDT(EVENT.STATUS, this, STATUS.STOPPING, workingMode);
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
			N.notifyEDT(EVENT.STATUS, this, STATUS.STOPPING, workingMode);
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
			N.notifyEDT(EVENT.STATUS, this, STATUS.IDLE, workingMode);
		}
	}

	private SessionMaster ms;
	private void deliver(final Log... logs) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				if (ms != null)
					ms.deliver(logs);
			}
		});
	}
}
