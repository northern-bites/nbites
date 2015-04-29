package nbtool.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.AbstractListModel;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.TransferHandler;
import javax.swing.TransferHandler.TransferSupport;
import javax.swing.border.Border;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import nbtool.data.Log;
import nbtool.io.CppIO;
import nbtool.io.CppIO.CppFunc;
import nbtool.io.CppIO.CppFuncCall;
import nbtool.io.CppIO.CppFuncListener;
import nbtool.util.N;
import nbtool.util.NBConstants;
import nbtool.util.U;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;

public class CppPane extends JPanel implements ActionListener, NListener, CppFuncListener {

	private static void exact(Dimension d, Component c) {
		c.setMinimumSize(d);
		c.setMaximumSize(d);
		c.setPreferredSize(d);
	}

	private void upEnable() {
		status.setText("[connected]");
		status.setForeground(Color.GREEN);
		
		functions.removeAllItems();
		for (CppIO.CppFunc f : found)
			functions.addItem(f.name);
		functions.setEnabled(true);
		functions.setSelectedIndex(-1);
		selected = null;
		
		macros.setEnabled(false);
		call.setEnabled(true);
		clear.setEnabled(true);
		
		arg_list.setEnabled(true);
		arg_model.reload();
		out_list.setEnabled(true);
		out_model.reload();
	}

	private void downDisable() {
		status.setText("[down]");
		status.setForeground(Color.BLACK);
		
		functions.removeAllItems();
		functions.setEnabled(false);
		functions.setSelectedIndex(-1);
		found = null;
		selected = null;
		
		macros.setEnabled(false);
		call.setEnabled(false);
		clear.setEnabled(false);
		
		arg_list.setEnabled(false);
		arg_model.reload();
		out_list.setEnabled(false);
		out_model.reload();
	}

	public CppPane() {
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		Border b = BorderFactory.createLineBorder(Color.BLACK);
		status = new JLabel("[placeholder...]");
		status.setForeground(Color.GRAY);
		status.setAlignmentX(Component.LEFT_ALIGNMENT);
		add(status);

		//add(Box.createRigidArea(new Dimension(200,5)));
		functions = new JComboBox<String>();
		functions.setEditable(false);
		functions.addActionListener(this);
		functions.setAlignmentY(Component.TOP_ALIGNMENT);
		functions.setBorder(BorderFactory.createTitledBorder(b, "functions"));
		functions.setMaximumSize(new Dimension(Short.MAX_VALUE,
				this.getPreferredSize().height));
		add(functions);
		add(Box.createRigidArea(new Dimension(0,10)));
		//add(Box.createRigidArea(new Dimension(200,5)));
		macros = new JList<String>(new String[]{CA_S,NA_S});
		macros.setDragEnabled(true);
		macros.setLayoutOrientation(JList.VERTICAL);
		macros.setVisibleRowCount(2);
		macros.setAlignmentX(Component.LEFT_ALIGNMENT);
		macros.setBorder(BorderFactory.createTitledBorder(b, "macros"));
		add(macros);
		add(Box.createRigidArea(new Dimension(0,10)));

		args = new ArrayList<Log>();
		arg_list = new JList<String>();
		arg_model = new ArgModel(arg_list);
		arg_list.setLayoutOrientation(JList.VERTICAL);
		arg_list.setVisibleRowCount(5);
		arg_list.setBorder(BorderFactory.createTitledBorder(b, "args"));
		arg_list.setMinimumSize(new Dimension(200,50));
		arg_list.setTransferHandler(new ArgImporter());
		add(arg_list);
		
		out = new ArrayList<Log>();
		out_list = new JList<String>();
		out_model = new OutModel(out_list);
		out_list.setLayoutOrientation(JList.VERTICAL);
		out_list.setVisibleRowCount(5);
		out_list.setBorder(BorderFactory.createTitledBorder(b, "out"));
		out_list.setMinimumSize(new Dimension(200, 50));
		add(out_list);

		call = new JButton("call");
		call.setAlignmentX(CENTER_ALIGNMENT);
		call.addActionListener(this);
		clear = new JButton("clear");
		clear.setAlignmentX(CENTER_ALIGNMENT);
		clear.addActionListener(this);
		add(call);
		add(clear);
		add(Box.createVerticalGlue());
		
		downDisable();
		
		N.listen(EVENT.CPP_CONNECTION, this);
		N.listen(EVENT.CPP_FUNCS_FOUND, this);
	}

	private JLabel status;
	private JComboBox<String> functions;

	private JButton call, clear;

	private JList<String> macros;

	private JList<String> arg_list;
	private ArgModel arg_model;
	private ArrayList<Log> args;

	private JList<String> out_list;
	private OutModel out_model;
	
	private ArrayList<CppIO.CppFunc> found;
	private CppFunc selected;

	@Override
	public void actionPerformed(ActionEvent e) {
		Object source = e.getSource();
		if (source == functions) {
			int i = functions.getSelectedIndex();
			if (i >= 0) {
				selected = found.get(i);
				args = new ArrayList<Log>();
				arg_model.reload();
			}
		} else if (source == call) {
			if (selected == null)
				return;
			if (args.size() != selected.args.length)
				return;
			CppFuncCall call = new CppFuncCall();
			call.index = found.indexOf(selected);
			call.name = selected.name;
			call.args = args;
			call.listener = this;
			CppIO.current.tryAddCall(call);
		} else if (source == clear) {
			args = new ArrayList<Log>();
			arg_model.reload();
		}
	}
	
	@Override
	public void notified(EVENT e, Object src, Object... args) {
		switch(e){
		case CPP_CONNECTION:
			Boolean con = (Boolean) args[0];
			if (!con)
				this.downDisable();
			break;
		case CPP_FUNCS_FOUND:
			found = (ArrayList<CppFunc>) args[0];
			this.upEnable();
			break;
		}
		
	}

	private static final String NA_S = "::next selection::";
	private static final String CA_S = "::current selection::";
	private static final Log NEXT_ALIAS = new Log(NA_S, null);
	private static final Log CUR_ALIAS = new Log(CA_S, null);

	private class ArgModel extends AbstractListModel<String>
	implements ListSelectionListener {

		protected ArgModel(JList<String> l) {
			super();
			l.setModel(this);
			l.addListSelectionListener(this);
		}

		@Override
		public int getSize() {
			if (selected != null)
				return selected.args.length;
			else return 0;
		}

		@Override
		public String getElementAt(int index) {
			if (index < args.size())
				return args.get(index).toString();
			else return selected.args[index];
		}

		@Override
		public void valueChanged(ListSelectionEvent e) {
			if (e.getValueIsAdjusting())
				return;
			
			int index = ((JList<String>) e.getSource()).getSelectedIndex();
			if (index >= args.size() || index < 0)
				return;
			
			Log l = args.get(index);
			N.notifyEDT(EVENT.LOG_SELECTION, this, l);
		}
		
		public void reload() {
			this.fireContentsChanged(this, 0, getSize());
		}
	}
	
	private class ArgImporter extends TransferHandler {		
		private static final long serialVersionUID = 1L;
		
		public boolean canImport(TransferSupport p) {
			if (args.size() == selected.args.length)
				return false;
			return p.isDataFlavorSupported(DataFlavor.stringFlavor)
					|| p.isDataFlavorSupported(NBConstants.treeFlavor);
		}
		
		public boolean importData(TransferSupport p) {
			Log imp = null;
			try {
				if (p.isDataFlavorSupported(NBConstants.treeFlavor)) {

					imp = (Log) p.getTransferable().getTransferData(NBConstants.treeFlavor);
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
			
			
			args.add(imp);
			arg_model.reload();
			return true;
			
			//U.w("CppPane: args: could not import: " + imp);
			//return false; 
		}
	}

	private ArrayList<Log> out;
	private class OutModel extends AbstractListModel<String>
	implements ListSelectionListener {

		protected OutModel(JList<String> l) {
			super();
			l.setModel(this);
			l.addListSelectionListener(this);
		}

		@Override
		public int getSize() {
			return out.size();
		}

		@Override
		public String getElementAt(int index) {
			return out.get(index).toString();
		}

		@Override
		public void valueChanged(ListSelectionEvent e) {
			if (e.getValueIsAdjusting())
				return;
			
			int index = ((JList<String>) e.getSource()).getSelectedIndex();
			Log l = out.get(index);
			N.notifyEDT(EVENT.LOG_SELECTION, this, l);
		}

		public void reload() {
			this.fireContentsChanged(this, 0, getSize());
		}
	}

	//For testing...
	public static void main(String[] args) {
		JFrame frame = new JFrame("test");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(200, 600);

		CppPane pane = new CppPane();
		frame.add(pane);
		pane.setBounds(0,20,200,580);

		frame.setVisible(true);
	}

	@Override
	public void returned(int ret, Log... out) {
		U.wf("CppPane returned: %d with %d logs\n", ret, out.length);
		
		this.out.addAll(Arrays.asList(out));
		out_model.reload();
	}
}
