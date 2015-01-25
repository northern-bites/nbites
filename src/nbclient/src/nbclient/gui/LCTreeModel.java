package nbclient.gui;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JComponent;
import javax.swing.JTree;
import javax.swing.TransferHandler;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

import nbclient.data.Session;
import nbclient.data.SessionHandler;
import nbclient.data.Log;
import nbclient.data.SessionMaster;
import nbclient.io.FileIO;
import nbclient.util.N;
import nbclient.util.NBConstants;
import nbclient.util.U;
import nbclient.util.N.EVENT;
import nbclient.util.N.NListener;

public class LCTreeModel implements TreeModel, TreeSelectionListener, NListener{

	/*
	 * DataModel:
	 * */
	
	String root;
	int curBranch;
	JTree tree;
	
	public Log NS_macro = null;
	public Log CS_macro = null;
	
	public LCTreeModel() {
		root = "";
		curBranch = -1;
		
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

	@Override
	public int getChildCount(Object parent) {
		if (parent == root) {
			return SessionMaster.INST.sessions.size();
		} else {
			return ((Session) parent).logs_DO.size();
		}
	}

	@Override
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
		TreePath path = e.getPath();
		switch (path.getPathCount()) {
		case 0:
			//??
			U.w("ERROR: LCTreeModel path size was: " + path.getPathCount());
			break;
		case 1:
			//Root selected
			U.w("ERROR: LCTreeModel path size was: " + path.getPathCount() + "ROOT SHOULD NOT BE VISIBLE");
			break;
		case 2:
			//Branch selected, 
			NS_macro = CS_macro = null;
			break;
		case 3:
			//LOG SELECTED.
			
			Object[] path_objs = path.getPath();
			Log lg = (Log) path_objs[2];
			Session b = (Session) path_objs[1];
			
			if (lg.bytes == null) {
				try {
					assert(b.dir != null && !b.dir.isEmpty());
					FileIO.loadLog(lg, b.dir);
					
					N.notify(EVENT.LOG_LOAD, this, lg);
				} catch (IOException ex) {
					ex.printStackTrace();
					U.w("message: " + ex.getMessage());
					U.w("Could not load log data.");
					return;
				}
			}
			
			CS_macro = lg;
			int index = this.getIndexOfChild(b, lg);
			if (index + 1 < b.logs_DO.size()) {
				NS_macro = b.logs_DO.get(index + 1);
			} else {
				NS_macro = null;
			}
			
			assert(lg != null);
			N.notify(EVENT.LOG_SELECTION, this, lg);
			break;
		default:
				U.w("ERROR: LCTreeModel path size was: " + path.getPathCount());
		}
	}

	public void notified(EVENT e, Object src, Object... args) {
		/*
		switch (e) {
		
		case LOGS_ADDED:
			assert(current != null);
			
			for (Object _lg : args) {
				Log lg = (Log) _lg;
				current.leaves.add(lg);
			}
			
			TreeModelEvent tme = new TreeModelEvent(this, new Object[]{root, current});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
			break;
		case STATUS:
			/*
			SessionHandler hndlr = (SessionHandler) src;
			if (hndlr.status == STATUS.RUNNING) {
				Branch newb = new Branch();
				newb.dir = hndlr.log_directory; //Should be ok if null.
				newb.name = (String) args[1] + " --> " + (String) args[2];
				
				current = newb;
				branches.add(newb);
				
				TreeModelEvent tme2 = new TreeModelEvent(this, new Object[]{root},
						new int[]{branches.indexOf(current)}, new Object[]{current});
				for (TreeModelListener l : listeners) {
					l.treeNodesInserted(tme2);
				}
			} */
			
		/*
			break;
			
		default:
			U.w("ERROR: LCTreeModel notified of unregistered event: " + e);
		} */
	}
	
	public Exporter EXPORT_HANDLER = new Exporter();
	private class Exporter extends TransferHandler {
		
		private class LogTransfer implements Transferable {
			
			public Log tp;

			public DataFlavor[] getTransferDataFlavors() {
				return new DataFlavor[]{NBConstants.treeFlavor};
			}

			@Override
			public boolean isDataFlavorSupported(DataFlavor flavor) {
				return flavor.equals(NBConstants.treeFlavor);
			}

			@Override
			public Object getTransferData(DataFlavor flavor)
					throws UnsupportedFlavorException, IOException {
				if (!flavor.equals(NBConstants.treeFlavor)) throw new UnsupportedFlavorException(flavor);
				else return tp;
			}
			
		}
		
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
}
