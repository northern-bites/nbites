package nbtool.gui.utilitypanes;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Vector;

import static nbtool.util.Logger.*;

public class UtilityManager {
	
	/* adding a field here that extends UtilityParent is enough to get it displayed. */
	public static final LogToViewUtility LogToViewUtility = new LogToViewUtility();
	public static final YUVColors YUVColors = new YUVColors();
	public static final ThreadStateUtility ThreadStateUtility = new ThreadStateUtility();
	public static final SyntheticImageUtility SyntheticImageUtility = new SyntheticImageUtility();
	public static final CameraCalibrateUtility CameraCalibrateUtility = new CameraCalibrateUtility();

	public static final UtilityParent[] utilities = findUtilityFields(); 
	private static UtilityParent[] findUtilityFields() {
		Field[] fields = UtilityManager.class.getDeclaredFields();
		Vector<UtilityParent> found = new Vector<>();
		
		for (Field f : fields) {
			int mod = f.getModifiers();
			if (Modifier.isFinal(mod) &&
					Modifier.isStatic(mod) &&
					Modifier.isPublic(mod)) {
				Object val = null;
				try {
					val = f.get(null);
				} catch (IllegalArgumentException e) {
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				}
				if (val instanceof UtilityParent) {
					System.out.printf("UtilityManager found field of class %s...\n", val.getClass().getName());
					found.add((UtilityParent) val);
				}
			}
		}
		
		return found.toArray(new UtilityParent[0]);
	}
}
