package nbtool.data;

import java.nio.ByteBuffer;

import com.google.protobuf.InvalidProtocolBufferException;

import nbtool.data._log._Log;
import nbtool.util.Debug;
import nbtool.util.Utility;
import messages.TeamPacket;
import messages.WorldModel;
import data.SPLStandardMessage;

public class NBitesTeamBroadcast {
	
	public static final String TEAM_BROADCAST_LOGTYPE = "NBitesTeamBroadcast";
	
	public String robotName;
	public String robotIp;
		
	//java version of c structure that was originally broadcast.
	public SPLStandardMessage message;
	
	//protobuf parsed from 'message's data field.
	public TeamPacket dataTeamPacket;
	//protobuf contained within 'dataTeamPacket,' this is just a direct link to it.
	public WorldModel dataWorldModel;

	public NBitesTeamBroadcast(String rn, String ri, SPLStandardMessage msg, TeamPacket tp, WorldModel wm) {
		this.robotName = rn;
		this.robotIp = ri;
		this.message = msg;
		this.dataTeamPacket = tp;
		this.dataWorldModel = wm;
	}
	
	public _Log toLog() {
		_Log ret = _Log.logWithTypePlus(TEAM_BROADCAST_LOGTYPE, message.toByteArray(),
				SExpr.pair("robotName", robotName),
				SExpr.pair("robotIp", robotIp),
				SExpr.pair(_Log.CONTENT_NBYTES_S, SPLStandardMessage.SIZE));
		
		return ret;
	}
	
	public static NBitesTeamBroadcast fromLog(_Log log) throws InvalidProtocolBufferException {
		if (!log.primaryType().equals(TEAM_BROADCAST_LOGTYPE)) {
			Debug.errorf("log of type [%s] cannot be parsed into TeamBroadcast object!", 
					log.primaryType());
			return null;
		}
		
		if (log.data().length < SPLStandardMessage.SIZE) {
			Debug.errorf("log of size [%d < SPLStandardMessage.SIZE] cannot be parsed into TeamBroadcast object!", 
					log.data().length);
			return null;
		}
		
		String rName = log.sexprForContentItem(0).firstValueOf("robotName").value();
		String rIP = log.sexprForContentItem(0).firstValueOf("robotIP").value();
		
		byte[] messageBytes = Utility.subArray(log.bytes, 0, SPLStandardMessage.SIZE);
		
		SPLStandardMessage message = new SPLStandardMessage();
		message.fromByteArray(ByteBuffer.wrap(messageBytes));
		
		TeamPacket tp = TeamPacket.parseFrom(message.data);
		
		return new NBitesTeamBroadcast(rName, rIP, message, tp, tp.getPayload());
	}
}
