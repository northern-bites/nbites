package nbtool.gui;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;

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

public class LogChooserModel implements TreeModel, TreeSelectionListener, Events.LogsFound,
	Events.SessionAdded
{

	/*
	 * DataModel:
	 * */
	
	String root;
	JTree tree;
	
	protected Log[] lastSelectedLogs;
	
	public LogChooserModel() {
		root = "ROOT PLACEHOLDER";
		
		Center.listen(Events.LogsFound.class, this, true);
		Center.listen(Events.SessionAdded.class, this, true);
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
			
			lastSelectedLogs = selected.toArray(new Log[0]);
			Events.GLogSelected.generate(this, selected.remove(0), selected);
			break;
		}
		default:
			Logger.logf(Logger.ERROR, "ERROR: LCTreeModel path size was: " + first.getPathCount());
		}
	}

	@Override
	public void logsFound(Object source, Log... found) {
		HashSet<Session> sessions = new HashSet<Session>();
		
		for (Log log : found) {
			//We can't display an image that is not part of session.
			if (log.parent != null) {
				sessions.add(log.parent);
			}
		}
		
		//System.out.println("BF1: logsFound " + sessions.size() + " sessions modified.");
		
		for (Session modified : sessions) {
			sas.sort(modified);
			
			TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root, modified});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
		}
	}
	
	@Override
	public void sessionAdded(Object source, Session session) {
		TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root},
				new int[]{SessionMaster.get().sessions.indexOf(session)},
				new Object[]{session});
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
	
	protected void deleteCurrent() {
		TreePath[] pathes = tree.getSelectionPaths();
		for (TreePath tp : pathes) {
			if( tp.getPathCount() == 3 ) {
				Session ses = (Session)	tp.getPath()[1];
				Log sel = (Log) tp.getPath()[2];
				
				Logger.warnf("deleting {%s} from {%s}", sel, ses);
				
				assert(ses.logs_ALL.contains(sel));
				ses.logs_ALL.remove(sel);
				ses.logs_DO.remove(sel);
				
				TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root, ses});
				for (TreeModelListener l : listeners) {
					l.treeStructureChanged(tme);
				}
				
			} else {
				Logger.warnf("cannot delete: %s", tp.toString());
			}
		}
		
		/*
		TreePath tp = tree.getSelectionPath();
		
		if( tp.getPathCount() == 3 ) {
			Session ses = (Session)	tp.getPath()[1];
			Log sel = (Log) tp.getPath()[2];
			
			Logger.warnf("deleting {%s} from {%s}", sel, ses);
			
			assert(ses.logs_ALL.contains(sel));
			ses.logs_ALL.remove(sel);
			ses.logs_DO.remove(sel);
			
			TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root, ses});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
			
		} else {
			Logger.warnf("cannot delete: %s", tp.toString());
		} */
	}
}
