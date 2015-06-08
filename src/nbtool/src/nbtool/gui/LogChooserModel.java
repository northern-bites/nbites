package nbtool.gui;

import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JTree;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.io.FileIO;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.NBConstants.STATUS;

public class LogChooserModel implements TreeModel, TreeSelectionListener, Events.LogsFound, Events.ToolStatus {

	/*
	 * DataModel:
	 * */
	
	String root;
	JTree tree;
	
	public LogChooserModel() {
		root = "ROOT PLACEHOLDER";
		
		Center.listen(Events.LogsFound.class, this, true);
		Center.listen(Events.ToolStatus.class, this, true);
	}
	
	public Object getRoot() {
		return root;
	}

	public Object getChild(Object parent, int index) {
		if (parent == root) {
			return SessionMaster.INST.sessions.get(index);
		} else {
			Session s = (Session) parent;
			return s.logs_DO.get(index);
		}
	}

	public int getChildCount(Object parent) {
		if (parent == root) {
			return SessionMaster.INST.sessions.size();
		} else {
			return ((Session) parent).logs_DO.size();
		}
	}

	public boolean isLeaf(Object node) {
		if (node.getClass() == Log.class) return true;
		else return false;
	}

	@Override
	public void valueForPathChanged(TreePath path, Object newValue) {
		Logger.logf(Logger.ERROR, "ERROR: TCTreeModel asked to change value, TREE SHOULD NOT BE EDITABLE.");
	}

	@Override
	public int getIndexOfChild(Object parent, Object child) {
		if (parent == root) {
			return SessionMaster.INST.sessions.indexOf(child);
		} else {
			return ((Session) parent).logs_DO.indexOf(child);
		}
	}

	private ArrayList<TreeModelListener> listeners = new ArrayList<TreeModelListener>();
	public void addTreeModelListener(TreeModelListener l) {
		listeners.add(l);
	}
	public void removeTreeModelListener(TreeModelListener l) {
		listeners.remove(l);
	}
	
	public void valueChanged(TreeSelectionEvent e) {
		
		if (!e.isAddedPath())
			return;
		
		TreePath first = tree.getSelectionPath();
		TreePath[] all = tree.getSelectionPaths();
		assert(first == all[0]);
		
		switch (first.getPathCount()) {
		case 0:
			Logger.logf(Logger.ERROR, "ERROR: LCTreeModel path size was: " + first.getPathCount());
			break;
		case 1:
			Logger.logf(Logger.ERROR, "ERROR: LCTreeModel path size was: " + first.getPathCount() + "ROOT SHOULD NOT BE VISIBLE");
			break;
		case 2: {
			Session session = (Session) first.getPath()[1];
			Events.GSessionSelected.generate(this, session);
			break;
		}
		case 3: {
			//LOG SELECTED.
			ArrayList<Log> selected = new ArrayList<Log>();
			
			for (TreePath p : all) {
				if (p.getPathCount() != 3)	//Skip selected items that can't be logs.
					continue;
				
				Session ses = (Session)p.getPath()[1];
				Log sel = (Log) p.getPath()[2];
								
				if (sel.bytes == null) {
					assert(ses.directoryFrom != null && !ses.directoryFrom.isEmpty());
					try {
						FileIO.loadLog(sel, ses.directoryFrom);
					} catch (IOException e1) {
						Logger.logf(Logger.ERROR, "Could not load log data!");
						e1.printStackTrace();
						
						return;
					}
					Events.GLogLoaded.generate(this, sel);
				}
				
				assert(sel.bytes != null);
				selected.add(sel);
			}
			
			Events.GLogSelected.generate(this, selected.remove(0), selected);
			break;
		}
		default:
			Logger.logf(Logger.ERROR, "ERROR: LCTreeModel path size was: " + first.getPathCount());
		}
	}
	
	@Override
	public void toolStatus(Object source, STATUS s, String desc) {
		Session relevant = SessionMaster.get().getLatestSession();
		
		TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root},
				new int[]{SessionMaster.get().sessions.indexOf(relevant)},
				new Object[]{relevant});
		for (TreeModelListener l : listeners) {
			l.treeStructureChanged(tme);
		}
	}

	@Override
	public void logsFound(Object source, Log... found) {
		Session relevant = SessionMaster.get().getLatestSession();
		Logger.logf(Logger.INFO, "TreeModel: rel=%s size=%d", relevant.toString(), relevant.logs_ALL.size());
		sas.sort(relevant);
		
		TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root, relevant});
		for (TreeModelListener l : listeners) {
			l.treeStructureChanged(tme);
		}
	}
	
	protected SortAndSearch sas;
	protected void ssChanged() {
		
		for (Session s : SessionMaster.INST.sessions)
			sas.sort(s);
		
		TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root});
		for (TreeModelListener l : listeners) {
			l.treeStructureChanged(tme);
		}
	}
}
