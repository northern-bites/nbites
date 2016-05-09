package nbtool.nio;

import nbtool.data.json.Json;
import nbtool.data.json.JsonObject;
import nbtool.data.log.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

public class LogRPC {
	
	public static class RemoteCall {
		protected String key;
		protected Log call;
		protected IOFirstResponder caller;
		
		public String callName() {
			return call.topLevelDictionary.get(SharedConstants.RPC_NAME()).asString().value;
		}
		
		protected void finish(IOInstance connection, Log ret) {
			assert(ret.logClass.equals(SharedConstants.LogClass_RPC_Return()));
			assert(ret.topLevelDictionary.get(SharedConstants.RPC_KEY()).asString().value.equals(key));
			
			Log[] out = new Log[ret.blocks.size()];
			for (int i = 0; i < out.length; ++i) 
				out[i] = ret.blocks.get(i).parseAsLog();
			
//			GIOFirstResponder.generateReceived(connection, caller, 0, out);
			Debug.error("ControlCall not actually calling generateReceived yet!");
		}
		
		public RemoteCall(IOFirstResponder clr, String fname, Log ... args) {
			this.caller = clr;
			this.key = Utility.getRandomHexString(20);
			JsonObject tlo = Json.object();
			tlo.put(SharedConstants.RPC_NAME(), fname);
			tlo.put(SharedConstants.RPC_KEY(), key);
			call = Log.explicitLog(null, tlo, SharedConstants.LogClass_RPC_Call(), 0);
			for (Log l : args) {
				call.addBlockFromLog(l);
			}
		}
	}
	
	public static void requestFlags(RobotConnection conn, int flagi) {
		
	}
	
	public static void setFlag(RobotConnection conn, int flagi, boolean val) {
		
	}
	
}
