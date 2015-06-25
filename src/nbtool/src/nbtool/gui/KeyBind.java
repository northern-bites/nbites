package nbtool.gui;

import java.awt.KeyEventPostProcessor;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.util.HashMap;
import java.util.Map;

import javax.swing.JFrame;
import javax.swing.JTabbedPane;

import nbtool.data.SExpr;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.gui.utilitypanes.UtilityParent;
import nbtool.util.Logger;

public class KeyBind {

	private static abstract class KeyBoundAction {
		protected abstract void happened();
	}
	
	private static abstract class UtilityKeyAction extends KeyBoundAction {
		protected UtilityParent utility;
		protected UtilityKeyAction(UtilityParent up) {
			this.utility = up;
		}
	}

	private static final Map<Character, KeyBoundAction> BIND_MAP = new HashMap<>();
	
	protected static JTabbedPane left;
	protected static JTabbedPane right;
	protected static JTabbedPane subRight;
	
	protected static ControlPanel controlPanel;
	protected static LogDisplayPanel mainPanel;
	
	public static void setupKeyBinds(Map<String, SExpr> misc_map) {
		assert(left != null);
		assert(right!= null);
		assert(subRight != null);
		assert(controlPanel != null);
		
		/* first install core keybinds */
		BIND_MAP.put('q', new KeyBoundAction(){
			@Override
			protected void happened() {
				left.setSelectedIndex(0);
			}
		});
		
		BIND_MAP.put('w', new KeyBoundAction(){
			@Override
			protected void happened() {
				left.setSelectedIndex(1);
			}
		});
		
		BIND_MAP.put('e', new KeyBoundAction(){
			@Override
			protected void happened() {
				right.setSelectedIndex(0);
			}
		});
		
		BIND_MAP.put('r', new KeyBoundAction(){
			@Override
			protected void happened() {
				right.setSelectedIndex(1);
			}
		});
		
		BIND_MAP.put('t', new KeyBoundAction(){
			@Override
			protected void happened() {
				right.setSelectedIndex(2);
			}
		});
		
		BIND_MAP.put('f', new KeyBoundAction(){
			@Override
			protected void happened() {
				right.setSelectedIndex(2);
				subRight.setSelectedIndex(0);
			}
		});
		
		BIND_MAP.put('g', new KeyBoundAction(){
			@Override
			protected void happened() {
				right.setSelectedIndex(2);
				subRight.setSelectedIndex(1);
			}
		});
		
		/* then install ldp keybinds */
		
		BIND_MAP.put('!', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(0);
			}
		});
		
		BIND_MAP.put('@', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(1);
			}
		});
		
		BIND_MAP.put('#', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(2);
			}
		});
		
		BIND_MAP.put('$', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(3);
			}
		});
		
		BIND_MAP.put('%', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(4);
			}
		});
		
		BIND_MAP.put('^', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(5);
			}
		});
		
		BIND_MAP.put('&', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(6);
			}
		});
		
		BIND_MAP.put('*', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(7);
			}
		});
		
		BIND_MAP.put('(', new KeyBoundAction(){
			@Override
			protected void happened() {
				ExternalLogDisplay.toggle(8);
			}
		});
		
		/* then install utility keybinds */
		
		for (UtilityParent up : UtilityManager.utilities) {
			if (!BIND_MAP.containsKey(up.preferredMemnonic())) {
				BIND_MAP.put(up.preferredMemnonic(), new UtilityKeyAction(up){
					@Override
					protected void happened() {
						JFrame display = this.utility.getDisplay();
						display.setVisible(!display.isVisible());
					}
				});
			} else {
				Logger.warnf("KeyBind cannot bind %s to key %c, key already bound!",
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
		if (Character.isDigit(c)) {
			mainPanel.trySetFocus(Character.getNumericValue(c) - 1);
		} else {
			KeyBoundAction kba = BIND_MAP.get(c);
			if (kba != null) {
				kba.happened();
			}
		}	
	}

	static {
		Logger.printf("Installing KeyBind post-processor...");
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
