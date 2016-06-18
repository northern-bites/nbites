package nbtool.gui.utilitypanes;

import java.util.ArrayList;

import nbtool.gui.utilitypanes.UtilityParent.UtilityListener;

public abstract class UtilityProvider<P, L extends UtilityListener<P>> extends UtilityParent {
	private final ArrayList<L> listeners = new ArrayList<>();

	public void listen(L listener) {
		listeners.add(listener);
	}

	public void stopListening(L listener) {
		listeners.remove(listener);
	}

	protected void fireChanged() {
		for (UtilityListener<P> ul : listeners) {
			ul.utilityChanged(this, getLatest());
		}
	}

	public abstract P getLatest();
}
