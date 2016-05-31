package nbtool.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FileDialog;
import java.awt.KeyboardFocusManager;
import java.awt.datatransfer.DataFlavor;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetDropEvent;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyVetoException;
import java.beans.VetoableChangeListener;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import javax.swing.DefaultComboBoxModel;
import javax.swing.InputVerifier;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.KeyStroke;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import nbtool.nio.FileIO;
import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.ToolSettings;
import nbtool.util.UserSettings;
import nbtool.util.Utility;

public class PathChooser {

	public static void main(String[] args) {
		Path path = chooseDirPath(null);
		Debug.print("end: %s", path);
	}

	/*
	 * input verifier,
	 * tab complete
	 * select via tree
	 * BACK BUTTON
	 * 
	 * */

	public static Path chooseLogPath(Component centered) {
		return internal(centered, true);
	}

	public static Path chooseDirPath(Component centered) {
		return internal(centered, false);
	}

	private static Path internal(Component centered, boolean file) {
		final JDialog dialog = new JDialog();
		dialog.setModal(true);
		dialog.setTitle("choose path");
		dialog.setLocationRelativeTo(centered);

		handler.setMode(file, dialog);

		dialog.setContentPane(handler.display);
		dialog.setSize(handler.display.getPreferredSize());
		dialog.setMinimumSize(handler.display.getPreferredSize());

		ActionListener escListener = new ActionListener() {

	        @Override
	        public void actionPerformed(ActionEvent e) {
	            dialog.dispose();
	        }
	    };

	    dialog.getRootPane().registerKeyboardAction(escListener,
	            KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
	            JComponent.WHEN_IN_FOCUSED_WINDOW);
	    
		dialog.setVisible(true);
		
		dialog.dispose();
		return handler.get();
	}
	
	private static final DebugSettings debug =
			Debug.createSettings(true, true, true, Debug.INFO, null);

	private static final PathChooserHandler handler = new PathChooserHandler();

	private static class PathChooserHandler implements KeyListener, ActionListener, TreeModel, TreeSelectionListener {

		protected final PathChooseDisplay display = new PathChooseDisplay();
		private JDialog dialog;
		private boolean forceFile;
		private Path chosen;

		protected PathChooserHandler() {
			display.chooserTextField.setDropTarget(new DropTarget() {
				public synchronized void drop(DropTargetDropEvent evt) {
					try {
						evt.acceptDrop(DnDConstants.ACTION_COPY);
						@SuppressWarnings("unchecked")
						List<File> droppedFiles = ((List<File>) evt
								.getTransferable().getTransferData(
										DataFlavor.javaFileListFlavor));

						if (!droppedFiles.isEmpty()) {
							File dropped = droppedFiles.get(droppedFiles.size() - 1);
							Debug.warn("using dragged file: %s", dropped.getAbsolutePath());
							display.chooserTextField.setText(dropped.getAbsolutePath());
						}

					} catch (Exception ex) {
						ex.printStackTrace();
					}
				}
			});

			display.chooserTextField.setFocusTraversalKeys(
					KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS,
					Collections.EMPTY_SET);
			display.chooserTextField.addKeyListener(this);
			
			display.loadPathComboBox.addActionListener(new ActionListener(){
				@Override
				public void actionPerformed(ActionEvent e) {
					display.chooserTextField.setText(
							display.loadPathComboBox.getSelectedItem().toString()
							);
					validate();
				}
			});
			
			display.choosePathButton.addActionListener(this);
			display.workingDirTree.setModel(this);
			display.workingDirTree.addTreeSelectionListener(this);
			
			display.workingDirTree.setEditable(false);
			display.workingDirTree.setRootVisible(false);
			display.workingDirTree.getSelectionModel().
				setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
			
			display.createPathBox.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					validate();
				}
			});
		}
		
		final String tip = " (prefix with '~' or '/',"
				+ " otherwise path is relative to NBITES_DIR)";
		final String fileText = "choose file path " + tip;
		final String dirText = "choose dir path " + tip;

		protected void setMode(boolean file, JDialog dialog) {
			this.forceFile = file;
			this.dialog = dialog;
			this.chosen = null;
			
			display.chooserInfoAndTips.setText(file ? fileText:dirText);
		
			display.loadPathComboBox.setModel(
					new DefaultComboBoxModel<Path>(UserSettings.loadPathes.vector()));
			if (UserSettings.loadPathes.vector().isEmpty()) {
				display.chooserTextField.setText(ToolSettings.NBITES_DIR);
			} else {
				display.loadPathComboBox.setSelectedIndex(0);
				display.chooserTextField.setText(display.loadPathComboBox.getSelectedItem().toString());
			}
			
			display.createPathBox.setSelected(false);
			validate();
		}

		protected Path get() {
			debug.event("get() returned: %s", chosen);
			return chosen;
		}
		
		private Path tabHelper() {
			Path current = this.expand();
			Path enclosing = this.enclosing();
			if (enclosing == null) {
				Debug.warn("cannot get enclosing: %s", current);
				return null;
			}
			
			Path[] parts = FileIO.getContentsOf(enclosing);
			assert(parts != null);
			if (parts.length == 1) {
				return parts[0];
			} else {
				if (!Files.isDirectory(current)) {
					Path last = current.getFileName();
					Path best = null;
					for (Path sub : parts) {
						Path subName = sub.getFileName();
						if (subName.toString().startsWith(last.toString())) {
							if (best == null ||
									subName.toString().length() < best.toString().length())
								best = subName;
						}
					}
					
					if (best != null) {
						Path resolved = enclosing.resolve(best);
						return resolved;
					} else return null;
					
				} else {
					return current;
				}
			}
		}

		@Override
		public void keyTyped(KeyEvent e) {
			if (e.getKeyChar() == '\t') {
				e.consume();
				
				Path tabCompleted = tabHelper();
				
				if (tabCompleted == null)
					return;
				if (Files.isDirectory(tabCompleted)) {
					display.chooserTextField.setText(tabCompleted.toString() + "/");

				} else {
					display.chooserTextField.setText(tabCompleted.toString());
				}
								
			}						
		}

		public void keyPressed(KeyEvent e) {}
		public void keyReleased(KeyEvent e) {
			validate();
		}
		
		private boolean isValid() {
			String text = display.chooserTextField.getText().trim();
			debug.event("isValid() on: %s", text);
			Path current = expand();
			debug.event("isValid() path: %s", current);
			
			if (forceFile) {
				if (text.endsWith("/")) return false;
				
				if (Files.exists(current)) {
					return !Files.isDirectory(current);
				}
				
				return display.createPathBox.isSelected();
			
			} else {
				if (Files.exists(current)) {
					return Files.isDirectory(current);
				}
				
				return display.createPathBox.isSelected();
			}
		}
		
		private boolean validate() {
			Path enclosing = enclosing();
			Debug.event("validate() enclosing = %s", enclosing);
			
			if (display.isVisible() && enclosing != null) {
				restructure();
			}
			
			boolean valid = isValid();
			display.choosePathButton.setEnabled(valid);
			display.choosePathButton.setForeground(
					valid ? Color.green : Color.BLACK);
			
			return valid;
		}
		
		private Path expand() {
			String text = display.chooserTextField.getText().trim();
			Path expanded = null;
			if (text.isEmpty()) {
				expanded = ToolSettings.NBITES_DIR_PATH;
			} else {
				char start = text.charAt(0);
				switch(start) {
				case '~':
					expanded = Paths.get(System.getProperty("user.home"), 
							text.substring(1)); break;
				case '/':
					expanded =  Paths.get(text); break;
				default:
					expanded =  ToolSettings.NBITES_DIR_PATH.resolve(text); break;
				}
			}
			
			assert(expanded.isAbsolute());
			return expanded;
		}
		
		private Path enclosing() {
			Path path = expand();
			
			if (Files.exists(path) && Files.isDirectory(path)) {
				return path;
			} else {
				Path enclosing = path.getParent();
//				Debug.info("enclosing() internal enclosing = %s exists %B isAbsolute %B", enclosing,
//						Files.exists(enclosing), enclosing.isAbsolute());
				
				if (Files.exists(enclosing)) {
					assert(Files.isDirectory(enclosing));
					return enclosing;
				} else {
					return null;
				}
			}
		}
		
		private Path[] workingContents() {
			Path dir = enclosing();
			if (dir == null) return new Path[0];
			Path[] found = FileIO.getContentsOf(dir);
			return (found == null) ? new Path[0] : found;
		}

		@Override
		public void valueChanged(TreeSelectionEvent e) {
			if (e.isAddedPath()) {
				TreePath path = e.getPath();
				Path clicked = (Path) path.getLastPathComponent();
				display.chooserTextField.setText(clicked.toString());
				validate();
			}
		}

		@Override
		public Object getRoot() {
			return this;
		}

		@Override
		public Object getChild(Object parent, int index) {
			if (parent == this) {
				return workingContents()[index];
			} else {
				throw new Error("no children at index " + index);
			}
		}

		@Override
		public int getChildCount(Object parent) {
			if (parent == this)
				return workingContents().length;
			else return 0;
		}

		@Override
		public boolean isLeaf(Object node) {
			if (node == this) return false;
			else {
				Path path = (Path) node;
				return !Files.isDirectory(path);
			}
		}

		@Override
		public void valueForPathChanged(TreePath path, Object newValue) { }

		@Override
		public int getIndexOfChild(Object parent, Object child) {
			assert(parent == this && child instanceof Path);
			return Arrays.asList(workingContents()).indexOf(child);
		}

		private final ArrayList<TreeModelListener> listeners = new ArrayList<TreeModelListener>();
		@Override
		public void addTreeModelListener(TreeModelListener l) {
			listeners.add(l);
		}
		@Override
		public void removeTreeModelListener(TreeModelListener l) {
			listeners.remove(l);
		}
		
		private void restructure() {
			TreeModelEvent changed = new TreeModelEvent(this, 
					new Object[]{this});
			for (TreeModelListener listener : listeners)
				listener.treeStructureChanged(changed);
		}
		
		private void chooseAction() {
			if (isValid()) {
				chosen = expand();
				if (display.createPathBox.isSelected() &&
						!Files.exists(chosen)) {
					
					if (forceFile) {
						//last part is file name
						Path needsCreate = chosen.getParent();
						if (!Files.exists(needsCreate)) {
							try {
								Files.createDirectories(needsCreate);
								ToolMessage.displayInfo("created path: %s", needsCreate);
							} catch (IOException e1) {
								e1.printStackTrace();
								ToolMessage.displayError("error creating path to: %s", chosen);
								chosen = null;
							}
						}
						
					} else {
						Path needsCreate = chosen;
						try {
							Files.createDirectories(needsCreate);
							ToolMessage.displayInfo("created path: %s", needsCreate);
						} catch (IOException e1) {
							e1.printStackTrace();
							ToolMessage.displayError("error creating path: %s", chosen);
							chosen = null;
						}
					}
				}
				
				dialog.dispose();
			}
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			assert(e.getSource() == display.choosePathButton);
			chooseAction();
		}
	}

	private static class PathChooseDisplay extends JPanel {
		private static final long serialVersionUID = 1L;
		protected PathChooseDisplay() {
			super();
			initComponents();
		}

		/**
		 * This method is called from within the constructor to initialize the form.
		 * WARNING: Do NOT modify this code. The content of this method is always
		 * regenerated by the Form Editor.
		 */
		// <editor-fold defaultstate="collapsed" desc="Generated Code">                          
		private void initComponents() {

			jScrollPane1 = new javax.swing.JScrollPane();
			chooserTextField = new javax.swing.JTextField();
			chooserInfoAndTips = new javax.swing.JLabel();
			loadPathComboBox = new javax.swing.JComboBox<>();
			jScrollPane2 = new javax.swing.JScrollPane();
			workingDirTree = new javax.swing.JTree();
			choosePathButton = new javax.swing.JButton();
			createPathBox = new javax.swing.JCheckBox();

			setMaximumSize(new java.awt.Dimension(600, 32767));
			setMinimumSize(new java.awt.Dimension(500, 200));

			chooserTextField.setText("jTextField1");
			jScrollPane1.setViewportView(chooserTextField);

			chooserInfoAndTips.setText("jLabel1");

			jScrollPane2.setViewportView(workingDirTree);

			choosePathButton.setText("choose this path");

			createPathBox.setText("create path if necessary");

			javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
			this.setLayout(layout);
			layout.setHorizontalGroup(
					layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
					.addGroup(layout.createSequentialGroup()
							.addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
									.addComponent(jScrollPane2)
									.addGroup(layout.createSequentialGroup()
											.addContainerGap()
											.addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
													.addComponent(loadPathComboBox, javax.swing.GroupLayout.Alignment.TRAILING, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
													.addComponent(jScrollPane1)
													.addComponent(chooserInfoAndTips, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
													.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
															.addComponent(createPathBox, javax.swing.GroupLayout.PREFERRED_SIZE, 289, javax.swing.GroupLayout.PREFERRED_SIZE)
															.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
															.addComponent(choosePathButton, javax.swing.GroupLayout.DEFAULT_SIZE, 242, Short.MAX_VALUE)))))
							.addContainerGap())
					);
			layout.setVerticalGroup(
					layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
					.addGroup(layout.createSequentialGroup()
							.addGap(7, 7, 7)
							.addComponent(chooserInfoAndTips)
							.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
							.addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 49, javax.swing.GroupLayout.PREFERRED_SIZE)
							.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
							.addComponent(loadPathComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
							.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
							.addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 171, Short.MAX_VALUE)
							.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
							.addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
									.addComponent(choosePathButton)
									.addComponent(createPathBox)))
					);
		}// </editor-fold>                        


		// Variables declaration - do not modify                     
		protected javax.swing.JButton choosePathButton;
		protected javax.swing.JLabel chooserInfoAndTips;
		protected javax.swing.JTextField chooserTextField;
		protected javax.swing.JCheckBox createPathBox;
		protected javax.swing.JScrollPane jScrollPane1;
		protected javax.swing.JScrollPane jScrollPane2;
		protected javax.swing.JComboBox<Path> loadPathComboBox;
		protected javax.swing.JTree workingDirTree;
		// End of variables declaration   
	}
}
