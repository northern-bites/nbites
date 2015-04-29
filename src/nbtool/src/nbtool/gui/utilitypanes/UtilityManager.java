package nbtool.gui.utilitypanes;

import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;

public class UtilityManager {
	public static Class[] utilities = {HardlyWorking.class, YUVColors.class, LogToViewUtility.class};
	
	private static HashMap<Class<? extends UtilityParent>, UtilityParent> map = setupMap();
	private static HashMap<Class<? extends UtilityParent>, UtilityParent> setupMap() {
		HashMap<Class<? extends UtilityParent>, UtilityParent> ret = new HashMap<Class<? extends UtilityParent>, UtilityParent>();
		
		for (Class c : utilities) {
			 if (UtilityParent.class.isAssignableFrom(c)) {
				 ret.put(c, null);
			 }
		}
		
		return ret;
	}
	
	public static UtilityParent instanceOf(Class<? extends UtilityParent> cls) {
		if (!UtilityParent.class.isAssignableFrom(cls)) {
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
			return null;
		}
	}
}
