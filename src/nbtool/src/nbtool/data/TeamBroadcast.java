package nbtool.data;

import messages.TeamPacket;
import messages.WorldModel;
import data.SPLStandardMessage;

public class TeamBroadcast {
	
	public String robotName;
	public String robotIp;
		
	public SPLStandardMessage message;
	public TeamPacket dataTeamPacket;
	public WorldModel dataWorldModel;

	public TeamBroadcast(String rn, String ri, SPLStandardMessage msg, TeamPacket tp, WorldModel wm) {
		this.robotName = rn;
		this.robotIp = ri;
		this.message = msg;
		this.dataTeamPacket = tp;
		this.dataWorldModel = wm;
	}
	
	public Log toLog() {
		Log ret = Log.logWithTypePlus("TeamBroadcast", null,
				SExpr.pair("robotName", robotName),
				SExpr.pair("robotIp", robotIp),
				SExpr.pair(Log.CONTENT_NBYTES_S, dataTeamPacket.getSerializedSize()));
		
		byte[] mb = message.toByteArray();
		byte[] tp = dataTeamPacket.toByteArray();
		byte[] all = new byte[mb.length + tp.length];
		System.arraycopy(tp, 0, all, 0, tp.length);
		System.arraycopy(mb, 0, all, tp.length, mb.length);
		
		ret.bytes = all;
		return ret;
	}
}
