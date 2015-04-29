package nbtool.data;

import java.io.IOException;
import java.util.ArrayList;

import javax.swing.SwingUtilities;

import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;
import nbtool.util.NBConstants.MODE;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.P;
import nbtool.util.U;

public class SessionMaster implements NListener {
	public static final SessionMaster INST = new SessionMaster();
	
	public ArrayList<Session> sessions;
	public Session workingSession;
	
	private SessionHandler handler;
	
	public volatile long max_data_bytes;
	
	private SessionMaster() {
		sessions = new ArrayList<Session>();
		handler = null;
		workingSession = null;
		
		max_data_bytes = P.getHeap();
		
		N.listen(EVENT.MAX_MEM_USAGE_CHANGED, this);
		N.listen(EVENT.STATUS, this);
	}
	
	public void startSession(MODE m, String fp, String addr) {
		U.wf("SessionMaster.startSession(m=%s, fp=%s, addr=%s)\n", m.toString(), fp, addr);
		if (handler != null)
			return;
		
		handler = new SessionHandler(this);
		
		if (handler.start(m, fp, addr)) {
			Session news = new Session(sessions.size(), m, fp, addr);
			sessions.add(news);
			workingSession = news;
		} else {
			U.w("SessionMaster.startSession(failed)");
			handler = null;
		}
	}
	
	public void stopSession() {
		U.w("SessionMaster.stopSession()");
		if (handler != null)
			handler.stop();
	}

	public void notified(EVENT e, Object src, Object... args) {
		switch(e) {
		case MAX_MEM_USAGE_CHANGED:
			Long nv = (Long) args[0];
			this.max_data_bytes = nv;
			break;
		case STATUS:
		{
			STATUS s = (STATUS) args[0];
			switch (s) {
			case IDLE:
				handler = null;
				workingSession = null;
				break;
			case RUNNING:
				assert(handler != null);
				break;
			case STARTING:
				break;
			case STOPPING:
				break;
			default:
				break;
			
			}
		}
			break;
		default:
			break;
		
		}
	}
	
	public boolean isIdle() {
		return (handler == null);
	}
	
	protected void deliver(final Log ... logs) {
		assert(workingSession != null);
		assert(SwingUtilities.isEventDispatchThread());

		Session destination = workingSession;



		Log lbs = null;

		for (Log l : logs) {
			destination.addLog(l);

			if (l.type().equals("stats"))
				lbs = l;
		}

		if (lbs != null && lbs.bytes != null) {
			try {
				BotStats bs = new BotStats(lbs);

				destination.most_relevant = bs;

				N.notifyEDT(EVENT.REL_BOTSTAT, this, bs);
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}

		N.notifyEDT(EVENT.LOG_FOUND, this, (Object[]) logs);
	}
}
