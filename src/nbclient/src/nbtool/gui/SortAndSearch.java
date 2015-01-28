package nbtool.gui;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.util.ArrayList;
import java.util.Collections;
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

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.test.TestUtils;
import nbtool.util.U;

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
		TIME(0, new Comparator<Log>(){

			public int compare(Log o1, Log o2) {
				Long s1 = o1.time();
				Long s2 = o2.time();
				
				if (s1 == null && s2 == null)
					return 0;
				if (s1 == null)
					return 1;
				if (s2 == null)
					return -1;
				
				return s1.compareTo(s2);
			}
			
		}), 
		TYPE(1, new Comparator<Log>(){

			public int compare(Log o1, Log o2) {
				String s1 = o1.type();
				String s2 = o2.type();
				
				if (s1 == null && s2 == null)
					return 0;
				if (s1 == null)
					return 1;
				if (s2 == null)
					return -1;
				
				return s1.compareTo(s2);
			}
			
		}),
		IMAGE(2, new Comparator<Log>(){

			public int compare(Log o1, Log o2) {
				Integer s1 = o1.index();
				Integer s2 = o2.index();
				
				if (s1 == null && s2 == null)
					return 0;
				if (s1 == null)
					return 1;
				if (s2 == null)
					return -1;
				
				return s1.compareTo(s2);
			}
			
		}),
		FROM(3, new Comparator<Log>(){

			public int compare(Log o1, Log o2) {
				String s1 = o1.from();
				String s2 = o2.from();
				
				if (s1 == null && s2 == null)
					return 0;
				if (s1 == null)
					return 1;
				if (s2 == null)
					return -1;
				
				return s1.compareTo(s2);
			}
			
		}),
		ARRIVED(4, null);
		
		public Comparator<Log> sorter;
		public final int index;
		private SortType(int i, Comparator<Log> s) 
		{index = i; sorter = s;}
	}
	
	private String[] sortNames = 
		{
			"time",
			"type",
			"image i",
			"from",
			"arrived"
		};
	
	public void actionPerformed(ActionEvent e) {
		U.wf("SortAndSearch: new specs: [%s, %s]\n", sortNames[sortBy.getSelectedIndex()], search_f.getText());
		
		lcm.ssChanged();
	}
	
	public void sort(Session s) {
		int i = sortBy.getSelectedIndex();
		Comparator<Log> cmp;
		
		if (i < 0)
			cmp = null;
		else
			cmp = SortType.values()[i].sorter;
		
		String mustContain = search_f.getText().trim();
		boolean rev = reverse.isSelected();
		
		assert(s != null);
		assert(s.logs_ALL != null);
		
		s.logs_DO = new ArrayList<Log>(s.logs_ALL.size());
		
		if (mustContain.isEmpty())
			s.logs_DO.addAll(s.logs_ALL);
		else for (Log l : s.logs_ALL)
				if (l.description.contains(mustContain)) s.logs_DO.add(l);
		
		if (cmp != null)
			Collections.sort(s.logs_DO, cmp);
		
		if (rev)
			Collections.reverse(s.logs_DO);
	}
	
	private LCTreeModel lcm;
	
	public static void main(String[] args) {
		SortAndSearch sas = new SortAndSearch(null);
		TestUtils.frameForPanel(sas);
	}
}
