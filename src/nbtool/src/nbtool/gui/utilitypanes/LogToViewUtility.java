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
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
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
import nbtool.data.ViewProfile;
import nbtool.data.ViewProfile.ViewState;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Events;
import nbtool.util.NBConstants;
import nbtool.util.Prefs;
import static nbtool.util.Logger.*;

public class LogToViewUtility extends UtilityParent {	

	public LogToViewUtility() {
		super();		
	}

	private LogToViewFrame display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display == null) {
			return (display = new LogToViewFrame());
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

	/* NEW STUFF */
	public class LogToViewFrame extends javax.swing.JFrame implements TreeModel, MouseListener, KeyListener  {

		private final Object root = new Object();
		protected ViewProfile vp = ViewProfile.DEFAULT_PROFILE;

		@Override
		public Object getRoot() {
			return root;
		}

		@Override
		public Object getChild(Object parent, int index) {
			if (parent == root) {
				return ViewProfile.TYPES[index];
			}

			int i2 = Arrays.asList(ViewProfile.TYPES).indexOf(parent);
			return vp.states[i2][index];
		}

		@Override
		public int getChildCount(Object parent) {
			if (parent == root)
				return ViewProfile.TYPES.length;

			int i2 = Arrays.asList(ViewProfile.TYPES).indexOf(parent);
			return vp.states[i2].length;
		}

		@Override
		public boolean isLeaf(Object node) {
			return (node instanceof ViewState);
		}

		@Override
		public void valueForPathChanged(TreePath path, Object newValue) {
			throw new Error("modification not allowed.");
		}

		@Override
		public int getIndexOfChild(Object parent, Object child) {
			if (parent == root) {
				return Arrays.asList(ViewProfile.TYPES).indexOf(child);	
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

			Color selectionForeground, selectionBackground,
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
			//int toIndex = e.getKeyChar() - '1';
			int toIndex = Character.getNumericValue(e.getKeyChar());

			if (path == null || toIndex < 0 || toIndex > 8) {
				return;
			}

			if (path.getPathCount() == 3) {

				ViewState vs = (ViewState) path.getPathComponent(2);
				String type = (String) path.getPathComponent(1);

				int tindex = Arrays.asList(ViewProfile.TYPES).indexOf(type);
				int fromIndex = Arrays.asList(vp.states[tindex]).indexOf(vs);

				if (toIndex < vp.states[tindex].length) {
					logf(INFO, "swapping %s %d to %d", type, fromIndex, toIndex);
					vp.states[tindex][fromIndex] = vp.states[tindex][toIndex];
					vp.states[tindex][toIndex] = vs;

					TreeModelEvent tme = new TreeModelEvent(this,
							new Object[]{root, type});
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

		private void resetProfileBoxModel() {
			Collection<ViewProfile> set = ViewProfile.PROFILES.values();
			profileComboBox.setModel(new DefaultComboBoxModel<>(
					set.toArray(new ViewProfile[0])));
			
			if (set.contains(vp)) {
				profileComboBox.setSelectedItem(vp);
			} else {
				ViewProfile def = ViewProfile.DEFAULT_PROFILE;
				profileComboBox.setSelectedItem(def);
			}
		}

		private void profileComboBoxActionPerformed(java.awt.event.ActionEvent evt) {                                                
			ViewProfile sel = (ViewProfile) profileComboBox.getSelectedItem();
			if (sel == null) return;
			
			vp = sel;
			profileChanged();
		}
		
		private void deleteAction() {
			if (vp == ViewProfile.DEFAULT_PROFILE) {
				JOptionPane.showMessageDialog(this, String.format("cannot delete default profile"));
			} else {
				ViewProfile.PROFILES.remove(vp.name);
				vp = ViewProfile.DEFAULT_PROFILE;
				resetProfileBoxModel();
				profileChanged();
				
				Events.GViewProfileSetChanged.generate(this);
			}
		}
		
		private void createAction() {
			String name = nameField.getText();
			if (name.isEmpty() || name.equals(ViewProfile.DEFAULT_PROFILE_NAME)) {
				JOptionPane.showMessageDialog(this, String.format("cannot use that name: {%s}", name));
				return;
			}
			
			vp = ViewProfile.addWithName(name);
			resetProfileBoxModel();
			profileChanged();
			
			Events.GViewProfileSetChanged.generate(this);
		}
		
		private void profileChanged() {
			TreeModelEvent tme = new TreeModelEvent(this,
					new Object[]{root});
			for (TreeModelListener l : listeners) {
				l.treeStructureChanged(tme);
			}
		}

		/**
		 * Creates new form LogToViewFrame
		 */
		public LogToViewFrame() {
			initComponents();

			tree.setEditable(false);
			tree.setRootVisible(false);
			tree.setScrollsOnExpand(true);

			tree.addMouseListener(this);
			tree.addKeyListener(this);

			tree.setCellRenderer(new LTVCellRenderer());
			tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

			tree.setDragEnabled(false);
			
			resetProfileBoxModel();
			
			createButton.addActionListener(new java.awt.event.ActionListener() {
	            public void actionPerformed(java.awt.event.ActionEvent evt) {
	                createAction();
	            }
	        });
			
			deleteButton.addActionListener(new java.awt.event.ActionListener() {
	            public void actionPerformed(java.awt.event.ActionEvent evt) {
	                deleteAction();
	            }
	        });
		}

		/**
	     * This method is called from within the constructor to initialize the form.
	     * WARNING: Do NOT modify this code. The content of this method is always
	     * regenerated by the Form Editor.
	     */
	    @SuppressWarnings("unchecked")
	    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
	    private void initComponents() {

	        profileComboBox = new javax.swing.JComboBox<>();
	        deleteButton = new javax.swing.JButton();
	        createButton = new javax.swing.JButton();
	        jScrollPane1 = new javax.swing.JScrollPane();
	        tree = new javax.swing.JTree(this);
	        nameField = new javax.swing.JTextField();

	        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

	        profileComboBox.addActionListener(new java.awt.event.ActionListener() {
	            public void actionPerformed(java.awt.event.ActionEvent evt) {
	                profileComboBoxActionPerformed(evt);
	            }
	        });

	        deleteButton.setText("delete this");

	        createButton.setText("create new");

	        jScrollPane1.setViewportView(tree);

	        nameField.setText("a name");

	        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
	        getContentPane().setLayout(layout);
	        layout.setHorizontalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addContainerGap()
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	                    .addGroup(layout.createSequentialGroup()
	                        .addComponent(jScrollPane1)
	                        .addContainerGap())
	                    .addGroup(layout.createSequentialGroup()
	                        .addComponent(profileComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, 228, javax.swing.GroupLayout.PREFERRED_SIZE)
	                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 147, Short.MAX_VALUE)
	                        .addComponent(createButton)
	                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                        .addComponent(nameField, javax.swing.GroupLayout.PREFERRED_SIZE, 150, javax.swing.GroupLayout.PREFERRED_SIZE)
	                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 73, Short.MAX_VALUE)
	                        .addComponent(deleteButton))))
	        );
	        layout.setVerticalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addContainerGap()
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
	                    .addComponent(profileComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
	                    .addComponent(deleteButton)
	                    .addComponent(createButton)
	                    .addComponent(nameField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 315, Short.MAX_VALUE)
	                .addContainerGap())
	        );

	        pack();
	    }// </editor-fold>                        


	    // Variables declaration - do not modify                     
	    private javax.swing.JButton createButton;
	    private javax.swing.JButton deleteButton;
	    private javax.swing.JScrollPane jScrollPane1;
	    private javax.swing.JTextField nameField;
	    private javax.swing.JComboBox<ViewProfile> profileComboBox;
	    private javax.swing.JTree tree;
	    // End of variables declaration                  
	}
}
