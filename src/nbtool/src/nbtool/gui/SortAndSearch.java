package nbtool.gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import javax.swing.JPanel;
import javax.swing.border.LineBorder;

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.test.TestUtils;
import nbtool.util.Logger;

public class SortAndSearch extends JPanel implements ActionListener {
	public SortAndSearch(final LogChooserModel lcm) {
		super();		
		this.lcm = lcm;
		
		setBorder(LineBorder.createGrayLineBorder());
		
		initComponents();
		
		sortByBox.addActionListener(this);
		reverseBox.addActionListener(this);
		searchField.addActionListener(this);
		
		deleteLogButton.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				lcm.deleteCurrent();
			}
		});
	}                                           
	
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
				Integer s1 = o1.primaryImgIndex();
				Integer s2 = o2.primaryImgIndex();
				
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
		Logger.logf(Logger.INFO, "SortAndSearch: new specs: [sort=%s, search=%s, order=%s]\n", sortNames[sortByBox.getSelectedIndex()],
				searchField.getText(), reverseBox.isSelected() ? "reverse" : "normal");
		
		lcm.ssChanged();
	}
	
	public void sort(Session s) {
		int i = sortByBox.getSelectedIndex();
		Comparator<Log> cmp;
		
		if (i < 0)
			cmp = null;
		else
			cmp = SortType.values()[i].sorter;
		
		String mustContain = searchField.getText().trim();
		boolean rev = reverseBox.isSelected();
		
		assert(s != null);
		assert(s.logs_ALL != null);
		
		s.logs_DO = new ArrayList<Log>(s.logs_ALL.size());
		
		if (mustContain.isEmpty())
			s.logs_DO.addAll(s.logs_ALL);
		else for (Log l : s.logs_ALL)
				if (l.description().contains(mustContain)) s.logs_DO.add(l);
		
		//System.out.printf("sort using %d of %d logs\n", s.logs_DO.size(), s.logs_ALL.size());
		
		if (cmp != null)
			Collections.sort(s.logs_DO, cmp);
		
		if (rev)
			Collections.reverse(s.logs_DO);
	}
	
	private LogChooserModel lcm;
	
	public static void main(String[] args) {
		SortAndSearch sas = new SortAndSearch(null);
		TestUtils.frameForPanel(sas);
	}
	
    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
	private void initComponents() {
        jLabel1 = new javax.swing.JLabel();
        sortByBox = new javax.swing.JComboBox<>(sortNames);
        jLabel2 = new javax.swing.JLabel();
        searchField = new javax.swing.JTextField();
        reverseBox = new javax.swing.JCheckBox();
        deleteLogButton = new javax.swing.JButton();

        jLabel1.setText("sort by:");

        jLabel2.setText("search:");

        reverseBox.setText("reverse");
        reverseBox.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);

        deleteLogButton.setText("rm log");

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
                        .addComponent(deleteLogButton)
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
                    .addComponent(deleteLogButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(searchField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
        );
    }// </editor-fold>                                                            

    // Variables declaration - do not modify                     
    private javax.swing.JButton deleteLogButton;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JCheckBox reverseBox;
    private javax.swing.JTextField searchField;
    private javax.swing.JComboBox<String> sortByBox;
    // End of variables declaration  
}
