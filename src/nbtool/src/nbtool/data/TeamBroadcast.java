package nbtool.data;

import java.nio.ByteBuffer;

import com.google.protobuf.InvalidProtocolBufferException;

import nbtool.util.Logger;
import nbtool.util.Utility;
import messages.TeamPacket;
import messages.WorldModel;
import data.SPLStandardMessage;

public class TeamBroadcast {
	
	public static final String TEAM_BROADCAST_LOGTYPE = "TeamBroadcast";
	
	public String robotName;
	public String robotIp;
		
	//java version of c structure that was originally broadcast.
	public SPLStandardMessage message;
	
	//protobuf parsed from 'message's data field.
	public TeamPacket dataTeamPacket;
	//protobuf contained within 'dataTeamPacket,' this is just a direct link to it.
	public WorldModel dataWorldModel;

	public TeamBroadcast(String rn, String ri, SPLStandardMessage msg, TeamPacket tp, WorldModel wm) {
		this.robotName = rn;
		this.robotIp = ri;
		this.message = msg;
		this.dataTeamPacket = tp;
		this.dataWorldModel = wm;
	}
	
	public Log toLog() {
		Log ret = Log.logWithTypePlus(TEAM_BROADCAST_LOGTYPE, message.toByteArray(),
				SExpr.pair("robotName", robotName),
				SExpr.pair("robotIp", robotIp),
				SExpr.pair(Log.CONTENT_NBYTES_S, SPLStandardMessage.SIZE));
		
		return ret;
	}
	
	public static TeamBroadcast fromLog(Log log) throws InvalidProtocolBufferException {
		if (!log.primaryType().equals(TEAM_BROADCAST_LOGTYPE)) {
			Logger.errorf("log of type [%s] cannot be parsed into TeamBroadcast object!", 
					log.primaryType());
			return null;
		}
		
		if (log.data().length < SPLStandardMessage.SIZE) {
			Logger.errorf("log of size [%d < SPLStandardMessage.SIZE] cannot be parsed into TeamBroadcast object!", 
					log.data().length);
			return null;
		}
		
		String rName = log.sexprForContentItem(0).firstValueOf("robotName").value();
		String rIP = log.sexprForContentItem(0).firstValueOf("robotIP").value();
		
		byte[] messageBytes = Utility.subArray(log.bytes, 0, SPLStandardMessage.SIZE);
		
		SPLStandardMessage message = new SPLStandardMessage();
		message.fromByteArray(ByteBuffer.wrap(messageBytes));
		
		TeamPacket tp = TeamPacket.parseFrom(message.data);
		
		return new TeamBroadcast(rName, rIP, message, tp, tp.getPayload());
	}
}
