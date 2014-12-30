package nbclient.data;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;

import nbclient.util.N;
import nbclient.util.N.EVENT;
import nbclient.util.N.NListener;

public class Stats implements NListener {
	public int numlogs;
	public long totalbytes;
	public Set<String> types;
	
	public Stats() {
		numlogs = 0;
		totalbytes = 0;
		types = new HashSet<String>();
		
		//Not happy about this, the current stats object belonds to datahandler, and
		//Logs are being loaded in LCTreeModel.  It would be nice if Stats was totally independent from all that...
		N.listen(EVENT.LOG_LOADED, this);
	}
	
	public void reset() {
		numlogs = 0;
		totalbytes = 0;
		types.clear();
		
		updateGUI();
	}
	
	private void _update(Log lg) {
		++numlogs;
		if (lg.bytes != null)
			totalbytes += lg.bytes.length;
		
		types.add((String) lg.getAttributes().get("type"));
	}
	
	public void loaded(Log lg) {
		totalbytes += lg.bytes.length;
		
		updateGUI();
	}
	
	public void update(Log lg) {
		_update(lg);
		updateGUI();
	}
	
	public void update(Log[] lgs) {
		for (Log l : lgs) {
			_update(l);
		}
		
		updateGUI();
	}
	
	private void updateGUI() {
		N.notifyEDT(EVENT.STATS, this, this);
	}

	
	public void notified(EVENT e, Object src, Object... args) {
		assert(e == EVENT.LOG_LOADED);
		this.loaded((Log) args[0]);
	}
}