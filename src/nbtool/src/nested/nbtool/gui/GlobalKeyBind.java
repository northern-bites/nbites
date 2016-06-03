package nbtool.gui;

import java.awt.KeyEventPostProcessor;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import javax.swing.JFrame;
import javax.swing.JTabbedPane;

import nbtool.data.SExpr;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.gui.utilitypanes.UtilityParent;
import nbtool.util.Debug;

public class GlobalKeyBind {

	private static abstract class KeyBoundAction {
		protected abstract void happened();
	}
	
	private static final class UtilityKeyAction extends KeyBoundAction {
		protected UtilityParent utility;
		protected UtilityKeyAction(UtilityParent up) {
			this.utility = up;
		}
		
		@Override
		protected void happened() {
			JFrame display = this.utility.getDisplay();
			display.setVisible(!display.isVisible());
		}
	}
	
	private static final Map<Character, KeyBoundAction> BIND_MAP = new HashMap<>();
		
	public static void setupKeyBinds() {
		
		/* reserved for use inside a tool display */
		for (int i = 1; i < 10; ++i) {
			char c = String.valueOf(i).charAt(0);
			BIND_MAP.put(c, null);
		}
		
		BIND_MAP.put('`', null);
		
		/* reserved for use showing multiple displays */
		BIND_MAP.put('=', new KeyBoundAction(){

			@Override
			protected void happened() {
				Debug.warn("...requested new display...");
				Displays.requestAnotherDisplay();
			}
			
		});
		
		/* then install utility keybinds */
		for (UtilityParent up : UtilityManager.utilities) {
			if (!BIND_MAP.containsKey(up.preferredMemnonic())) {
				BIND_MAP.put(up.preferredMemnonic(), new UtilityKeyAction(up));
			} else {
				Debug.error("KeyBind cannot bind %s to key %c, key already bound!",
						up.getClass().getSimpleName(), up.preferredMemnonic());
			}
		}
	}

	public static boolean simulate(char c) {
		if (BIND_MAP.containsKey(c)) {
			handle(c);
			return true;
		} else {
			return false;
		}
	}

	private static void handle(char c) {
		KeyBoundAction kba = BIND_MAP.get(c);
		if (kba != null) {
			kba.happened();
		}
	}

	static {
		Debug.info("Installing KeyBind post-processor...");
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(
				new KeyEventPostProcessor() {

					@Override
					public boolean postProcessKeyEvent(KeyEvent e) {
						if (!e.isConsumed() && (e.getID() == KeyEvent.KEY_TYPED))
							handle(e.getKeyChar());
						return false;
					}

				}
				);
	}
}
