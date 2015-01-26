package nbtool.gui;

import java.util.ArrayList;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.P;
import nbtool.util.U;
import static nbtool.util.NBConstants.*;

public class LogToViewLookup {
	
	public static ArrayList<Class<? extends ViewParent>> viewsForLog(Log lg) {
		ArrayList<Class<? extends ViewParent>> ret = new ArrayList<Class<? extends ViewParent>>();
		
		//Try to list most specific views first
		String type = lg.type();
		assert(type != null);
		
		ArrayList<Class<? extends ViewParent>> specific = P.LTVIEW_MAP.get(type);
		if (specific != null)
			ret.addAll(specific);
		
		if (type.startsWith(PROTOBUF_TYPE_PREFIX)) {
			/*
			String ptype = type.substring(PROTOBUF_TYPE_PREFIX.length());
			
			specific = P.LTVIEW_MAP.get(ptype);
			if (specific != null)
				ret.addAll(specific); */
			
			//Add views for generic protobufs
			ret.addAll(P.LTVIEW_MAP.get(PROTOBUF_S));
		}
		
		ret.addAll(P.LTVIEW_MAP.get(DEFAULT_S));
		U.w("LogToViewLookup: Found " + ret.size() + " views for type: " + type);
		return ret;
	}
}
