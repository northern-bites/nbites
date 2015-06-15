package nbtool.gui.utilitypanes;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Map;

import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JTree;
import javax.swing.UIManager;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreeCellRenderer;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.NBConstants;
import nbtool.util.Prefs;
import static nbtool.util.Logger.*;

public class LogToViewUtility extends UtilityParent {
	
	public Class<? extends ViewParent>[] selected(String type) {
		int tindex = Arrays.asList(types).indexOf(type);
		
		ArrayList<Class<? extends ViewParent>> sel = new ArrayList<Class<? extends ViewParent>>();
		for (ViewState vs : states[tindex]) {
			if (vs.showing)
				sel.add(vs.viewClass);
		}
		
		return sel.toArray(new Class[sel.size()]);
	}
	
	public Class<? extends ViewParent>[] viewsForLog(Log log) {
		ArrayList<Class<? extends ViewParent>> views = new ArrayList<Class<? extends ViewParent>>();
		String ptype = log.primaryType();
		assert(ptype != null);

		int tindex = Arrays.asList(types).indexOf(ptype);
		if (tindex >= 0) {
			for (ViewState vs : states[tindex]) {
				if (vs.showing)
					views.add(vs.viewClass);
			}
		}

		if (ptype.startsWith("proto-")) {
			views.addAll(Arrays.asList(this.selected(NBConstants.PROTOBUF_S)));
		}
		
		views.addAll(Arrays.asList(this.selected(NBConstants.DEFAULT_S)));
		
		logf(INFO, "LogToViewUtility found %d views for log of type %s.", views.size(), ptype);
		
		return views.toArray(new Class[views.size()]);		
	}
	
	
	private class ViewState {
		boolean showing;
		Class<? extends ViewParent> viewClass;
		
		ViewState(boolean s, Class<? extends ViewParent> cls) {
			showing = s;
			viewClass = cls;
		}
		
		public String toString() {
			return viewClass.getName();
		}
	}
	
	private ViewState[] resolve(String typename, Class<? extends ViewParent>[] possible, Class<? extends ViewParent>[] lastShown) {
		ViewState[] ret = new ViewState[possible.length];
		HashSet<Class<? extends ViewParent>> pset = new HashSet<Class<? extends ViewParent>>(Arrays.asList(possible));
		
		int i = 0;
		if (lastShown != null) {
			for (Class<? extends ViewParent> cls : lastShown) {
				if (pset.contains(cls)) {
					ret[i++] = new ViewState(true, cls);
					pset.remove(cls);
				}
			}
		}
		
		for (Class<? extends ViewParent> cls : pset) {
			ret[i++] = new ViewState(false, cls);
		}
		
		return ret;
	}
	
	private String[] types;
	private ViewState[][] states;
	
	private class LTVU_Frame extends JFrame implements TreeModel, TreeSelectionListener, MouseListener, KeyListener {
		private JTree tree;
		
		protected LTVU_Frame() {
			setLayout(null);
			setSize(600, 400);
			this.setResizable(true);
			
			tree = new JTree(this);
			tree.setEditable(false);
			tree.setRootVisible(false);
			tree.setScrollsOnExpand(true);
			
			tree.addMouseListener(this);
			tree.addKeyListener(this);
			
			tree.setCellRenderer(new LTVCellRenderer());
			tree.addTreeSelectionListener(this);
			tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
			
			tree.setDragEnabled(false);
			
			//this.setContentPane(tree);
			this.getContentPane().setLayout(new BorderLayout());
			this.getContentPane().add(tree, BorderLayout.CENTER);	
		}
		
		/* Tree methods */

		@Override
		public Object getRoot() {
			return states;
		}

		@Override
		public Object getChild(Object parent, int index) {
			if (parent == states) {
				return types[index];
			}
			
			int i2 = Arrays.asList(types).indexOf(parent);
			return states[i2][index];
		}

		@Override
		public int getChildCount(Object parent) {
			if (parent == states)
				return states.length;
			
			int i2 = Arrays.asList(types).indexOf(parent);
			return states[i2].length;
		}

		@Override
		public boolean isLeaf(Object node) {
			return node instanceof ViewState;
		}

		@Override
		public void valueForPathChanged(TreePath path, Object newValue) {
			throw new Error("modification not allowed.");
		}

		@Override
		public int getIndexOfChild(Object parent, Object child) {
			if (parent == states) {
				Arrays.asList(states).indexOf(child);	
			}
			
			return Arrays.asList((ViewState[]) parent).indexOf(child);
		}

		
		private ArrayList<TreeModelListener> listeners = new ArrayList<TreeModelListener>();
		public void addTreeModelListener(TreeModelListener l) {
			listeners.add(l);
		}
		
		public void removeTreeModelListener(TreeModelListener l) {
			listeners.remove(l);
		}
		
		/*Rendering code from "Definitive Guide to Swing for Java 2, Second Edition"*/
		private class LTVCellRenderer implements TreeCellRenderer {
			
			private JCheckBox leafRenderer = new JCheckBox();
			private DefaultTreeCellRenderer nonLeafRenderer = new DefaultTreeCellRenderer();

			Color selectionBorderColor, selectionForeground, selectionBackground,
			textForeground, textBackground;
			
			LTVCellRenderer(){
				Font fontValue;
				fontValue = UIManager.getFont("Tree.font");
				if (fontValue != null) {
					leafRenderer.setFont(fontValue);
				}
				Boolean booleanValue = (Boolean) UIManager
						.get("Tree.drawsFocusBorderAroundIcon");
				leafRenderer.setFocusPainted((booleanValue != null)
						&& (booleanValue.booleanValue()));

				selectionBorderColor = UIManager.getColor("Tree.selectionBorderColor");
				selectionForeground = UIManager.getColor("Tree.selectionForeground");
				selectionBackground = UIManager.getColor("Tree.selectionBackground");
				textForeground = UIManager.getColor("Tree.textForeground");
				textBackground = UIManager.getColor("Tree.textBackground");
			}

			@Override
			public Component getTreeCellRendererComponent(JTree tree, Object value,
					boolean selected, boolean expanded, boolean leaf, int row,
					boolean hasFocus) {
				
				Component returnValue;
				if (leaf) {
					assert(value instanceof ViewState);
					ViewState vs = (ViewState) value;
					
					leafRenderer.setText(vs.toString());
					leafRenderer.setSelected(vs.showing);

					leafRenderer.setEnabled(tree.isEnabled());

					if (selected) {
						leafRenderer.setForeground(selectionForeground);
						leafRenderer.setBackground(selectionBackground);
					} else {
						leafRenderer.setForeground(textForeground);
						leafRenderer.setBackground(textBackground);
					}

					returnValue = leafRenderer;
				} else {
					returnValue = nonLeafRenderer.getTreeCellRendererComponent(tree,
							value, selected, expanded, leaf, row, hasFocus);
				}
				
				return returnValue;
			}
			
		}

		@Override
		public void valueChanged(TreeSelectionEvent e) {
			//tree.clearSelection();
		}

		@Override
		public void mouseClicked(MouseEvent e) {
			TreePath tp = tree.getClosestPathForLocation(e.getX(), e.getY());
		
			if (tp.getLastPathComponent() != null &&
					tp.getLastPathComponent() instanceof ViewState) {
				ViewState changed = (ViewState) tp.getLastPathComponent();
				changed.showing = !changed.showing;
				
				TreeModelEvent tme = new TreeModelEvent(this,
						tp.getParentPath(),
						new int[0],
						new Object[]{changed});
				for (TreeModelListener l : listeners) {
					l.treeNodesChanged(tme);
				}
				
			}	
		}

		@Override
		public void mousePressed(MouseEvent e) {}

		@Override
		public void mouseReleased(MouseEvent e) {}

		@Override
		public void mouseEntered(MouseEvent e) {}

		@Override
		public void mouseExited(MouseEvent e) {}

		@Override
		public void keyTyped(KeyEvent e) {
			TreePath path = tree.getSelectionPath();
			int toIndex = e.getKeyChar() - '1';
			
			if (path == null || toIndex < 0 || toIndex > 8) {
				return;
			}
			
			if (path.getPathCount() == 3) {
				
				ViewState vs = (ViewState) path.getPathComponent(2);
				String type = (String) path.getPathComponent(1);
				
				int tindex = Arrays.asList(types).indexOf(type);
				int fromIndex = Arrays.asList(states[tindex]).indexOf(vs);
				
				if (toIndex < states[tindex].length) {
					logf(INFO, "swapping %s %d to %d", type, fromIndex, toIndex);
					states[tindex][fromIndex] = states[tindex][toIndex];
					states[tindex][toIndex] = vs;
					
					TreeModelEvent tme = new TreeModelEvent(this,
							new Object[]{states, type});
					for (TreeModelListener l : listeners) {
						l.treeStructureChanged(tme);
					}
				}
			}
		}

		@Override
		public void keyPressed(KeyEvent e) {}

		@Override
		public void keyReleased(KeyEvent e) {}
	}
	
	
	public LogToViewUtility() {
		super();		
		types = new String[NBConstants.POSSIBLE_VIEWS.size()];
		Map<String, Class<? extends ViewParent>[]> constPossible = NBConstants.POSSIBLE_VIEWS;
		{
			int i = 0;
			for (String s : constPossible.keySet()) {
				types[i++] = s;
			}
		}
		
		states = new ViewState[types.length][];
		Map<String, Class<? extends ViewParent>[]> lastShown = Prefs.last_shown;
		
		for (int i = 0; i < states.length; ++i)
			states[i] = resolve(types[i], constPossible.get(types[i]), lastShown.get(types[i]));	
	}

	private LTVU_Frame display = null;
	
	@Override
	public JFrame supplyDisplay() {
		if (display == null) {
			return (display = new LTVU_Frame());
		}
		
		return display;
	}

	@Override
	public String purpose() {
		return "Manage views displayed for a given log";
	}

	@Override
	public char preferredMemnonic() {
		return 'l';
	}
}
