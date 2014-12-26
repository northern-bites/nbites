package nbclient.gui;

import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.ListSelectionModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import nbclient.data.SessionHandler;
import nbclient.data.OpaqueLog;
import nbclient.util.N;
import nbclient.util.N.EVENT;
import nbclient.util.N.NListener;

public class LogChooser extends JPanel {
	private static final long serialVersionUID = 1L;
	public LogChooser(SessionHandler handler) {
		setLayout(null);
		dh = handler;
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		//N.listen(EVENT.LOGS_ADDED, this);
		
		model = new LCTreeModel();
		tree = new JTree(model);
		model.tree = tree;
		tree.setEditable(false);
		tree.setRootVisible(false);
		tree.setScrollsOnExpand(true);
		tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
		tree.addTreeSelectionListener(model);
		tree.setTransferHandler(model.EXPORT_HANDLER);
		tree.setDragEnabled(true);
		
		sp = new JScrollPane(tree);
		sp.setViewportView(tree);
		
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		//sp.setVerticalScrollBarPolicy();
		
		add(sp);
	}
	
	private void useSize(Dimension size) {
		sp.setBounds(0, 0, size.width, size.height);
	}
	
	public OpaqueLog currentlySelected() {
		return model.CS_macro;
	}
	
	public OpaqueLog nextSelection() {
		return model.NS_macro;
	}
	
	private JScrollPane sp;
	private JTree tree;
	private LCTreeModel model;
	private SessionHandler dh;	
}
