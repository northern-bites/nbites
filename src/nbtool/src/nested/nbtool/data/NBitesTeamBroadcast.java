package nbtool.data;

import java.nio.ByteBuffer;
import java.util.Vector;

import com.google.protobuf.InvalidProtocolBufferException;

import nbtool.data.json.JsonObject;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
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
	
	public Log toLog() {
		
		Block block = new Block(message.toByteArray(), null,
				TEAM_BROADCAST_LOGTYPE, "NBitesTeamBroadcast.toLog()", 0,0);
		block.dict.put("robotName", robotName);
		block.dict.put("robotIP", robotIp);
		
		return Log.explicitLogFromArray(new Block[]{block}, null, TEAM_BROADCAST_LOGTYPE, 0);
	}
	
	public static NBitesTeamBroadcast fromLog(Log log) throws InvalidProtocolBufferException {
		if (!log.logClass.equals(TEAM_BROADCAST_LOGTYPE)) {
			Debug.error("log of type [%s] cannot be parsed into TeamBroadcast object!", 
					log.logClass);
			return null;
		}
		
		Block primary = log.blocks.get(0);
		String rName = primary.dict.get("robotName").asString().value();
		String rIP = primary.dict.get("robotIP").asString().value();
		
		byte[] messageBytes = primary.data;
		SPLStandardMessage message = new SPLStandardMessage();
		message.fromByteArray(ByteBuffer.wrap(messageBytes));
		
		TeamPacket tp = TeamPacket.parseFrom(message.data);
		
		return new NBitesTeamBroadcast(rName, rIP, message, tp, tp.getPayload());
	}
}
