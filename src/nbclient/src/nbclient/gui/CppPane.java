package nbclient.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.AbstractListModel;
import javax.swing.BorderFactory;
import javax.swing.DropMode;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.ListModel;
import javax.swing.ListSelectionModel;
import javax.swing.TransferHandler;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.event.ListDataEvent;
import javax.swing.event.ListDataListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.tree.TreePath;

import nbclient.data.OpaqueLog;
import nbclient.io.CppIO;
import nbclient.io.CppIO.CppFuncListener;
import nbclient.util.N;
import nbclient.util.N.EVENT;
import nbclient.util.N.NListener;
import nbclient.util.U;



public class CppPane extends JPanel implements ActionListener, NListener, CppFuncListener {
	private static final long serialVersionUID = 1L;

	protected CppPane(LogChooser lc) {
		super();
		
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		this.lc = lc;
		
		connect_status = new JLabel(CppIO.current.connected ? "status: good    " : "status: bad    ");
		connect_status.setForeground(CppIO.current.connected ? Color.GREEN : Color.RED);
		
		funcName = new JComboBox<String>();
		funcName.addActionListener(this);
		funcName.setEnabled(false);
		
		arg_data = new ArrayList<OpaqueLog>();
		der_data = new ArrayList<OpaqueLog>();
		arg_model = new MODEL(arg_data);
		der_model = new MODEL(der_data);
		
		Border line = BorderFactory.createBevelBorder(BevelBorder.RAISED, Color.BLACK, Color.DARK_GRAY);
				
		macros = new JList<String>(new String[]{CA_S, NA_S});
		macros.setDragEnabled(true);
		macros.setLayoutOrientation(JList.VERTICAL);
		macros.setVisibleRowCount(2);
		macros.setBorder(BorderFactory.createTitledBorder(line, "macros"));
		macros.setEnabled(false);
		
		handler = new HANDLER();
		
		args = new JList<String>(arg_model);
		args.setLayoutOrientation(JList.VERTICAL);
		args.setVisibleRowCount(5);
		args.setTransferHandler(handler);
		args.setBorder(BorderFactory.createTitledBorder(line, "arguments"));
		args.setEnabled(false);
		
		derived = new JList<String>(der_model);
		derived.addListSelectionListener(der_model);
		derived.setLayoutOrientation(JList.VERTICAL);
		derived.setVisibleRowCount(5);
		derived.setBorder(BorderFactory.createTitledBorder(line, "derived"));
		derived.setEnabled(false);
		
		go = new JButton("GO");
		clear = new JButton("CLEAR");
		go.addActionListener(this);
		clear.addActionListener(this);
		
		N.listen(EVENT.CPP_CONNECTED, this);
		N.listen(EVENT.CPP_FUNCS, this);
		
		add(funcName);
		add(macros);add(args);add(derived);add(go);add(clear);
		add(connect_status);
	}
	
	private void useSize(Dimension size) {
		int y_offset = 0;
		
		Dimension p = connect_status.getPreferredSize();
		connect_status.setBounds(size.width - p.width, 0,
				p.width, p.height);
		y_offset += p.height;
		
		p = funcName.getPreferredSize();
		funcName.setBounds(5, y_offset, size.width, p.height);
		y_offset += p.height + 10;
		
		p = macros.getPreferredSize();
		macros.setBounds(0, y_offset, size.width - 5, p.height);
		y_offset += p.height + 10;
		
		p = args.getPreferredScrollableViewportSize();
		args.setBounds(0, y_offset, size.width, p.height);
		y_offset += p.height + 20;
		
		p = derived.getPreferredScrollableViewportSize();
		derived.setBounds(0, y_offset, size.width, p.height);
		y_offset += p.height + 20;
		
		p = clear.getPreferredSize();
		clear.setBounds(size.width / 2 - p.width / 2, y_offset, p.width, p.height);
		y_offset += p.height ;
		
		p = go.getPreferredSize();
		go.setBounds(size.width / 2 - p.width / 2, y_offset, p.width, p.height);
	}
	
	private JComboBox<String> funcName;
		
	private JLabel connect_status;
	private JButton go, clear;
	
	private JList<String> macros;
	private JList<String> args;
	private JList<String> derived;
	private HANDLER handler;
	
	private MODEL arg_model;
	private MODEL der_model;
	
	private ArrayList<OpaqueLog> arg_data;
	private ArrayList<OpaqueLog> der_data;
	
	private class HANDLER extends TransferHandler {		
		private static final long serialVersionUID = 1L;
		
		public boolean canImport(TransferSupport p) {
			return p.isDataFlavorSupported(DataFlavor.stringFlavor) || p.isDataFlavorSupported(U.treeFlavor);
		}
		
		public boolean importData(TransferSupport p) {
			OpaqueLog imp = null;
			try {
				if (p.isDataFlavorSupported(U.treeFlavor)) {

					imp = (OpaqueLog) p.getTransferable().getTransferData(U.treeFlavor);
				}
				else if (p.isDataFlavorSupported(DataFlavor.stringFlavor)) {
					String mcro = (String) p.getTransferable().getTransferData(DataFlavor.stringFlavor);
					if (mcro.equalsIgnoreCase(CA_S)) {
						imp = CUR_ALIAS;
					} else if (mcro.equalsIgnoreCase(NA_S)) {
						imp = NEXT_ALIAS;
					} else {
						U.w("CppPane: Handler: Unknown import: " + mcro);
					}
				} else {
					U.w("Unknown import type.");
				}
			} catch (UnsupportedFlavorException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			
			
			
			for (int i = 0; i < arg_data.size(); ++i) {
				OpaqueLog cur = arg_data.get(i);
				if (cur.description.startsWith("::") || cur.bytes != null) 
					continue;
				else if (imp.description.startsWith("::")) {
					arg_data.set(i, imp);
					U.wf("CppPane: args: import for [%s] is MACRO\n", cur.description);
					arg_model.reload();
					return true;
				} else {
					String req_type = cur.description;
					String imp_type = (String) imp.getAttributes().get("type");
					assert(imp_type != null);
					if (req_type.equalsIgnoreCase(imp_type)) {
						U.wf("CppPane: args: import for [%s] exact: %s\n", req_type, imp_type);
						arg_data.set(i, imp);
						arg_model.reload();
						return true;
					}
				}
			}
			
			U.w("CppPane: args: could not import: " + imp);
			return false; 
		}
	}
	
	private class MODEL extends AbstractListModel<String> implements ListSelectionListener {
		private static final long serialVersionUID = 1L;
		private ArrayList<OpaqueLog> data;
		protected MODEL(ArrayList<OpaqueLog> l) {this.data = l;}

		@Override
		public int getSize() {
			return this.data.size();
		}

		@Override
		public String getElementAt(int index) {
			// TODO Auto-generated method stub
			return this.data.get(index).description;
		}
		
		/*
		public void addString(String s) {
			data.add(s);
			fireIntervalAdded(this, data.size() - 1, data.size() - 1);
		} */
		
		
		public void reload() {
			this.fireContentsChanged(this, 0, data.size());
		}

		public void valueChanged(ListSelectionEvent e) {
			if (e.getValueIsAdjusting())
				return;
			
			int index = ((JList) e.getSource()).getSelectedIndex();
			OpaqueLog l = der_data.get(index);
			N.notify(EVENT.SELECTION, this, l);
		}
	}
	
	private String tryCall() {
		int i = funcName.getSelectedIndex();
		if (i < 0) return "no function selected";
		CppIO.CppFunc f = CppIO.current.foundFuncs.get(i);
		
		if (f.args.length != arg_data.size())
			return "not enough arguments";
		
		OpaqueLog[] ARGS = new OpaqueLog[f.args.length];
		arg_data.toArray(ARGS);
		
		//Get listing of arguments (I.e. convert macro to log)
		for (int j = 0; j < f.args.length; ++j) {
			OpaqueLog a = arg_data.get(i);
			
			if (a.description.equalsIgnoreCase(NA_S)) {
				a = lc.nextSelection();
				ARGS[j] = a;
			}
			
			if (a.description.equalsIgnoreCase(CA_S)) {
				a = lc.currentlySelected();
				ARGS[j] = a;
			}
			
			if (a == null) {
				return "arg " + j + " was null.";
			}
			
			if (a.bytes == null) {
				return "arg " + j + " had null bytes.";
			}
			
			String atype = (String) a.getAttributes().get("type");
			String rtype = f.args[j];
			
			if (!atype.equalsIgnoreCase(rtype)) {
				return "arg had type [" + atype + "] wanted [" + rtype + "].";
			}
		}
		
		CppIO.CppFuncCall call = CppIO.current.new CppFuncCall();
		call.args = new ArrayList<OpaqueLog>(Arrays.asList(ARGS));
		call.listener = this;
		call.index = funcName.getSelectedIndex(); assert(call.index >= 0);
		call.name = f.name;
		
		U.wf("CppPane: call to %s (%d)\n", call.name, call.index);
		for (OpaqueLog a : call.args) 
			U.wf("\t%s\n", a.description);
		
		if (!CppIO.current.tryAddCall(call)) {
			return "CppIO rejected call with name: " + call.name;
		}
		
		return null;
	}
	
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == go) {
			String msg = tryCall();
			if (msg != null) {
				JOptionPane.showMessageDialog(this, msg, "error", JOptionPane.ERROR_MESSAGE);
			}
			
			return;
		}
		
		if (e.getSource() == clear) {
			//data = new ArrayList<String>();
			arg_data.clear();
			arg_model.reload();
		}
		
		//Need to reload func args.
		if (e.getSource() == funcName || e.getSource() == clear){
			//Change args...
			int i = funcName.getSelectedIndex();
			if (i < 0) return;
			
			CppIO.CppFunc f = CppIO.current.foundFuncs.get(i);
			
			arg_data.clear();
			
			for (String s : f.args) {
				arg_data.add(new OpaqueLog(s, null));
			}
			
			arg_model.reload();
			this.useSize(this.getSize());
		} else {
			U.w("ERROR: unknown source! " + e.getSource());
		}
	}
	
	public void notified(EVENT e, Object src, Object... argArray) {
		switch(e) {
		case CPP_CONNECTED:
			String n = CppIO.current.connected ? "status: good" : "status: bad";
			Color nc = CppIO.current.connected ? Color.GREEN : Color.RED;
			connect_status.setText(n);
			connect_status.setForeground(nc);;
			
			if (CppIO.current.connected) {
				args.setEnabled(true);
				//macros.setEnabled(true);
			} else {
				args.setEnabled(false);
				macros.setEnabled(false);
				funcName.setEnabled(false);
			}
			break;
		case CPP_FUNCS:
			ArrayList<CppIO.CppFunc> funcs = (ArrayList<CppIO.CppFunc>) argArray[0];
			funcName.removeAllItems();
			for (CppIO.CppFunc f : funcs) {
				funcName.addItem(f.name);
			}
			
			funcName.setEnabled(true);
			arg_data.clear();
			arg_model.reload();
			this.useSize(this.getSize());
			
			break;
		default: {U.w("CppPane notified of event it did not listen for! " + e);}
		}
		
		this.repaint();
	}
	
	//CppIO.CppCallListener
	public void returned(int ret, OpaqueLog... out) {
		
		U.w("CppPane: function returned with ret:" + ret + " and " + out.length + " out.");
		
		der_data.clear();
		der_data.addAll(Arrays.asList(out));
		der_model.reload();
		derived.setEnabled(true);
	}
	
	private LogChooser lc;
	private static final String NA_S = "::next selection::";
	private static final String CA_S = "::current selection::";
	private static final OpaqueLog NEXT_ALIAS = new OpaqueLog(NA_S, null);
	private static final OpaqueLog CUR_ALIAS = new OpaqueLog(CA_S, null);

	
}
