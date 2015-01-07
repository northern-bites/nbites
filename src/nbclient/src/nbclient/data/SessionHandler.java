package nbclient.data;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;

import javax.swing.JList;
import javax.swing.ListModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ListDataListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import nbclient.io.FileIO;
import nbclient.io.NetIO;
import nbclient.util.N;
import nbclient.util.N.EVENT;
import nbclient.util.U;

public class SessionHandler implements NetIO.Boss, FileIO.Boss{
		
	public String log_directory; //Where we write to if from server, where we read from if from fs
	
	public enum MODE {
		NETWORK_SAVING(0), NETWORK_NOSAVE(1), FILESYSTEM(2), NONE(3);
		public final int index;
		
		private MODE(int i) {
			this.index = i;
		}
	}
	public MODE mode;
	
	public enum STATUS {
		IDLE(0),
		STARTING(1),
		RUNNING(2),
		STOPPING(3);
		
		public final int index;
		private STATUS(int i) {
			this.index = i;
		}
	}
	public STATUS status;
		
	/* our logs.  If data's coming in from the network, all logs have their name and data already.  However,
	 * if coming from filesystem, the logs are created with only the description (their file name) set.  Bytes are unset.
	 * If we request such a file, we must then go to the filesystem to get the bytes.
	 * */
	public OldStats stats;
			
	private FileIO fileioRunnable;
	private NetIO netioRunnable;
	
	public SessionHandler() {
		mode = MODE.NONE;
		status = STATUS.IDLE;
		log_directory = "";
		
		stats = new OldStats();
	}

	/*
	 * NetIO
	 * */
	public void takeDelivery(Log log) {
		if (mode == MODE.NETWORK_SAVING) {
			FileIO.addObject(log);
		}
		
		assert(log.bytes != null);
		
		stats.update(log);
		
		N.notifyEDT(N.EVENT.LOGS_ADDED, this, log);
	}
	
	public void netThreadExiting() {
		this.netioRunnable = null;		
		this.updateStopping();
	}
	
	/*
	 * FileIO
	 * */
	public void fileioThreadExiting() {
		this.fileioRunnable = null;		
		this.updateStopping();
	}
	
	
	/*Try to start new data collection mode.*/
	
	public void start(MODE m, String p, String s) {
		assert(SwingUtilities.isEventDispatchThread());
		String primary = p.trim();
		//Expand tilde
		primary = U.localizePath(primary);
		
		String secondary = s.trim();
		
		if (status != STATUS.IDLE) {
			U.w("DataHandler.start(): cannot start, not in status==waiting.");
			return;
		}
		
		if (m == MODE.FILESYSTEM || m == MODE.NETWORK_SAVING) {
			if (primary.isEmpty()) {
				U.w("DataHandler.start(): Empty primary (log folder name) setting.  Could not start.");
				return;
			}
			
			log_directory = primary + "/";
			if (!FileIO.checkLogFolder(log_directory)) {
				U.w("DataHandler.start(): Invalid log folder, could not start.");
				return;
			} else {
				U.w("DataHandler.start(): Set log folder.");
			}
		}
		
		int portnum = -1;
		String address = null;
		
		if (m == MODE.NETWORK_NOSAVE || m == MODE.NETWORK_SAVING) {
			
			if (secondary.isEmpty()) {
				U.w("DataHandler.start(): Empty secondary (address:port) setting.  Could not start.");
				return;
			}
			
			String addr_port = secondary;
			String[] split = addr_port.split(":");
			if (split.length != 2) {
				U.w("DataHandler.start(): Invalid server address or port.  Syntax is server_address:port.");
				return;
			}
			try {
				portnum = Integer.parseInt(split[1]);
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}
			address = split[0];
		}
		
		mode = m;
		status = STATUS.RUNNING;
		U.w("DataHandler.start(): settings good, starting.");

		N.notify(EVENT.STATUS, this, status, secondary, primary);
		
		netioRunnable = null;
		fileioRunnable = null;
		
		switch(m) {
		case NETWORK_SAVING:
			//start fileio thread, move to network_nosave
			fileioRunnable = new FileIO(this, log_directory);
			
			fileioRunnable.running = true;
			Thread fileioThread = new Thread(fileioRunnable);
			fileioThread.start();
			
		case NETWORK_NOSAVE:
			//start net thread
			netioRunnable = new NetIO();
			netioRunnable.running = true;
			
			netioRunnable.server_address = address;
			netioRunnable.server_port = portnum;
			netioRunnable.boss = this;
			
			Thread netioThread = new Thread(netioRunnable);			
			netioThread.start();
			
			break;
		case FILESYSTEM:
			Log[] logArray = FileIO.fetchLogs(log_directory);
			
			stats.update(logArray);
			N.notify(EVENT.LOGS_ADDED, this, (Object[]) logArray);
			break;
		default:{
			U.w("DataHandler.start(): Cannot start that mode:" + m.toString());
			return;
			}
		}
	}
	
	//Called from display (so EDThread)
	public void stop() {
		assert(SwingUtilities.isEventDispatchThread());
		status = STATUS.STOPPING;
		
		if (mode == MODE.FILESYSTEM) {
			status = STATUS.IDLE;
		}
		
		N.notify(EVENT.STATUS, this, status);
		if (fileioRunnable != null)
			fileioRunnable.running = false;
		if (netioRunnable != null)
			netioRunnable.running = false;
	}
	
	//ONLY called from other threads.
	//Synch'd because we don't want two threads checking stop status at once
	public synchronized void updateStopping() {
		if (status != STATUS.STOPPING) {
			status = STATUS.STOPPING;
			if (fileioRunnable != null)
				fileioRunnable.running = false;
			if (netioRunnable != null)
				netioRunnable.running = false;
			U.w("DataHandler.updateStopping(): moved to status.STOPPING.");
		}
		
		if (fileioRunnable == null && netioRunnable == null) {
			status = STATUS.IDLE;
		}
		
		N.notifyEDT(EVENT.STATUS, this, status);
	}
}
