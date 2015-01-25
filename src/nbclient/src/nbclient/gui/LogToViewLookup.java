package nbclient.gui;

import java.util.ArrayList;

import nbclient.data.Log;
import nbclient.gui.logviews.misc.ViewParent;
import nbclient.util.P;
import nbclient.util.U;
import static nbclient.util.NBConstants.*;

public class LogToViewLookup {
	
	public static ArrayList<Class<? extends ViewParent>> viewsForLog(Log lg) {
		ArrayList<Class<? extends ViewParent>> ret = new ArrayList<Class<? extends ViewParent>>();
		
		//Try to list most specific views first
		String type = (String) lg.getAttributes().get("type");
		
		if (type.startsWith(TEST_S)) {
			//Don't want this going in with the protobufs.
			//No specific views atm.
		} else if (type.equalsIgnoreCase(STATS_S)) { 
			ret.addAll(P.LTVIEW_MAP.get(STATS_S));
		} else if (type.equalsIgnoreCase(IMAGE_S)) {
			ret.addAll(P.LTVIEW_MAP.get(IMAGE_S));
		} else {
			//Assume some type of protobuf
			ArrayList<Class<? extends ViewParent>> specific = P.LTVIEW_MAP.get(type);
			if (specific != null) 
				ret.addAll(specific);
			ret.addAll(P.LTVIEW_MAP.get(PROTOBUF_S));
		}
		
		ret.addAll(P.LTVIEW_MAP.get(DEFAULT_S));
		U.w("LogToViewLookup: Found " + ret.size() + " views for type: " + type);
		return ret;
	}
}
