package nbtool.gui.utilitypanes;

import java.awt.Rectangle;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Vector;

import nbtool.util.Center;
import nbtool.util.Debug;
import nbtool.util.UserSettings;
import nbtool.util.UserSettings.DisplaySettings;
import nbtool.util.Center.NBToolShutdownListener;
import static nbtool.util.Debug.*;

public class UtilityManager {
	
	/* adding a field here that
	 * ** extends UtilityParent 
	 * ** is 'public static final'
	 * is enough to get it displayed.  This is also the preferred way of added utilities to the display. */
	
	public static final LogToViewUtility LogToViewUtility = new LogToViewUtility();
	public static final YUVColorUtility YUVColorUtility = new YUVColorUtility();
	public static final ThreadStateUtility ThreadStateUtility = new ThreadStateUtility();

	public static final WorldViewUtility WorldViewUtility = new WorldViewUtility();
	public static final BroadcastUtility BroadcastUtility = new BroadcastUtility();
	
	public static final ReachableRobots ReachableRobots = new ReachableRobots();
	public static final CameraOffsetsUtility CameraOffsetsUtility = new CameraOffsetsUtility();
	public static final CameraSettingsUtility CameraSettingsUtility = new CameraSettingsUtility();
	
	public static final BallTestUtility BallTestUtility = new BallTestUtility();

	//Unused at the moment – comment out to re-enable.
//	public static final SyntheticImageUtility SyntheticImageUtility = new SyntheticImageUtility();
//	public static final ReplayUtility ReplayUtility = new ReplayUtility();

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
					Debug.info("UtilityManager found field of class %s...", val.getClass().getName());
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
						UserSettings.BOUNDS_MAP.put(up.preferenceKey(), new DisplaySettings(bnds, null, 0));
					}
				}
			}
		});
	}
}
