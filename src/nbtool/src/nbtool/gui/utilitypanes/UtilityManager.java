package nbtool.gui.utilitypanes;

import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;

import static nbtool.util.Logger.*;

public class UtilityManager {
	public static Class[] utilities = {LogToViewUtility.class, ThreadStateUtility.class, HardlyWorking.class, YUVColors.class};
	
	private static HashMap<Class<? extends UtilityParent>, UtilityParent> map = setupMap();
	private static HashMap<Class<? extends UtilityParent>, UtilityParent> setupMap() {
		HashMap<Class<? extends UtilityParent>, UtilityParent> ret = new HashMap<Class<? extends UtilityParent>, UtilityParent>();
		
		for (Class c : utilities) {
			 if (UtilityParent.class.isAssignableFrom(c)) {
				 ret.put(c, null);
			 } else {
				 logf(ERROR, "UtilityManager asked to manage non-UtilityParent class %s", c.getName());
			 }
		}
		
		return ret;
	}
	
	public static UtilityParent instanceOf(Class<? extends UtilityParent> cls) {
		if (!UtilityParent.class.isAssignableFrom(cls)) {
			logf(ERROR, "UtilityManager asked for instance of non-UtilityParent class %s", cls.getName());
			return null;
		 }
		
		if (map.containsKey(cls)) {
			try {
				if (map.get(cls) != null)
					return map.get(cls);
				UtilityParent instance = cls.getConstructor().newInstance();
				map.put(cls, instance);
				return instance;
			} catch (InstantiationException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				e.printStackTrace();
			} catch (SecurityException e) {
				e.printStackTrace();
			} 
			
			return null;
		} else {
			logf(ERROR, "UtilityManager asked for instance of class NOT IN MAP: %s", cls.getName());
			return null;
		}
	}
	
	public static LogToViewUtility instanceOfLTV() {
		UtilityParent up = instanceOf(LogToViewUtility.class);
		assert(up instanceof LogToViewUtility);
		
		return (LogToViewUtility) up;
	}
}
