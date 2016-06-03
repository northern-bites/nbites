package nbtool.nio;

import nbtool.data.json.Json;
import nbtool.data.json.JsonObject;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.*;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

public class LogRPC {
	
	public static class RemoteCall {
		protected String key;
		protected String name;
		
		/* warning, call is NULL after it is sent! */
		protected Log call;
		protected IOFirstResponder caller;
		
		public String callName() {
			return name;
		}
		
		protected void finish(IOInstance connection, Log ret) {
			assert(ret.logClass.equals(SharedConstants.LogClass_RPC_Return()));
			assert(ret.topLevelDictionary.get(SharedConstants.RPC_KEY()).asString().value.equals(key));
			
			Log[] out = new Log[ret.blocks.size()];
			for (int i = 0; i < out.length; ++i) 
				out[i] = ret.blocks.get(i).parseAsLog();
			
//			Debug.error("ControlCall not actually calling generateReceived yet!");
			GIOFirstResponder.generateReceived(connection, caller, 0, out);
		}
		
		public RemoteCall(IOFirstResponder clr, String fname, Log ... args) {
			this.caller = clr;
			this.key = Utility.getRandomHexString(20);
			this.name = fname;
			
			JsonObject tlo = Json.object();
			tlo.put(SharedConstants.RPC_NAME(), fname);
			tlo.put(SharedConstants.RPC_KEY(), key);
			call = Log.explicitLog(null, tlo, SharedConstants.LogClass_RPC_Call(), 0);
			for (Log l : args) {
				call.addBlockFromLog(l);
			}
		}
	}
	
	public static void requestFlags(IOFirstResponder ifr, RobotConnection conn) {
		conn.addControlCall(ifr, "GetFlags");
	}
	
	public static void setFlag(IOFirstResponder ifr, RobotConnection conn, int flagi, boolean val) {
		byte[] data = new byte[2];
		data[0] = (byte) flagi;
		data[1] = (byte) (val ? 1 : 0);
		Block block = Block.explicit(data, null, null, null, 0, 0);
		Log only = Log.explicitLogFromArray(new Block[]{block},
				null,
				SharedConstants.LogClass_Null(), 0);
		
		conn.addControlCall(ifr, "SetFlag", only);
	}
	
}
