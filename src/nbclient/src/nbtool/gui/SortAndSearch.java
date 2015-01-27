package nbtool.gui;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.util.ArrayList;
import java.util.Comparator;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.border.LineBorder;

public class SortAndSearch extends JPanel implements ActionListener {
	public SortAndSearch(LCTreeModel lcm) {
		super();
		setLayout(null);
		
		this.lcm = lcm;
		
		sort_l = new JLabel("sort by:");
		sort_l.setBorder(BorderFactory.createEmptyBorder( 3, 0, 0, 0 ));
		add(sort_l);
		
		sortBy = new JComboBox<String>(sortNames);
		sortBy.setEditable(false);
		sortBy.setSelectedIndex(0);
		sortBy.addActionListener(this);
		add(sortBy);
		
		reverse = new JCheckBox("reverse:");
		reverse.setHorizontalTextPosition(JCheckBox.LEFT);
		reverse.setSelected(false);
		reverse.addActionListener(this);
		add(reverse);
		
		search_l = new JLabel("search for:");
		search_l.setBorder(BorderFactory.createEmptyBorder( 3, 0, 0, 0 ));
		search_f = new JTextField(12);
		search_f.addActionListener(this);
		add(search_l); add(search_f);
		
		setBorder(LineBorder.createGrayLineBorder());
		
		setBounds();
	}
	
	private void setBounds() {
		Dimension d1, d2;
		int x = getInsets().left + 2;
		int y = getInsets().top;
		
		d1 = sort_l.getPreferredSize();
		sort_l.setBounds(x, y, d1.width, d1.height);
		x += d1.width;
		d2 = sortBy.getPreferredSize();
		sortBy.setBounds(x, y, d2.width, d2.height);
		x += d2.width;
		d1 = reverse.getPreferredSize();
		reverse.setBounds(x, y, d1.width, d1.height);
		
		y += d2.height + 1; //largest
		
		int max_x = x + d1.width;
		x = getInsets().left + 2;
		d1 = search_l.getPreferredSize();
		search_l.setBounds(x, y, d1.width, d1.height);
		x += d1.width;
		d1 = search_f.getPreferredSize();
		search_f.setBounds(x, y, d1.width, d1.height);
		y += d1.height;
		
		Dimension exact = new Dimension(
				max_x + getInsets().right + 2,
				y + getInsets().bottom );
		this.setPreferredSize(exact);
		this.setMinimumSize(exact);
	}
	
	private JLabel sort_l;
	private JComboBox<String> sortBy;
	private JCheckBox reverse;
	
	private JLabel search_l;
	private JTextField search_f;
	
	public static enum SortType {
		TIME(0), TYPE(1), IMAGE(2), FROM(3);
		
		public final int index;
		private SortType(int i) {index = i;}
	}
	
	private String[] sortNames = 
		{
			"time",
			"type",
			"image",
			"from"
		};

	public void actionPerformed(ActionEvent e) {
		System.out.println("" + System.currentTimeMillis());
	}
	
	public void sort(ArrayList<Object> sessionContents) {
		switch (sortBy.getSelectedIndex()) {
		case -1:
			break;
		
		}
	}
	
	public boolean searchPasses(Object o) {
		return false;
	}
	
	private LCTreeModel lcm;
}
