package nbtool.gui.utilitypanes;

import javax.swing.JFrame;

import nbtool.data.Log;
import nbtool.data.TeamBroadcast;
import nbtool.io.BroadcastIO;
import nbtool.io.BroadcastIO.TeamBroadcastInstance;
import nbtool.io.BroadcastIO.TeamBroadcastListener;
import nbtool.io.CommonIO.IOInstance;
import nbtool.util.Logger;

public class WorldViewUtility extends UtilityParent {
	
	private class WorldView extends JFrame implements TeamBroadcastListener {
		
		private TeamBroadcastInstance broadcastListener;
		
		protected WorldView() {
			Logger.println("WorldView creating broadcast listener...");
			this.broadcastListener = BroadcastIO.createTBI(this);
		}

		@Override
		public void ioFinished(IOInstance instance) {
			// listener died for some reason.
		}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			Logger.println("recvd...");
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return false;
		}

		@Override
		public void acceptTeamBroadcast(TeamBroadcast tb) {
			//use tb
			Logger.printf("got from {%s:%s}", tb.robotName, tb.robotIp);
		}
		
	}

	private WorldView display = null;
	@Override
	public JFrame supplyDisplay() {
		return (display != null) ? display : (display = new WorldView());
	}

	@Override
	public String purpose() {
		return "Listen to and display broadcast messages.";
	}

	@Override
	public char preferredMemnonic() {
		return 'w';
	}
}
