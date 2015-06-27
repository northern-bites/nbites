package nbtool.data;

import java.util.ArrayList;

import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.ControlIO;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.io.FileIO;
import nbtool.io.FileIO.FileInstance;
import nbtool.io.StreamIO;
import nbtool.io.StreamIO.StreamInstance;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.NBConstants.STATUS;

public final class SessionMaster implements IOFirstResponder {
	
	public static SessionMaster get() {
		return INST;
	}
	
	public static final SessionMaster INST = new SessionMaster();
	public final ArrayList<Session> sessions = new ArrayList<>();
	
	private Session workingSession = null;
	
	public Session getLatestSession() {
		Session link = workingSession;
		if (link != null)
			return link;
		if (sessions.isEmpty())
			return null;
		else return sessions.get(sessions.size() - 1);
	}
	
	public static volatile int saveMod = 1;
	public static volatile int keepMod = 1;
	
	public static volatile boolean dropSTATS = false;
		
	private ControlInstance control = null;
	private FileInstance fileio = null;
	private StreamInstance streamio = null;
	
	/* path is expected to be absolute. */
	public synchronized void loadSession(String path) {
		if (!isIdle()) {
			Logger.log(Logger.WARN, "SessionMaster cannot load new session, not idle.");
			return;
		}
		
		String fullPath = path.trim();
		
		if (!FileIO.checkLogFolder(fullPath)) {
			Logger.log(Logger.WARN, "SessionMaster cannot load new session, bad path.");
			return;
		}
		
		Log[] logArray = FileIO.fetchLogs(fullPath);
		Session newsess = new Session(fullPath, null);
		//Use loading addLog
		newsess.addLog(logArray);
		
		sessions.add(newsess);
		Events.GSessionAdded.generate(this, workingSession);
		Events.GToolStatus.generate(this, STATUS.RUNNING, newsess.name);
		Events.GLogsFound.generate(this, logArray);
		Events.GToolStatus.generate(this, STATUS.IDLE, "idle");
	}
	
	public synchronized void streamSession(String addr, String path) {
		if (!isIdle()) {
			Logger.log(Logger.WARN, "SessionMaster cannot stream new session, not idle.");
			return;
		}
		
		if (addr == null || addr.isEmpty()) {
			Logger.log(Logger.WARN, "SessionMaster cannot stream new session, bad address.");
			return;
		}
		
		if (path != null && !path.isEmpty()) {
			Logger.log(Logger.INFO, "SessionMaster setting up file writer.");
			fileio = FileIO.makeFileWriter(path, this);
		}
		
		workingSession = new Session(null, addr);
		
		sessions.add(workingSession);
		Events.GSessionAdded.generate(this, workingSession);
		Events.GToolStatus.generate(this, STATUS.RUNNING, workingSession.name);
		
		Logger.log(Logger.WARN, "SessionMaster setting up stream.");
		control = ControlIO.create(this, addr, NBConstants.CONTROL_PORT);
		streamio = StreamIO.create(this, addr, NBConstants.STREAM_PORT);
	}
	
	public synchronized Session requestSession(String title) {
		Session requested = new Session(title);
		sessions.add(0, requested);
		Events.GSessionAdded.generate(this, requested);
		return requested;
	}
	
	public synchronized void stopWorkingSession() {
		if (isIdle()) {
			Events.GToolStatus.generate(this, STATUS.IDLE, "idle");
			return;
		} else {
			Events.GToolStatus.generate(this, STATUS.STOPPING, workingSession.name);
			
			if (control != null)
				control.kill();
			if (fileio != null)
				fileio.kill();
			if (streamio != null)
				streamio.kill();
		}
	}
	
	public synchronized boolean isIdle() {
		if (workingSession == null) {
			assert(control == null && fileio == null && streamio == null);
			
			return true;
		} else {
			return false;
		}
	}
	
	private synchronized void updateStopping() {
		if (control == null && fileio == null && streamio == null) {
			workingSession = null;
			Events.GToolStatus.generate(this, STATUS.IDLE, "idle");
		}
	}
	
	public synchronized void lateStartFileWriting(String path) {
		if (!isIdle() && streamio != null) {
			Logger.infof("SessionMaster latestarting a FileInstance...");
			
			assert(FileIO.checkLogFolder(path));
			fileio = FileIO.makeFileWriter(path, this);
		}
	}
	
	public synchronized void earlyStopFileWriting() {
		if (fileio != null) {
			Logger.warnf("SessionMaster stopping a FileInstance early...");
			fileio.kill();
		}
	}

	/* IOFirstResponder methods */
	
	@Override
	public void ioFinished(IOInstance instance) {
		synchronized(this) {
			if (instance == fileio) {
				fileio = null;
			} else if (instance == control) {
				control = null;
			} else if (instance == streamio) {
				streamio = null;
			}
			
			updateStopping();
		}
	}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		if (workingSession == null)
			return;
		
		if (inst == streamio) {
			synchronized(this) {
				
				for (Log log : out) {
					
					if (saveMod != 0 && fileio != null && 
							(log.unique_id % saveMod == 0)) 
					{
						if (!log.primaryType().equals("STATS"))
							fileio.add(log);
						else if (!dropSTATS) {
							fileio.add(log);
						} else {
							//Drop it.
						}
					}
					
					//Add via streaming addLog
					workingSession.addLog(log,
							keepMod != 0 && (log.unique_id % keepMod == 0));
				}
				
				Events.GLogsFound.generate(this, out);
			}
		} else {
			if (!(inst == control && out.length == 0))
				Logger.logf(Logger.WARN, "SessionMaster got %d surprising logs from %s.", out.length, inst.name());
		}
		
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return true;
	}
}
