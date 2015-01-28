package nbtool.gui.utilitypanes;

import java.util.ArrayList;
import java.util.HashSet;

import javax.swing.JFrame;

public abstract class UtilityParent extends JFrame {
	public UtilityParent() {
		super();
		setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
	}
	
	public abstract Object getCurrentValue();
	
	//Implementations of UtilityParent can decide how and when to use this...
	private HashSet<UtilityChangeListener> listeners = new HashSet<UtilityChangeListener>();
	public void addListener(UtilityChangeListener l) {listeners.add(l);}
	public void removeListener(UtilityChangeListener l) {listeners.remove(l);}
	public interface UtilityChangeListener {
		public void utilValueChanged(UtilityParent src, Object... new_values);
	}
}
