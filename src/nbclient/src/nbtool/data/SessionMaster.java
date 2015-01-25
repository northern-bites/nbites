package nbtool.data;

import java.io.IOException;
import java.util.ArrayList;

import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;
import nbtool.util.NBConstants.MODE;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.P;

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
		
		N.listen(EVENT.LOG_FOUND, this);
		N.listen(EVENT.MAX_MEM_USAGE_CHANGED, this);
		N.listen(EVENT.STATUS, this);
	}
	
	public void startSession(MODE m, String p, String s) {
		if (handler != null)
			return;
		
		handler = new SessionHandler();
		
		if (handler.start(m, p, s)) {
			Session news = new Session(sessions.size(), m, p, s);
			sessions.add(news);
			workingSession = news;
		}
	}
	
	public void stopSession() {
		if (handler != null)
			handler.stop();
	}

	public void notified(EVENT e, Object src, Object... args) {
		switch(e) {
		case LOG_FOUND:
			assert(workingSession != null);
			
			Log lbs = null;
			
			for (Object o : args) {
				Log l = (Log) o;
				workingSession.addLog(l);
				
				if (l.type().equals("stats"))
					lbs = l;
			}
			
			if (lbs != null) {
				try {
					BotStats bs = new BotStats(lbs);
					
					workingSession.most_relevant = bs;
					
					N.notify(EVENT.REL_BOTSTAT, this, bs);
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			}
			
			break;
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
}
