package nbclient.data;

import java.util.ArrayList;

public class SessionMaster {
	public static final SessionMaster INSTANCE = new SessionMaster();
	
	public ArrayList<Session> sessions;
	public Session workingSession;
	
	private SessionHandler handler;
	
	private SessionMaster() {
		sessions = new ArrayList<Session>();
	}
	
	public void startSession() {
		
	}
}
