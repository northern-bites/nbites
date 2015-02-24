package nbtool.gui.utilitypanes;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Properties;

import javax.swing.JButton;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JTextField;

import nbtool.util.P;
import nbtool.util.U;

public class LogToViewUtility extends UtilityParent implements ActionListener {
	
	public LogToViewUtility() {
		super();
		setLayout(null);
		setSize(800, 800);
		this.setResizable(false);
		
		this.setLayout(new BorderLayout());
		
		loadDirectory = new JTextField(40);
		load = new JButton("load");
		load.addActionListener(this);
		
		JPanel container = U.fieldWithButton(loadDirectory, load);
		
		maplist = new JList();
		maplist.setMinimumSize(new Dimension(800, 600));
		
		this.add(container, BorderLayout.NORTH);
		this.add(maplist, BorderLayout.CENTER);
	}
	
	@Override
	public void actionPerformed(ActionEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Object getCurrentValue() {
		// TODO Auto-generated method stub
		return null;
	}

	private JTextField loadDirectory;
	private JButton load;
	private JList<String> maplist;
	
	private LinkedList<String> foundPaths;
	private ArrayList<Properties> foundProperties;
	
}
