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
import nbtool.util.Logger;
import nbtool.util.Utility;

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
		
		//
		searchField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                searchFieldActionPerformed(evt);
            }
        });
		
		reverseBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                reverseBoxActionPerformed(evt);
            }
        });
		
		
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
				Long s1 = o1.primaryTime();
				Long s2 = o2.primaryTime();
				
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
				String s1 = o1.primaryType();
				String s2 = o2.primaryType();
				
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
				Integer s1 = o1.primaryI_Index();
				Integer s2 = o2.primaryI_Index();
				
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
				String s1 = o1.primaryFrom();
				String s2 = o2.primaryFrom();
				
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
		Logger.logf(Logger.INFO, "SortAndSearch: new specs: [sort=%s, search=%s, order=%s]\n", sortNames[sortBy.getSelectedIndex()],
				search_f.getText(), reverse.isSelected() ? "reverse" : "normal");
		
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
				if (l.description().contains(mustContain)) s.logs_DO.add(l);
		
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
	
	@SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
    private void initComponents() {

        jCheckBox1 = new javax.swing.JCheckBox();
        jLabel1 = new javax.swing.JLabel();
        sortByBox = new javax.swing.JComboBox<String>();
        jLabel2 = new javax.swing.JLabel();
        searchField = new javax.swing.JTextField();
        reverseBox = new javax.swing.JCheckBox();
        windowButton = new javax.swing.JButton();

        jCheckBox1.setText("jCheckBox1");

        jLabel1.setText("sort by:");

        jLabel2.setText("search:");

        searchField.setText("jTextField1");

        reverseBox.setText("reverse");
        reverseBox.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
        

        windowButton.setText("window");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(sortByBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(reverseBox)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(windowButton)
                        .addGap(0, 0, Short.MAX_VALUE))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(searchField))))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(sortByBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(reverseBox)
                    .addComponent(windowButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(searchField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
        );
    }// </editor-fold>                        

    private void reverseBoxActionPerformed(java.awt.event.ActionEvent evt) {                                           
        // TODO add your handling code here:
    }                                          

    private void searchFieldActionPerformed(java.awt.event.ActionEvent evt) {                                            
        // TODO add your handling code here:
    }                                           


    // Variables declaration - do not modify                     
    private javax.swing.JCheckBox jCheckBox1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JCheckBox reverseBox;
    private javax.swing.JTextField searchField;
    private javax.swing.JComboBox<String> sortByBox;
    private javax.swing.JButton windowButton;
    // End of variables declaration   
}
