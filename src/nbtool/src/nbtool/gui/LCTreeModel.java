package nbtool.gui;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Vector;

import javax.swing.JComponent;
import javax.swing.JTree;
import javax.swing.TransferHandler;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

import nbtool.data.Session;
import nbtool.data.Log;
import nbtool.data.SessionMaster;
import nbtool.io.FileIO;
import nbtool.util.N;
import nbtool.util.NBConstants;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.U;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;

public class LCTreeModel implements TreeModel, TreeSelectionListener, NListener{

	/*
	 * DataModel:
	 * */
	
	String root;
	JTree tree;
	
	public LCTreeModel() {
		root = "ROOT PLACEHOLDER";
		
		N.listen(EVENT.STATUS, this);
		N.listen(EVENT.LOG_FOUND, this);
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
		U.w("ERROR: TCTreeModel asked to change value, TREE SHOULD NOT BE EDITABLE.");
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
		
		//TreePath path = e.getPath();
		TreePath first = tree.getSelectionPath();
		TreePath[] all = tree.getSelectionPaths();
		assert(first == all[0]);
		
		switch (first.getPathCount()) {
		case 0:
			//??
			U.w("ERROR: LCTreeModel path size was: " + first.getPathCount());
			break;
		case 1:
			//Root selected
			U.w("ERROR: LCTreeModel path size was: " + first.getPathCount() + "ROOT SHOULD NOT BE VISIBLE");
			break;
		case 2:
			//Branch selected, 			
			N.notifyEDT(EVENT.SES_SELECTION, this,
					(Session) first.getPath()[1]);
			
			break;
		case 3:
			//LOG SELECTED.
			ArrayList<Log> selected = new ArrayList<Log>();
			
			for (TreePath p : all) {
				if (p.getPathCount() != 3)	//Skip selected items that can't be logs.
					continue;
				
				Session ses = (Session)p.getPath()[1];
				Log sel = (Log) p.getPath()[2];
				
				sel.lastSeen = p;
				
				if (sel.bytes == null) {
					assert(ses.dir != null && !ses.dir.isEmpty());
					try {
						FileIO.loadLog(sel, ses.dir);
					} catch (IOException e1) {
						U.w("Could not load log data!");
						e1.printStackTrace();
						
						return;
					}
					
					N.notifyEDT(EVENT.LOG_LOAD, this, (Object[]) new Log[]{sel});
				}
				
				assert(sel.bytes != null);
				selected.add(sel);
			}
			
			N.notifyEDT(EVENT.LOG_SELECTION, this,
					selected.remove(0), selected);
			break;
		default:
				U.w("ERROR: LCTreeModel path size was: " + first.getPathCount());
		}
	}

	public void notified(EVENT e, Object src, Object... args) {
		TreeModelEvent tme = null;
		Session cur = SessionMaster.INST.workingSession;
		
		//This is a hack, it needs to change.
		int size = SessionMaster.INST.sessions.size();
		if (cur == null && size > 0)
			cur = SessionMaster.INST.sessions.get(
					size - 1
					);
		switch(e) {
		case LOG_FOUND:
			
			sas.sort(cur);
			
			tme = new TreeModelEvent(this, new Object[]{root, cur});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
			break;
		
		case STATUS:
			STATUS s = (STATUS) args[0];
			if (s == STATUS.RUNNING) {
				//New session
				
				tme = new TreeModelEvent(this, new Object[]{root},
						new int[]{SessionMaster.INST.sessions.indexOf(cur)},
						new Object[]{cur});
				for (TreeModelListener l : listeners) {
					l.treeNodesInserted(tme);
				}
			}
			
			break;
			
		default:
			break;
		
		}
	}
	
	private class LogTransfer implements Transferable {
		
		public Log tp;

		public DataFlavor[] getTransferDataFlavors() {
			return new DataFlavor[]{NBConstants.treeFlavor};
		}

		
		public boolean isDataFlavorSupported(DataFlavor flavor) {
			return flavor.equals(NBConstants.treeFlavor);
		}

		public Object getTransferData(DataFlavor flavor)
				throws UnsupportedFlavorException, IOException {
			if (!flavor.equals(NBConstants.treeFlavor)) throw new UnsupportedFlavorException(flavor);
			else return tp;
		}
		
	}
	
	public Exporter EXPORT_HANDLER = new Exporter();
	private class Exporter extends TransferHandler {
			
		public int getSourceActions(JComponent c) {
		    return LINK;
		}

		public Transferable createTransferable(JComponent c) {
			TreePath p = tree.getSelectionPath();
			LogTransfer lt = new LogTransfer();
			
			if (p.getPathCount() != 3) return null;
			lt.tp = (Log) p.getLastPathComponent();
			
		    return lt;
		}

		public void exportDone(JComponent c, Transferable t, int action) {}
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
