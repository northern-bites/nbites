package nbtool.gui.utilitypanes;

import java.awt.Rectangle;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Vector;

import nbtool.util.Center;
import nbtool.util.Prefs;
import nbtool.util.Center.NBToolShutdownListener;
import nbtool.util.Prefs.ExtBounds;
import static nbtool.util.Logger.*;

public class UtilityManager {
	
	/* adding a field here that
	 * ** extends UtilityParent 
	 * ** is 'public static final'
	 * is enough to get it displayed.  This is also the preferred way of added utilities to the display. */
	public static final LogToViewUtility LogToViewUtility = new LogToViewUtility();
	public static final YUVColorUtility YUVColorUtility = new YUVColorUtility();
	public static final ThreadStateUtility ThreadStateUtility = new ThreadStateUtility();
	public static final ReplayUtility ReplayUtility = new ReplayUtility();
		
	public static final SyntheticImageUtility SyntheticImageUtility = new SyntheticImageUtility();
	//public static final CameraCalibrateUtility CameraCalibrateUtility = new CameraCalibrateUtility();
	public static final CameraCalibrateUtility2 CameraCalibrateUtility2 = new CameraCalibrateUtility2();

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
	
	static {
		Center.listen(new NBToolShutdownListener() {
			@Override
			public void nbtoolShutdownCallback() {
				for (UtilityParent up : utilities) {
					if (up.previouslySupplied != null) {
						Rectangle bnds = up.previouslySupplied.getBounds();
						Prefs.BOUNDS_MAP.put(up.preferenceKey(), new ExtBounds(bnds, null));
					}
				}
			}
		});
	}
}
